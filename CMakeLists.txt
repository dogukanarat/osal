# ==============================================================================
# Project Definition
# ==============================================================================
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(Osal
    VERSION 1.0.0
    DESCRIPTION "A scaffold library for CMake projects"
    LANGUAGES C
)

# ==============================================================================
# CMake Modules and Policies
# ==============================================================================
# Include standard CMake modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set custom module path for additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Export compile commands for IDE integration (clangd, ccls, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands" FORCE)

# ==============================================================================
# Build Options
# ==============================================================================
# Option to build shared libraries instead of static (default: OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)

# Initialize TARGET_PLATFORM to an empty string
set(TARGET_PLATFORM "")

# Option to build the test suite (default: OFF)
option(BUILD_TESTS "Build unit tests with Unity framework" OFF)

# Option to build example applications (default: OFF)
option(BUILD_EXAMPLES "Build example applications" OFF)

# ==============================================================================
# Project Configuration
# ==============================================================================
# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    # Set possible build type values for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
    )
endif()

if(TARGET_PLATFORM STREQUAL "")
    # Find target platform
    # Determine the target platform
    if(UNIX AND NOT APPLE)
        set(TARGET_PLATFORM "posix")
    elseif(APPLE)
        set(TARGET_PLATFORM "macos")
    elseif(DEFINED ENV{FREERTOS})
        set(TARGET_PLATFORM "freertos")
    elseif(DEFINED ENV{ZEPHYR_BASE})
        set(TARGET_PLATFORM "zephyr")
    else()
        message(FATAL_ERROR "Unknown or unsupported platform")
    endif()
endif()

# Check if the target platform directory exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET_PLATFORM}")
    message(FATAL_ERROR "Target platform '${TARGET_PLATFORM}' is not supported or does not exist")
endif()

set(TARGET_PLATFORM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET_PLATFORM}")

message(STATUS "Target platform: ${TARGET_PLATFORM}")

# ==============================================================================
# Library Target Definition
# ==============================================================================
# Create the main library target (static or shared based on BUILD_SHARED_LIBS)
add_library(Osal)

# Create an alias for consistent namespacing (allows using Osal::Osal)
add_library(Osal::Osal ALIAS Osal)

# Add source files to the library target
target_sources(Osal
    PRIVATE
        ${TARGET_PLATFORM_PATH}/osal_event_flags.c
        ${TARGET_PLATFORM_PATH}/osal_memory.c
        ${TARGET_PLATFORM_PATH}/osal_message_queue.c
        ${TARGET_PLATFORM_PATH}/osal_mutex.c
        ${TARGET_PLATFORM_PATH}/osal_semaphore.c
        ${TARGET_PLATFORM_PATH}/osal_thread.c
        ${TARGET_PLATFORM_PATH}/osal_time.c
)

# ==============================================================================
# Library Include Directories
# ==============================================================================
# Configure include directories for the library
# PUBLIC: Propagated to targets linking against this library
# BUILD_INTERFACE: Used when building the library
# INSTALL_INTERFACE: Used when library is installed
target_include_directories(Osal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ==============================================================================
# Compiler Features and Options
# ==============================================================================
# Require C99 standard (public requirement propagates to consumers)
target_compile_features(Osal PUBLIC c_std_99)

# Add compiler warnings (private - only when building this library)
target_compile_options(Osal
    PRIVATE
        # GCC and Clang warnings
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
            -Wall           # Enable common warnings
            -Wextra         # Enable extra warnings
            -Wpedantic      # Strict ISO C compliance warnings
        >
        # MSVC warnings
        $<$<C_COMPILER_ID:MSVC>:
            /W4             # Warning level 4
        >
)

target_compile_definitions(ScaffoldProject
    PRIVATE
        SCAFFOLD_PROJECT_EXPORTS
)

# ==============================================================================
# Library Properties
# ==============================================================================
# Set library version and symbol visibility
set_target_properties(Osal
    PROPERTIES
        # Library versioning (for shared libraries)
        VERSION ${PROJECT_VERSION}                  # Full version (e.g., 1.0.0)
        SOVERSION ${PROJECT_VERSION_MAJOR}          # Major version (e.g., 1)

        # Symbol visibility (hide internal symbols in shared libraries)
        C_VISIBILITY_PRESET hidden                  # Hide symbols by default
        VISIBILITY_INLINES_HIDDEN YES               # Hide inline functions

        # Output name (can be customized if needed)
        OUTPUT_NAME "Osal"
)

# ==============================================================================
# Dependencies
# ==============================================================================
# Find and link external dependencies required by the library
# Uncomment and modify as needed for your project

# Example: Math library (commonly needed for mathematical functions)
# find_library(MATH_LIBRARY m)
# if(MATH_LIBRARY)
#     target_link_libraries(Osal PUBLIC ${MATH_LIBRARY})
# endif()

# Example: Threading support
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)
# target_link_libraries(Osal PUBLIC Threads::Threads)

# Example: OpenSSL
# find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
# target_link_libraries(Osal PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# Example: Custom library via pkg-config
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(SOMELIB REQUIRED IMPORTED_TARGET somelibrary>=1.0)
# target_link_libraries(Osal PUBLIC PkgConfig::SOMELIB)

# Note: Use PUBLIC for dependencies that appear in your public headers
#       Use PRIVATE for dependencies only used in implementation files
#       Use INTERFACE for header-only dependencies

# ==============================================================================
# Optional Components
# ==============================================================================
# Enable testing if BUILD_TESTS is ON
if(BUILD_TESTS)
    message(STATUS "Building tests enabled")
    enable_testing()
    add_subdirectory(test)
endif()

# Add example subdirectory if BUILD_EXAMPLES is ON
if(BUILD_EXAMPLES)
    message(STATUS "Building examples enabled")
    add_subdirectory(example)
endif()

# ==============================================================================
# Installation Rules
# ==============================================================================
# Install the library target
# Components: Runtime (shared libs), Development (static libs, headers)
install(
    TARGETS Osal
    EXPORT OsalTargets

    # Shared libraries go to lib/ (Runtime component)
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Runtime
        NAMELINK_COMPONENT Development

    # Static libraries go to lib/ (Development component)
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development

    # Windows DLLs go to bin/ (Runtime component)
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime

    # Public headers are in the include directory
    INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public header files
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/osal
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h"
)

# ==============================================================================
# CMake Package Configuration
# ==============================================================================
# Install CMake targets file (allows find_package to work)
install(
    EXPORT OsalTargets
    FILE OsalTargets.cmake
    NAMESPACE Osal::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Osal
    COMPONENT Development
)

# Generate package config file from template
# This file is used by find_package(Osal)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/OsalConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/OsalConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Osal
)

# Generate package version file
# Ensures version compatibility when using find_package
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/OsalConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion  # 1.x.x is compatible with 1.y.z
)

# Install the generated config files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/OsalConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/OsalConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Osal
    COMPONENT Development
)

# ==============================================================================
# pkg-config Support
# ==============================================================================
# Generate and install pkg-config file for projects using pkg-config
# instead of CMake's find_package

# Generate the .pc file from template
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Osal.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/Osal.pc
    @ONLY
)

# Install the pkg-config file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/Osal.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT Development
)

# ==============================================================================
# Summary
# ==============================================================================
message(STATUS "")
message(STATUS "==================================================")
message(STATUS "  ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "==================================================")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler:        ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  Build shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests:       ${BUILD_TESTS}")
message(STATUS "  Build examples:    ${BUILD_EXAMPLES}")
message(STATUS "  Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Target platform:   ${TARGET_PLATFORM}")
message(STATUS "==================================================")
message(STATUS "")
