#ifndef __MCU_PRIVATE_H
#define __MCU_PRIVATE_H


/* -------------------------------------------------------------------------- */
/* MCU Status                                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief MCU driver initialization status.
 */
typedef enum
{
    MCU_UNINIT = 0U,
    MCU_IDLE

} Mcu_StatusType;


/* -------------------------------------------------------------------------- */
/* Global Variables                                                           */
/* -------------------------------------------------------------------------- */

extern Mcu_StatusType Mcu_Status;


#endif /* MCU_PRIVATE_H */