#ifndef GPT_PBCFG_H
#define GPT_PBCFG_H

#include "Gpt_Types.h"

/* -------------------------------------------------------------------------- */
/* GPT compile-time configuration                                             */
/* -------------------------------------------------------------------------- */

#define GPT_CHANNEL_COUNT                       (4U)

#define GPT_CHANNEL_ID0                         (Gpt_ChannelType)(0U)
#define GPT_CHANNEL_ID1                         (Gpt_ChannelType)(1U)
#define GPT_CHANNEL_ID2                         (Gpt_ChannelType)(2U)
#define GPT_CHANNEL_ID3                         (Gpt_ChannelType)(3U)

/*
 * LPIT0 clock source:
 * PCS = 6 -> SPLL2_DIV2_CLK
 *
 * For the S32K144 clock configuration used by the NXP cookbook:
 * SPLL = 160 MHz -> SPLL2_DIV2 = 40 MHz.
 *
 * This value must match the actual clock configured by the Mcu driver.
 */
#define GPT_LPIT_CLOCK_SOURCE                   (6U)
#define GPT_LPIT_CLOCK_FREQUENCY_HZ             (40000000UL)

/* -------------------------------------------------------------------------- */
/* Optional AUTOSAR GPT functionality                                         */
/* -------------------------------------------------------------------------- */

#define GPT_VERSION_INFO_API                    STD_ON
#define GPT_DEINIT_API                          STD_ON
#define GPT_ENABLE_DISABLE_NOTIFICATION_API    STD_ON

#define GPT_WAKEUP_FUNCTIONALITY_API            STD_OFF
#define GPT_PREDEF_TIMER_API                    STD_OFF

/*
 * Development Error Tracer integration is not included in this first
 * implementation.
 */
#define GPT_DEV_ERROR_DETECT                    STD_OFF

/* -------------------------------------------------------------------------- */
/* Version information                                                         */
/* -------------------------------------------------------------------------- */



/* -------------------------------------------------------------------------- */
/* Post-build configuration table                                              */
/* -------------------------------------------------------------------------- */

extern const Gpt_ConfigType Gpt_Config;

#endif /* GPT_PBCFG_H */