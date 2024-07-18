cmake_minimum_required(VERSION 3.5)

project(bcfDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

source_group("Header Files" FILES ${HEADERS})

add_subdirectory(bcfcore)
add_subdirectory(demo)
