cmake_minimum_required(VERSION 3.21)
project(rift)

# std::format is not available in GNU++20?
include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#10.2.1")

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE RIFT_SOURCES "src/*.cpp")
add_library(rift STATIC ${RIFT_SOURCES})
target_include_directories(rift PUBLIC include)
target_link_libraries(rift fmt)

# Tests
option(RIFT_BUILD_TESTS "Build tests" OFF)
if (NOT RIFT_BUILD_TESTS)
    return()
endif()

add_executable(rift_test test/main.cpp)
target_link_libraries(rift_test rift)
