include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

enable_testing()

set(${TEST_NAME} ${PROJECT_NAME}-test)

add_executable(
    unittest
    main.cpp
    )

target_link_libraries(
    unittest
    gtest_main
    ${PROJECT_NAME}
    )

gtest_discover_tests(unittest
    PROPERTIES
      LABELS "unit"
    DISCOVERY_TIMEOUT  # how long to wait (in seconds) before crashing
      240
    )
