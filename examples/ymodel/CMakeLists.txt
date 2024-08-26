cmake_minimum_required(VERSION 3.5)

project(ymodel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(ymodel_send
    ymodel_send.cpp)
target_link_libraries(ymodel_send PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core)


add_executable(ymodel_recv
    ymodel_recv.cpp)
target_link_libraries(ymodel_recv PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core)
