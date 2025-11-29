# ============================================================================
# Test Configuration
# ============================================================================
cmake_minimum_required(VERSION 3.14)

# ============================================================================
# Fetch Unity Test Framework
# ============================================================================
include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.5.2
)
FetchContent_MakeAvailable(unity)

# ============================================================================
# Test Executable
# ============================================================================
if(OSAL_PLATFORM STREQUAL "POSIX")
    set(TEST_SOURCE
        ../src/posix/osalTime.c
        ../src/posix/osalMutex.c
        ../src/posix/osalSemaphore.c
        ../src/posix/osalMessageQueue.c
        ../src/posix/osalThread.c
    )

    add_executable(test_posix
        test_posix.c
        ${TEST_SOURCE}
    )

    target_link_libraries(test_posix
        PRIVATE
            unity
    )
    target_include_directories(test_posix
        PRIVATE
            ../include
    )

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

# ============================================================================
# Test Executable
# ============================================================================
elseif(OSAL_PLATFORM STREQUAL "FREERTOS")
    add_executable(test_freertos
        test_freertos.c
    )

    target_link_libraries(test_freertos
        PRIVATE
        osal::osal
    )
    target_include_directories(test_freertos
        PRIVATE
            ../include
            include # For mock FreeRTOS headers
            .
    )

    target_compile_definitions(test_freertos
        PRIVATE
            OSAL_PLATFORM_FREERTOS
    )

    add_test(NAME OSAL_FreeRTOS_Tests COMMAND test_freertos)
endif()
