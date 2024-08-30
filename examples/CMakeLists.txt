cmake_minimum_required(VERSION 3.5)

project(simpletcpclient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(simpleserialport)
add_subdirectory(simpletcp)
add_subdirectory(ymodel)
add_subdirectory(rawdata)
add_subdirectory(customprotocol)
