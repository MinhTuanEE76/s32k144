#ifndef MCU_H__
#define MCU_H__

#include <stdbool.h>
#include "Mcu_Types.h"


void Mcu_Init(const Mcu_ConfigType *ConfigPtr);

Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting);
Std_ReturnType Mcu_DistributePllClock(void);
Mcu_PllStatusType Mcu_GetPllStatus(void);

#endif