#ifndef GPT_H
#define GPT_H

#include "Std_Types.h"
#include "Gpt_Types.h"

/* -------------------------------------------------------------------------- */
/* AUTOSAR GPT Driver API                                                     */
/* -------------------------------------------------------------------------- */

#if (GPT_VERSION_INFO_API == STD_ON)

void Gpt_GetVersionInfo(Std_VersionInfoType *versioninfo);

#endif /* GPT_VERSION_INFO_API */


/**
 * @brief Initializes the GPT driver.
 *
 * @param ConfigPtr Pointer to GPT configuration.
 */
void Gpt_Init(const Gpt_ConfigType *ConfigPtr);


/**
 * @brief Deinitializes the GPT driver.
 */
void Gpt_DeInit(void);


/**
 * @brief Returns the elapsed timer value of a GPT channel.
 *
 * @param Channel GPT channel.
 *
 * @return Elapsed timer value in ticks.
 */
Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType Channel);


/**
 * @brief Returns the remaining timer value of a GPT channel.
 *
 * @param Channel GPT channel.
 *
 * @return Remaining timer value in ticks.
 */
Gpt_ValueType Gpt_GetTimeRemaining(Gpt_ChannelType Channel);


/**
 * @brief Starts a GPT timer channel.
 *
 * @param Channel GPT channel.
 * @param Value Timer period in ticks.
 */
void Gpt_StartTimer(Gpt_ChannelType Channel,
                    Gpt_ValueType Value);


/**
 * @brief Stops a GPT timer channel.
 *
 * @param Channel GPT channel.
 */
void Gpt_StopTimer(Gpt_ChannelType Channel);


/**
 * @brief Enables notification for a GPT channel.
 *
 * @param Channel GPT channel.
 */
void Gpt_EnableNotification(Gpt_ChannelType Channel);


/**
 * @brief Disables notification for a GPT channel.
 *
 * @param Channel GPT channel.
 */
void Gpt_DisableNotification(Gpt_ChannelType Channel);


/* -------------------------------------------------------------------------- */
/* Implementation-specific helper                                             */
/* -------------------------------------------------------------------------- */

/*
 * This function is not an AUTOSAR public API.
 * It is the internal service called by Gpt_Irq.c after the hardware
 * interrupt vector has been entered.
 */
void Gpt_lProcessChannelInterrupt(Gpt_ChannelType Channel);


/* -------------------------------------------------------------------------- */
/* Project-specific convenience API                                            */
/* -------------------------------------------------------------------------- */

/*
 * This function is an implementation extension and is not part of the
 * AUTOSAR GPT standard API.
 */
Gpt_ValueType Gpt_MsToTicks(Gpt_ChannelType Channel,
                            uint32 Milliseconds);

#endif /* GPT_H */