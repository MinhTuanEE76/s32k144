#include "Port_Cfg.h"
#include "Port_Types.h"

static const Port_PinConfigsType Port_PinConfigs[] =
{
    /* ===== PORTD: RGB LED (GPIO output, initial OFF - LED is active low on EVB) ===== */
    {
        .PinId = PORT_PIN_PTD0,  .PinMode = PORT_PIN_MODE_GPIO, .PinDirection = PORT_PIN_OUT,
        .PullUpPullDown = PORT_PULL_NONE, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = FALSE,
        .InitialValue = STD_HIGH, .PinDirectionChangeable = FALSE, .PinModeChangeable = FALSE
    },
    {
        .PinId = PORT_PIN_PTD15, .PinMode = PORT_PIN_MODE_GPIO, .PinDirection = PORT_PIN_OUT,
        .PullUpPullDown = PORT_PULL_NONE, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = FALSE,
        .InitialValue = STD_HIGH, .PinDirectionChangeable = FALSE, .PinModeChangeable = FALSE
    },
    {
        .PinId = PORT_PIN_PTD16, .PinMode = PORT_PIN_MODE_GPIO, .PinDirection = PORT_PIN_OUT,
        .PullUpPullDown = PORT_PULL_NONE, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = FALSE,
        .InitialValue = STD_HIGH, .PinDirectionChangeable = TRUE,  .PinModeChangeable = FALSE
    },

    /* ===== PORTC: Button (GPIO input) ===== */
    {
        .PinId = PORT_PIN_PTC12, .PinMode = PORT_PIN_MODE_GPIO, .PinDirection = PORT_PIN_IN,
        .PullUpPullDown = PORT_PULL_NONE, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = TRUE,
        .InitialValue = STD_LOW, .PinDirectionChangeable = FALSE, .PinModeChangeable = FALSE
    },

    /* ===== PORTC: LPUART1 (ALT2) ===== */
    {
        .PinId = PORT_PIN_PTC6,  .PinMode = PORT_PIN_MODE_ALT2, .PinDirection = PORT_PIN_IN,
        .PullUpPullDown = PORT_PULL_UP, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = FALSE,
        .InitialValue = STD_LOW, .PinDirectionChangeable = FALSE, .PinModeChangeable = FALSE
    },
    {
        .PinId = PORT_PIN_PTC7,  .PinMode = PORT_PIN_MODE_ALT2, .PinDirection = PORT_PIN_OUT,
        .PullUpPullDown = PORT_PULL_NONE, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = FALSE,
        .InitialValue = STD_LOW, .PinDirectionChangeable = FALSE, .PinModeChangeable = FALSE
    },

    /* ===== PORTE: CAN0 (ALT5) ===== */
    {
        .PinId = PORT_PIN_PTE4,  .PinMode = PORT_PIN_MODE_ALT5, .PinDirection = PORT_PIN_IN,
        .PullUpPullDown = PORT_PULL_NONE, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = FALSE,
        .InitialValue = STD_LOW, .PinDirectionChangeable = FALSE, .PinModeChangeable = FALSE
    },
    {
        .PinId = PORT_PIN_PTE5,  .PinMode = PORT_PIN_MODE_ALT5, .PinDirection = PORT_PIN_OUT,
        .PullUpPullDown = PORT_PULL_NONE, .DriveStrength = PORT_DRIVE_LOW, .PassiveFilter = FALSE,
        .InitialValue = STD_LOW, .PinDirectionChangeable = FALSE, .PinModeChangeable = FALSE
    },
};

const Port_ConfigType Port_ConfigTable =
{
    .PinConfigs = Port_PinConfigs,
    .PinCount   = (uint16)(sizeof(Port_PinConfigs) / sizeof(Port_PinConfigs[0]))
};