#include "Gpt.h"
#include "Gpt_PBCfg.h"
#include "S32K144.h"
#include "nvic.h"

/* -------------------------------------------------------------------------- */
/* Private global data                                                        */
/* -------------------------------------------------------------------------- */

static const Gpt_ConfigType *Gpt_ConfigPtr = NULL_PTR;

static Gpt_DriverStatusType Gpt_DriverStatus = GPT_DRIVER_UNINIT;

static Gpt_ChannelRuntimeType Gpt_ChannelRuntime[GPT_CHANNEL_COUNT];

/*
 * Requested timer period in GPT ticks.
 *
 * LPIT hardware stores:
 *      TVAL = Value - 1
 *
 * Therefore the AUTOSAR requested value is stored separately.
 */
static Gpt_ValueType Gpt_ChannelTargetValue[GPT_CHANNEL_COUNT];

/*
 * Cached values used after Gpt_StopTimer().
 */
static Gpt_ValueType Gpt_ChannelElapsedValue[GPT_CHANNEL_COUNT];
static Gpt_ValueType Gpt_ChannelRemainingValue[GPT_CHANNEL_COUNT];


/* -------------------------------------------------------------------------- */
/* LPIT interrupt mapping                                                     */
/* -------------------------------------------------------------------------- */

static const IRQn_Type Gpt_ChannelIrq[GPT_CHANNEL_COUNT] =
{
    LPIT0_Ch0_IRQn,
    LPIT0_Ch1_IRQn,
    LPIT0_Ch2_IRQn,
    LPIT0_Ch3_IRQn
};


/* -------------------------------------------------------------------------- */
/* Static helper functions                                                    */
/* -------------------------------------------------------------------------- */

static const Gpt_ChannelConfigType *Gpt_lGetChannelConfig(Gpt_ChannelType Channel)
{
    uint8 i;

    if ((Gpt_ConfigPtr == NULL_PTR) ||
        (Gpt_ConfigPtr->Channels == NULL_PTR))
    {
        return NULL_PTR;
    }

    for (i = 0U; i < Gpt_ConfigPtr->ChannelCfgNumber; i++)
    {
        if (Gpt_ConfigPtr->Channels[i].ChannelId == Channel)
        {
            return &Gpt_ConfigPtr->Channels[i];
        }
    }

    return NULL_PTR;
}


/**
 * @brief Validate a GPT channel.
 */
static boolean Gpt_lIsValidChannel(Gpt_ChannelType Channel)
{
    if (Channel >= GPT_CHANNEL_COUNT)
    {
        return FALSE;
    }

    if (Gpt_lGetChannelConfig(Channel) == NULL_PTR)
    {
        return FALSE;
    }

    return TRUE;
}


/**
 * @brief Returns the LPIT interrupt/event bit corresponding to a channel.
 */
static uint32 Gpt_lGetChannelBit(Gpt_ChannelType Channel)
{
    return (1UL << Channel);
}


/**
 * @brief Disable one LPIT timer channel.
 */
static void Gpt_lDisableHardwareTimer(Gpt_ChannelType Channel)
{
    IP_LPIT0->CLRTEN = LPIT_CLRTEN_CLR_T_EN_0(
                           (Channel == 0U) ? 1U : 0U)
                     | LPIT_CLRTEN_CLR_T_EN_1(
                           (Channel == 1U) ? 1U : 0U)
                     | LPIT_CLRTEN_CLR_T_EN_2(
                           (Channel == 2U) ? 1U : 0U)
                     | LPIT_CLRTEN_CLR_T_EN_3(
                           (Channel == 3U) ? 1U : 0U);
}


/**
 * @brief Enable one LPIT timer channel.
 */
static void Gpt_lEnableHardwareTimer(Gpt_ChannelType Channel)
{
    IP_LPIT0->SETTEN = LPIT_SETTEN_SET_T_EN_0(
                           (Channel == 0U) ? 1U : 0U)
                     | LPIT_SETTEN_SET_T_EN_1(
                           (Channel == 1U) ? 1U : 0U)
                     | LPIT_SETTEN_SET_T_EN_2(
                           (Channel == 2U) ? 1U : 0U)
                     | LPIT_SETTEN_SET_T_EN_3(
                           (Channel == 3U) ? 1U : 0U);
}


/**
 * @brief Clear the timer interrupt flag.
 *
 * MSR.TIFn uses write-one-to-clear.
 */
static void Gpt_lClearInterruptFlag(Gpt_ChannelType Channel)
{
    IP_LPIT0->MSR = Gpt_lGetChannelBit(Channel);
}


/**
 * @brief Read current LPIT counter.
 *
 * LPIT is a down-counter.
 */
static Gpt_ValueType Gpt_lReadCurrentCounter(Gpt_ChannelType Channel)
{
    return (Gpt_ValueType)IP_LPIT0->TMR[Channel].CVAL;
}


/**
 * @brief Get remaining time while the timer is running.
 *
 * Hardware:
 *
 *     TVAL = TargetValue - 1
 *     CVAL counts downward
 *
 * Therefore:
 *
 *     Remaining = CVAL + 1
 */
static Gpt_ValueType Gpt_lGetRunningRemaining(Gpt_ChannelType Channel)
{
    Gpt_ValueType CurrentValue;
    Gpt_ValueType TargetValue;

    CurrentValue = Gpt_lReadCurrentCounter(Channel);
    TargetValue  = Gpt_ChannelTargetValue[Channel];

    /*
     * Protect against an asynchronous read or an unexpected hardware value.
     */
    if (CurrentValue >= TargetValue)
    {
        return TargetValue;
    }

    return CurrentValue + 1UL;
}


/**
 * @brief Get elapsed time while timer is running.
 */
static Gpt_ValueType Gpt_lGetRunningElapsed(Gpt_ChannelType Channel)
{
    Gpt_ValueType Remaining;
    Gpt_ValueType TargetValue;

    TargetValue = Gpt_ChannelTargetValue[Channel];
    Remaining   = Gpt_lGetRunningRemaining(Channel);

    if (Remaining >= TargetValue)
    {
        return 0UL;
    }

    return TargetValue - Remaining;
}


/**
 * @brief Configure one LPIT channel for 32-bit periodic counter mode.
 *
 * LPIT TCTRL:
 *     T_EN     = 0
 *     CHAIN    = 0
 *     MODE     = 00
 *     TSOT     = 0
 *     TSOI     = 0
 *     TROT     = 0
 *     TRG_SRC  = 0
 *     TRG_SEL  = 0
 */
static void Gpt_lConfigureChannel(Gpt_ChannelType Channel)
{
    IP_LPIT0->TMR[Channel].TCTRL =
          LPIT_TMR_TCTRL_MODE(0U)
        | LPIT_TMR_TCTRL_CHAIN(0U)
        | LPIT_TMR_TCTRL_TSOT(0U)
        | LPIT_TMR_TCTRL_TSOI(0U)
        | LPIT_TMR_TCTRL_TROT(0U)
        | LPIT_TMR_TCTRL_TRG_SRC(0U)
        | LPIT_TMR_TCTRL_TRG_SEL(0U);
}


/**
 * @brief Configure LPIT0 peripheral clock.
 */
static void Gpt_lEnableLpitClock(void)
{
    /*
     * Disable clock gate before changing PCS.
     */
    IP_PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC_MASK;

    /*
     * PCS = 6 -> SPLL2_DIV2_CLK.
     */
    IP_PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_PCS_MASK;
    IP_PCC->PCCn[PCC_LPIT_INDEX] |=
        PCC_PCCn_PCS(GPT_LPIT_CLOCK_SOURCE);

    /*
     * Enable LPIT peripheral clock.
     */
    IP_PCC->PCCn[PCC_LPIT_INDEX] |=
        PCC_PCCn_CGC(1U);
}


/**
 * @brief Reset runtime state for one channel.
 */
static void Gpt_lResetRuntimeChannel(Gpt_ChannelType Channel)
{
    Gpt_ChannelRuntime[Channel].Status =
        GPT_CHANNEL_STOPPED;

    Gpt_ChannelRuntime[Channel].NotificationEnabled =
        FALSE;

    Gpt_ChannelTargetValue[Channel] =
        0UL;

    Gpt_ChannelElapsedValue[Channel] =
        0UL;

    Gpt_ChannelRemainingValue[Channel] =
        0UL;
}


/* -------------------------------------------------------------------------- */
/* AUTOSAR GPT API                                                            */
/* -------------------------------------------------------------------------- */

#if (GPT_VERSION_INFO_API == STD_ON)

/**
 * @brief Returns GPT module version information.
 */
void Gpt_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
    if (versioninfo == NULL_PTR)
    {
        return;
    }

    versioninfo->vendorID =
        GPT_VENDOR_ID;

    versioninfo->moduleID =
        GPT_MODULE_ID;

    versioninfo->sw_major_version =
        GPT_SW_MAJOR_VERSION;

    versioninfo->sw_minor_version =
        GPT_SW_MINOR_VERSION;

    versioninfo->sw_patch_version =
        GPT_SW_PATCH_VERSION;
}

#endif /* GPT_VERSION_INFO_API */


/**
 * @brief Initializes the GPT driver.
 */
void Gpt_Init(const Gpt_ConfigType *ConfigPtr)
{
    uint8 i;
    uint32 ChannelMask = 0UL;

    if (ConfigPtr == NULL_PTR)
    {
        return;
    }

    if (ConfigPtr->Channels == NULL_PTR)
    {
        return;
    }

    if ((ConfigPtr->ChannelCfgNumber == 0U) ||
        (ConfigPtr->ChannelCfgNumber > GPT_CHANNEL_COUNT))
    {
        return;
    }

    if (Gpt_DriverStatus != GPT_DRIVER_UNINIT)
    {
        return;
    }

    /* Store configuration pointer. */
    Gpt_ConfigPtr = ConfigPtr;

    /* Enable peripheral clock in PCC. */
    Gpt_lEnableLpitClock();

    /*
     * Enable LPIT module clock.
     *
     * M_CEN must be enabled before accessing timer registers such as
     * MSR, MIER, SETTEN, CLRTEN, TVAL, CVAL and TCTRL.
     */
    IP_LPIT0->MCR |= LPIT_MCR_M_CEN(1U);

    /* Disable all configured timer channels first. */
    for (i = 0U; i < Gpt_ConfigPtr->ChannelCfgNumber; i++)
    {
        Gpt_ChannelType Channel =
            Gpt_ConfigPtr->Channels[i].ChannelId;

        if (Gpt_lIsValidChannel(Channel))
        {
            ChannelMask |= Gpt_lGetChannelBit(Channel);

            Gpt_lDisableHardwareTimer(Channel);
            Gpt_lClearInterruptFlag(Channel);
            Gpt_lConfigureChannel(Channel);

            IP_LPIT0->TMR[Channel].TVAL = 0UL;

            Gpt_lResetRuntimeChannel(Channel);

            /*
             * The LPIT NVIC lines are enabled, but GPT interrupt requests
             * remain disabled through MIER until Gpt_EnableNotification()
             * is called.
             */
            Nvic_EnableIRQ(Gpt_ChannelIrq[Channel]);
        }
    }

    /*
     * Disable interrupt requests for all configured channels.
     */
    IP_LPIT0->MIER &= ~ChannelMask;

    Gpt_DriverStatus = GPT_DRIVER_INIT;
}


/**
 * @brief Deinitializes the GPT driver.
 */
void Gpt_DeInit(void)
{
    uint8 i;

    if (Gpt_DriverStatus == GPT_DRIVER_UNINIT)
    {
        return;
    }

    /*
     * AUTOSAR GPT DeInit shall not deinitialize while a timer is running.
     */
    for (i = 0U; i < Gpt_ConfigPtr->ChannelCfgNumber; i++)
    {
        Gpt_ChannelType Channel =
            Gpt_ConfigPtr->Channels[i].ChannelId;

        if (Gpt_ChannelRuntime[Channel].Status ==
            GPT_CHANNEL_RUNNING)
        {
            return;
        }
    }

    for (i = 0U; i < Gpt_ConfigPtr->ChannelCfgNumber; i++)
    {
        Gpt_ChannelType Channel =
            Gpt_ConfigPtr->Channels[i].ChannelId;

        Gpt_lDisableHardwareTimer(Channel);
        Gpt_lClearInterruptFlag(Channel);

        IP_LPIT0->MIER &=
            ~Gpt_lGetChannelBit(Channel);

        Gpt_lResetRuntimeChannel(Channel);
    }

    /*
     * Disable LPIT module clock.
     */
    IP_LPIT0->MCR &=
        ~LPIT_MCR_M_CEN_MASK;

    /*
     * Disable LPIT peripheral clock gate.
     */
    IP_PCC->PCCn[PCC_LPIT_INDEX] &=
        ~PCC_PCCn_CGC_MASK;

    Gpt_ConfigPtr =
        NULL_PTR;

    Gpt_DriverStatus =
        GPT_DRIVER_UNINIT;
}


/**
 * @brief Returns elapsed timer value.
 */
Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType Channel)
{
    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return 0UL;
    }

    switch (Gpt_ChannelRuntime[Channel].Status)
    {
        case GPT_CHANNEL_RUNNING:

            return Gpt_lGetRunningElapsed(Channel);

        case GPT_CHANNEL_EXPIRED:

            return Gpt_ChannelTargetValue[Channel];

        case GPT_CHANNEL_STOPPED:

            return Gpt_ChannelElapsedValue[Channel];

        default:

            return 0UL;
    }
}


/**
 * @brief Returns remaining timer value.
 */
Gpt_ValueType Gpt_GetTimeRemaining(Gpt_ChannelType Channel)
{
    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return 0UL;
    }

    switch (Gpt_ChannelRuntime[Channel].Status)
    {
        case GPT_CHANNEL_RUNNING:

            return Gpt_lGetRunningRemaining(Channel);

        case GPT_CHANNEL_STOPPED:

            return Gpt_ChannelRemainingValue[Channel];

        case GPT_CHANNEL_EXPIRED:

            return 0UL;

        default:

            return 0UL;
    }
}


/**
 * @brief Starts a GPT timer.
 */
void Gpt_StartTimer(Gpt_ChannelType Channel,
                    Gpt_ValueType Value)
{
    const Gpt_ChannelConfigType *ChannelConfig;

    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return;
    }

    ChannelConfig =
        Gpt_lGetChannelConfig(Channel);

    if (ChannelConfig == NULL_PTR)
    {
        return;
    }

    /*
     * AUTOSAR GPT does not accept zero timer values.
     */
    if ((Value == 0UL) ||
        (Value > ChannelConfig->ChannelTickValueMax))
    {
        return;
    }

    /*
     * Do not restart an already running channel.
     */
    if (Gpt_ChannelRuntime[Channel].Status ==
        GPT_CHANNEL_RUNNING)
    {
        return;
    }

    /*
     * Disable timer before changing TVAL/TCTRL.
     */
    Gpt_lDisableHardwareTimer(Channel);

    /*
     * Clear any previous timeout indication.
     */
    Gpt_lClearInterruptFlag(Channel);

    /*
     * Reconfigure channel in case the previous operation changed it.
     */
    Gpt_lConfigureChannel(Channel);

    /*
     * LPIT counter:
     *
     *     TVAL = requested ticks - 1
     *
     * so that a requested Value represents Value timer periods.
     */
    IP_LPIT0->TMR[Channel].TVAL =
        Value - 1UL;

    Gpt_ChannelTargetValue[Channel] =
        Value;

    Gpt_ChannelElapsedValue[Channel] =
        0UL;

    Gpt_ChannelRemainingValue[Channel] =
        Value;

    Gpt_ChannelRuntime[Channel].Status =
        GPT_CHANNEL_RUNNING;

    /*
     * Enable the LPIT timer channel.
     */
    Gpt_lEnableHardwareTimer(Channel);
}


/**
 * @brief Stops a GPT timer.
 */
void Gpt_StopTimer(Gpt_ChannelType Channel)
{
    Gpt_ValueType Remaining;

    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return;
    }

    if (Gpt_ChannelRuntime[Channel].Status !=
        GPT_CHANNEL_RUNNING)
    {
        return;
    }

    /*
     * Read the remaining value before stopping the counter.
     */
    Remaining =
        Gpt_lGetRunningRemaining(Channel);

    Gpt_ChannelRemainingValue[Channel] =
        Remaining;

    Gpt_ChannelElapsedValue[Channel] =
        Gpt_ChannelTargetValue[Channel] - Remaining;

    /*
     * Stop hardware timer.
     */
    Gpt_lDisableHardwareTimer(Channel);

    /*
     * Clear pending timeout event.
     */
    Gpt_lClearInterruptFlag(Channel);

    Gpt_ChannelRuntime[Channel].Status =
        GPT_CHANNEL_STOPPED;
}


/**
 * @brief Enables notification for a GPT channel.
 */
void Gpt_EnableNotification(Gpt_ChannelType Channel)
{
    const Gpt_ChannelConfigType *ChannelConfig;
    uint32 ChannelBit;

    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return;
    }

    ChannelConfig =
        Gpt_lGetChannelConfig(Channel);

    if (ChannelConfig == NULL_PTR)
    {
        return;
    }

    /*
     * Do not enable the interrupt request if no callback is configured.
     */
    if (ChannelConfig->Notification == NULL_PTR)
    {
        return;
    }

    ChannelBit =
        Gpt_lGetChannelBit(Channel);

    Gpt_ChannelRuntime[Channel].NotificationEnabled =
        TRUE;

    /*
     * NVIC is already enabled from Gpt_Init().
     * MIER is the GPT-side interrupt request enable.
     */
    IP_LPIT0->MIER |= ChannelBit;
}


/**
 * @brief Disables notification for a GPT channel.
 */
void Gpt_DisableNotification(Gpt_ChannelType Channel)
{
    uint32 ChannelBit;

    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return;
    }

    ChannelBit =
        Gpt_lGetChannelBit(Channel);

    Gpt_ChannelRuntime[Channel].NotificationEnabled =
        FALSE;

    IP_LPIT0->MIER &=
        ~ChannelBit;
}


/* -------------------------------------------------------------------------- */
/* Internal interrupt processing                                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Process a single LPIT channel interrupt.
 *
 * This function is implementation-specific and is called from Gpt_Irq.c.
 */
void Gpt_lProcessChannelInterrupt(Gpt_ChannelType Channel)
{
    const Gpt_ChannelConfigType *ChannelConfig;
    uint32 ChannelBit;

    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return;
    }

    ChannelBit =
        Gpt_lGetChannelBit(Channel);

    /*
     * Ignore the call if the corresponding timeout flag is not asserted.
     */
    if ((IP_LPIT0->MSR & ChannelBit) == 0UL)
    {
        return;
    }

    /*
     * Clear interrupt source first to prevent repeated entry.
     */
    Gpt_lClearInterruptFlag(Channel);

    ChannelConfig =
        Gpt_lGetChannelConfig(Channel);

    if (ChannelConfig == NULL_PTR)
    {
        return;
    }

    if (ChannelConfig->ChannelMode ==
        GPT_CHANNEL_MODE_ONESHOT)
    {
        /*
         * One-shot:
         * hardware channel must stop after timeout.
         */
        Gpt_lDisableHardwareTimer(Channel);

        Gpt_ChannelElapsedValue[Channel] =
            Gpt_ChannelTargetValue[Channel];

        Gpt_ChannelRemainingValue[Channel] =
            0UL;

        Gpt_ChannelRuntime[Channel].Status =
            GPT_CHANNEL_EXPIRED;
    }
    else
    {
        /*
         * Continuous:
         * LPIT remains enabled and automatically reloads TVAL
         * after timeout because TSOI = 0.
         *
         * The timer period therefore starts again from the beginning.
         */
        Gpt_ChannelRuntime[Channel].Status =
            GPT_CHANNEL_RUNNING;
    }

    /*
     * Execute notification only when explicitly enabled.
     */
    if ((Gpt_ChannelRuntime[Channel].NotificationEnabled == TRUE) &&
        (ChannelConfig->Notification != NULL_PTR))
    {
        ChannelConfig->Notification();
    }
}


/* -------------------------------------------------------------------------- */
/* Project-specific helper                                                    */
/* -------------------------------------------------------------------------- */

/**
 * @brief Convert milliseconds to GPT timer ticks.
 *
 * Implementation extension, not part of the AUTOSAR GPT API.
 */
Gpt_ValueType Gpt_MsToTicks(Gpt_ChannelType Channel,
                            uint32 Milliseconds)
{
    const Gpt_ChannelConfigType *ChannelConfig;
    uint64 TickValue;

    if ((Gpt_DriverStatus == GPT_DRIVER_UNINIT) ||
        (!Gpt_lIsValidChannel(Channel)))
    {
        return 0UL;
    }

    ChannelConfig =
        Gpt_lGetChannelConfig(Channel);

    if (ChannelConfig == NULL_PTR)
    {
        return 0UL;
    }

    TickValue =
        ((uint64)ChannelConfig->ChannelTickFrequency *
         (uint64)Milliseconds) / 1000ULL;

    if (TickValue > (uint64)ChannelConfig->ChannelTickValueMax)
    {
        return 0UL;
    }

    return (Gpt_ValueType)TickValue;
}