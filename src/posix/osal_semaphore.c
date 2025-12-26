/* osalSemaphore.c - POSIX Semaphore Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_semaphore.h"
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    sem_t sem;
    bool is_static;
} osal_semaphore_control_block_t;

/* Functions */

osal_semaphore_handle_t osal_semaphore_create(const osal_semaphore_attr_t *attr)
{
    osal_semaphore_control_block_t *cb;
    uint32_t initial_count = 0;
    int ret;

    if (attr)
    {
        initial_count = attr->initial_count;
    }

    /* Allocate control block */
    if (attr && attr->cb_mem)
    {
        if (attr->cb_size < sizeof(osal_semaphore_control_block_t))
        {
            return NULL;
        }
        cb = (osal_semaphore_control_block_t *)attr->cb_mem;
        cb->is_static = true;
    }
    else
    {
        cb = (osal_semaphore_control_block_t *)malloc(sizeof(osal_semaphore_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static = false;
    }

    /* Initialize semaphore */
    ret = sem_init(&cb->sem, 0, initial_count);

    if (ret != 0)
    {
        if (!cb->is_static)
        {
            free(cb);
        }
        return NULL;
    }

    return (osal_semaphore_handle_t)cb;
}

osal_status_t osal_semaphore_delete(osal_semaphore_handle_t sem)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    sem_destroy(&cb->sem);

    if (!cb->is_static)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

osal_status_t osal_semaphore_take(osal_semaphore_handle_t sem, uint32_t timeout_ms)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;
    struct timespec ts;
    int ret;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (timeout_ms == OSAL_WAIT_FOREVER)
    {
        ret = sem_wait(&cb->sem);
    }
    else if (timeout_ms == OSAL_NO_WAIT)
    {
        ret = sem_trywait(&cb->sem);
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        ret = sem_timedwait(&cb->sem, &ts);
    }

    if (ret == 0)
    {
        return OSAL_SUCCESS;
    }
    else if (errno == ETIMEDOUT || errno == EAGAIN)
    {
        return OSAL_ERROR_TIMEOUT;
    }
    else
    {
        return OSAL_ERROR;
    }
}

osal_status_t osal_semaphore_give(osal_semaphore_handle_t sem)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (sem_post(&cb->sem) != 0)
    {
        return OSAL_ERROR;
    }

    return OSAL_SUCCESS;
}

uint32_t osal_semaphore_get_count(osal_semaphore_handle_t sem)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;
    int val;

    if (cb == NULL)
    {
        return 0;
    }

    if (sem_getvalue(&cb->sem, &val) != 0)
    {
        return 0;
    }

    return (uint32_t)val;
}
