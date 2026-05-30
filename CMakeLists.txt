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

# ====================== 构建类型默认值 ======================
# 如果用户未通过 -DCMAKE_BUILD_TYPE=... 指定构建类型，则默认设为 Debug
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug or Release)" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# Release 构建优化选项
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND MSVC)
    # MSVC Release 默认开启 /O2，以下为额外建议
    add_compile_options(/GL)       # 全程序优化
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
endif()

