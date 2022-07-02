cmake_minimum_required(VERSION 3.1)
project(LibAPRS)

file(GLOB SOURCES "*.cpp")

add_executable(aprstest ${SOURCES})
target_link_libraries(aprstest aprs)