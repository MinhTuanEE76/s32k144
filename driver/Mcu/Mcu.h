#ifndef __CLOCK_S32K1XX__
#define __CLOCK_S32K1XX__

#include <stdbool.h>
#include "Mcu_Types.h"

void Mcu_Init(const Mcu_ConfigType *ConfigPtr);

Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting);

Std_ReturnType Mcu_DistributePllClock(void);

Mcu_PllStatusType Mcu_GetPllStatus(void);



void Mcu_ExGenerateClockout(void);
Std_ReturnType Mcu_SetPowerMode(Mcu_PowerModeType Transition);

#endif