#ifndef PORT_CFG_H
#define PORT_CFG_H

#include "Port_Types.h"

/* Pre-compile switches (AUTOSAR PortGeneral) */
#define PORT_VERSION_INFO_API           1   /* 1: enable Port_GetVersionInfo()  */
#define PORT_SET_PIN_DIRECTION_API      1   /* 1: enable Port_SetPinDirection() */
#define PORT_SET_PIN_MODE_API           1   /* 1: enable Port_SetPinMode()      */

extern const Port_ConfigType Port_ConfigTable;

#endif /* PORT_CFG_H */