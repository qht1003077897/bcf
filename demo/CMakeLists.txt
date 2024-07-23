cmake_minimum_required(VERSION 3.5)

project(bcf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(bcf
    main.cpp
    aprotocolbuilder.h
    aprotocolbuilder.cpp)

target_link_libraries(bcf PRIVATE bcfcore)

include(GNUInstallDirs)
install(TARGETS bcf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
