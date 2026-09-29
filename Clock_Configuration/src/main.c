#include "S32K144.h"
#include <stdint.h>
#include "Mcu.h"
#include "Mcu_Cfg.h"


int main(void)
{
    Mcu_Init(&ex_McuClockConfiguration);
    Mcu_InitClock(MCU_CLOCK_SETTING_ID0);
    Mcu_DistributePllClock();

    Mcu_SetPowerMode(MCU_HIGH_SPEED_RUN_MODE);
    Mcu_ExGenerateClockout();
    while(true);
    return -1;
}

