/* osalSemaphore.h - Counting semaphore synchronization */

/* All Rights Reserved */

#ifndef INC_OSAL_SEMAPHORE_H
#define INC_OSAL_SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osal_types.h"

/* Configurations */

/* Definitions*/
/* Types */

/**
 * @brief Semaphore attributes
 */
typedef struct
{
    const char *name;      ///< Name of the semaphore
    uint32_t max_count;     ///< Maximum count
    uint32_t initial_count; ///< Initial count
    void *cb_mem;           ///< Memory for control block (optional)
    uint32_t cb_size;       ///< Size of control block memory
} osal_semaphore_attr_t;

typedef void *osal_semaphore_handle_t;

/* Functions */

/**
 * @brief Create a semaphore.
 * @param attr Semaphore attributes (optional).
 * @return Handle to the created semaphore, or NULL on failure.
 */
osal_semaphore_handle_t osal_semaphore_create(const osal_semaphore_attr_t *attr);

/**
 * @brief Delete a semaphore.
 * @param sem Handle to the semaphore.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_semaphore_delete(osal_semaphore_handle_t sem);

/**
 * @brief Acquire (take) a semaphore.
 * @param sem Handle to the semaphore.
 * @param timeout_ms Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT on timeout.
 */
osal_status_t osal_semaphore_take(osal_semaphore_handle_t sem, uint32_t timeout_ms);

/**
 * @brief Give (signal) a semaphore.
 * @param sem Handle to the semaphore.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_semaphore_give(osal_semaphore_handle_t sem);

/**
 * @brief Get current semaphore count.
 * @param sem Handle to the semaphore.
 * @return Current count.
 */
uint32_t osal_semaphore_get_count(osal_semaphore_handle_t sem);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_SEMAPHORE_H */
