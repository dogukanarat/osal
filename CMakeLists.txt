# ============================================================================
# Project Configuration
# ============================================================================
cmake_minimum_required(VERSION 3.14)

project(libscaffold
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
add_library(libscaffold)
add_library(libscaffold::libscaffold ALIAS libscaffold)

# Core sources
target_sources(libscaffold
    PRIVATE
        src/libscaffold.c
        src/libscaffoldInternal.c
)

# Include directories
target_include_directories(libscaffold
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compile features
target_compile_features(libscaffold PUBLIC c_std_99)

# Compile options
target_compile_options(libscaffold
    PRIVATE
        $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<C_COMPILER_ID:MSVC>:/W4>
)

# Set library properties
set_target_properties(libscaffold PROPERTIES
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
install(TARGETS libscaffold
    EXPORT libscaffoldTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers
install(DIRECTORY include/libscaffold
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h"
)

# Install CMake targets
install(EXPORT libscaffoldTargets
    FILE libscaffoldTargets.cmake
    NAMESPACE libscaffold::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscaffold
    COMPONENT Development
)

# Generate and install package config files
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/libscaffoldConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/libscaffoldConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscaffold
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/libscaffoldConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libscaffoldConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libscaffoldConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscaffold
    COMPONENT Development
)
