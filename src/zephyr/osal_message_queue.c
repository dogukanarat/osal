/* osalMessageQueue.c - Zephyr Message Queue Implementation */

/* All Rights Reserved */

/* Includes */
#include <osal/osal_message_queue.h>
#include <zephyr/kernel.h>
#include <stdlib.h>
#include <string.h>

/* Internal Types */
typedef struct
{
    struct k_msgq msgq;    /* Zephyr native message queue */
    void *buffer;          /* Pointer to the buffer memory */
    bool is_static_cb;       /* Flag: Was the CB provided by user? */
    bool is_static_buf;      /* Flag: Was the buffer provided by user? */
} osalMessageQueueControlBlock_t;

/* Functions */

osal_message_queue_handle_t osal_message_queue_create(uint32_t depth, uint32_t item_size, const osal_message_queue_attr_t *attr)
{
    osalMessageQueueControlBlock_t *cb;
    void *buffer;

    if (depth == 0 || item_size == 0)
    {
        return NULL;
    }

    /* ----------------------------------------------------------------
     * 1. Control Block Allocation
     * ---------------------------------------------------------------- */
    if (attr && attr->cb_mem)
    {
        /* Static CB */
        if (attr->cb_size < sizeof(osalMessageQueueControlBlock_t))
        {
            return NULL;
        }
        cb = (osalMessageQueueControlBlock_t *)attr->cb_mem;
        cb->is_static_cb = true;
    }
    else
    {
        /* Dynamic CB */
        cb = (osalMessageQueueControlBlock_t *)k_malloc(sizeof(osalMessageQueueControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static_cb = false;
    }

    /* ----------------------------------------------------------------
     * 2. Buffer Allocation
     * ---------------------------------------------------------------- */
    /* Zephyr k_msgq requires a buffer of size (msg_size * max_msgs)
     * AND it must be aligned to word boundaries.
     */
    uint32_t totalBufferSize = depth * item_size;

    if (attr && attr->mq_mem)
    {
        /* Static Buffer */
        if (attr->mq_size < totalBufferSize)
        {
            if (!cb->is_static_cb) k_free(cb);
            return NULL;
        }
        buffer = attr->mq_mem;
        cb->is_static_buf = true;
    }
    else
    {
        /* Dynamic Buffer */
        /* Using k_aligned_alloc is safer for queue buffers to ensure alignment */
        buffer = k_aligned_alloc(4, totalBufferSize); // 4-byte alignment
        if (buffer == NULL)
        {
            if (!cb->is_static_cb) k_free(cb);
            return NULL;
        }
        cb->is_static_buf = false;
    }

    /* ----------------------------------------------------------------
     * 3. Initialization
     * ---------------------------------------------------------------- */
    cb->buffer = buffer;

    k_msgq_init(&cb->msgq, cb->buffer, item_size, depth);

    return (osal_message_queue_handle_t)cb;
}

osal_status_t osal_message_queue_delete(osal_message_queue_handle_t queue)
{
    osalMessageQueueControlBlock_t *cb = (osalMessageQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Clean up resources */
    /* Zephyr k_msgq does not have a strict "destroy" call,
     * but we must free the memory we allocated.
     */

    if (!cb->is_static_buf)
    {
        k_free(cb->buffer);
    }

    if (!cb->is_static_cb)
    {
        k_free(cb);
    }

    return OSAL_SUCCESS;
}

osal_status_t osal_message_queue_send(osal_message_queue_handle_t queue, const void *item, uint32_t timeout_ms)
{
    osalMessageQueueControlBlock_t *cb = (osalMessageQueueControlBlock_t *)queue;
    k_timeout_t timeout;
    int ret;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Timeout Mapping */
    if (timeout_ms == OSAL_WAIT_FOREVER)
    {
        timeout = K_FOREVER;
    }
    else if (timeout_ms == OSAL_NO_WAIT)
    {
        timeout = K_NO_WAIT;
    }
    else
    {
        timeout = K_MSEC(timeout_ms);
    }

    /* Zephyr k_msgq_put copies the data into the queue buffer */
    ret = k_msgq_put(&cb->msgq, item, timeout);

    if (ret == 0)
    {
        return OSAL_SUCCESS;
    }
    else if (ret == -EAGAIN)
    {
        return OSAL_ERROR_TIMEOUT; /* Queue is full */
    }
    else
    {
        return OSAL_ERROR;
    }
}

osal_status_t osal_message_queue_receive(osal_message_queue_handle_t queue, void *item, uint32_t timeout_ms)
{
    osalMessageQueueControlBlock_t *cb = (osalMessageQueueControlBlock_t *)queue;
    k_timeout_t timeout;
    int ret;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Timeout Mapping */
    if (timeout_ms == OSAL_WAIT_FOREVER)
    {
        timeout = K_FOREVER;
    }
    else if (timeout_ms == OSAL_NO_WAIT)
    {
        timeout = K_NO_WAIT;
    }
    else
    {
        timeout = K_MSEC(timeout_ms);
    }

    /* Zephyr k_msgq_get copies data from the queue buffer to 'item' */
    ret = k_msgq_get(&cb->msgq, item, timeout);

    if (ret == 0)
    {
        return OSAL_SUCCESS;
    }
    else if (ret == -EAGAIN)
    {
        return OSAL_ERROR_TIMEOUT; /* Queue is empty */
    }
    else
    {
        return OSAL_ERROR;
    }
}

uint32_t osal_message_queue_get_count(osal_message_queue_handle_t queue)
{
    osalMessageQueueControlBlock_t *cb = (osalMessageQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return 0;
    }

    return k_msgq_num_used_get(&cb->msgq);
}

osal_status_t osal_message_queue_reset(osal_message_queue_handle_t queue)
{
    osalMessageQueueControlBlock_t *cb = (osalMessageQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* k_msgq_purge discards all pending messages */
    k_msgq_purge(&cb->msgq);

    return OSAL_SUCCESS;
}
