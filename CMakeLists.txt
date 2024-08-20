cmake_minimum_required(VERSION 3.5)

project(bcfDemo LANGUAGES CXX)

set(VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BCF_BUILD_EXAMPLES "Build examples" ON)
option(BCF_USE_TCP "使用bcf内部的TCP能力,由QT5提供" ON)
option(BCF_USE_QT_SERIALPORT "使用bcf内部的串口能力,由QT5提供" ON)

# 生成配置头文件
configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_SOURCE_DIR}/include/config.h
)

FILE(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

source_group("Header Files" FILES ${HEADERS})

add_subdirectory(bcfcore)
if (BCF_BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

