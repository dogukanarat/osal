#!/bin/bash

# Update all #include statements to use snake_case header names

find include src test example -type f \( -name "*.c" -o -name "*.h" \) -exec sed -i \
    -e 's|"osal/osalEventFlags\.h"|"osal/osal_event_flags.h"|g' \
    -e 's|"osal/osalMemory\.h"|"osal/osal_memory.h"|g' \
    -e 's|"osal/osalMessageQueue\.h"|"osal/osal_message_queue.h"|g' \
    -e 's|"osal/osalMutex\.h"|"osal/osal_mutex.h"|g' \
    -e 's|"osal/osalSemaphore\.h"|"osal/osal_semaphore.h"|g' \
    -e 's|"osal/osalThread\.h"|"osal/osal_thread.h"|g' \
    -e 's|"osal/osalTime\.h"|"osal/osal_time.h"|g' \
    -e 's|"osal/osalTypes\.h"|"osal/osal_types.h"|g' \
    {} +

echo "Include statements updated!"
