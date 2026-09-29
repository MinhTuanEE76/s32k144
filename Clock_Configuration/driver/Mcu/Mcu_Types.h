#ifndef __MCUTYPES_H__
#define __MCUTYPES_H__

#include "Std_Types.h"

/*--------------------------------------------------------------------------------------- */
/*----------------------Type Definition by AUTOSAR----------------------------------------*/
/*--------------------------------------------------------------------------------------- */
/**/
typedef uint8 Mcu_ClockType;

/**
 * @brief: This is a status value returned by the function Mcu_GetPllStatus of the MCU module
 */
typedef enum
{
    MCU_PLL_LOCKED = 0x00U,
    MCU_PLL_UNLOCKED,
    MCU_PLL_STATUS_UNDEFINED

} Mcu_PllStatusType;

/**
 * @brief: This is the type of the reset enumerator containing the subset of reset types. It is not
 *         required that all reset types are supported by hardware.
 */
typedef enum
{
    MCU_POWER_ON_RESET = 0x00U,
    MCU_WATCHDOG_RESET,
    MCU_SW_RESET,
    MCU_RESET_UNDEFINED
} Mcu_ResetType;

/**/
typedef uint8 Mcu_RawResetType;

/**/
typedef uint8 Mcu_ModeType;

/**/
typedef uint8 Mcu_RamSectionType;

typedef enum
{
    MCU_RAMSTATE_INVALID = 0x00U,
    MCU_RAMSTATE_VALID
} Mcu_RamStateType;

/*--------------------------------------------------------------------------------------- */
/*----------------------Type Definition by user-------------------------------------------*/
/*--------------------------------------------------------------------------------------- */

typedef enum
{
    MCU_NORMAL_RUN_MODE = 0U,
    MCU_VERY_LOW_POWER_RUN_MODE,
    MCU_HIGH_SPEED_RUN_MODE,
    MCU_INVALID_MODE
} Mcu_PowerModeType;

/**
 * 
 */
typedef enum
{
    MCU_SYSCLK_SIRC = 1U,
    MCU_SYSCLK_FIRC = 2U,
    MCU_SYSCLK_SOSC = 3U,
    MCU_SYSCLK_SPLL = 6U
} Mcu_SystemClockSourceType;

/**
 * 
 */
typedef enum
{
    MCU_SPLL_SOURCE_SOSC,
    MCU_SPLL_SOURCE_FIRC
} Mcu_SpllClockSourceType;

/**
 * 
 */
typedef enum
{
    MCU_SOSC_EXTERNAL_CLOCK = 0x00U,
    MCU_SOSC_CRYSTAL_OSC
} Mcu_SoscExternalReferenceType;

/**
 * 
 */
typedef struct
{
    Mcu_SpllClockSourceType SpllClockSource;
    uint8 PreDivider;
    uint8 Multiplier;
} Mcu_SpllConfigType;

/**
 * 
 */
typedef struct
{
    Mcu_SoscExternalReferenceType ReferenceType;
    uint32 FrequencyHz;
} Mcu_SoscConfigType;

/**
 * @brief: Mcu_ClockSettingConfigType
 */
typedef struct
{
    Mcu_SystemClockSourceType SystemClockSource;
    Mcu_SpllConfigType        SpllClockConfig; //this field only used when SystemClockSource = MCU_SYSCLK_SPLL
    Mcu_SoscConfigType        SoscClockConfig;

    uint8 CoreDivider;
    uint8 BusDivider;
    uint8 SlowDivider;
} Mcu_ClockSettingConfigType;

/**
 * @brief: A structure to hold the MCU driver configuration.
 */
typedef struct
{
    const Mcu_ClockSettingConfigType *ClockConfigPtr;
    Mcu_ClockType ClockConfigCount;
} Mcu_ConfigType;


#endif