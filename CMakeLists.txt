cmake_minimum_required(VERSION 3.28)
project(rift)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE RIFT_SOURCES "src/*.cpp")
add_library(rift STATIC ${RIFT_SOURCES})
target_include_directories(rift PUBLIC include)

# Tests
option(RIFT_BUILD_TESTS "Build tests" ON)
if (NOT RIFT_BUILD_TESTS)
    return()
endif()

add_executable(rift_test test/main.cpp)
target_link_libraries(rift_test rift)