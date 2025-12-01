/* osalMessageQueue.h - Message queue communication */

/* All Rights Reserved */

#ifndef INC_OSAL_MESSAGE_QUEUE_H
#define INC_OSAL_MESSAGE_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osalTypes.h"

/* Configurations */

/* Definitions */

/* Types */

/**
 * @brief Message Queue attributes
 */
typedef struct
{
    const char *name; ///< Name of the queue
    void *cbMem;      ///< Memory for control block (optional)
    uint32_t cbSize;  ///< Size of control block memory
    void *mqMem;      ///< Memory for message buffer (optional)
    uint32_t mqSize;  ///< Size of message buffer memory
} osalMessageQueueAttr_t;

/* Functions */

/**
 * @brief Create a message queue.
 * @param depth Maximum number of items in the queue.
 * @param itemSize Size of each item in bytes.
 * @param attr Queue attributes (optional).
 * @return Handle to the created queue, or NULL on failure.
 */
osalQueueHandle_t osalMessageQueueCreate(uint32_t depth, uint32_t itemSize, const osalMessageQueueAttr_t *attr);

/**
 * @brief Delete a message queue.
 * @param queue Handle to the queue.
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalMessageQueueDelete(osalQueueHandle_t queue);

/**
 * @brief Send an item to a queue.
 * @param queue Handle to the queue.
 * @param item Pointer to the item to send.
 * @param timeoutMs Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT (or FULL) on timeout.
 */
osalStatus_t osalMessageQueueSend(osalQueueHandle_t queue, const void *item, uint32_t timeoutMs);

/**
 * @brief Receive an item from a queue.
 * @param queue Handle to the queue.
 * @param item Pointer to the buffer to store the received item.
 * @param timeoutMs Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT (or EMPTY) on timeout.
 */
osalStatus_t osalMessageQueueReceive(osalQueueHandle_t queue, void *item, uint32_t timeoutMs);

/**
 * @brief Get number of items in the queue.
 * @param queue Handle to the queue.
 * @return Number of items.
 */
uint32_t osalMessageQueueGetCount(osalQueueHandle_t queue);

/**
 * @brief Reset the queue (clear all messages).
 * @param queue Handle to the queue.
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalMessageQueueReset(osalQueueHandle_t queue);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_MESSAGE_QUEUE_H */
