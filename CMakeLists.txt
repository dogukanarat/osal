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
# Library Target
# ============================================================================
add_library(osal)
add_library(osal::osal ALIAS osal)

# Core sources
target_sources(osal
    PRIVATE
        src/osal.c
        src/osalInternal.c
)

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
