cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(his)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/Head)

# MySQL C Client library
include_directories("E:/MySQL/8-0/include")
link_directories("E:/MySQL/8-0/lib")

file(GLOB PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/Source/Core/*.cpp"
    "${CMAKE_SOURCE_DIR}/Source/Roles/*.cpp"
    "${CMAKE_SOURCE_DIR}/Source/Modules/*.cpp"
    "${CMAKE_SOURCE_DIR}/main.cpp"
)

add_executable(his ${PROJECT_SOURCES})

target_link_libraries(his PRIVATE libmysql)

