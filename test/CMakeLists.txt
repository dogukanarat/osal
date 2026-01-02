# ============================================================================
# Test Configuration
# ============================================================================
cmake_minimum_required(VERSION 3.14)

# ============================================================================
# Fetch Unity Test Framework
# ============================================================================
find_package(unity REQUIRED)

# ============================================================================
# Test Executable
# ============================================================================
if(OSAL_PLATFORM STREQUAL "POSIX" OR OSAL_PLATFORM STREQUAL "MACOS")
    # Use integration tests (no mocking, works on all platforms)
    add_executable(test_osal
        test_osal.c
    )

    find_package(Threads REQUIRED)
    target_link_libraries(test_osal
        PRIVATE
            osal::osal
            unity
            Threads::Threads
    )
    target_include_directories(test_osal
        PRIVATE
            ../include
    )

    add_test(NAME OSAL_Integration_Tests COMMAND test_osal)

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
