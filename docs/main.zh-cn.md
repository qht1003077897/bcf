Bcf
=======

**>NOTE: Bcf 不是一个网络库，只是一个易用的客户端协议通信框架。!!!**

--- 

基于c++11和qt5(串口和tcp)的一个基础协议通信框架，内置多种协议，开箱即用。
本库的重点是打造简单易用的协议通信层，支持自定义串口后端和tcp后端，所以重点不是一个tcp库或者串口库。 当然如果你想的话，也可以将它当作一个串口库或者tcp库。
使用本库，可帮助你在开发串口通信软件或者tcp客户端软件时节省大量的关于协议设计的时间。

[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![MIT](https://img.shields.io/badge/LICENSE-MIT-blue)](https://gitpub.sietium.com/tools/toolkits/bcf/-/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-NPL%20(The%20996%20Prohibited%20License)-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux,%20Windows-green.svg?style=flat-square)](https://github.com/qht1003077897/bcf)

为什么会有这个框架？
=======
对于涉及到串口、或者tcp业务的中小型软件而言，为了和下位机进行通信，我们经常要先制定一套通信协议，然后双方基于此协议进行协议层的开发，大概会涉及到：协议组装、协议解析、粘包、分包处理，超时处理，业务分发等。那么我们能否将这几部分固化下来，将这部分功能封装到一个框架或者库中，对于后续开发类似的软件，都使用这个库的功能，是不是可以减少我们大部分时间呢？

基于以上诉求，便引申出bcf的功能列表：
## Features
* 跨平台 (Linux | Windows)
* 内置协议（指定头部长度协议），对于内置协议，bcf在内部会处理粘包、分包等情况，保证回调给用户的协议model都是完整的一包数据。当然也支持扩展协议.详见example/customprotocol.
* 支持在一个通道上使用多个协议(比如在串口通道上使用两种协议模型).
* 支持非协议模型的使用方式，比如直接使用原始的16进制裸流字节数据进行通信.
* 自带qt串口和qtcp通信后端（如果你是QT软件，直接使用bcf自带的串口类或者tcp类就好了）。当然也支持用户扩展，比如使用asio的tcp或者boost的串口等等.详见example/customchannel.
* 支持串口基于Ymodel协议发送文件和接收文件，可以给xshell发送文件进行测试使用.
* 

## Compatibility
* Visual C++ 2017+ (32/64-bit)
* GCC 4.8+ (32/64-bit)
* Clang (Supported C++ 11)
* 最低建议 Qt5.12.0

## Macro
* BCF_USE_QT_SERIALPORT
  - 使用bcf自带的qt串口后端
* BCF_USE_QT_TCP
  - 使用bcf自带的qt tcp后端

## Build

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
* [最简单的使用bcf 串口的演示](https://github.com/qht1003077897/bcf/blob/master/examples/simpleserialport)
* [最简单的使用bcf TCP的演示](https://github.com/qht1003077897/bcf/blob/master/examples/simpletcp)
* [如何收发原始裸流数据](https://github.com/qht1003077897/bcf/blob/master/examples/rawdata)
* [自定义通道](https://github.com/qht1003077897/bcf/blob/master/examples/customchannel)
* [自定义协议](https://github.com/qht1003077897/bcf/blob/master/examples/customprotocol)
* [ymodel 文件发送和接收](https://github.com/qht1003077897/bcf/blob/master/examples/ymodel)
* more examples please see [examples](https://github.com/qht1003077897/bcf/blob/master/examples);



一些问题:
----------------------------

1. bcf内置哪几种协议？在项目中如何使用，使用要求。
2. 如果一个通道上支持多协议，如何支持多协议？收到数据后如何知道被哪个解析器解析呢（探测）？
3. 用户如何基于AbstractProtocolModel扩展自己的协议？
4. 如果我不想使用模型的方式，而是想直接使用原始裸流数据通信，怎么做？
5. bcf内部如何处理大小端的？
6. 为什么让用户指定唯一的通道id？
7. 协议设计为什么需要协议类型、seq、cmd？
8. 超时的原理是什么？如何区分bcf的请求超时和底层的io超时？
9. 接口中的这些回调函数和请求类的构造是同线程吗？会不会线程不安全？
10. 为什么没有定长协议？
