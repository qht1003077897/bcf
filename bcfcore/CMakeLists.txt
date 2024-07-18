cmake_minimum_required(VERSION 3.5)

project(bcfcore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DBUILD_BCF_LIB)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files.
FILE(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Header files.
FILE(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

add_library(bcfcore SHARED
    ${HEADERS}
    ${SOURCES}
)
