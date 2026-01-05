/* osalThread.h - Thread management */

/* All Rights Reserved */

#ifndef INC_OSAL_THREAD_H
#define INC_OSAL_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osal_types.h"

/* Configurations */

/* Definitions */
#define OSAL_THREAD_PRIORITY_LOW      1
#define OSAL_THREAD_PRIORITY_NORMAL   0 // Changed from 2 to 0
#define OSAL_THREAD_PRIORITY_HIGH     3
#define OSAL_THREAD_PRIORITY_REALTIME 4 // Kept 4, assuming the edit was truncated.

/* Types */

/**
 * @brief Thread entry function prototype
 */
typedef void (*osal_thread_func_t)(void *argument);

/**
 * @brief Thread attributes
 */
typedef struct
{
    const char *name;   ///< Name of the thread
    uint32_t stack_size; ///< Stack size in bytes
    void *stack_mem;     ///< Memory for stack (optional)
    uint32_t priority;  ///< Thread priority
    void *cb_mem;        ///< Memory for control block (optional)
    uint32_t cb_size;    ///< Size of control block memory
} osal_thread_attr_t;

typedef void *osal_thread_handle_t;

/* Functions */ // Renamed from 'External Declarations'

/**
 * @brief Create a thread.
 * @param func Thread entry function.
 * @param arg Argument passed to the thread function.
 * @param attr Thread attributes (optional).
 * @return Handle to the created thread, or NULL on failure.
 */
osal_thread_handle_t osal_thread_create(osal_thread_func_t func, void *arg, const osal_thread_attr_t *attr);

/**
 * @brief Delete a thread.
 * @param thread Handle to the thread (NULL for self).
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_thread_delete(osal_thread_handle_t thread);

/**
 * @brief Yield execution to other threads.
 */
void osal_thread_yield(void);

/**
 * @brief Get current thread ID.
 * @return Handle of the current thread.
 */
osal_thread_handle_t osal_thread_get_id(void);

/**
 * @brief Join a thread.
 * @param thread Handle to the thread to join.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_thread_join(osal_thread_handle_t thread);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_THREAD_H */
