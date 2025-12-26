/* osalSemaphore.c - POSIX Semaphore Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_semaphore.h"
#include <zephyr/kernel.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    struct k_sem sem;
} osal_semaphore_control_block_t;

/* Functions */

osal_semaphore_handle_t osal_semaphore_create(const osal_semaphore_attr_t *attr)
{
    if (attr == NULL) {
        return NULL;
    }

    // Allocate memory for the semaphore wrapper
    // Note: This requires CONFIG_HEAP_MEM_POOL_SIZE to be set in prj.conf
    osal_semaphore_control_block_t *wrapper = (osal_semaphore_control_block_t *)k_malloc(sizeof(osal_semaphore_control_block_t));
    if (wrapper == NULL) {
        return NULL;
    }

    // Initialize the Zephyr semaphore
    // k_sem_init(sem, initial_count, limit)
    int ret = k_sem_init(&wrapper->sem, attr->initial_count, attr->max_count);
    if (ret != 0) {
        k_free(wrapper); // Clean up if initialization fails
        return NULL;
    }

    return (osal_semaphore_handle_t)wrapper;
}

osal_status_t osal_semaphore_delete(osal_semaphore_handle_t sem)
{
    if (sem == NULL) {
        return OSAL_ERROR_PARAMETER;
    }

    // Zephyr kernel objects don't strictly need "deletion" if they aren't threads,
    // but we must free the memory we allocated for the wrapper.
    k_free(sem);
    return OSAL_SUCCESS;
}

osal_status_t osal_semaphore_take(osal_semaphore_handle_t sem, uint32_t timeout_ms)
{
    if (sem == NULL) {
        return OSAL_ERROR_PARAMETER;
    }

    osal_semaphore_control_block_t *wrapper = (osal_semaphore_control_block_t *)sem;
    k_timeout_t timeout;

    // Convert OSAL timeout to Zephyr timeout
    if (timeout_ms == OSAL_WAIT_FOREVER) {
        timeout = K_FOREVER;
    } else {
        timeout = K_MSEC(timeout_ms);
    }

    // Take the semaphore
    int ret = k_sem_take(&wrapper->sem, timeout);

    if (ret == 0) {
        return OSAL_SUCCESS;
    } else if (ret == -EAGAIN) {
        return OSAL_ERROR_TIMEOUT;
    } else {
        return OSAL_ERROR;
    }
}

osal_status_t osal_semaphore_give(osal_semaphore_handle_t sem)
{
    if (sem == NULL) {
        return OSAL_ERROR_PARAMETER;
    }

    osal_semaphore_control_block_t *wrapper = (osal_semaphore_control_block_t *)sem;
    k_sem_give(&wrapper->sem);
    return OSAL_SUCCESS;
}

uint32_t osal_semaphore_get_count(osal_semaphore_handle_t sem)
{
    if (sem == NULL) {
        return 0;
    }

    osal_semaphore_control_block_t *wrapper = (osal_semaphore_control_block_t *)sem;
    return k_sem_count_get(&wrapper->sem);
}
