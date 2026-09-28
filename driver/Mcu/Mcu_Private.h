#ifndef __MCUPRIVATE_H__
#define __MCUPRIVATE_H__

/**
 * @brief: 
 */
typedef enum
{
    MCU_UNINIT = 0U,
    MCU_IDLE
} Mcu_StatusType;

extern Mcu_StatusType Mcu_Status;

#endif
