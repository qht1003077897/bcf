#include <QCoreApplication>
#include "bcf.h"
using namespace std;

/**
*  @file        : rawdata.cpp
*  @brief       : 演示bcf如何不使用协议model进行通信,而是使用原始数据裸流进行通信
*/

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    std::shared_ptr<bcf::IChannel> tmpChannel;

    auto requestPtr = bcf::RequestHandlerBuilder()
    .withChannel([&tmpChannel]() {
        tmpChannel  = std::make_shared<bcf::SerialChannel_QT>("COM2");  //使用bcf内部的串口通道类
        tmpChannel->setRawDataCallback([](std::shared_ptr<bb::ByteBuffer> bb) {
            bb->data();
            bb->printHex();
        });
        return tmpChannel;
    })
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback" << std::endl;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        std::cout <<  "withConnectionCompletedCallback channelID:" << channel->channelID();
    })
    .build();
    requestPtr->connect();

    assert(tmpChannel != nullptr);
    std::string str("this is 1 times request");
    tmpChannel->send(str.data(), str.length());
    return app.exec();
}
