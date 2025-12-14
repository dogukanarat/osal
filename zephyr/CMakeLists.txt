# Define the library
zephyr_library()

# Add the Zephyr port sources you just created
zephyr_library_sources(
    ../src/zephyr/osalTime.c
    ../src/zephyr/osalSemaphore.c
    ../src/zephyr/osalMutex.c
    ../src/zephyr/osalMessageQueue.c
    ../src/zephyr/osalEventFlags.c
    ../src/zephyr/osalThread.c
)

# Add include directories (publicly accessible to other modules)
zephyr_include_directories(../include)

# Define this as an interface library so DANP can link against it
zephyr_interface_library_named(osal)
