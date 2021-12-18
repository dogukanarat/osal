cmake_minimum_required(VERSION 3.15)

# add_subdirectory(library)

set(CMAKE_CXX_FLAGS "-g -Wall")

project(osal 
    VERSION 1.1.0
    LANGUAGES CXX
    )

add_library(
    ${PROJECT_NAME}
    )

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        src/os_chrono.cpp
        src/os_memory.cpp
        src/os_socket.cpp
        src/os_stream.cpp
        src/os_thread.cpp
        src/os_timer.cpp
    )

include(GNUInstallDirs)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES DEBUG_POSTFIX "d")

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    EXPORT ${PROJECT_NAME}-config
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/cmake/
    )

install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    )

# add_subdirectory(test)
