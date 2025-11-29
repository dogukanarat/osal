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
    GIT_TAG v2.6.0
)

FetchContent_MakeAvailable(unity)

# ============================================================================
# Test Executable
# ============================================================================
add_executable(test_libscaffold
    test_libscaffold.c
)

# Link against the library and Unity
target_link_libraries(test_libscaffold
    PRIVATE
        libscaffold::libscaffold
        unity
)

# Include Unity headers
target_include_directories(test_libscaffold
    PRIVATE
        ${unity_SOURCE_DIR}/src
)

# ============================================================================
# Add test to CTest
# ============================================================================
add_test(
    NAME test_libscaffold
    COMMAND test_libscaffold
)

# Set test properties
set_tests_properties(test_libscaffold
    PROPERTIES
        TIMEOUT 30
)
