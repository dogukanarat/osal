/* osalMessageQueue.c - Zephyr Message Queue Implementation */

/* All Rights Reserved */

/* Includes */
#include <osal/osalMessageQueue.h>
#include <zephyr/kernel.h>
#include <stdlib.h>
#include <string.h>

/* Internal Types */
typedef struct
{
    struct k_msgq msgq;    /* Zephyr native message queue */
    void *buffer;          /* Pointer to the buffer memory */
    bool isStaticCb;       /* Flag: Was the CB provided by user? */
    bool isStaticBuf;      /* Flag: Was the buffer provided by user? */
} osalQueueControlBlock_t;

/* Functions */

osalQueueHandle_t osalQueueCreate(uint32_t depth, uint32_t itemSize, const osalQueueAttr_t *attr)
{
    osalQueueControlBlock_t *cb;
    void *buffer;

    if (depth == 0 || itemSize == 0)
    {
        return NULL;
    }

    /* ----------------------------------------------------------------
     * 1. Control Block Allocation
     * ---------------------------------------------------------------- */
    if (attr && attr->cbMem)
    {
        /* Static CB */
        if (attr->cbSize < sizeof(osalQueueControlBlock_t))
        {
            return NULL;
        }
        cb = (osalQueueControlBlock_t *)attr->cbMem;
        cb->isStaticCb = true;
    }
    else
    {
        /* Dynamic CB */
        cb = (osalQueueControlBlock_t *)k_malloc(sizeof(osalQueueControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStaticCb = false;
    }

    /* ----------------------------------------------------------------
     * 2. Buffer Allocation
     * ---------------------------------------------------------------- */
    /* Zephyr k_msgq requires a buffer of size (msg_size * max_msgs)
     * AND it must be aligned to word boundaries.
     */
    uint32_t totalBufferSize = depth * itemSize;

    if (attr && attr->mqMem)
    {
        /* Static Buffer */
        if (attr->mqSize < totalBufferSize)
        {
            if (!cb->isStaticCb) k_free(cb);
            return NULL;
        }
        buffer = attr->mqMem;
        cb->isStaticBuf = true;
    }
    else
    {
        /* Dynamic Buffer */
        /* Using k_aligned_alloc is safer for queue buffers to ensure alignment */
        buffer = k_aligned_alloc(4, totalBufferSize); // 4-byte alignment
        if (buffer == NULL)
        {
            if (!cb->isStaticCb) k_free(cb);
            return NULL;
        }
        cb->isStaticBuf = false;
    }

    /* ----------------------------------------------------------------
     * 3. Initialization
     * ---------------------------------------------------------------- */
    cb->buffer = buffer;

    k_msgq_init(&cb->msgq, cb->buffer, itemSize, depth);

    return (osalQueueHandle_t)cb;
}

osalStatus_t osalQueueDelete(osalQueueHandle_t queue)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Clean up resources */
    /* Zephyr k_msgq does not have a strict "destroy" call,
     * but we must free the memory we allocated.
     */

    if (!cb->isStaticBuf)
    {
        k_free(cb->buffer);
    }

    if (!cb->isStaticCb)
    {
        k_free(cb);
    }

    return OSAL_SUCCESS;
}

osalStatus_t osalQueueSend(osalQueueHandle_t queue, const void *item, uint32_t timeoutMs)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;
    k_timeout_t timeout;
    int ret;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Timeout Mapping */
    if (timeoutMs == OSAL_WAIT_FOREVER)
    {
        timeout = K_FOREVER;
    }
    else if (timeoutMs == OSAL_NO_WAIT)
    {
        timeout = K_NO_WAIT;
    }
    else
    {
        timeout = K_MSEC(timeoutMs);
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

osalStatus_t osalQueueReceive(osalQueueHandle_t queue, void *item, uint32_t timeoutMs)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;
    k_timeout_t timeout;
    int ret;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Timeout Mapping */
    if (timeoutMs == OSAL_WAIT_FOREVER)
    {
        timeout = K_FOREVER;
    }
    else if (timeoutMs == OSAL_NO_WAIT)
    {
        timeout = K_NO_WAIT;
    }
    else
    {
        timeout = K_MSEC(timeoutMs);
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

uint32_t osalQueueGetCount(osalQueueHandle_t queue)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return 0;
    }

    return k_msgq_num_used_get(&cb->msgq);
}

osalStatus_t osalQueueReset(osalQueueHandle_t queue)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* k_msgq_purge discards all pending messages */
    k_msgq_purge(&cb->msgq);

    return OSAL_SUCCESS;
}
