#include "Mcu_Cfg.h"
#include "Mcu_Types.h"


#define MCU_SOSC_FREQUENCY_HZ  (8000000U) /* 8MHz */

const Mcu_ClockSettingConfigType Mcu_ClockConfigurations[] = 
{
    /*CLock config ID0*/
    {
        .SystemClockSource = MCU_SYSCLK_FIRC,
        .BusDivider        = MCU_CLOCK_BUS_DIV2,
        .CoreDivider       = MCU_CLOCK_CORE_DIV1,
        .SlowDivider       = MCU_CLOCK_SLOW_DIV4,

        .SpllClockConfig   =
        {
            .SpllClockSource = MCU_SPLL_SOURCE_SOSC,
            .PreDivider      = MCU_SPLL_PREDIV2,
            .Multiplier      = MCU_SPLL_MULT16
        },
        
        .SoscClockConfig = 
        {
            .ReferenceType = MCU_SOSC_CRYSTAL_OSC,
            .FrequencyHz   = MCU_SOSC_FREQUENCY_HZ
        }
    }
    ,
    /*Clock config ID1*/
    {
        .SystemClockSource = MCU_SYSCLK_SPLL,
        .BusDivider        = MCU_CLOCK_BUS_DIV2,
        .CoreDivider       = MCU_CLOCK_CORE_DIV1,
        .SlowDivider       = MCU_CLOCK_SLOW_DIV2,
        .SpllClockConfig   =
        {
            .SpllClockSource = MCU_SPLL_SOURCE_SOSC,
            .PreDivider      = MCU_SPLL_PREDIV2,
            .Multiplier      = MCU_SPLL_MULT16
        }
        ,
        .SoscClockConfig =
        {
            .ReferenceType = MCU_SOSC_CRYSTAL_OSC,
            .FrequencyHz   = MCU_SOSC_FREQUENCY_HZ
        }
    }
};

/*This variable will be exported to MCU.c through Mcu_Cfg.h or maybe declared extern in Mcu.c directly*/
const Mcu_ConfigType ex_McuClockConfiguration =
{
    .ClockConfigPtr = Mcu_ClockConfigurations,
    .ClockConfigCount = 2U
};

