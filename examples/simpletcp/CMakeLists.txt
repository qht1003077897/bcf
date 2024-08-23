cmake_minimum_required(VERSION 3.5)

project(simpletcpclient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)


#client
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(simpletcpclient
    simpletcpclient.cpp)
target_link_libraries(simpletcpclient PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core)


#server
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_executable(simpletcpserver
    simpletcpserver.cpp)
target_link_libraries(simpletcpserver PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
