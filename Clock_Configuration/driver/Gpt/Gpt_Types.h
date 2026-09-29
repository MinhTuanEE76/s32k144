#ifndef GPT_TYPES_H
#define GPT_TYPES_H

#include "Std_Types.h"

/* -------------------------------------------------------------------------- */
/* AUTOSAR GPT standard data types                                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Numeric ID of a GPT channel.
 *
 * AUTOSAR:
 * SWS_Gpt_00358
 */
typedef uint8 Gpt_ChannelType;

/**
 * @brief Data type for reading and setting timer values in ticks.
 *
 * AUTOSAR:
 * SWS_Gpt_00359
 *
 * The range is microcontroller dependent. For the S32K144 LPIT
 * implementation, uint32 is appropriate because the LPIT timer
 * value register is 32 bits wide.
 */
typedef uint32 Gpt_ValueType;

/**
 * @brief Modes of the GPT driver.
 *
 * AUTOSAR:
 * SWS_Gpt_00360
 */
typedef enum
{
    GPT_MODE_NORMAL = 0x00U,
    GPT_MODE_SLEEP  = 0x01U
} Gpt_ModeType;

/**
 * @brief GPT predefined timer identifiers.
 *
 * AUTOSAR:
 * SWS_Gpt_00389
 */
typedef enum
{
    GPT_PREDEF_TIMER_1US_16BIT  = 0x00U,
    GPT_PREDEF_TIMER_1US_24BIT  = 0x01U,
    GPT_PREDEF_TIMER_1US_32BIT  = 0x02U,
    GPT_PREDEF_TIMER_100US_32BIT = 0x03U
} Gpt_PredefTimerType;


/* -------------------------------------------------------------------------- */
/* Implementation-specific GPT types                                         */
/* -------------------------------------------------------------------------- */

/**
 * @brief Timer channel operating mode.
 *
 * Implementation-specific representation of the AUTOSAR
 * GptChannelMode configuration parameter.
 */
typedef enum
{
    GPT_CHANNEL_MODE_ONESHOT   = 0x00U,
    GPT_CHANNEL_MODE_CONTINUOUS = 0x01U
} Gpt_ChannelModeType;

/**
 * @brief GPT notification callback function type.
 *
 * The callback is configured through the AUTOSAR GptNotification
 * configuration parameter.
 */
typedef void (*Gpt_NotificationType)(void);

/**
 * @brief Reference to a GPT clock reference point.
 *
 * The actual clock source is configured by the MCU/clock subsystem.
 * For this S32K144 implementation, this value identifies the clock
 * reference used by a GPT channel.
 */
typedef uint8 Gpt_ClockReferencePointType;

/**
 * @brief GPT channel configuration.
 *
 * Implementation-specific configuration structure used to represent
 * the relevant AUTOSAR GptChannelConfiguration parameters.
 */
typedef struct
{
    Gpt_ChannelType             ChannelId;
    Gpt_ChannelModeType         ChannelMode;
    uint32                      ChannelTickFrequency;
    Gpt_ValueType               ChannelTickValueMax;
    boolean                     EnableWakeup;
    Gpt_NotificationType        Notification;
    Gpt_ClockReferencePointType ChannelClkSrcRef;
} Gpt_ChannelConfigType;

/**
 * @brief GPT driver configuration.
 *
 * AUTOSAR:
 * SWS_Gpt_00357
 *
 * The contents of Gpt_ConfigType are implementation-specific.
 */
typedef struct
{
    const Gpt_ChannelConfigType *Channels;
    uint8                       ChannelCfgNumber;
} Gpt_ConfigType;


/* -------------------------------------------------------------------------- */
/* Internal runtime types                                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Runtime status of a GPT channel.
 *
 * Implementation-specific internal state.
 */
typedef enum
{
    GPT_CHANNEL_STOPPED = 0x00U,
    GPT_CHANNEL_RUNNING,
    GPT_CHANNEL_EXPIRED
} Gpt_ChannelStatusType;

/**
 * @brief Runtime status of the GPT driver.
 *
 * Implementation-specific internal state.
 */
typedef enum
{
    GPT_DRIVER_UNINIT = 0x00U,
    GPT_DRIVER_INIT
} Gpt_DriverStatusType;

/**
 * @brief Runtime information maintained for a GPT channel.
 *
 * Implementation-specific internal state.
 */
typedef struct
{
    Gpt_ChannelStatusType Status;
    boolean               NotificationEnabled;
} Gpt_ChannelRuntimeType;

#endif /* GPT_TYPES_H */