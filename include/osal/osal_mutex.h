/* osalMutex.h - Mutex synchronization */

/* All Rights Reserved */

#ifndef INC_OSAL_MUTEX_H
#define INC_OSAL_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osal_types.h"

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
    uint32_t attr_bits;  ///< Attribute bits (e.g. OSAL_MUTEX_RECURSIVE)
    void *cb_mem;        ///< Memory for control block (optional, for static allocation)
    uint32_t cb_size;    ///< Size of control block memory
} osal_mutex_attr_t;

typedef void *osal_mutex_handle_t;

/* Functions */

/**
 * @brief Create a mutex.
 * @param attr Mutex attributes (optional).
 * @return Handle to the created mutex, or NULL on failure.
 */
osal_mutex_handle_t osal_mutex_create(const osal_mutex_attr_t *attr);

/**
 * @brief Delete a mutex.
 * @param mutex Handle to the mutex.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_mutex_delete(osal_mutex_handle_t mutex);

/**
 * @brief Acquire a mutex.
 * @param mutex Handle to the mutex.
 * @param timeout_ms Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT on timeout.
 */
osal_status_t osal_mutex_lock(osal_mutex_handle_t mutex, uint32_t timeout_ms);

/**
 * @brief Release a mutex.
 * @param mutex Handle to the mutex.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_mutex_unlock(osal_mutex_handle_t mutex);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_MUTEX_H */
