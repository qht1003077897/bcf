find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(simpleserialport
    simpleserialport.cpp)

target_link_libraries(simpleserialport PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core)
