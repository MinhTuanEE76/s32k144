#ifndef DIO_CFG_H
#define DIO_CFG_H

#include "Dio_Types.h"

/* Pre-compile switches (AUTOSAR DioGeneral) */
#define DIO_VERSION_INFO_API            1   /* 1: enable Dio_GetVersionInfo()   */
#define DIO_FLIP_CHANNEL_API            1   /* 1: enable Dio_FlipChannel()      */
#define DIO_MASKED_WRITE_PORT_API       1   /* 1: enable Dio_MaskedWritePort()  */

/* Symbolic names of channels (must match Port_Cfg.c) */
#define DioConf_DioChannel_LED_BLUE     DIO_CHANNEL_ID(DIO_PORT_D, 0U)
#define DioConf_DioChannel_LED_RED      DIO_CHANNEL_ID(DIO_PORT_D, 15U)
#define DioConf_DioChannel_LED_GREEN    DIO_CHANNEL_ID(DIO_PORT_D, 16U)
#define DioConf_DioChannel_BTN_SW2      DIO_CHANNEL_ID(DIO_PORT_C, 12U)
#define DioConf_DioChannel_BTN_SW3      DIO_CHANNEL_ID(DIO_PORT_C, 13U)

/* Symbolic names of ports */
#define DioConf_DioPort_PORTC           DIO_PORT_C
#define DioConf_DioPort_PORTD           DIO_PORT_D

/* Channel groups */
extern const Dio_ChannelGroupType DioConf_DioChannelGroup_LED_RG;

#endif /* DIO_CFG_H */