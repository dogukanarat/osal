#!/bin/bash
# Script to convert OSAL from camelCase to snake_case

set -e

echo "Converting OSAL to snake_case naming convention..."

# Find all C/H files
FILES=$(find /home/user/osal/include /home/user/osal/src /home/user/osal/test /home/user/osal/example -type f \( -name "*.c" -o -name "*.h" \) 2>/dev/null || true)

for file in $FILES; do
    echo "Processing: $file"

    # Types
    sed -i 's/osalStatus_t/osal_status_t/g' "$file"
    sed -i 's/osalTimeVal_t/osal_time_val_t/g' "$file"
    sed -i 's/osalMutexHandle_t/osal_mutex_handle_t/g' "$file"
    sed -i 's/osalMutexAttr_t/osal_mutex_attr_t/g' "$file"
    sed -i 's/osalSemaphoreHandle_t/osal_semaphore_handle_t/g' "$file"
    sed -i 's/osalSemaphoreAttr_t/osal_semaphore_attr_t/g' "$file"
    sed -i 's/osalThreadHandle_t/osal_thread_handle_t/g' "$file"
    sed -i 's/osalThreadAttr_t/osal_thread_attr_t/g' "$file"
    sed -i 's/osalThreadFunc_t/osal_thread_func_t/g' "$file"
    sed -i 's/osalEventFlagsHandle_t/osal_event_flags_handle_t/g' "$file"
    sed -i 's/osalEventFlagsAttr_t/osal_event_flags_attr_t/g' "$file"
    sed -i 's/osalMessageQueueHandle_t/osal_message_queue_handle_t/g' "$file"
    sed -i 's/osalMessageQueueAttr_t/osal_message_queue_attr_t/g' "$file"

    # Internal types
    sed -i 's/osalMutexControlBlock_t/osal_mutex_control_block_t/g' "$file"
    sed -i 's/osalSemaphoreControlBlock_t/osal_semaphore_control_block_t/g' "$file"
    sed -i 's/osalThreadControlBlock_t/osal_thread_control_block_t/g' "$file"
    sed -i 's/osalEventFlagsControlBlock_t/osal_event_flags_control_block_t/g' "$file"
    sed -i 's/osalQueueControlBlock_t/osal_queue_control_block_t/g' "$file"

    # Memory functions
    sed -i 's/osalMemoryAlloc/osal_memory_alloc/g' "$file"
    sed -i 's/osalMemoryFree/osal_memory_free/g' "$file"

    # Time functions
    sed -i 's/osalDelayMs/osal_delay_ms/g' "$file"
    sed -i 's/osalGetTickMs/osal_get_tick_ms/g' "$file"
    sed -i 's/osalGetUnixTime/osal_get_unix_time/g' "$file"

    # Mutex functions
    sed -i 's/osalMutexCreate/osal_mutex_create/g' "$file"
    sed -i 's/osalMutexDelete/osal_mutex_delete/g' "$file"
    sed -i 's/osalMutexLock/osal_mutex_lock/g' "$file"
    sed -i 's/osalMutexUnlock/osal_mutex_unlock/g' "$file"

    # Semaphore functions
    sed -i 's/osalSemaphoreCreate/osal_semaphore_create/g' "$file"
    sed -i 's/osalSemaphoreDelete/osal_semaphore_delete/g' "$file"
    sed -i 's/osalSemaphoreTake/osal_semaphore_take/g' "$file"
    sed -i 's/osalSemaphoreGive/osal_semaphore_give/g' "$file"
    sed -i 's/osalSemaphoreGetCount/osal_semaphore_get_count/g' "$file"

    # Thread functions
    sed -i 's/osalThreadCreate/osal_thread_create/g' "$file"
    sed -i 's/osalThreadDelete/osal_thread_delete/g' "$file"
    sed -i 's/osalThreadYield/osal_thread_yield/g' "$file"
    sed -i 's/osalThreadGetId/osal_thread_get_id/g' "$file"

    # Event flags functions
    sed -i 's/osalEventFlagsCreate/osal_event_flags_create/g' "$file"
    sed -i 's/osalEventFlagsDelete/osal_event_flags_delete/g' "$file"
    sed -i 's/osalEventFlagsSet/osal_event_flags_set/g' "$file"
    sed -i 's/osalEventFlagsClear/osal_event_flags_clear/g' "$file"
    sed -i 's/osalEventFlagsGet/osal_event_flags_get/g' "$file"
    sed -i 's/osalEventFlagsWait/osal_event_flags_wait/g' "$file"

    # Message queue functions
    sed -i 's/osalMessageQueueCreate/osal_message_queue_create/g' "$file"
    sed -i 's/osalMessageQueueDelete/osal_message_queue_delete/g' "$file"
    sed -i 's/osalMessageQueueSend/osal_message_queue_send/g' "$file"
    sed -i 's/osalMessageQueuePut/osal_message_queue_put/g' "$file"
    sed -i 's/osalMessageQueueReceive/osal_message_queue_receive/g' "$file"
    sed -i 's/osalMessageQueueGetCount/osal_message_queue_get_count/g' "$file"
    sed -i 's/osalMessageQueueGetSpace/osal_message_queue_get_space/g' "$file"
    sed -i 's/osalMessageQueueReset/osal_message_queue_reset/g' "$file"

    # Variables (common patterns)
    sed -i 's/\bmutexAttr\b/mutex_attr/g' "$file"
    sed -i 's/\bthreadAttr\b/thread_attr/g' "$file"
    sed -i 's/\btimebaseInfo\b/timebase_info/g' "$file"
    sed -i 's/\btimebaseInitialized\b/timebase_initialized/g' "$file"
    sed -i 's/\bisStatic\b/is_static/g' "$file"
    sed -i 's/\bisStaticCb\b/is_static_cb/g' "$file"
    sed -i 's/\bisStaticBuf\b/is_static_buf/g' "$file"
    sed -i 's/\binitialCount\b/initial_count/g' "$file"
    sed -i 's/\bmaxCount\b/max_count/g' "$file"
    sed -i 's/\battrBits\b/attr_bits/g' "$file"
    sed -i 's/\bcbMem\b/cb_mem/g' "$file"
    sed -i 's/\bcbSize\b/cb_size/g' "$file"
    sed -i 's/\bstackMem\b/stack_mem/g' "$file"
    sed -i 's/\bstackSize\b/stack_size/g' "$file"
    sed -i 's/\bmqMem\b/mq_mem/g' "$file"
    sed -i 's/\bmqSize\b/mq_size/g' "$file"
    sed -i 's/\bitemSize\b/item_size/g' "$file"
    sed -i 's/\btimeoutMs\b/timeout_ms/g' "$file"
    sed -i 's/\bflagsToSet\b/flags_to_set/g' "$file"
    sed -i 's/\bflagsToClear\b/flags_to_clear/g' "$file"
    sed -i 's/\bflagsToWait\b/flags_to_wait/g' "$file"
    sed -i 's/\bcurrentFlags\b/current_flags/g' "$file"
    sed -i 's/\bconditionMet\b/condition_met/g' "$file"
    sed -i 's/\bwaitAll\b/wait_all/g' "$file"
    sed -i 's/\bnotFull\b/not_full/g' "$file"
    sed -i 's/\bnotEmpty\b/not_empty/g' "$file"
    sed -i 's/\binit_thread_key\b/init_thread_key/g' "$file"
    sed -i 's/\binitTimebase\b/init_timebase/g' "$file"
    sed -i 's/\bthreadWrapper\b/thread_wrapper/g' "$file"

done

echo "Conversion complete!"
