/* osalEventFlags.h - Event flag synchronization */

/* All Rights Reserved */

#ifndef INC_OSAL_EVENT_FLAGS_H
#define INC_OSAL_EVENT_FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osal_types.h"

/* Configurations */

/* Definitions */
#define OSAL_EVENT_WAIT_ANY      0x00000000U
#define OSAL_EVENT_WAIT_ALL      0x00000001U
#define OSAL_EVENT_NO_CLEAR      0x00000002U

/* Types */

/**
 * @brief Event Flags attributes
 */
typedef struct
{
    const char *name; ///< Name of the event flags
    void *cb_mem;      ///< Memory for control block (optional)
    uint32_t cb_size;  ///< Size of control block memory
} osal_event_flags_attr_t;

typedef void *osal_event_flags_handle_t;

/* Functions */

/**
 * @brief Create event flags.
 * @param attr Attributes (optional).
 * @return Handle to the created event flags, or NULL on failure.
 */
osal_event_flags_handle_t osal_event_flags_create(const osal_event_flags_attr_t *attr);

/**
 * @brief Delete event flags.
 * @param flags Handle to the event flags.
 * @return OSAL_SUCCESS on success.
 */
osal_status_t osal_event_flags_delete(osal_event_flags_handle_t flags);

/**
 * @brief Set event flags.
 * @param flags Handle to the event flags.
 * @param flags_to_set Bitmask of flags to set.
 * @return Resulting flags after set.
 */
uint32_t osal_event_flags_set(osal_event_flags_handle_t flags, uint32_t flags_to_set);

/**
 * @brief Clear event flags.
 * @param flags Handle to the event flags.
 * @param flags_to_clear Bitmask of flags to clear.
 * @return Resulting flags before clear.
 */
uint32_t osal_event_flags_clear(osal_event_flags_handle_t flags, uint32_t flags_to_clear);

/**
 * @brief Get current event flags.
 * @param flags Handle to the event flags.
 * @return Current flags.
 */
uint32_t osal_event_flags_get(osal_event_flags_handle_t flags);

/**
 * @brief Wait for event flags.
 * @param flags Handle to the event flags.
 * @param flags_to_wait Bitmask of flags to wait for.
 * @param options Wait options (OSAL_EVENT_WAIT_ALL, OSAL_EVENT_NO_CLEAR).
 * @param timeout_ms Timeout in milliseconds.
 * @return Flags that caused the wait to unblock, or error code.
 */
uint32_t osal_event_flags_wait(osal_event_flags_handle_t flags, uint32_t flags_to_wait, uint32_t options, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_EVENT_FLAGS_H */
