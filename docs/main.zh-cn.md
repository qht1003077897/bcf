Bcf
=======
基于c++11和qt5(串口和tcp)的一个基础协议通信框架，内置多种协议，开箱即用。
本库的重点是打造简单易用的协议通信层，支持自定义串口后端和tcp后端，所以重点不是一个tcp库或者串口库。 当然如果你想的话，也可以将它当作一个串口库或者tcp库。
使用本库，可帮助你在开发串口通信软件或者tcp客户端软件时节省80%的工作。

[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![MIT](https://img.shields.io/badge/LICENSE-MIT-blue)](https://gitpub.sietium.com/tools/toolkits/bcf/-/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-NPL%20(The%20996%20Prohibited%20License)-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux,%20Windows-green.svg?style=flat-square)](https://github.com/qht1003077897/bcf)

## Features
* 跨平台 (Linux | Windows)
* 内置协议（定长协议、头部指定长度协议），对于内置协议，bcf内部处理粘包、分包等情况。当然也支持扩展协议.详见example/customprotocol
* 自带qt串口和qtcp通信后端，当然也支持用户扩展，比如使用asio的tcp或者boost的串口等等.详见example/customchannel
* 支持串口Ymodel发送文件，可以给xshell发送测试.
* 支持

## Documentation
- [简体中文](https://github.com/qht1003077897/bcf/blob/master/docs/main.zh-cn.md)

## Compatibility
* Visual C++ 2017+ (32/64-bit)
* GCC 4.8+ (32/64-bit)
* Clang (Supported C++ 11)

## Macro
* BCF_USE_QT_SERIALPORT
  > 使用bcf自带的qt串口后端
* BCF_USE_TCP
  > 使用bcf自带的qt tcp后端

## Build Example

直接用qtcreator打开bcf根目录的CMakeList.txt文件进行编译。

或者:
### linux
1. mkdir build
2. cmake -B build -DBCF_BUILD_EXAMPLES=ON -DBCF_BUILD_TESTS=ON
3. cd build
4. make

### Windows
1. mkdir build
2. cmake -G "Visual Studio 17 2022" -A x64 ./build
3. 打开build目录下的sln文件即可。

## Usages
* [Examples](#examples)

Examples
----------------------------
* [最简单的使用bcf的演示](https://github.com/qht1003077897/bcf/blob/master/examples/simple/simple.cpp)
* [如何收发原始裸流数据](https://github.com/qht1003077897/bcf/blob/master/examples/rawdata/rawdata.cpp)
* [自定义通道](https://github.com/qht1003077897/bcf/blob/master/examples/customchannel/customchannel.cpp)
* [自定义协议](https://github.com/qht1003077897/bcf/blob/master/examples/customprotocol/customprotocol.cpp)
* [ymodel 文件发送](https://github.com/qht1003077897/bcf/blob/master/examples/ymodel/ymodel.cpp)
* more examples please see [examples](https://github.com/qht1003077897/bcf/blob/master/examples);

Users
----------------------------
