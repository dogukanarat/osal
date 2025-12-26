/* osalMutex.c - macOS Mutex Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_mutex.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

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
        /* macOS doesn't have pthread_mutex_timedlock, use polling approach */
        struct timeval start, now;
        uint32_t elapsed;

        gettimeofday(&start, NULL);

        while (1)
        {
            ret = pthread_mutex_trylock(&cb->mutex);
            if (ret == 0)
            {
                break; /* Successfully acquired */
            }

            gettimeofday(&now, NULL);
            elapsed = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;

            if (elapsed >= timeout_ms)
            {
                ret = ETIMEDOUT;
                break;
            }

            /* Sleep for a short time before retrying (1ms) */
            usleep(1000);
        }
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
