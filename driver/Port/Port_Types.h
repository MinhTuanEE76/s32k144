#ifndef PORT_TYPES_H
#define PORT_TYPES_H

#include "Std_Types.h"

/*
 * Pin ID = (PortIndex << 5) | PinNumber
 * S32K144 has PORTA..PORTE, up to 18 pins per port -> 5 bits for the pin number.
 * Usage: PORT_PIN_ID(PORT_D, 15U) -> PTD15
 */
#define PORT_A                   0U
#define PORT_B                   1U
#define PORT_C                   2U
#define PORT_D                   3U
#define PORT_E                   4U

#define PORT_PIN_ID(port, pin)   ((Port_PinType)(((port) << 5U) | (pin)))
#define PORT_PIN_GET_PORT(id)    ((uint8)((id) >> 5U))
#define PORT_PIN_GET_NUM(id)     ((uint8)((id) & 0x1FU))

/* Pins used on S32K144EVB (for convenience) */
#define PORT_PIN_PTD0            PORT_PIN_ID(PORT_D, 0U)   /* RGB LED - Blue  */
#define PORT_PIN_PTD15           PORT_PIN_ID(PORT_D, 15U)  /* RGB LED - Red   */
#define PORT_PIN_PTD16           PORT_PIN_ID(PORT_D, 16U)  /* RGB LED - Green */
#define PORT_PIN_PTC12           PORT_PIN_ID(PORT_C, 12U)  /* Button SW2      */
#define PORT_PIN_PTC13           PORT_PIN_ID(PORT_C, 13U)  /* Button SW3      */
#define PORT_PIN_PTC6            PORT_PIN_ID(PORT_C, 6U)   /* LPUART1_RX      */
#define PORT_PIN_PTC7            PORT_PIN_ID(PORT_C, 7U)   /* LPUART1_TX      */
#define PORT_PIN_PTE4            PORT_PIN_ID(PORT_E, 4U)   /* CAN0_RX         */
#define PORT_PIN_PTE5            PORT_PIN_ID(PORT_E, 5U)   /* CAN0_TX         */

/** Port_PinType: ID of a port pin */
typedef uint16 Port_PinType;

/** Port_PinDirectionType */
typedef enum
{
    PORT_PIN_IN  = 0x00U,
    PORT_PIN_OUT = 0x01U
} Port_PinDirectionType;

/**
 * Port_PinModeType: value written directly into PCR[MUX] (0..7).
 * ALT2..ALT7 meaning depends on the pin (see S32K144 signal multiplexing table).
 */
typedef uint8 Port_PinModeType;

#define PORT_PIN_MODE_ANALOG     ((Port_PinModeType)0U)  /* MUX=0: disabled / analog */
#define PORT_PIN_MODE_GPIO       ((Port_PinModeType)1U)  /* MUX=1: GPIO              */
#define PORT_PIN_MODE_ALT2       ((Port_PinModeType)2U)
#define PORT_PIN_MODE_ALT3       ((Port_PinModeType)3U)
#define PORT_PIN_MODE_ALT4       ((Port_PinModeType)4U)
#define PORT_PIN_MODE_ALT5       ((Port_PinModeType)5U)
#define PORT_PIN_MODE_ALT6       ((Port_PinModeType)6U)
#define PORT_PIN_MODE_ALT7       ((Port_PinModeType)7U)
#define PORT_PIN_MODE_MAX        ((Port_PinModeType)7U)

/** Internal pull resistor */
typedef enum
{
    PORT_PULL_NONE = 0U,
    PORT_PULL_UP,
    PORT_PULL_DOWN
} Port_PinPullType;

/** Drive strength (only effective on high-drive pins, see datasheet) */
typedef enum
{
    PORT_DRIVE_LOW = 0U,
    PORT_DRIVE_HIGH
} Port_DriveStrengthType;

/** Configuration of a single pin */
typedef struct
{
    Port_PinType            PinId;
    Port_PinModeType        PinMode;                /* MUX value                          */
    Port_PinDirectionType   PinDirection;           /* used when PinMode == GPIO          */
    Port_PinPullType        PullUpPullDown;
    Port_DriveStrengthType  DriveStrength;
    boolean                 PassiveFilter;          /* PCR[PFE]                           */
    uint8                   InitialValue;           /* STD_LOW / STD_HIGH (GPIO output)   */
    boolean                 PinDirectionChangeable; /* AUTOSAR PortPinDirectionChangeable */
    boolean                 PinModeChangeable;      /* AUTOSAR PortPinModeChangeable      */
} Port_PinConfigsType;

/** Configuration of the whole driver */
typedef struct
{
    const Port_PinConfigsType* PinConfigs;
    uint16                     PinCount;
} Port_ConfigType;

#endif /* PORT_TYPES_H */