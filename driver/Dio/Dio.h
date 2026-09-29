#ifndef DIO_H
#define DIO_H

#include "Dio_Types.h"
#include "Dio_Cfg.h"

/* Module identification */
#define DIO_VENDOR_ID           0x0000U
#define DIO_MODULE_ID           120U
#define DIO_SW_MAJOR_VERSION    1U
#define DIO_SW_MINOR_VERSION    0U
#define DIO_SW_PATCH_VERSION    0U

/* Note: AUTOSAR Dio has no init function. Pins must be configured by Port_Init() first. */

Dio_LevelType     Dio_ReadChannel(Dio_ChannelType ChannelId);
void              Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level);

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId);
void              Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level);

Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr);
void              Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level);

#if (DIO_VERSION_INFO_API == 1)
void              Dio_GetVersionInfo(Std_VersionInfoType* VersionInfo);
#endif

#if (DIO_FLIP_CHANNEL_API == 1)
Dio_LevelType     Dio_FlipChannel(Dio_ChannelType ChannelId);
#endif

#if (DIO_MASKED_WRITE_PORT_API == 1)
void              Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask);
#endif

#endif /* DIO_H */