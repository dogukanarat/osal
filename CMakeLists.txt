add_subdirectory(
    src
)

include_directories(
    src
)

target_include_directories(
    osal
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)