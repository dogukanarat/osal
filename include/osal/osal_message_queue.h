/* osalMessageQueue.h - Message queue communication */

/* All Rights Reserved */

#ifndef INC_OSAL_MESSAGE_QUEUE_H
#define INC_OSAL_MESSAGE_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */

#include "osal_types.h"

/* Configurations */

/* Definitions */

/* Types */

/**
 * @brief Message Queue attributes
 */
typedef struct
{
    const char *name; ///< Name of the queue
    void *cb_mem;      ///< Memory for control block (optional)
    uint32_t cb_size;  ///< Size of control block memory
    void *mq_mem;      ///< Memory for message buffer (optional)
    uint32_t mq_size;  ///< Size of message buffer memory
} osal_message_queue_attr_t;

typedef void *osal_message_queue_handle_t;

/* Functions */

/**
 * @brief Create a message queue.
 * @param depth Maximum number of items in the queue.
 * @param item_size Size of each item in bytes.
 * @param attr Queue attributes (optional).
 * @return Handle to the created queue, or NULL on failure.
 */
osal_message_queue_handle_t osal_message_queue_create(uint32_t depth, uint32_t item_size, const osal_message_queue_attr_t *attr);

/**
 * @brief Delete a message queue.
 * @param queue Handle to the queue.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_message_queue_delete(osal_message_queue_handle_t queue);

/**
 * @brief Send an item to a queue.
 * @param queue Handle to the queue.
 * @param item Pointer to the item to send.
 * @param timeout_ms Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT (or FULL) on timeout.
 */
osal_status_t osal_message_queue_send(osal_message_queue_handle_t queue, const void *item, uint32_t timeout_ms);

/**
 * @brief Receive an item from a queue.
 * @param queue Handle to the queue.
 * @param item Pointer to the buffer to store the received item.
 * @param timeout_ms Timeout in milliseconds.
 * @return OSAL_SUCCESS on success, OSAL_ERROR_TIMEOUT (or EMPTY) on timeout.
 */
osal_status_t osal_message_queue_receive(osal_message_queue_handle_t queue, void *item, uint32_t timeout_ms);

/**
 * @brief Get number of items in the queue.
 * @param queue Handle to the queue.
 * @return Number of items.
 */
uint32_t osal_message_queue_get_count(osal_message_queue_handle_t queue);

/**
 * @brief Reset the queue (clear all messages).
 * @param queue Handle to the queue.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_message_queue_reset(osal_message_queue_handle_t queue);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_MESSAGE_QUEUE_H */
