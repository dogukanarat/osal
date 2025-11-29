cmake_minimum_required(VERSION 3.14)

project(osal_example LANGUAGES C)

add_executable(osal_example osal_example.c)

target_link_libraries(osal_example PRIVATE osal::osal)
