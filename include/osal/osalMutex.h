/* osalMutex.h - Mutex synchronization */

/* All Rights Reserved */

#ifndef INC_OSAL_MUTEX_H
#define INC_OSAL_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osalTypes.h"

/* Configurations */

/* Definitions */
#define OSAL_MUTEX_RECURSIVE    0x00000001U
#define OSAL_MUTEX_PRIO_INHERIT 0x00000002U
#define OSAL_MUTEX_ROBUST       0x00000004U

/* Types */

/**
 * @brief Mutex attributes
 */
typedef struct
{
    const char *name;   ///< Name of the mutex
    uint32_t attrBits;  ///< Attribute bits (e.g. OSAL_MUTEX_RECURSIVE)
    void *cbMem;        ///< Memory for control block (optional, for static allocation)
    uint32_t cbSize;    ///< Size of control block memory
} osalMutexAttr_t;

#define OSAL_MUTEX_RECURSIVE 0x00000001U

/* Functions */

/**
 * @brief Create a mutex.
 * @param attr Mutex attributes (optional).
 * @return Handle to the created mutex, or NULL on failure.
 */
osalMutexHandle_t osalMutexCreate(const osalMutexAttr_t *attr);

/**
 * @brief Delete a mutex.
 * @param mutex Handle to the mutex.
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalMutexDelete(osalMutexHandle_t mutex);

/**
 * @brief Acquire a mutex.
 * @param mutex Handle to the mutex.
 * @param timeoutMs Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT on timeout.
 */
osalStatus_t osalMutexLock(osalMutexHandle_t mutex, uint32_t timeoutMs);

/**
 * @brief Release a mutex.
 * @param mutex Handle to the mutex.
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalMutexUnlock(osalMutexHandle_t mutex);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_MUTEX_H */