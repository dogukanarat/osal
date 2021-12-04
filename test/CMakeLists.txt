if(CMAKE_BUILD_TYPE MATCHES DEBUG)

    add_executable(osal_test main.cpp)

    target_link_libraries(osal_test PRIVATE gtest osal)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

    add_test(test_all osal_test)

endif()


