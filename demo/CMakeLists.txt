cmake_minimum_required(VERSION 3.5)

project(bcf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)

add_definitions(-DUSE_BCF_SERIALPORT)

add_executable(bcf
    main.cpp
    aprotocolbuilder.h
    aprotocolbuilder.cpp)

target_link_libraries(bcf PRIVATE bcfcore Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::SerialPort)

include(GNUInstallDirs)
install(TARGETS bcf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
