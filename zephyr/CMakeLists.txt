# Define the library
zephyr_library()

# Add the Zephyr port sources you just created
zephyr_library_sources(
    ../src/zephyr/osal_time.c
    ../src/zephyr/osal_semaphore.c
    ../src/zephyr/osal_mutex.c
    ../src/zephyr/osal_message_queue.c
    ../src/zephyr/osal_event_flags.c
    ../src/zephyr/osal_thread.c
    ../src/zephyr/osal_memory.c
)

# Add include directories (publicly accessible to other modules)
zephyr_include_directories(../include)

# Define this as an interface library so DANP can link against it
zephyr_interface_library_named(osal)
