# bcf

**>NOTE: bcf 不是一个网络库，只是适用于客户端的协议通信框架。!!!**

***

基于c++11和qt5(串口和tcp)的一个基础协议通信框架，内置多种协议，开箱即用。
本库的重点是打造简单易用的协议通信层，支持自定义串口后端和tcp后端，所以重点不是一个tcp库或者串口库。 当然如果你想的话，也可以将它当作一个串口库或者tcp库。
使用本库，可帮助你在开发串口通信软件或者tcp客户端软件时节省大量的关于协议设计的时间。

[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![MIT](https://img.shields.io/badge/LICENSE-MIT-blue)](https://gitpub.sietium.com/tools/toolkits/bcf/-/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-NPL%20\(The%20996%20Prohibited%20License\)-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux,%20Windows-green.svg?style=flat-square)](https://github.com/qht1003077897/bcf)

# 为什么会有这个框架？

对于涉及到串口、或者tcp业务的中小型软件而言，为了和下位机进行通信，我们经常要先制定一套通信协议，然后双方基于此协议进行协议层的开发，大概会涉及到：协议组装、协议解析、粘包、分包处理，超时处理，业务分发等。那么我们能否将这几部分固化下来，将这部分功能封装到一个框架或者库中，对于后续开发类似的软件，都使用这个库的功能，是不是可以减少我们大部分时间呢？

基于以上诉求，便引申出bcf的功能列表：

## Features

*   跨平台 (Linux | Windows)
*   内置协议（指定头部长度协议），对于内置协议，bcf在内部会处理粘包、分包等情况，保证回调给用户的协议model都是完整的一包数据。当然也支持扩展协议.详见 [customprotocol.cpp](examples/customprotocol/customprotocol.h)
*   支持在一个通道上使用多个协议(比如在串口通道上使用两种协议模型).
*   支持非协议模型的使用方式，比如直接使用原始的16进制裸流字节数据进行通信.
*   自带qt串口和qtcp通信后端（如果你是QT软件，直接使用bcf自带的串口类或者tcp类就好了）。当然也支持用户扩展，比如使用asio的tcp或者boost的串口等等.详见[customchannel.cpp](examples/customchannel/customchannel.cpp)
*   支持使用串口基于Ymodel协议发送文件和接收文件，可以给xshell发送文件进行测试使用.

## Compatibility

*   Visual C++ 2017+ (32/64-bit)
*   GCC 4.8+ (32/64-bit)
*   Clang (Supported C++ 11)
*   最低建议 Qt5.12.0

## Macro

*   BCF\_USE\_QT\_SERIALPORT
    *   使用bcf自带的qt串口后端
*   BCF\_USE\_QT\_TCP
    *   使用bcf自带的qt tcp后端

## Build

直接用qtcreator打开bcf根目录的CMakeList.txt文件进行编译。或者:
### linux

1.  mkdir build
2.  cmake -B build -DBCF\_BUILD\_EXAMPLES=ON -DBCF\_BUILD\_TESTS=ON
3.  cd build
4.  make

### Windows

1.  mkdir build
2.  cmake -G "Visual Studio 17 2022" -A x64 ./build
3.  打开build目录下的sln文件即可。

## Usages

*   [Examples](#examples)

## Examples

*   [最简单的使用bcf 串口的演示](./examples/simpleserialport)
*   [最简单的使用bcf TCP的演示](./examples/simpletcp)
*   [如何收发原始裸流数据](./examples/rawdata)
*   [自定义通道](./examples/customchannel)
*   [自定义协议](./examples/customprotocol)
*   [ymodel 文件发送和接收](./examples/ymodel)
*   more examples please see [examples](./examples)

## 一些问题:

### 1.  bcf内置哪几种协议？在项目中如何使用？如何自定义通信协议？

> bcf目前只支持"头部指定长度"协议。在我看来，大多数场景下使用这种协议就够用了。他没有定长协议的缺点，支持动态长度。另外还支持此协议的一个变种，详见:[customprotocol.h](./examples/customprotocol/customprotocol.h),这个变种接近于"头部指定长度"协议，只是将cmd拆分为了2个字段(为了定义更复杂的业务),将body的内容用json格式代替。

> 如果你想直接使用bcf内置的协议进行开发通信功能，则可以参考:[simpletcpclient.cpp](./examples/simpletcp/simpletcpclient.cpp) 演示了如何使用协议model进行tcp通信。

> 如果你要自定义自己的协议，需要注意的是，bcf要求报文中【协议类型】和【会话唯一序列号】必须存在,且位于开头,seq后面的内容可以任意自定义 可以参考: [byheadprotocol.h](./include/protocolparser/byheadprotocol.h)

**一个自定义协议需要的三要素如下:**
 - 1. 一个继承自bcf::AbstractProtocolModel的协议model。
 - 2. 一个继承自 bcf::IProtocolBuilder 的协议编码器。
 - 3. 一个继承自 bcf::IProtocolParser 的协议解码器。

### 2.  如果一个通道上支持多协议，如何支持多协议？收到数据后如何知道被哪个解析器解析呢？

> 如果存在这样的情况，意味着你想在一个 RequestHandler 对象上使用不同的协议model进行request请求。同时也意味着你需要有多个协议编码器和协议解码器。

> registProtocolBuilders()函数进行配置协议编码器，支持配置多个。参考:[customprotocol_send.cpp](./examples/customprotocol/customprotocol_send.cpp) 
registProtocolParsers()函数进行配置协议解码器，支持配置多个。比如我们在例子中配置了 CustomProtocolBuilder 和 ByHeadProtocolBuilder 两种协议编码器。

> 同样的，registProtocolParsers()函数进行配置协议解码器，支持配置多个。比如我们在例子中配置了 CustomProtocolParser 和 ByHeadProtocolParser 两种解码器。
因为无法知道下位机或者对端给我们送过来的数据能被哪个解码器解码，所以bcf还有个要求是在自定义协议时需要指定协议类型,参考:[customprotocol.h](./examples/customprotocol/customprotocol.h) 。bcf内部根据协议类型选择上面配置的解码器，然后探测尝试解码，如果探测成功，则使用当前的解码器，否则切换到下一个解码器。


### 3.  如果我不想使用model的方式进行请求，而是想直接使用原始的裸流数据进行通信，怎么做？
> 参考:[rawdata.cpp](./examples/rawdata/rawdata.cpp)
> 在调用withChannel时就可以将用于收发数据的通道指针保存下来，这是一个 bcf::Ichannel指针。使用这个指针即可调用send方法进行数据发送。IChannel的setRawDataCallback接口可以注册监听原始数据的回调，返回的是一个 ByteBuffer 指针，ByteBuffer的data()函数就可以获取到原始char*数据。

### 4.  bcf是否支持大小端设置，是如何处理大小端的？
> 支持。在使用registProtocolBuilders函数和registProtocolParsers函数注册编码器和解码器时，可以在其构造函数入参指定大小端。默认大端。即在编码时，bcf会将数据从主机端转换成大端(tcp/ip协议规定大端传输)进行发送。在解码时，bcf会将数据从大端序转换成主机序再封装到model中。详见:[byheadprotocol.h](./include/protocolparser/byheadprotocol.h)。bcf内置大小端转换函数，对于自定义协议而言，也可以参考 byheadprotocol的做法，展示了如何处理大小端。

### 5.  bcf的协议设计时为什么需要协议类型、seq、cmd？
> 参考:[byheadprotocol.h](./include/protocolparser/byheadprotocol.h)
> 关于type:
> 【要求】: 在协议设计时每个Frame的包头第一个字段要求是type，占1byte。如果涉及多协议，保证type各不相同。
> 【原因】:因为如果在同一个通道上，用户使用了2套通信协议，则在协议解码时，bcf如何知道用哪个解码器解析，并封装成对应的ProtocolModel呢？所以需要一个type字段进行标识，bcf根据type决定使用哪个解码器进行解码。

> 关于seq:
> 【要求】: 在协议设计时每个Frame的包头第二个字段要求是seq，占4byte。在同一个通道上，保证seq唯一，可以使用bcf提供的工具函数 bcf::util::getNextSeq() 获取seq。
> 【原因】: 因为大部分场景下，通信都是异步的。比如客户端连着发送了2条指令，分别是1和2。对于服务端而言，完全可能先回复2，再回复1。回到发送端，如何确定收到的这条回复是1的回复呢还是2的回复呢？所以我们依赖一个seq，用于异步消息的唯一序列号，就像tcp协议中的seq一样，bcf根据这个seq确定包的顺序。

> 关于cmd: (command的缩写，不一定是一个int，它也有可能是几个字段，或者一段json等)
> 【要求】: bcf对cmd没有要求，有无cmd均可。示例 [byheadprotocol.h](./include/protocolparser/byheadprotocol.h) 中的cmd是用来标识业务id的。
> 【原因】: 其实大部分业务场景下，cmd也是必须的，它可以用来表示业务id，如果不用 [byheadprotocol.h](./include/protocolparser/byheadprotocol.h)  中的方式，也可以将cmd封装到body中。比如客户端发送了一个指令，如果没有cmd，服务端如何知道这个指令想干什么呢？

### 6. bcf超时的原理是什么？
> withTimeOut() 接口配置的超时不是类似于tcp底层keepalive的超时。而是对于request()接口而言，timeout时间内没有收到对应seq的回复，则认为当前request超时，request()接口会回调 ErrorCode::TIME_OUT。也就是说，哪怕在timeout时间内收到了数据，但是经过解码器后，并不是某个seq请求的回复，则对应的request()接口就会超时。
> 另外，如果数据不能被解码器解码，则会被丢弃。如果数据能被解码器解码，则会把数据抛给withReceiveData()设置的ReceiveCallback回调，视为对端主动push上来的数据。

### 7. request是否线程安全？
> request回调给用户的数据是线程安全的，回调线程会被转移到构造RequestHandler时所在的线程，一般是QT的主线程。除非你在自定义的QThread中构造了RequestHandler(只是，这样做没有任何好处)。
> 另外的（这部分属于bcf内部实现，感兴趣可参考源码: [ichannel.cpp](./bcfcore/ichannel.cpp) 和 [requesthandler.cpp](./bcfcore/requesthandler.cpp) 的 receive 函数。 ），由于我们无法确定后端io线程是什么线程。如果后端比如使用QTSerialPort，则io数据会通过槽函数接收到QT主线程。那如果后端是另一个io库，比如asio，则数据回调可能在单独的线程。所以，为了屏蔽这种差异性，bcf会将io数据先放到缓冲区，然后再emit给构造RequestHandler时所在的线程,即QT的主线程，然后在slot运行的线程中回调给用户。这样做还有个好处是防止用户处理数据时如果进行耗时操作则会导致阻塞后端io线程，这样可能会导致后端io库因为发生阻塞导致异常等。

### 8. 待补充。
