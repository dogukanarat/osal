/* osalSemaphore.c - POSIX Semaphore Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalSemaphore.h"
#include <zephyr/kernel.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    struct k_sem sem;
} osalSemaphoreControlBlock_t;

/* Functions */

osalSemaphoreHandle_t osalSemaphoreCreate(const osalSemaphoreAttr_t *attr)
{
    if (attr == NULL) {
        return NULL;
    }

    // Allocate memory for the semaphore wrapper
    // Note: This requires CONFIG_HEAP_MEM_POOL_SIZE to be set in prj.conf
    osalSemaphoreControlBlock_t *wrapper = (osalSemaphoreControlBlock_t *)k_malloc(sizeof(osalSemaphoreControlBlock_t));
    if (wrapper == NULL) {
        return NULL;
    }

    // Initialize the Zephyr semaphore
    // k_sem_init(sem, initial_count, limit)
    int ret = k_sem_init(&wrapper->sem, attr->initialCount, attr->maxCount);
    if (ret != 0) {
        k_free(wrapper); // Clean up if initialization fails
        return NULL;
    }

    return (osalSemaphoreHandle_t)wrapper;
}

osalStatus_t osalSemaphoreDelete(osalSemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return OSAL_ERROR_PARAMETER;
    }

    // Zephyr kernel objects don't strictly need "deletion" if they aren't threads,
    // but we must free the memory we allocated for the wrapper.
    k_free(sem);
    return OSAL_SUCCESS;
}

osalStatus_t osalSemaphoreTake(osalSemaphoreHandle_t sem, uint32_t timeoutMs)
{
    if (sem == NULL) {
        return OSAL_ERROR_PARAMETER;
    }

    osalSemaphoreControlBlock_t *wrapper = (osalSemaphoreControlBlock_t *)sem;
    k_timeout_t timeout;

    // Convert OSAL timeout to Zephyr timeout
    if (timeoutMs == OSAL_WAIT_FOREVER) {
        timeout = K_FOREVER;
    } else {
        timeout = K_MSEC(timeoutMs);
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

osalStatus_t osalSemaphoreGive(osalSemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return OSAL_ERROR_PARAMETER;
    }

    osalSemaphoreControlBlock_t *wrapper = (osalSemaphoreControlBlock_t *)sem;
    k_sem_give(&wrapper->sem);
    return OSAL_SUCCESS;
}

uint32_t osalSemaphoreGetCount(osalSemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return 0;
    }

    osalSemaphoreControlBlock_t *wrapper = (osalSemaphoreControlBlock_t *)sem;
    return k_sem_count_get(&wrapper->sem);
}
