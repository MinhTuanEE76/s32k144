#include "Port.h"
#include "S32K144.h"

#define PORT_NUM_PORTS   5U   /* PORTA..PORTE */

/*============================== Local data ==============================*/
static const Port_ConfigType* Port_ConfigPtr = NULL_PTR;

static PORT_Type* const Port_PortTable[PORT_NUM_PORTS] =
{
    IP_PORTA, IP_PORTB, IP_PORTC, IP_PORTD, IP_PORTE
};

static GPIO_Type* const Port_GpioTable[PORT_NUM_PORTS] =
{
    IP_PTA, IP_PTB, IP_PTC, IP_PTD, IP_PTE
};

/*============================== Local functions =========================*/

/* Enable PCC clock gate of PORTx */
static void Port_lEnableClock(uint8 PortIdx)
{
    /* PCC_PORTA_INDEX .. PCC_PORTE_INDEX are consecutive */
    IP_PCC->PCCn[PCC_PORTA_INDEX + PortIdx] |= PCC_PCCn_CGC_MASK;
}

/* Find configuration of a pin, NULL_PTR if the pin is not configured */
static const Port_PinConfigsType* Port_lFindPin(Port_PinType Pin)
{
    const Port_PinConfigsType* result = NULL_PTR;

    for (uint16 i = 0U; i < Port_ConfigPtr->PinCount; i++)
    {
        if (Port_ConfigPtr->PinConfigs[i].PinId == Pin)
        {
            result = &Port_ConfigPtr->PinConfigs[i];
            break;
        }
    }
    return result;
}

static void Port_lSetDirection(Port_PinType Pin, Port_PinDirectionType Dir)
{
    GPIO_Type* gpio = Port_GpioTable[PORT_PIN_GET_PORT(Pin)];
    uint32 mask = (1UL << PORT_PIN_GET_NUM(Pin));

    if (Dir == PORT_PIN_OUT)
    {
        gpio->PDDR |= mask;
    }
    else
    {
        gpio->PDDR &= ~mask;
    }
}

static void Port_lWriteLevel(Port_PinType Pin, uint8 Level)
{
    GPIO_Type* gpio = Port_GpioTable[PORT_PIN_GET_PORT(Pin)];
    uint32 mask = (1UL << PORT_PIN_GET_NUM(Pin));

    if (Level == STD_LOW)
    {
        gpio->PCOR = mask;      /* atomic clear */
    }
    else
    {
        gpio->PSOR = mask;      /* atomic set   */
    }
}

static uint32 Port_lBuildPcr(const Port_PinConfigsType* Cfg)
{
    uint32 pcr = ((uint32)Cfg->PinMode << PORT_PCR_MUX_SHIFT) & PORT_PCR_MUX_MASK;

    if (Cfg->PullUpPullDown == PORT_PULL_UP)
    {
        pcr |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
    }
    else if (Cfg->PullUpPullDown == PORT_PULL_DOWN)
    {
        pcr |= PORT_PCR_PE_MASK;            /* PE=1, PS=0 */
    }
    else
    {
        /* no pull */
    }

    if (Cfg->DriveStrength == PORT_DRIVE_HIGH) { pcr |= PORT_PCR_DSE_MASK; }
    if (Cfg->PassiveFilter == TRUE)            { pcr |= PORT_PCR_PFE_MASK; }

    return pcr;
}

/*============================== API =====================================*/

void Port_Init(const Port_ConfigType* ConfigPtr)
{
    if (ConfigPtr == NULL_PTR)
    {
        return;
    }

    Port_ConfigPtr = ConfigPtr;

    for (uint16 i = 0U; i < ConfigPtr->PinCount; i++)
    {
        const Port_PinConfigsType* cfg = &ConfigPtr->PinConfigs[i];
        uint8 portIdx = PORT_PIN_GET_PORT(cfg->PinId);
        uint8 pinNum  = PORT_PIN_GET_NUM(cfg->PinId);

        if (portIdx >= PORT_NUM_PORTS)
        {
            continue;
        }

        Port_lEnableClock(portIdx);

        /* Order matters to avoid glitches on outputs:
         * 1) preload output level, 2) set direction, 3) finally enable the mux */
        if (cfg->PinMode == PORT_PIN_MODE_GPIO)
        {
            if (cfg->PinDirection == PORT_PIN_OUT)
            {
                Port_lWriteLevel(cfg->PinId, cfg->InitialValue);
            }
            Port_lSetDirection(cfg->PinId, cfg->PinDirection);
        }
        else if (cfg->PinMode == PORT_PIN_MODE_ANALOG)
        {
            Port_lSetDirection(cfg->PinId, PORT_PIN_IN);
        }
        else
        {
            /* Alternate function: direction is controlled by the peripheral */
        }

        /* PIDR resets to all-1 (digital input disabled): enable it for every non-analog pin
         * so that Dio_ReadChannel()/Dio_ReadPort() (GPIOx->PDIR) return valid data */
        if (cfg->PinMode == PORT_PIN_MODE_ANALOG)
        {
            Port_GpioTable[portIdx]->PIDR |= (1UL << pinNum);
        }
        else
        {
            Port_GpioTable[portIdx]->PIDR &= ~(1UL << pinNum);
        }

        Port_PortTable[portIdx]->PCR[pinNum] = Port_lBuildPcr(cfg);
    }
}

#if (PORT_SET_PIN_DIRECTION_API == 1)
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)
{
    const Port_PinConfigsType* cfg;

    if (Port_ConfigPtr == NULL_PTR)
    {
        return;
    }

    cfg = Port_lFindPin(Pin);
    if ((cfg == NULL_PTR) || (cfg->PinDirectionChangeable == FALSE))
    {
        return;
    }

    Port_lSetDirection(Pin, Direction);
}
#endif

void Port_RefreshPortDirection(void)
{
    if (Port_ConfigPtr == NULL_PTR)
    {
        return;
    }

    for (uint16 i = 0U; i < Port_ConfigPtr->PinCount; i++)
    {
        const Port_PinConfigsType* cfg = &Port_ConfigPtr->PinConfigs[i];

        /* AUTOSAR: only pins whose direction is NOT changeable are refreshed */
        if ((cfg->PinDirectionChangeable == FALSE) && (cfg->PinMode == PORT_PIN_MODE_GPIO))
        {
            Port_lSetDirection(cfg->PinId, cfg->PinDirection);
        }
    }
}

#if (PORT_VERSION_INFO_API == 1)
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    if (versioninfo == NULL_PTR)
    {
        return;
    }
    versioninfo->vendorID         = PORT_VENDOR_ID;
    versioninfo->moduleID         = PORT_MODULE_ID;
    versioninfo->sw_major_version = PORT_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = PORT_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = PORT_SW_PATCH_VERSION;
}
#endif

#if (PORT_SET_PIN_MODE_API == 1)
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
    const Port_PinConfigsType* cfg;
    uint32 pcr;

    if (Port_ConfigPtr == NULL_PTR)
    {
        return;
    }

    cfg = Port_lFindPin(Pin);
    if ((cfg == NULL_PTR) || (cfg->PinModeChangeable == FALSE) || (Mode > PORT_PIN_MODE_MAX))
    {
        return;
    }

    pcr  = Port_PortTable[PORT_PIN_GET_PORT(Pin)]->PCR[PORT_PIN_GET_NUM(Pin)];
    pcr &= ~PORT_PCR_MUX_MASK;
    pcr |= ((uint32)Mode << PORT_PCR_MUX_SHIFT) & PORT_PCR_MUX_MASK;
    Port_PortTable[PORT_PIN_GET_PORT(Pin)]->PCR[PORT_PIN_GET_NUM(Pin)] = pcr;

    if (Mode == PORT_PIN_MODE_ANALOG)
    {
        Port_GpioTable[PORT_PIN_GET_PORT(Pin)]->PIDR |= (1UL << PORT_PIN_GET_NUM(Pin));
    }
    else
    {
        Port_GpioTable[PORT_PIN_GET_PORT(Pin)]->PIDR &= ~(1UL << PORT_PIN_GET_NUM(Pin));
    }
}
#endif