add_subdirectory(src)

include_directories(
    ${CMAKE_PROJECT_NAME}_lib_osal/src
)
target_include_directories(
    ${CMAKE_PROJECT_NAME}_lib_osal
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    )