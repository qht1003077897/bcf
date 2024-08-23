Bcf
=======

**>NOTE: Bcf is not a network library, but an easy-to-use client protocol communication framework !!!**

--- 

A basic protocol communication framework based on C++11 and QT5 (serial and TCP), with multiple built-in protocols, ready to use out of the box.
The focus of this library is to create a simple and easy-to-use protocol communication layer that supports custom serial and TCP backends, so the emphasis is not on a TCP library or serial library. \Of course, if you want, you can also use it as a serial port library or TCP library.
By using this library, you can save 80% of your work when developing serial communication software or TCP client software.

[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![MIT](https://img.shields.io/badge/LICENSE-MIT-blue)](https://gitpub.sietium.com/tools/toolkits/bcf/-/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-NPL%20(The%20996%20Prohibited%20License)-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux,%20Windows-green.svg?style=flat-square)](https://github.com/qht1003077897/bcf)

## Features
* Cross platform (Linux | Windows)
* Built in protocols (fixed length protocol, header specified length protocol), for built-in protocols, bcf internally handles situations such as packet sticking and subcontracting. Of course, it also supports extension protocols. See example/customprotocol for details
* Comes with Qt serialport and Qt TCP communication backend, and of course supports user extensions such as using ASIO's TCP or Boost serialport, etc. See example/customchannel for details
* Support sending files via Ymodel serial port, which can be used to send tests to xshell

## Documentation
- [简体中文](https://github.com/qht1003077897/bcf/blob/master/docs/main.zh-cn.md)

## Compatibility
* Visual C++ 2017+ (32/64-bit)
* GCC 4.8+ (32/64-bit)
* Clang (Supported C++ 11)

## Macro
 - BCF_USE_QT_SERIALPORT
  > Use the built-in Qt serial backend of BCF
 - BCF_USE_QT_TCP
  > Use the built-in Qt TCP backend of BCF


## Build Example
Directly use qtcreator to open the CMakeList.exe file in the bcf root directory for compilation.

OR：
### linux
1. mkdir build
2. cmake -B build -DBCF_BUILD_EXAMPLES=ON -DBCF_BUILD_TESTS=ON
3. cd build
4. make

### Windows
1. mkdir build
2. cmake -G "Visual Studio 17 2022" -A x64 ./build
3. Open the sln file in the build directory

## Usages
* [Examples](#examples)
Examples
----------------------------
* [The simplest demonstration of using BCF](https://github.com/qht1003077897/bcf/blob/master/examples/simple/simple.cpp)
* [How to send and receive raw raw raw stream data](https://github.com/qht1003077897/bcf/blob/master/examples/rawdata/rawdata.cpp)
* [Custom channel](https://github.com/qht1003077897/bcf/blob/master/examples/customchannel/customchannel.cpp)
* [Custom protocol](https://github.com/qht1003077897/bcf/blob/master/examples/customprotocol/customprotocol.cpp)
* [Sending files using the ymodel protocol](https://github.com/qht1003077897/bcf/blob/master/examples/ymodel/ymodel.cpp)
* more examples please see [examples](https://github.com/qht1003077897/bcf/blob/master/examples);

Users
----------------------------
