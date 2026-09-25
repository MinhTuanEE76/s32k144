#include "Mcu.h"
#include "Mcu_Types.h"
#include "Mcu_Private.h"
#include "Mcu_Cfg.h"

#include "S32K144.h"

static const Mcu_ConfigType *Mcu_ConfigPtr = NULL_PTR;

Mcu_StatusType Mcu_Status = MCU_UNINIT;
Mcu_PllStatusType Pll_Status = MCU_PLL_STATUS_UNDEFINED;

/* -------------------------------------------------------------------------- */
/* Static Function Helpers                                                   */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure SPLL clock.
 *
 * @param[in] SpllConfig Pointer to SPLL configuration.
 * @note configure SpllSource, PreDivider and Multiplier.
 * @return E_OK     Configuration successful.
 * @return E_NOT_OK Configuration failed.
 */
static inline Std_ReturnType Mcu_lConfigureSpllClock(Mcu_SpllConfigType *SpllConfig)
{
    /* Defensive check */
    if((SpllConfig == NULL_PTR) || (Mcu_Status == MCU_UNINIT))
    {
        return E_NOT_OK;
    }

    IP_SCG->SPLLCFG |= SpllConfig->SpllClockSource;

    IP_SCG->SPLLCFG &= ~SCG_SPLLCFG_PREDIV_MASK;
    IP_SCG->SPLLCFG |= (SpllConfig->PreDivider << SCG_SPLLCFG_PREDIV_SHIFT);

    IP_SCG->SPLLCFG &= ~SCG_SPLLCFG_MULT_MASK;
    IP_SCG->SPLLCFG |= (SpllConfig->Multiplier << SCG_SPLLCFG_MULT_SHIFT);

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
    IP_SCG->SIRCCSR &= ~SCG_SIRCCSR_LK_MASK;
    IP_SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN(1U);

    while (!(IP_SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK))
    {
        // do nothing wait SIRC valid
    }
    /* Lock configuration */
    IP_SCG->SIRCCSR |= (SCG_SIRCCSR_LK_MASK << SCG_SIRCCSR_LK_SHIFT);

    /* Configure dividers */
    IP_SCG->RCCR &= ~SCG_RCCR_DIVCORE_MASK;
    IP_SCG->RCCR |= (SysclkConfig->CoreDivider << SCG_RCCR_DIVCORE_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVBUS_MASK;
    IP_SCG->RCCR |= (SysclkConfig->BusDivider << SCG_RCCR_DIVBUS_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVSLOW_MASK;
    IP_SCG->RCCR |= (SysclkConfig->SlowDivider << SCG_RCCR_DIVSLOW_SHIFT);

    /* Switch system clock to SIRC*/
    IP_SCG->RCCR &= ~SCG_RCCR_SCS_MASK;
    IP_SCG->RCCR |= (MCU_SYSCLK_SIRC << SCG_RCCR_SCS_SHIFT);

    /* Wait until SIRC is selected as the system clock source */
    while(!(IP_SCG->SIRCCSR & SCG_SIRCCSR_SIRCSEL_MASK))
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
    IP_SCG->FIRCCSR &= ~SCG_FIRCCSR_LK_MASK;
    IP_SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(1U);

    while ((IP_SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) == 0U)
    {
        /* Wait until FIRC is valid */
    }

    /* Lock configuration */
    IP_SCG->FIRCCSR |= (SCG_FIRCCSR_LK_MASK << SCG_FIRCCSR_LK_SHIFT);

    /* Configure dividers */
    IP_SCG->RCCR &= ~SCG_RCCR_DIVCORE_MASK;
    IP_SCG->RCCR |= (SysclkConfig->CoreDivider << SCG_RCCR_DIVCORE_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVBUS_MASK;
    IP_SCG->RCCR |= (SysclkConfig->BusDivider << SCG_RCCR_DIVBUS_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVSLOW_MASK;
    IP_SCG->RCCR |= (SysclkConfig->SlowDivider << SCG_RCCR_DIVSLOW_SHIFT);

    /*switch FIRC to system clock*/
    IP_SCG->RCCR &= ~SCG_RCCR_SCS_MASK;
    IP_SCG->RCCR |= (MCU_SYSCLK_FIRC << SCG_RCCR_SCS_SHIFT);

    /* Wait until FIRC is selected as the system clock source */
    while(!(IP_SCG->FIRCCSR & SCG_FIRCCSR_FIRCSEL_MASK))
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
    /*Enable SOSC*/
    IP_SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;
    IP_SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN(1U);

    while(!(IP_SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK))
    {
        // do nothing wait SOSC valid
    }

    IP_SCG->SOSCCSR |= SCG_SOSCCSR_LK_MASK; // lock SOSC configuration

    /* Configure dividers */
    IP_SCG->RCCR &= ~SCG_RCCR_DIVCORE_MASK;
    IP_SCG->RCCR |= (SysclkConfig->CoreDivider << SCG_RCCR_DIVCORE_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVBUS_MASK;
    IP_SCG->RCCR |= (SysclkConfig->BusDivider << SCG_RCCR_DIVBUS_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVSLOW_MASK;
    IP_SCG->RCCR |= (SysclkConfig->SlowDivider << SCG_RCCR_DIVSLOW_SHIFT);

    /* Switch system clock to FIRC */
    IP_SCG->RCCR &= ~SCG_RCCR_SCS_MASK;
    IP_SCG->RCCR |= (SysclkConfig->SystemClockSource << SCG_RCCR_SCS_SHIFT);

    /* Wait until SOSC is selected as the system clock source */
    while(!(IP_SCG->SOSCCSR & SCG_SOSCCSR_SOSCSEL_MASK))
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
    /*Configure SPLL*/
    if(Mcu_lConfigureSpllClock(&SysclkConfig->SpllClockConfig) != E_OK)
    {
        return E_NOT_OK;
    }

    /*Enable SPLL*/
    IP_SCG->SPLLCSR &= ~SCG_SPLLCSR_LK_MASK;
    IP_SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN(1U);

    while(!(IP_SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK))
    {
        // do nothing wait SPLL valid
    }

    IP_SCG->SPLLCSR |= SCG_SPLLCSR_LK_MASK;//lock SPLL configuration

    IP_SCG->RCCR &= ~SCG_RCCR_DIVCORE_MASK;
    IP_SCG->RCCR |= (SysclkConfig->CoreDivider << SCG_RCCR_DIVCORE_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVBUS_MASK;
    IP_SCG->RCCR |= (SysclkConfig->BusDivider << SCG_RCCR_DIVBUS_SHIFT);

    IP_SCG->RCCR &= ~SCG_RCCR_DIVSLOW_MASK;
    IP_SCG->RCCR |= (SysclkConfig->SlowDivider << SCG_RCCR_DIVSLOW_SHIFT);

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
            Mcu_lInitSystemClock_SIRC(ClockCfg);
            return E_OK;
            break;

        case MCU_SYSCLK_FIRC:
            Mcu_lInitSystemClock_FIRC(ClockCfg);
            return E_OK;
            break;

        case MCU_SYSCLK_SOSC:
            Mcu_lInitSystemClock_SOSC(ClockCfg);
            return E_OK;
            break;

        case MCU_SYSCLK_SPLL:
            return Mcu_lInitSystemClock_SPLL(ClockCfg);
            break;
        
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
    IP_SCG->RCCR &= ~SCG_RCCR_SCS_MASK;
    IP_SCG->RCCR |= (MCU_SYSCLK_SPLL << SCG_RCCR_SCS_SHIFT);

    /*wait SPLL switch actualy*/
    while(!(IP_SCG->SPLLCSR & SCG_SPLLCSR_SPLLSEL_MASK));

    return E_OK;
}