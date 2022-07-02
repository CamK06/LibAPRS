cmake_minimum_required(VERSION 3.1)
project(LibAPRS)

file(GLOB SOURCES "*.cpp" "**/*.cpp")

add_library(aprs STATIC ${SOURCES})

install(TARGETS aprs LIBRARY DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h")