/* osalThread.h - Thread management */

/* All Rights Reserved */

#ifndef INC_OSAL_THREAD_H
#define INC_OSAL_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osalTypes.h"

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
typedef void (*osalThreadFunc_t)(void *argument);

/**
 * @brief Thread attributes
 */
typedef struct
{
    const char *name;   ///< Name of the thread
    uint32_t stackSize; ///< Stack size in bytes
    void *stackMem;     ///< Memory for stack (optional)
    uint32_t priority;  ///< Thread priority
    void *cbMem;        ///< Memory for control block (optional)
    uint32_t cbSize;    ///< Size of control block memory
} osalThreadAttr_t;

typedef void *osalThreadHandle_t;

/* Functions */ // Renamed from 'External Declarations'

/**
 * @brief Create a thread.
 * @param func Thread entry function.
 * @param arg Argument passed to the thread function.
 * @param attr Thread attributes (optional).
 * @return Handle to the created thread, or NULL on failure.
 */
osalThreadHandle_t osalThreadCreate(osalThreadFunc_t func, void *arg, const osalThreadAttr_t *attr);

/**
 * @brief Delete a thread.
 * @param thread Handle to the thread (NULL for self).
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalThreadDelete(osalThreadHandle_t thread);

/**
 * @brief Yield execution to other threads.
 */
void osalThreadYield(void);

/**
 * @brief Get current thread ID.
 * @return Handle of the current thread.
 */
osalThreadHandle_t osalThreadGetId(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_THREAD_H */
