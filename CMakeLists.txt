cmake_minimum_required(VERSION 3.21)
project(rift)

include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#11.0.2")
CPMAddPackage("gh:geode-sdk/result#64792cc")

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE RIFT_SOURCES "src/*.cpp")
add_library(rift STATIC ${RIFT_SOURCES})
target_include_directories(rift PUBLIC include)
target_link_libraries(rift fmt GeodeResult)

# Tests
option(RIFT_BUILD_TESTS "Build tests" OFF)
option(RIFT_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(RIFT_BUILD_FUZZERS "Build fuzzers" OFF)

if (RIFT_BUILD_TESTS)
    add_executable(rift_test test/main.cpp)
    target_link_libraries(rift_test rift)
endif()
