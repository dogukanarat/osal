/* osalMutex.c - POSIX Mutex Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_mutex.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    pthread_mutex_t mutex;
    bool is_static;
} osal_mutex_control_block_t;

/* Functions */

osal_mutex_handle_t osal_mutex_create(const osal_mutex_attr_t *attr)
{
    osal_mutex_control_block_t *cb;
    pthread_mutexattr_t mutex_attr;
    int ret;

    /* Allocate control block */
    if (attr && attr->cb_mem)
    {
        if (attr->cb_size < sizeof(osal_mutex_control_block_t))
        {
            return NULL;
        }
        cb = (osal_mutex_control_block_t *)attr->cb_mem;
        cb->is_static = true;
    }
    else
    {
        cb = (osal_mutex_control_block_t *)malloc(sizeof(osal_mutex_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static = false;
    }

    /* Initialize mutex attributes */
    pthread_mutexattr_init(&mutex_attr);

    /* Handle attributes */
    if (attr && (attr->attr_bits & OSAL_MUTEX_RECURSIVE))
    {
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    }
    else
    {
        /* Default to recursive for general robustness if not specified,
           or normal if strictly following bits. Let's follow bits but
           note that many OSALs default to recursive. */
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_NORMAL);
    }

    /* Initialize mutex */
    ret = pthread_mutex_init(&cb->mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    if (ret != 0)
    {
        if (!cb->is_static)
        {
            free(cb);
        }
        return NULL;
    }

    return (osal_mutex_handle_t)cb;
}

osal_status_t osal_mutex_delete(osal_mutex_handle_t mutex)
{
    osal_mutex_control_block_t *cb = (osal_mutex_control_block_t *)mutex;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_destroy(&cb->mutex);

    if (!cb->is_static)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

osal_status_t osal_mutex_lock(osal_mutex_handle_t mutex, uint32_t timeout_ms)
{
    osal_mutex_control_block_t *cb = (osal_mutex_control_block_t *)mutex;
    struct timespec ts;
    int ret;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (timeout_ms == OSAL_WAIT_FOREVER)
    {
        ret = pthread_mutex_lock(&cb->mutex);
    }
    else if (timeout_ms == OSAL_NO_WAIT)
    {
        ret = pthread_mutex_trylock(&cb->mutex);
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
        ret = pthread_mutex_timedlock(&cb->mutex, &ts);
    }

    if (ret == 0)
    {
        return OSAL_SUCCESS;
    }
    else if (ret == ETIMEDOUT || ret == EBUSY)
    {
        return OSAL_ERROR_TIMEOUT;
    }
    else
    {
        return OSAL_ERROR;
    }
}

osal_status_t osal_mutex_unlock(osal_mutex_handle_t mutex)
{
    osal_mutex_control_block_t *cb = (osal_mutex_control_block_t *)mutex;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (pthread_mutex_unlock(&cb->mutex) != 0)
    {
        return OSAL_ERROR;
    }

    return OSAL_SUCCESS;
}
