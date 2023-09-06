cmake_minimum_required(VERSION 3.25)
project(canopen_lite)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Include FetchContent module to download GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
)
FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG        v0.5.0  # Adjust the version/tag as needed
)

FetchContent_MakeAvailable(googletest glog)

add_subdirectory(vendor/fmt)
#add_subdirectory(vendor/glog)
#find_package(glog)

# Add your main executable
add_executable(canopen_main
        main.cpp)

target_link_libraries(canopen_main PRIVATE
        canopen_lite_lib
        glog::glog
)

# Add library for canopen_lite
add_library(canopen_lite_lib
        od/od_array.cpp
        od/od_record.cpp
        od/od_variable.cpp
        od/od_object_dictionary.cpp
        od/od_data.cpp
        od/eds.cpp
        network/socketcan.cpp
)
target_include_directories(canopen_lite_lib PUBLIC
        .
        vendor/simpleini
        vendor/fmt/include
)
target_link_libraries(canopen_lite_lib PRIVATE
        fmt::fmt
        glog::glog
)

# Add tests
enable_testing()
add_executable(tests
        tests/eds_test.cc
        tests/socketcan_network_test.cpp
)

target_include_directories(tests PRIVATE
        .
        vendor/fmt/include
)
target_link_libraries(tests
        canopen_lite_lib
        fmt::fmt
        glog::glog
        gtest_main
)
target_compile_definitions(tests PRIVATE
        PROJECT_DIR=\"${CMAKE_SOURCE_DIR}\"
)

# Add tests to ctest
add_test(
        NAME eds_test
        COMMAND tests
)

function(print_target_details target)
    get_target_property(includes ${target} INCLUDE_DIRECTORIES)
    get_target_property(link_libs ${target} LINK_LIBRARIES)

    if (NOT includes STREQUAL "includes-NOTFOUND")
        message(STATUS "Target ${target} includes: ${includes}")
    endif()

    if (NOT link_libs STREQUAL "link_libs-NOTFOUND")
        message(STATUS "Target ${target} linked libraries: ${link_libs}")
    endif()
endfunction()

function(print_all_targets_recursive dir)
    get_property(subdirs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirs})
        print_all_targets_recursive(${subdir})
    endforeach()

    get_property(targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target ${targets})
        message(STATUS "Target: ${target}")
        print_target_details(${target})
    endforeach()
endfunction()

#print_all_targets_recursive(${CMAKE_SOURCE_DIR})

get_target_property(GLOG_INCLUDE_DIRS glog::glog INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "GLOG include dirs: ${GLOG_INCLUDE_DIRS}")

get_target_property(CANOPEN_INCLUDE_DIRS canopen_main INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "CANOPEN include dirs: ${CANOPEN_INCLUDE_DIRS}")
