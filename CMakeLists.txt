# ============================================================================
# Project Configuration
# ============================================================================
cmake_minimum_required(VERSION 3.14)

project(osal
    VERSION 1.0.0
    DESCRIPTION "A scaffold library for CMake projects"
    LANGUAGES C
)

# ============================================================================
# Build Options
# ============================================================================
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build test suite" OFF)
option(BUILD_EXAMPLES "Build example applications" OFF)

# ============================================================================
# Export compile commands for IDE integration
# ============================================================================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Platform Selection
# ============================================================================
set(OSAL_PLATFORM "AUTO" CACHE STRING "Target platform (AUTO, POSIX, FREERTOS)")

if(OSAL_PLATFORM STREQUAL "AUTO")
    if(UNIX AND NOT APPLE) # Linux
        set(OSAL_PLATFORM "POSIX")
    elseif(APPLE) # macOS
        set(OSAL_PLATFORM "POSIX")
    elseif(DEFINED ENV{FREERTOS_KERNEL_PATH} OR DEFINED FREERTOS_KERNEL_PATH)
        set(OSAL_PLATFORM "FREERTOS")
    else()
        message(STATUS "OSAL: Defaulting to POSIX")
        set(OSAL_PLATFORM "POSIX")
    endif()
endif()

message(STATUS "OSAL: Selected platform: ${OSAL_PLATFORM}")

# ============================================================================
# Library Target
# ============================================================================
add_library(osal)
add_library(osal::osal ALIAS osal)



# Platform sources
if(OSAL_PLATFORM STREQUAL "POSIX")
    target_sources(osal
        PRIVATE
            src/posix/osalTime.c
            src/posix/osalMutex.c
            src/posix/osalSemaphore.c
            src/posix/osalMessageQueue.c
            src/posix/osalThread.c
            src/posix/osalEventFlags.c
    )
    find_package(Threads REQUIRED)
    target_link_libraries(osal PRIVATE Threads::Threads)
    target_compile_definitions(osal PRIVATE OSAL_PLATFORM_POSIX _GNU_SOURCE)

elseif(OSAL_PLATFORM STREQUAL "FREERTOS")
    target_sources(osal
        PRIVATE
            src/freertos/osalTime.c
            src/freertos/osalMutex.c
            src/freertos/osalSemaphore.c
            src/freertos/osalMessageQueue.c
            src/freertos/osalThread.c
            src/freertos/osalEventFlags.c
    )
    # User must provide FreeRTOS include paths and library via other means
    # or we can try to find it if a standard package exists (rare for embedded)
    target_compile_definitions(osal PRIVATE OSAL_PLATFORM_FREERTOS)
endif()

# Include directories
target_include_directories(osal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compile features
target_compile_features(osal PUBLIC c_std_99)

# Compile options
target_compile_options(osal
    PRIVATE
        $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<C_COMPILER_ID:MSVC>:/W4>
)

# Set library properties
set_target_properties(osal PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

# ============================================================================
# Optional Components
# ============================================================================
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

# ============================================================================
# Installation
# ============================================================================
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install library
install(TARGETS osal
    EXPORT osalTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers
install(DIRECTORY include/osal
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h"
)

# Install CMake targets
install(EXPORT osalTargets
    FILE osalTargets.cmake
    NAMESPACE osal::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osal
    COMPONENT Development
)

# Generate and install package config files
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/osalConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/osalConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osal
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/osalConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/osalConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/osalConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/osal
    COMPONENT Development
)
