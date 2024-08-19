cmake_minimum_required(VERSION 3.5)

project(bcf LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

option(BCF_USE_QT_SERIALPORT "使用bcf内部的串口能力，由QT5提供" ON)
option(BCF_USE_TCP "使用bcf内部的TCP能力，由QT5提供" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
 set(LINK_BCF Qt${QT_VERSION_MAJOR}::Core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "d")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DBUILD_BCF_LIB)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files.
FILE(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Header files.
FILE(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
                          "${CMAKE_SOURCE_DIR}/include/*.h"
                          "${CMAKE_SOURCE_DIR}/include/*.hpp")

if(BCF_USE_QT_SERIALPORT)
    add_definitions(-DBCF_USE_QT_SERIALPORT)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS SerialPort)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SerialPort)
    set(LINK_BCF ${LINK_BCF} Qt${QT_VERSION_MAJOR}::SerialPort)
endif()

if(BCF_USE_TCP)
    add_definitions(-DBCF_USE_TCP)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
     set(LINK_BCF ${LINK_BCF} Qt${QT_VERSION_MAJOR}::Network)
endif()

add_library(bcf SHARED
    ${HEADERS}
    ${SOURCES}
)

 target_link_libraries(bcf PRIVATE ${LINK_BCF})
