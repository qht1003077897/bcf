﻿find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(customprotocol_send
    customprotocol.h
    customprotocol_send.cpp)

target_link_libraries(customprotocol_send PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core)


add_executable(customprotocol_recv
    customprotocol.h
    customprotocol_recv.cpp)

target_link_libraries(customprotocol_recv PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core)
