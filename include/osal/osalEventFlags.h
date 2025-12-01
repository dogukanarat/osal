/* osalEventFlags.h - Event flag synchronization */

/* All Rights Reserved */

#ifndef INC_OSAL_EVENT_FLAGS_H
#define INC_OSAL_EVENT_FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osalTypes.h"

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
    void *cbMem;      ///< Memory for control block (optional)
    uint32_t cbSize;  ///< Size of control block memory
} osalEventFlagsAttr_t;

typedef void *osalEventFlagsHandle_t;

/* Functions */

/**
 * @brief Create event flags.
 * @param attr Attributes (optional).
 * @return Handle to the created event flags, or NULL on failure.
 */
osalEventFlagsHandle_t osalEventFlagsCreate(const osalEventFlagsAttr_t *attr);

/**
 * @brief Delete event flags.
 * @param flags Handle to the event flags.
 * @return OSAL_SUCCESS on success.
 */
osalStatus_t osalEventFlagsDelete(osalEventFlagsHandle_t flags);

/**
 * @brief Set event flags.
 * @param flags Handle to the event flags.
 * @param flagsToSet Bitmask of flags to set.
 * @return Resulting flags after set.
 */
uint32_t osalEventFlagsSet(osalEventFlagsHandle_t flags, uint32_t flagsToSet);

/**
 * @brief Clear event flags.
 * @param flags Handle to the event flags.
 * @param flagsToClear Bitmask of flags to clear.
 * @return Resulting flags before clear.
 */
uint32_t osalEventFlagsClear(osalEventFlagsHandle_t flags, uint32_t flagsToClear);

/**
 * @brief Get current event flags.
 * @param flags Handle to the event flags.
 * @return Current flags.
 */
uint32_t osalEventFlagsGet(osalEventFlagsHandle_t flags);

/**
 * @brief Wait for event flags.
 * @param flags Handle to the event flags.
 * @param flagsToWait Bitmask of flags to wait for.
 * @param options Wait options (OSAL_EVENT_WAIT_ALL, OSAL_EVENT_NO_CLEAR).
 * @param timeoutMs Timeout in milliseconds.
 * @return Flags that caused the wait to unblock, or error code.
 */
uint32_t osalEventFlagsWait(osalEventFlagsHandle_t flags, uint32_t flagsToWait, uint32_t options, uint32_t timeoutMs);

/**
 * @brief Get current event flags.
 * @param flags Handle to the event flags.
 * @return Current flags.
 */
uint32_t osalEventFlagsGet(osalEventFlagsHandle_t flags);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_EVENT_FLAGS_H */
