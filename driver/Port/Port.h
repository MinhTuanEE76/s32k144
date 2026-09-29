#ifndef PORT_H
#define PORT_H

#include "Port_Types.h"
#include "Port_Cfg.h"

/* Module identification */
#define PORT_VENDOR_ID          0x0000U
#define PORT_MODULE_ID          124U
#define PORT_SW_MAJOR_VERSION   1U
#define PORT_SW_MINOR_VERSION   0U
#define PORT_SW_PATCH_VERSION   0U

/** Initializes the Port driver with the given configuration. */
void Port_Init(const Port_ConfigType* ConfigPtr);

#if (PORT_SET_PIN_DIRECTION_API == 1)
/** Sets the direction of a pin at runtime (only if PinDirectionChangeable == TRUE). */
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);
#endif

/** Re-writes the configured direction of all pins whose direction is NOT changeable. */
void Port_RefreshPortDirection(void);

#if (PORT_VERSION_INFO_API == 1)
/** Returns the version information of this module. */
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo);
#endif

#if (PORT_SET_PIN_MODE_API == 1)
/** Sets the mode (PCR.MUX) of a pin at runtime (only if PinModeChangeable == TRUE). */
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);
#endif

#endif /* PORT_H */