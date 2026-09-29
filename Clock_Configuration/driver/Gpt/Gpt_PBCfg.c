#include "Gpt_PBCfg.h"

/* -------------------------------------------------------------------------- */
/* GPT Channel Configuration                                                  */
/* -------------------------------------------------------------------------- */

/*
 * LPIT0 provides four timer channels.
 *
 * ChannelId:
 *     0 -> LPIT0 channel 0
 *     1 -> LPIT0 channel 1
 *     2 -> LPIT0 channel 2
 *     3 -> LPIT0 channel 3
 *
 * Notification:
 *     NULL_PTR means no application callback is configured initially.
 *     Replace NULL_PTR with an application notification function when needed.
 */

const Gpt_ChannelConfigType Gpt_ChannelConfigSet[GPT_CHANNEL_COUNT] =
{
    {
        .ChannelId            = GPT_CHANNEL_ID0,
        .ChannelMode          = GPT_CHANNEL_MODE_CONTINUOUS,
        .ChannelTickFrequency = GPT_LPIT_CLOCK_FREQUENCY_HZ,
        .ChannelTickValueMax  = 0xFFFFFFFFUL,
        .EnableWakeup         = FALSE,
        .Notification         = NULL_PTR,
        .ChannelClkSrcRef     = 0U
    },

    {
        .ChannelId            = GPT_CHANNEL_ID1,
        .ChannelMode          = GPT_CHANNEL_MODE_ONESHOT,
        .ChannelTickFrequency = GPT_LPIT_CLOCK_FREQUENCY_HZ,
        .ChannelTickValueMax  = 0xFFFFFFFFUL,
        .EnableWakeup         = FALSE,
        .Notification         = NULL_PTR,
        .ChannelClkSrcRef     = 0U
    },

    {
        .ChannelId            = GPT_CHANNEL_ID2,
        .ChannelMode          = GPT_CHANNEL_MODE_ONESHOT,
        .ChannelTickFrequency = GPT_LPIT_CLOCK_FREQUENCY_HZ,
        .ChannelTickValueMax  = 0xFFFFFFFFUL,
        .EnableWakeup         = FALSE,
        .Notification         = NULL_PTR,
        .ChannelClkSrcRef     = 0U
    },

    {
        .ChannelId            = GPT_CHANNEL_ID3,
        .ChannelMode          = GPT_CHANNEL_MODE_ONESHOT,
        .ChannelTickFrequency = GPT_LPIT_CLOCK_FREQUENCY_HZ,
        .ChannelTickValueMax  = 0xFFFFFFFFUL,
        .EnableWakeup         = FALSE,
        .Notification         = NULL_PTR,
        .ChannelClkSrcRef     = 0U
    }
};

/* -------------------------------------------------------------------------- */
/* GPT Configuration                                                           */
/* -------------------------------------------------------------------------- */

const Gpt_ConfigType Gpt_Config =
{
    .Channels         = Gpt_ChannelConfigSet,
    .ChannelCfgNumber = GPT_CHANNEL_COUNT
};