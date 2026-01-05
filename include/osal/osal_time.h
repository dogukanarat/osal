/* osalTime.h - Time management functions */

/* All Rights Reserved */

#ifndef INC_OSAL_TIME_H
#define INC_OSAL_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osal_types.h"

/* Configurations */

/* Definitions */

/* Types */

/**
 * @brief Time value structure for Unix time
 */
typedef struct
{
    uint32_t sec;  /**< Seconds */
    uint32_t usec; /**< Microseconds */
} osal_time_val_t;

/* External Declarations */

/**
 * @brief Delay execution for a specified number of milliseconds.
 * @param ms Milliseconds to delay.
 */
void osal_delay_ms(uint32_t ms);

/**
 * @brief Get the current system tick in milliseconds.
 * @return Current system tick.
 */
uint32_t osal_get_tick_ms(void);

/**
 * @brief Get current Unix time.
 * @param tv Pointer to time value structure to store result.
 * @return OSAL_SUCCESS on success, error code otherwise.
 */
osal_status_t osal_get_unix_time(osal_time_val_t *tv);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_TIME_H */