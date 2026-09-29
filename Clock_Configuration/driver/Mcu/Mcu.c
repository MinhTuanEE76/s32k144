#include "Mcu.h"
#include "Mcu_Types.h"
#include "Mcu_Private.h"
#include "Mcu_Cfg.h"

#include "S32K144.h"

/*Marco for enabling SOSC */
#define MCU_ENABLE_SOSC()                                                        \
    do                                                                           \
    {                                                                            \
        IP_SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;                                \
        IP_SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN(1U);                               \
        while (!(IP_SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK))                  \
        {                                                                        \
            /* Wait until SOSC is valid */                                      \
        }                                                                        \
        IP_SCG->SOSCCSR |= SCG_SOSCCSR_LK_MASK;                                 \
    } while (0)

/*Macro for disabling SOSC */
#define MCU_DISABLE_SOSC()                                                       \
    do                                                                           \
    {                                                                            \
        IP_SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;                                \
        IP_SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;                            \
        IP_SCG->SOSCCSR |= SCG_SOSCCSR_LK_MASK;                                 \
    } while (0)


/*Marco for enabling FIRC */
#define MCU_ENABLE_FIRC()                                                        \
    do                                                                           \
    {                                                                            \
        IP_SCG->FIRCCSR &= ~SCG_FIRCCSR_LK_MASK;                                \
        IP_SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(1U);                              \
        while (!(IP_SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK))                  \
        {                                                                        \
            /* Wait until FIRC is valid */                                      \
        }                                                                        \
        IP_SCG->FIRCCSR |= SCG_FIRCCSR_LK_MASK;                                  \
    } while (0)

/*Marco for disabling FIRC */
#define MCU_DISABLE_FIRC()                                                       \
    do                                                                           \
    {                                                                            \
        IP_SCG->FIRCCSR &= ~SCG_FIRCCSR_LK_MASK;                                \
        IP_SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;                            \
        IP_SCG->FIRCCSR |= SCG_FIRCCSR_LK_MASK;                                 \
    } while (0)

/*Marco for enabling SIRC */
#define MCU_ENABLE_SIRC()                                                        \
    do                                                                           \
    {                                                                            \
        IP_SCG->SIRCCSR &= ~SCG_SIRCCSR_LK_MASK;                                \
        IP_SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN(1U);                              \
        while (!(IP_SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK))                  \
        {                                                                        \
            /* Wait until SIRC is valid */                                      \
        }                                                                        \
        IP_SCG->SIRCCSR |= SCG_SIRCCSR_LK_MASK;                                  \
    } while (0)

/*Marco for disabling SIRC */
#define MCU_DISABLE_SIRC()                                                       \
    do                                                                           \
    {                                                                            \
        IP_SCG->SIRCCSR &= ~SCG_SIRCCSR_LK_MASK;                                \
        IP_SCG->SIRCCSR &= ~SCG_SIRCCSR_SIRCEN_MASK;                            \
        IP_SCG->SIRCCSR |= SCG_SIRCCSR_LK_MASK;                                 \
    } while (0)

/*Marco for enabling SPLL */
#define MCU_ENABLE_SPLL()                                                        \
    do                                                                           \
    {                                                                            \
        IP_SCG->SPLLCSR &= ~SCG_SPLLCSR_LK_MASK;                                \
        IP_SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN(1U);                              \
        while (!(IP_SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK))                  \
        {                                                                        \
            /* Wait until SPLL is valid */                                      \
        }                                                                        \
        IP_SCG->SPLLCSR |= SCG_SPLLCSR_LK_MASK;                                 \
    } while (0)

/*Marco for disabling SPLL */
#define MCU_DISABLE_SPLL()                                                       \
    do                                                                           \
    {                                                                            \
        IP_SCG->SPLLCSR &= ~SCG_SPLLCSR_LK_MASK;                                \
        IP_SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;                            \
        IP_SCG->SPLLCSR |= SCG_SPLLCSR_LK_MASK;                                 \
    } while (0)



static const Mcu_ConfigType *Mcu_ConfigPtr = NULL_PTR;

Mcu_StatusType Mcu_Status = MCU_UNINIT;
Mcu_PowerModeType Mcu_PowerModeStatus = MCU_NORMAL_RUN_MODE; //S32K144 default run FIRC

Mcu_SystemClockSourceType Mcu_PreviousRunClockSource;


/* -------------------------------------------------------------------------- */
/* Static Function Helpers                                                   */
/* -------------------------------------------------------------------------- */


/**
 * @param SoscConfig
 * @note  Configure RANGE and select source for SOSC block (RANG & EREFS) not Enbale SOSC in this function
 */
static inline Std_ReturnType Mcu_lConfigureSoscClock(const Mcu_SoscConfigType *SoscConfig)
{
    if((SoscConfig == NULL_PTR) || (Mcu_Status == MCU_UNINIT))
    {
        return E_NOT_OK;
    }

    if(SoscConfig->ReferenceType == MCU_SOSC_CRYSTAL_OSC)
    {
        // Set the SOSC range and reference type for crystal oscillator
        IP_SCG->SOSCCFG |= SCG_SOSCCFG_RANGE(2U);
        IP_SCG->SOSCCFG |= SCG_SOSCCFG_EREFS(1U);
    }
    else
    {
        // Set the SOSC range and reference type for external clock
        IP_SCG->SOSCCFG &= ~SCG_SOSCCFG_RANGE_MASK;
        IP_SCG->SOSCCFG &= ~SCG_SOSCCFG_EREFS_MASK;
    }

    return E_OK;
}

/**
 * @brief Configure SPLL clock.
 *
 * @param[in] SysclkConfig Pointer to sysclk configuration.
 * @note Define Spll clock will be using SOSC or FIRC. Configure SpllSource, PreDivider and Multiplier.
 * @return E_OK     Configuration successful.
 * @return E_NOT_OK Configuration failed.
 */
static inline Std_ReturnType Mcu_lConfigureSpllClock(const Mcu_ClockSettingConfigType *SysclkConfig)
{
    /* Defensive check */
    if((SysclkConfig == NULL_PTR) || (Mcu_Status == MCU_UNINIT))
    {
        return E_NOT_OK;
    }

    /*Configure SPLL clock source */
    if (SysclkConfig->SpllClockConfig.SpllClockSource == MCU_SPLL_SOURCE_SOSC)
    {
        if(Mcu_lConfigureSoscClock(&SysclkConfig->SoscClockConfig) != E_OK)
        {
            return E_NOT_OK;
        }

        IP_SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;                                
        IP_SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN(1U);                               
        while (!(IP_SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK))                  
        {                                                                        
            /* Wait until SOSC is valid */                                      
        }                                                                        
        IP_SCG->SOSCCSR |= SCG_SOSCCSR_LK_MASK;
    }
    else if(SysclkConfig->SpllClockConfig.SpllClockSource == MCU_SPLL_SOURCE_FIRC)
    {
        /*Enable FIRC*/
        MCU_ENABLE_FIRC();
    }
    else
    {
        //not SOSC and FIRC -> not valid
        return E_NOT_OK;
    }

    /*Prediv SPLL Clock In*/
    IP_SCG->SPLLCFG &= ~SCG_SPLLCFG_PREDIV_MASK;
    IP_SCG->SPLLCFG |= (SysclkConfig->SpllClockConfig.PreDivider << SCG_SPLLCFG_PREDIV_SHIFT);

    /*Multiplier After Prediv*/
    IP_SCG->SPLLCFG &= ~SCG_SPLLCFG_MULT_MASK;
    IP_SCG->SPLLCFG |= (SysclkConfig->SpllClockConfig.Multiplier << SCG_SPLLCFG_MULT_SHIFT);

    /*Select SPLL Clock Source*/
    IP_SCG->SPLLCFG |= SysclkConfig->SpllClockConfig.SpllClockSource;

    return E_OK;
}



/**
 * @brief: Enable SIRC as a system clock source.
 */
static inline Std_ReturnType Mcu_lInitSystemClock_SIRC(const Mcu_ClockSettingConfigType *SysclkConfig)
{
    if(SysclkConfig == NULL_PTR)
    {
        return E_NOT_OK;
    }

    /* Enable SIRC */
    MCU_ENABLE_SIRC();

    /* Configure dividers */
    uint32 RegValue = 0U;

    RegValue |= SCG_RCCR_SCS(SysclkConfig->SystemClockSource);
    RegValue |= SCG_RCCR_DIVCORE(SysclkConfig->CoreDivider);
    RegValue |= SCG_RCCR_DIVBUS(SysclkConfig->BusDivider);
    RegValue |= SCG_RCCR_DIVSLOW(SysclkConfig->SlowDivider);

    IP_SCG->RCCR = RegValue;

    /* Wait until SIRC is selected as the system clock source */
    while((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(MCU_SYSCLK_SIRC))
    {
        // do nothing wait SIRC selected
    }

    return E_OK;
}

/**
 * @brief: Enable FIRC as a system clock source.
 */
static inline Std_ReturnType Mcu_lInitSystemClock_FIRC(const Mcu_ClockSettingConfigType *SysclkConfig)
{
    if(SysclkConfig == NULL_PTR)
    {
        return E_NOT_OK;
    }

    /* Enable FIRC */
    MCU_ENABLE_FIRC();

    while ((IP_SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) == 0U)
    {
        /* Wait until FIRC is valid */
    }

    /* Lock configuration */
    IP_SCG->FIRCCSR |= (SCG_FIRCCSR_LK_MASK << SCG_FIRCCSR_LK_SHIFT);

    uint32 RegValue = 0U;

    RegValue |= SCG_RCCR_SCS(MCU_SYSCLK_FIRC);
    RegValue |= SCG_RCCR_DIVCORE(SysclkConfig->CoreDivider);
    RegValue |= SCG_RCCR_DIVBUS(SysclkConfig->BusDivider);
    RegValue |= SCG_RCCR_DIVSLOW(SysclkConfig->SlowDivider);

    IP_SCG->RCCR = RegValue;

    /* Wait until FIRC is selected as the system clock source */
    while((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(MCU_SYSCLK_FIRC))
    {
        // do nothing wait FIRC selected
    }

    return E_OK;
}

/**
 * @brief: Enable SOSC
 */
static inline Std_ReturnType Mcu_lInitSystemClock_SOSC(const Mcu_ClockSettingConfigType *SysclkConfig)
{
    if(SysclkConfig == NULL_PTR)
    {
        return E_NOT_OK;
    }

    /*Configure SOSC*/
    if(Mcu_lConfigureSoscClock(&SysclkConfig->SoscClockConfig) != E_OK)
    {
        return E_NOT_OK;
    }

    /*Enable SOSC*/
    MCU_ENABLE_SOSC();

    /* Configure dividers */
    uint32 RegValue = 0U;

    RegValue |= SCG_RCCR_SCS(SysclkConfig->SystemClockSource);
    RegValue |= SCG_RCCR_DIVCORE(SysclkConfig->CoreDivider);
    RegValue |= SCG_RCCR_DIVBUS(SysclkConfig->BusDivider);
    RegValue |= SCG_RCCR_DIVSLOW(SysclkConfig->SlowDivider);

    IP_SCG->RCCR = RegValue;

    /* Wait until SOSC is selected as the system clock source */
    while((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(MCU_SYSCLK_SOSC))
    {
        // do nothing wait SOSC selected
    }

    return E_OK;
}

/**
 * @brief: Set-up parameters for SPLL clock but not yet switch SPLL as a system clock source
 */
static inline Std_ReturnType Mcu_lInitSystemClock_SPLL(const Mcu_ClockSettingConfigType *SysclkConfig)
{
    if(SysclkConfig == NULL_PTR)
    {
        return E_NOT_OK;
    }

    /*Configure SPLL*/
    if(Mcu_lConfigureSpllClock(SysclkConfig) != E_OK)
    {
        return E_NOT_OK;
    }

    /*Enable SPLL*/
    MCU_ENABLE_SPLL();

    /*Configure Dividers but not yet switch SPLL as a system clock source
     *We will switch SPLL as a system clock source in Mcu_DistributePllClock()
      function after SPLL is locked
     */
    uint32 RegValue = 0U;
    
    RegValue |= SCG_RCCR_DIVCORE(SysclkConfig->CoreDivider);
    RegValue |= SCG_RCCR_DIVBUS(SysclkConfig->BusDivider);
    RegValue |= SCG_RCCR_DIVSLOW(SysclkConfig->SlowDivider);

    IP_SCG->RCCR = RegValue;
    
    return E_OK;
}

/**
 * 
 */
static inline void Mcu_lConfigureVeryLowPowerRunClock(void)
{
    uint32 RegValue = 0U;

    RegValue |= SCG_VCCR_DIVCORE(MCU_CLOCK_CORE_DIV1);
    RegValue |= SCG_VCCR_DIVBUS(MCU_CLOCK_BUS_DIV2);
    RegValue |= SCG_VCCR_DIVSLOW(MCU_CLOCK_SLOW_DIV4);
    RegValue |= SCG_VCCR_SCS(MCU_SYSCLK_SIRC);

    IP_SCG->VCCR = RegValue;
}

/**
 * 
 */
static inline Mcu_SystemClockSourceType Mcu_lGetCurrentSystemClockSource(void)
{
    return (Mcu_SystemClockSourceType)((IP_SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT);
}

/**
 * 
 */
static Std_ReturnType Mcu_lSwitchVeryLowPowerRun_To_NormalRun(void)
{
    /* Current mode must actually be VLPR */
    if ((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK)  != MCU_PMSTAT_VLPR)
    {
        return E_NOT_OK;
    }

    /* Request Normal RUN */
    IP_SMC->PMCTRL =(IP_SMC->PMCTRL & ~SMC_PMCTRL_RUNM_MASK) | SMC_PMCTRL_RUNM(0U);

    /* Wait until transition is completed */
    while((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != MCU_PMSTAT_RUN);

    /* Now MCU is really in RUN. Restore the desired RUN clock configuration here. */
    MCU_ENABLE_FIRC();
    MCU_ENABLE_SOSC();
    MCU_ENABLE_SPLL();

    IP_SCG->RCCR = (IP_SCG->RCCR & ~SCG_RCCR_SCS_MASK) | SCG_RCCR_SCS(Mcu_PreviousRunClockSource);

    /* Wait until the requested source becomes the actual SYSCLK */
    while ((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(Mcu_PreviousRunClockSource));
    
    Mcu_PowerModeStatus = MCU_NORMAL_RUN_MODE;

    return E_OK;
}


/**
 * 
 */
static Std_ReturnType Mcu_lSwitchNormalRun_To_VeryLowPowerRun(void)
{
    /* Current mode must actually be Normal RUN */
    if ((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK)  != MCU_PMSTAT_RUN)
    {
        return E_NOT_OK;
    }

    /*Setup before switching to Very Low Power RUN */
    MCU_ENABLE_SIRC();
    IP_SCG->RCCR = (IP_SCG->RCCR & ~SCG_RCCR_SCS_MASK) | SCG_RCCR_SCS(MCU_SYSCLK_SIRC);

    while((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(MCU_SYSCLK_SIRC))
    {
        // do nothing wait SIRC selected
    }

    Mcu_lConfigureVeryLowPowerRunClock();

    /* Request Very Low Power RUN */
    IP_SMC->PMCTRL =(IP_SMC->PMCTRL & ~SMC_PMCTRL_RUNM_MASK) | SMC_PMCTRL_RUNM(2U);

    /* Wait until transition is completed */
    while((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != MCU_PMSTAT_VLPR);

    Mcu_PowerModeStatus = MCU_VERY_LOW_POWER_RUN_MODE;

    return E_OK;
}

/**
 * 
 */
static inline void Mcu_lConfigureHighSpeedRunClock(void)
{
    
    /*This version uses SPLL when entering HSRUN default 
       and FIRC is a spll source clock
       FIRC = 48MHZ->SPLL Config-> 48*(MULT/PREDIV) -> 48*(28/6) = 224 MHZ -> SPLL clock = 112Mhz
       CORE_CLOCK = 112MHZ -> DivCore = 1
       BUS_CLOCK  = 56     -> DivBus  = 2
       FLASH_CLOCK = 28    -> DivSlow = 4
    */

    /*Prediv SPLL Clock In*/
    IP_SCG->SPLLCFG &= ~SCG_SPLLCFG_PREDIV_MASK;
    IP_SCG->SPLLCFG |= (MCU_SPLL_PREDIV6 << SCG_SPLLCFG_PREDIV_SHIFT);

    /*Multiplier After Prediv*/
    IP_SCG->SPLLCFG &= ~SCG_SPLLCFG_MULT_MASK;
    IP_SCG->SPLLCFG |= (MCU_SPLL_MULT28 << SCG_SPLLCFG_MULT_SHIFT);

    /*Select SPLL Clock Source*/
    IP_SCG->SPLLCFG |= 1U;   /*FIRC*/


    uint32 RegValue = 0U;

    RegValue |= SCG_HCCR_DIVCORE(MCU_CLOCK_CORE_DIV1);
    RegValue |= SCG_HCCR_DIVBUS(MCU_CLOCK_BUS_DIV2);
    RegValue |= SCG_HCCR_DIVSLOW(MCU_CLOCK_SLOW_DIV4);
    RegValue |= SCG_HCCR_SCS(MCU_SYSCLK_SPLL);

    IP_SCG->HCCR = RegValue;
}

/**
 * 
 */
static Std_ReturnType Mcu_lSwitchNormalRun_To_HighSpeedRun(void)
{
    /* Current mode must actually be Normal RUN */
    if ((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != MCU_PMSTAT_RUN)
    {
        return E_NOT_OK;
    }

    /*
     * Enable all clock sources required by the HSRUN configuration.
     * HSRUN system clock can use FIRC or SPLL.
     */
    MCU_ENABLE_FIRC();
    MCU_ENABLE_SOSC();
    MCU_ENABLE_SPLL();

    /* Configure HSRUN clock before entering HSRUN */
    Mcu_lConfigureHighSpeedRunClock();

    /* Allow transition to High Speed RUN */
    IP_SMC->PMPROT |= SMC_PMPROT_AHSRUN_MASK;

    /* Request High Speed RUN */
    IP_SMC->PMCTRL = (IP_SMC->PMCTRL & ~SMC_PMCTRL_RUNM_MASK) | SMC_PMCTRL_RUNM(3U);

    /* Wait until transition is completed */
    while ((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != MCU_PMSTAT_HSPR)
    {
    }

    /* Confirm actual HSRUN system clock */
    while ((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(MCU_SYSCLK_SPLL));


    Mcu_PowerModeStatus = MCU_HIGH_SPEED_RUN_MODE;

    return E_OK;
}


/**
 * 
 */
static Std_ReturnType Mcu_lSwitchHighSpeedPowerRun_To_NormalRun(void)
{
    /* Current mode must actually be HSRUN */
    if ((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != MCU_PMSTAT_HSPR)
    {
        return E_NOT_OK;
    }

    /*
     * Configure the Normal RUN clock profile before requesting
     * the transition to Normal RUN.
     */

    /* Wait until RUN clock source becomes active */
    while ((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(Mcu_PreviousRunClockSource));

    /* Request Normal RUN */
    IP_SMC->PMCTRL =(IP_SMC->PMCTRL & ~SMC_PMCTRL_RUNM_MASK) | SMC_PMCTRL_RUNM(0U);

    /* Wait until transition is completed */
    while ((IP_SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != MCU_PMSTAT_RUN);

    Mcu_PowerModeStatus = MCU_NORMAL_RUN_MODE;

    return E_OK;
}

/* -------------------------------------------------------------------------- */
/* MCU Driver API                                                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize MCU driver.
 *
 * @param[in] ConfigPtr Pointer to MCU configuration.
 */
void Mcu_Init(const Mcu_ConfigType *ConfigPtr)
{
    if ((ConfigPtr == NULL_PTR) || (ConfigPtr->ClockConfigPtr == NULL_PTR) || (ConfigPtr->ClockConfigCount == 0U))
    {
        return;
    }

    Mcu_ConfigPtr = ConfigPtr;
    Mcu_Status = MCU_IDLE;
}


/**
 * @brief Initialize the selected MCU clock configuration.
 *
 * @param[in] ClockSetting Clock configuration index.
 *
 * @return E_OK     Clock Init successful.
 * @return E_NOT_OK Clock Init failed.
 */
Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting)
{
    const Mcu_ClockSettingConfigType *ClockCfg = NULL_PTR;

    /* Defensive check */
    if ((Mcu_Status == MCU_UNINIT) || (Mcu_ConfigPtr == NULL_PTR) || (ClockSetting >= Mcu_ConfigPtr->ClockConfigCount))
    {
        return E_NOT_OK;
    }
    
    /* Get corresponding clock configuration */
    ClockCfg = &Mcu_ConfigPtr->ClockConfigPtr[ClockSetting];

    /* Init system clock source */
    switch(ClockCfg->SystemClockSource)
    {
        case MCU_SYSCLK_SIRC:
            if(Mcu_lInitSystemClock_SIRC(ClockCfg) != E_OK)
            {
                return E_NOT_OK;
            }
            
            return E_OK;

        case MCU_SYSCLK_FIRC:
            if(Mcu_lInitSystemClock_FIRC(ClockCfg) != E_OK)
            {
                return E_NOT_OK;
            }
            
            return E_OK;

        case MCU_SYSCLK_SOSC:
            if(Mcu_lInitSystemClock_SOSC(ClockCfg) != E_OK)
            {
                return E_NOT_OK;
            }
            
            return E_OK;

        case MCU_SYSCLK_SPLL:
            if(Mcu_lInitSystemClock_SPLL(ClockCfg) != E_OK)
            {
                return E_NOT_OK;
            }

            return E_OK;
        
        default:
            return E_NOT_OK;
            break;
    }
}

/**
 * 
 */
Mcu_PllStatusType Mcu_GetPllStatus(void)
{
    if((IP_SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) && (IP_SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK))
    {
        return MCU_PLL_LOCKED;
    }

    return MCU_PLL_UNLOCKED;
}

/**
 * @brief Distribute the PLL clock to the system.
 *
 * @return E_OK     Distribution successful.
 * @return E_NOT_OK Distribution failed.
 */
Std_ReturnType Mcu_DistributePllClock(void)
{
    /* Defensive check */
    if (Mcu_Status == MCU_UNINIT)
    {
        return E_NOT_OK;
    }

    if(Mcu_GetPllStatus() !=  MCU_PLL_LOCKED)
    {
        return E_NOT_OK;
    }

    /*switch system clock source to SPLL*/
    IP_SCG->RCCR = (IP_SCG->RCCR & ~SCG_RCCR_SCS_MASK) |
               	   ((MCU_SYSCLK_SPLL << SCG_RCCR_SCS_SHIFT) & SCG_RCCR_SCS_MASK);


    /*wait SPLL switch actualy*/
    while((IP_SCG->CSR & SCG_CSR_SCS_MASK) != SCG_CSR_SCS(MCU_SYSCLK_SPLL));

    return E_OK;
}

/**
 * 
 */
Std_ReturnType Mcu_SetPowerMode(Mcu_PowerModeType Transition)
{
    /*Validate Transition parameter*/
    if(Transition >= MCU_INVALID_MODE)
    {
        return E_NOT_OK;
    }

    Mcu_PreviousRunClockSource = Mcu_lGetCurrentSystemClockSource();

    /*POWER MODE S32K14X FSM*/
    switch(Transition)
    {
        case MCU_NORMAL_RUN_MODE:
        {
            if(Mcu_PowerModeStatus == MCU_NORMAL_RUN_MODE)   // Normal to normal->no action needed
            {
                return E_OK;
            }
            else if(Mcu_PowerModeStatus == MCU_VERY_LOW_POWER_RUN_MODE) // VLPR->Normal Run
            {
                if(Mcu_lSwitchVeryLowPowerRun_To_NormalRun() != E_OK)
                {
                    return E_NOT_OK;
                }
                /*Transition success*/
                Mcu_PowerModeStatus = Transition;
                return E_OK;
            }
            else if(Mcu_PowerModeStatus == MCU_HIGH_SPEED_RUN_MODE) // HSPR -> Normal Run
            {
                if(Mcu_lSwitchHighSpeedPowerRun_To_NormalRun() != E_OK)
                {
                    return E_NOT_OK;
                }
                /*Transition success*/
                Mcu_PowerModeStatus = Transition;
                return E_OK;
            }
            else
            {
                //At here variable Mcu_PowerModeStatus isn't expected
                return E_NOT_OK;
            }
           
        }/*end case MCU_NORMAL_RUN_MODE*/

        case MCU_VERY_LOW_POWER_RUN_MODE:
        {
            if(Mcu_PowerModeStatus == MCU_NORMAL_RUN_MODE)   // Normal Run -> VLPR
            {
                if(Mcu_lSwitchNormalRun_To_VeryLowPowerRun() != E_OK)
                {
                    return E_NOT_OK;
                }
                /*Transition success*/
                Mcu_PowerModeStatus = Transition;
                return E_OK;
            }
            else if(Mcu_PowerModeStatus == MCU_VERY_LOW_POWER_RUN_MODE) // VLPR->VLPR
            {
                /*Ignore*/
                return E_OK;
            }
            else if(Mcu_PowerModeStatus == MCU_HIGH_SPEED_RUN_MODE) // HSPR -> VLPR
            {
                /*According RM can't switch from HSPR to VLPR directly*/
                return E_NOT_OK;
            }
            else
            {
                //At here variable Mcu_PowerModeStatus isn't expected
                return E_NOT_OK;
            }
           
        }/*end case MCU_VERY_LOW_POWER_RUN_MODE*/

        case MCU_HIGH_SPEED_RUN_MODE:
        {
            if(Mcu_PowerModeStatus == MCU_NORMAL_RUN_MODE)   // Normal Run -> HSPR
            {
                if(Mcu_lSwitchNormalRun_To_HighSpeedRun() != E_OK)
                {
                    return E_NOT_OK;
                }
                /*Transition success*/
                Mcu_PowerModeStatus = Transition;
                return E_OK;
            }
            else if(Mcu_PowerModeStatus == MCU_VERY_LOW_POWER_RUN_MODE) // VLPR->HSPR
            {
                return E_NOT_OK;
            }
            else if(Mcu_PowerModeStatus == MCU_HIGH_SPEED_RUN_MODE) // HSPR -> HSPR
            {
                /*Ignored*/
                return E_OK;
            }
            else
            {
                //At here variable Mcu_PowerModeStatus isn't expected
                return E_NOT_OK;
            }
           
        } /*end case MCU_HIGH_SPEED_RUN_MODE*/

        default:
            return E_NOT_OK;
    }

}

/**
 * @brief : SCG_OUT is SPLL clock
 * @brief : SIM_CHIPCTL_CLKOUTSEL is SCG_OUT
 * @brief : SIM_CHIPCTL_CLKOUTDIV divide 8 (0b111)
 */
void Mcu_ExGenerateClockout(void)
{
    /* Enable clock for PORTD */
    IP_PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

    /* PTD14 -> CLKOUT (ALT7) */
    IP_PORTD->PCR[14] = (IP_PORTD->PCR[14] & ~PORT_PCR_MUX_MASK) |PORT_PCR_MUX(7U);

    /* Select SPLL clock for SCG_CLKOUT */
    IP_SCG->CLKOUTCNFG = (IP_SCG->CLKOUTCNFG & ~SCG_CLKOUTCNFG_CLKOUTSEL_MASK) | SCG_CLKOUTCNFG_CLKOUTSEL(1U);

    /* Select SCG_OUT as SIM CLKOUT source */
    IP_SIM->CHIPCTL = (IP_SIM->CHIPCTL & ~SIM_CHIPCTL_CLKOUTSEL_MASK) | SIM_CHIPCTL_CLKOUTSEL(9U);

    /* Divide by 8 */
    IP_SIM->CHIPCTL = (IP_SIM->CHIPCTL & ~SIM_CHIPCTL_CLKOUTDIV_MASK) | SIM_CHIPCTL_CLKOUTDIV(6U);

    /* Enable CLKOUT */
    IP_SIM->CHIPCTL |= SIM_CHIPCTL_CLKOUTEN_MASK;
}
