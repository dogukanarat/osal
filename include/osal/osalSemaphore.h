/* osalSemaphore.h - Counting semaphore synchronization */

/* All Rights Reserved */

#ifndef INC_OSAL_SEMAPHORE_H
#define INC_OSAL_SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osalTypes.h"

/* Configurations */

/* Definitions*/
/* Types */

/**
 * @brief Semaphore attributes
 */
typedef struct
{
    const char *name;      ///< Name of the semaphore
    uint32_t maxCount;     ///< Maximum count
    uint32_t initialCount; ///< Initial count
    void *cbMem;           ///< Memory for control block (optional)
    uint32_t cbSize;       ///< Size of control block memory
} osalSemaphoreAttr_t;

/* Functions */

/**
 * @brief Create a semaphore.
 * @param attr Semaphore attributes (optional).
 * @return Handle to the created semaphore, or NULL on failure.
 */
osalSemaphoreHandle_t osalSemaphoreCreate(const osalSemaphoreAttr_t *attr);

/**
 * @brief Delete a semaphore.
 * @param sem Handle to the semaphore.
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalSemaphoreDelete(osalSemaphoreHandle_t sem);

/**
 * @brief Acquire (take) a semaphore.
 * @param sem Handle to the semaphore.
 * @param timeoutMs Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT on timeout.
 */
osalStatus_t osalSemaphoreTake(osalSemaphoreHandle_t sem, uint32_t timeoutMs);

/**
 * @brief Give (signal) a semaphore.
 * @param sem Handle to the semaphore.
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalSemaphoreGive(osalSemaphoreHandle_t sem);

/**
 * @brief Get current semaphore count.
 * @param sem Handle to the semaphore.
 * @return Current count.
 */
uint32_t osalSemaphoreGetCount(osalSemaphoreHandle_t sem);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_SEMAPHORE_H */