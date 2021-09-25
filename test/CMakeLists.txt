file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true src/*.cpp)
file(GLOB_RECURSE HEADERS LIST_DIRECTORIES true inc/.h)

add_executable(osal_unit_testing ${SOURCES})
target_include_directories(osal_unit_testing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(osal_unit_testing PUBLIC gtest osal)
add_test(test_all osal_unit_testing)
