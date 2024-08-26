find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(rawdata
    rawdata.cpp)

target_link_libraries(rawdata PRIVATE bcf Qt${QT_VERSION_MAJOR}::Core)
