file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.hpp *.cpp)

set(SOURCES ${SOURCES})

add_library(
    ${CMAKE_PROJECT_NAME}_lib_osal ${SOURCES}
    )