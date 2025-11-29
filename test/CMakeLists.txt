cmake_minimum_required(VERSION 3.14)

project(osal_tests LANGUAGES C)

# --- POSIX Test Suite ---
add_executable(test_posix test_posix.c)

# Link against the main OSAL library (configured for POSIX on host)
target_link_libraries(test_posix PRIVATE osal::osal)
target_include_directories(test_posix PRIVATE ../include)

# Wrap system calls for POSIX mocking
target_link_options(test_posix PRIVATE
    -Wl,--wrap=pthread_mutex_lock
    -Wl,--wrap=pthread_mutex_unlock
    -Wl,--wrap=pthread_mutex_init
    -Wl,--wrap=pthread_mutex_destroy
    -Wl,--wrap=clock_gettime
    -Wl,--wrap=nanosleep
    -Wl,--wrap=sem_init
    -Wl,--wrap=sem_wait
    -Wl,--wrap=sem_timedwait
    -Wl,--wrap=sem_post
    -Wl,--wrap=sem_destroy
    -Wl,--wrap=pthread_create
    -Wl,--wrap=pthread_join
    -Wl,--wrap=pthread_cond_init
    -Wl,--wrap=pthread_cond_wait
    -Wl,--wrap=pthread_cond_timedwait
    -Wl,--wrap=pthread_cond_signal
    -Wl,--wrap=pthread_cond_broadcast
    -Wl,--wrap=pthread_cond_destroy
)

add_test(NAME OSAL_POSIX_Tests COMMAND test_posix)

# --- FreeRTOS Test Suite ---
# Since we can't link against the real FreeRTOS kernel easily, we compile the FreeRTOS
# implementation sources directly and link them with a mock FreeRTOS kernel.

set(OSAL_FREERTOS_SOURCES
    ../src/freertos/osalMutex.c
    ../src/freertos/osalSemaphore.c
    ../src/freertos/osalMessageQueue.c
    ../src/freertos/osalThread.c
    ../src/freertos/osalTime.c
    ../src/freertos/osalEventFlags.c
)

add_executable(test_freertos
    test_freertos.c
    ${OSAL_FREERTOS_SOURCES}
)

target_include_directories(test_freertos PRIVATE
    ../include
    include # For mock FreeRTOS headers
    .
)

# We need to define OSAL_PLATFORM_FREERTOS for these sources
target_compile_definitions(test_freertos PRIVATE OSAL_PLATFORM_FREERTOS)

add_test(NAME OSAL_FreeRTOS_Tests COMMAND test_freertos)
