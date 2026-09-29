#include "Dio.h"
#include "S32K144.h"

#define DIO_NUM_PORTS   5U   /* PTA..PTE */

static GPIO_Type* const Dio_GpioTable[DIO_NUM_PORTS] =
{
    IP_PTA, IP_PTB, IP_PTC, IP_PTD, IP_PTE
};

/*============================== Channel ==============================*/

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    Dio_PortType port = DIO_CHANNEL_GET_PORT(ChannelId);
    uint8        pin  = DIO_CHANNEL_GET_PIN(ChannelId);
    Dio_LevelType level = STD_LOW;

    if (port < DIO_NUM_PORTS)
    {
        level = (Dio_GpioTable[port]->PDIR & (1UL << pin)) ? STD_HIGH : STD_LOW;
    }
    return level;
}

void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
    Dio_PortType port = DIO_CHANNEL_GET_PORT(ChannelId);
    uint32       mask = (1UL << DIO_CHANNEL_GET_PIN(ChannelId));

    if (port < DIO_NUM_PORTS)
    {
        if (Level == STD_LOW)
        {
            Dio_GpioTable[port]->PCOR = mask;   /* atomic clear */
        }
        else
        {
            Dio_GpioTable[port]->PSOR = mask;   /* atomic set   */
        }
    }
}

#if (DIO_FLIP_CHANNEL_API == 1)
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
    Dio_PortType port = DIO_CHANNEL_GET_PORT(ChannelId);
    uint32       mask = (1UL << DIO_CHANNEL_GET_PIN(ChannelId));
    Dio_LevelType level = STD_LOW;

    if (port < DIO_NUM_PORTS)
    {
        Dio_GpioTable[port]->PTOR = mask;       /* atomic toggle */
        /* Level after flip is taken from the output latch (PDOR) */
        level = (Dio_GpioTable[port]->PDOR & mask) ? STD_HIGH : STD_LOW;
    }
    return level;
}
#endif

/*============================== Port =================================*/

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
    Dio_PortLevelType level = 0UL;

    if (PortId < DIO_NUM_PORTS)
    {
        level = Dio_GpioTable[PortId]->PDIR;
    }
    return level;
}

void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{
    if (PortId < DIO_NUM_PORTS)
    {
        Dio_GpioTable[PortId]->PDOR = Level;
    }
}

#if (DIO_MASKED_WRITE_PORT_API == 1)
void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask)
{
    if (PortId < DIO_NUM_PORTS)
    {
        /* Only bits in Mask change; PSOR/PCOR avoid a read-modify-write on PDOR */
        Dio_GpioTable[PortId]->PSOR = (Level & Mask);
        Dio_GpioTable[PortId]->PCOR = (~Level & Mask);
    }
}
#endif

/*============================== Channel group ========================*/

Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr)
{
    Dio_PortLevelType level = 0UL;

    if ((ChannelGroupIdPtr != NULL_PTR) && (ChannelGroupIdPtr->port < DIO_NUM_PORTS))
    {
        level = (Dio_GpioTable[ChannelGroupIdPtr->port]->PDIR & ChannelGroupIdPtr->mask)
                >> ChannelGroupIdPtr->offset;
    }
    return level;
}

void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level)
{
    if ((ChannelGroupIdPtr != NULL_PTR) && (ChannelGroupIdPtr->port < DIO_NUM_PORTS))
    {
        Dio_PortLevelType value = (Level << ChannelGroupIdPtr->offset) & ChannelGroupIdPtr->mask;

        Dio_GpioTable[ChannelGroupIdPtr->port]->PSOR = value;
        Dio_GpioTable[ChannelGroupIdPtr->port]->PCOR = (~value & ChannelGroupIdPtr->mask);
    }
}

/*============================== Version ==============================*/

#if (DIO_VERSION_INFO_API == 1)
void Dio_GetVersionInfo(Std_VersionInfoType* VersionInfo)
{
    if (VersionInfo != NULL_PTR)
    {
        VersionInfo->vendorID         = DIO_VENDOR_ID;
        VersionInfo->moduleID         = DIO_MODULE_ID;
        VersionInfo->sw_major_version = DIO_SW_MAJOR_VERSION;
        VersionInfo->sw_minor_version = DIO_SW_MINOR_VERSION;
        VersionInfo->sw_patch_version = DIO_SW_PATCH_VERSION;
    }
}
#endif