cmake_minimum_required(VERSION 3.5)

project(bcfcore LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

option(USE_BCF_SERIALPORT "使用bcf内部的串口能力，由QT5提供" ON)
option(USE_BCF_TCP "使用bcf内部的TCP能力，由QT5提供" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "d")

add_definitions(-DBUILD_BCF_LIB)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files.
FILE(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Header files.
FILE(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")

if(USE_BCF_SERIALPORT)
    add_definitions(-DUSE_BCF_SERIALPORT)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core SerialPort)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)
    set(LINK_BCF ${LINK_BCF} Qt${QT_VERSION_MAJOR}::SerialPort)
endif()

if(USE_BCF_TCP)
    add_definitions(-DUSE_BCF_TCP)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)
     set(LINK_BCF ${LINK_BCF} Qt${QT_VERSION_MAJOR}::Network)
endif()

add_library(bcfcore SHARED
    ${HEADERS}
    ${SOURCES}
)

if(USE_BCF_SERIALPORT OR USE_BCF_TCP)
target_link_libraries(bcfcore PRIVATE Qt${QT_VERSION_MAJOR}::Core ${LINK_BCF})
endif()
