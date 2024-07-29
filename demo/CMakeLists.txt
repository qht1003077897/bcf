cmake_minimum_required(VERSION 3.5)

project(bcfDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)

add_definitions(-DUSE_BCF_SERIALPORT)

add_executable(bcfDemo
    main.cpp
    aprotocolbuilder.h)

target_link_libraries(bcfDemo PRIVATE bcfcore Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::SerialPort)
