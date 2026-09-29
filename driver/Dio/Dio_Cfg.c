#include "Dio_Cfg.h"

/* Group of PTD15 (red) and PTD16 (green): 2 contiguous bits, offset 15 */
const Dio_ChannelGroupType DioConf_DioChannelGroup_LED_RG =
{
    .mask   = (0x3UL << 15U),
    .offset = 15U,
    .port   = DIO_PORT_D
};