cmake_minimum_required(VERSION 3.24)

project(his)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/Head)

file(GLOB PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/Source/*.cpp" "${CMAKE_SOURCE_DIR}/main.cpp")

add_executable(his ${PROJECT_SOURCES})

