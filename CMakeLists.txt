cmake_minimum_required(VERSION 3.1)
project(LibAPRS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
include_directories(include thirdparty/spdlog/include)

option(BUILD_TEST "Build test program" OFF)

add_subdirectory(thirdparty/spdlog)
add_subdirectory(lib)
if(BUILD_TEST)
    add_subdirectory(test)
endif()