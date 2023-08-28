cmake_minimum_required(VERSION 3.25)
project(canopen_lite)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Include FetchContent module to download GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Add your main executable
add_executable(canopen_lite main.c)

# Add tests
enable_testing()
add_executable(tests tests/simple_test.cc)

# Link tests to GoogleTest libraries
target_link_libraries(tests gtest_main)

# Add tests to ctest
add_test(
        NAME simple_test
        COMMAND tests
)
