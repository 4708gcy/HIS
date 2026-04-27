cmake_minimum_required(VERSION 3.24)

project(his)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/Head)

file(GLOB PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/Source/*.cpp" "${CMAKE_SOURCE_DIR}/main.cpp")
file(GLOB SERVER_SOURCES "${CMAKE_SOURCE_DIR}/Source/*.cpp" "${CMAKE_SOURCE_DIR}/server_main.cpp")

# 原控制台程序
add_executable(his ${PROJECT_SOURCES})

# REST API 服务器
add_executable(his_server ${SERVER_SOURCES})
if(MSVC)
    target_compile_options(his_server PRIVATE /utf-8)
endif()

