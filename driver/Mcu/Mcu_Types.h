#ifndef __MCUTYPES_H__
#define __MCUTYPES_H__

#include "Std_Types.h"


/* -------------------------------------------------------------------------- */
/* Type Definition by AUTOSAR                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief MCU clock configuration index type.
 */
typedef uint8 Mcu_ClockType;


/**
 * @brief Status value returned by Mcu_GetPllStatus().
 */
typedef enum
{
    MCU_PLL_LOCKED = 0x00U,
    MCU_PLL_UNLOCKED,
    MCU_PLL_STATUS_UNDEFINED

} Mcu_PllStatusType;


/**
 * @brief Reset type supported by the MCU module.
 *
 * It is not required that all reset types are supported by hardware.
 */
typedef enum
{
    MCU_POWER_ON_RESET = 0x00U,
    MCU_WATCHDOG_RESET,
    MCU_SW_RESET,
    MCU_RESET_UNDEFINED

} Mcu_ResetType;


/**
 * @brief Raw reset value.
 */
typedef uint8 Mcu_RawResetType;


/**
 * @brief MCU mode type.
 */
typedef uint8 Mcu_ModeType;


/**
 * @brief RAM section type.
 */
typedef uint8 Mcu_RamSectionType;


/**
 * @brief RAM state.
 */
typedef enum
{
    MCU_RAMSTATE_INVALID = 0x00U,
    MCU_RAMSTATE_VALID

} Mcu_RamStateType;


/* -------------------------------------------------------------------------- */
/* Type Definition by User                                                    */
/* -------------------------------------------------------------------------- */

/**
 * @brief System clock source selection.
 */
typedef enum
{
    MCU_SYSCLK_SIRC = 1U,
    MCU_SYSCLK_FIRC = 2U,
    MCU_SYSCLK_SOSC = 3U,
    MCU_SYSCLK_SPLL = 6U

} Mcu_SystemClockSourceType;


/**
 * @brief SPLL clock source selection.
 */
typedef enum
{
    MCU_SPLL_SOURCE_SOSC,
    MCU_SPLL_SOURCE_FIRC

} Mcu_SpllClockSourceType;


/**
 * @brief SOSC reference type.
 */
typedef enum
{
    MCU_SOSC_EXTERNAL_CLOCK = 0x00U,
    MCU_SOSC_CRYSTAL_OSC

} Mcu_SoscExternalReferenceType;


/**
 * @brief SPLL clock configuration.
 */
typedef struct
{
    Mcu_SpllClockSourceType SpllClockSource;
    uint8                   PreDivider;
    uint8                   Multiplier;
} Mcu_SpllConfigType;


/**
 * @brief SOSC clock configuration.
 */
typedef struct
{
    Mcu_SoscExternalReferenceType ReferenceType;
    uint32                        FrequencyHz;
    uint32                        StartupTimeout;

} Mcu_SoscConfigType;


/**
 * @brief MCU clock setting configuration.
 *
 * SpllClockConfig is only used when SystemClockSource is
 * set to MCU_SYSCLK_SPLL.
 */
typedef struct
{
    Mcu_SystemClockSourceType SystemClockSource;
    Mcu_SpllConfigType        SpllClockConfig;
    Mcu_SoscConfigType        SoscClockConfig;

    uint8 CoreDivider;
    uint8 BusDivider;
    uint8 SlowDivider;

} Mcu_ClockSettingConfigType;


/**
 * @brief Structure holding the MCU driver configuration.
 */
typedef struct
{
    const Mcu_ClockSettingConfigType *ClockConfigPtr;
    Mcu_ClockType                     ClockSettingId;
    uint8                             ClockConfigCount;

} Mcu_ConfigType;


#endif /* __MCUTYPES_H__ */