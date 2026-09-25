#include "Mcu_Cfg.h"
#include "Mcu_Types.h"


const Mcu_ClockSettingConfigType Mcu_ClockConfigurations[] =
{
    {
        .SystemClockSource = MCU_SYSCLK_SPLL,

        .BusDivider        = MCU_CLOCK_BUS_DIV2,
        .CoreDivider       = MCU_CLOCK_CORE_DIV1,
        .SlowDivider       = MCU_CLOCK_SLOW_DIV2,

        .SpllClockConfig =
        {
            .SpllClockSource = MCU_SPLL_SOURCE_SOSC,
            .PreDivider      = MCU_SPLL_PREDIV2,
            .Multiplier      = MCU_SPLL_MULT16
        },

        .SoscClockConfig =
        {
            .ReferenceType = MCU_SOSC_CRYSTAL_OSC
        }
    },

    {
        .SystemClockSource = MCU_SYSCLK_SPLL,

        .BusDivider        = MCU_CLOCK_BUS_DIV2,
        .CoreDivider       = MCU_CLOCK_CORE_DIV1,
        .SlowDivider       = MCU_CLOCK_SLOW_DIV2,

        .SpllClockConfig =
        {
            .SpllClockSource = MCU_SPLL_SOURCE_SOSC,
            .PreDivider      = MCU_SPLL_PREDIV2,
            .Multiplier      = MCU_SPLL_MULT16
        }
    }
};