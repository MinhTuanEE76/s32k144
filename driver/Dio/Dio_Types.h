#ifndef DIO_TYPES_H
#define DIO_TYPES_H

#include "Std_Types.h"

/*
 * Channel ID = (PortIndex << 5) | PinNumber   (same scheme as the Port module)
 * Usage: DIO_CHANNEL_ID(DIO_PORT_D, 15U) -> PTD15
 */
#define DIO_PORT_A                 0U
#define DIO_PORT_B                 1U
#define DIO_PORT_C                 2U
#define DIO_PORT_D                 3U
#define DIO_PORT_E                 4U

#define DIO_CHANNEL_ID(port, pin)  ((Dio_ChannelType)(((port) << 5U) | (pin)))
#define DIO_CHANNEL_GET_PORT(ch)   ((Dio_PortType)((ch) >> 5U))
#define DIO_CHANNEL_GET_PIN(ch)    ((uint8)((ch) & 0x1FU))

/** Numeric ID of a DIO channel (single pin) */
typedef uint16 Dio_ChannelType;

/** Numeric ID of a DIO port */
typedef uint8 Dio_PortType;

/** Level of a channel: STD_LOW / STD_HIGH */
typedef uint8 Dio_LevelType;

/** Level of all channels of a port (S32K GPIO ports are 32-bit) */
typedef uint32 Dio_PortLevelType;

/** Channel group: contiguous bits of one port */
typedef struct
{
    Dio_PortLevelType mask;     /* bit mask of the group inside the port */
    uint8             offset;   /* position of the lowest bit of the group */
    Dio_PortType      port;     /* port the group belongs to */
} Dio_ChannelGroupType;

#endif /* DIO_TYPES_H */