/* osalEventFlags.c - macOS Event Flags Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_event_flags.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    uint32_t flags;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool is_static;
} osal_event_flags_control_block_t;

/* Functions */

osal_event_flags_handle_t osal_event_flags_create(const osal_event_flags_attr_t *attr)
{
    osal_event_flags_control_block_t *cb;

    /* Allocate control block */
    if (attr && attr->cb_mem)
    {
        if (attr->cb_size < sizeof(osal_event_flags_control_block_t))
        {
            return NULL;
        }
        cb = (osal_event_flags_control_block_t *)attr->cb_mem;
        cb->is_static = true;
    }
    else
    {
        cb = (osal_event_flags_control_block_t *)malloc(sizeof(osal_event_flags_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static = false;
    }

    cb->flags = 0;
    pthread_mutex_init(&cb->mutex, NULL);
    pthread_cond_init(&cb->cond, NULL);

    return (osal_event_flags_handle_t)cb;
}

osal_status_t osal_event_flags_delete(osal_event_flags_handle_t flags)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_destroy(&cb->mutex);
    pthread_cond_destroy(&cb->cond);

    if (!cb->is_static)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

uint32_t osal_event_flags_set(osal_event_flags_handle_t flags, uint32_t flags_to_set)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;
    uint32_t current_flags;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);
    cb->flags |= flags_to_set;
    current_flags = cb->flags;
    pthread_cond_broadcast(&cb->cond);
    pthread_mutex_unlock(&cb->mutex);

    return current_flags;
}

uint32_t osal_event_flags_clear(osal_event_flags_handle_t flags, uint32_t flags_to_clear)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;
    uint32_t current_flags;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);
    current_flags = cb->flags;
    cb->flags &= ~flags_to_clear;
    pthread_mutex_unlock(&cb->mutex);

    return current_flags;
}

uint32_t osal_event_flags_wait(osal_event_flags_handle_t flags, uint32_t flags_to_wait, uint32_t options, uint32_t timeout_ms)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;
    struct timespec ts;
    struct timeval tv;
    uint32_t rflags = 0;
    int ret = 0;
    bool wait_all = (options & OSAL_EVENT_WAIT_ALL) != 0;
    bool clear = (options & OSAL_EVENT_NO_CLEAR) == 0;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);

    while (1)
    {
        uint32_t current_flags = cb->flags & flags_to_wait;
        bool condition_met = false;

        if (wait_all)
        {
            if (current_flags == flags_to_wait)
            {
                condition_met = true;
            }
        }
        else
        {
            if (current_flags != 0)
            {
                condition_met = true;
            }
        }

        if (condition_met)
        {
            rflags = current_flags;
            if (clear)
            {
                cb->flags &= ~current_flags;
            }
            break;
        }

        if (timeout_ms == OSAL_NO_WAIT)
        {
            ret = ETIMEDOUT;
            break;
        }
        else if (timeout_ms == OSAL_WAIT_FOREVER)
        {
            pthread_cond_wait(&cb->cond, &cb->mutex);
        }
        else
        {
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (timeout_ms / 1000);
            ts.tv_nsec = (tv.tv_usec * 1000) + ((timeout_ms % 1000) * 1000000);
            if (ts.tv_nsec >= 1000000000)
            {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }
            ret = pthread_cond_timedwait(&cb->cond, &cb->mutex, &ts);
            if (ret == ETIMEDOUT)
            {
                break;
            }
        }
    }

    pthread_mutex_unlock(&cb->mutex);

    if (ret == ETIMEDOUT)
    {
        return 0;
    }

    return rflags;
}

uint32_t osal_event_flags_get(osal_event_flags_handle_t flags)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;
    uint32_t current_flags;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);
    current_flags = cb->flags;
    pthread_mutex_unlock(&cb->mutex);

    return current_flags;
}
