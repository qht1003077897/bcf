/**
*  @FileName    : simple.cpp
*  @Brief       : 使用 ByHeadProtocolModel 协议即指定头部长度协议演示一个简单的使用方法
*/

#include <QCoreApplication>
#include <QDebug>
#include "bcf.h"
using namespace std;

#define CHANNEL_ID_SERIALPORT 0   //作为通道的唯一id，在真实的业务环境中随意定义为任意int即可

#define CMD_REQ_NAME 0x01         //模拟一个请求对方名字的cmd，在真实的业务环境中请和对端协商定义唯一的int值即可

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    //New Line
    std::shared_ptr<bcf::IChannel> tmpChannel;

    auto requestPtr = bcf::RequestHandlerBuilder()
    .withChannel(CHANNEL_ID_SERIALPORT, [ &tmpChannel ]() {
        tmpChannel  = std::make_shared<bcf::SerialChannel_QT>("COM2");  //使用bcf内部的串口通道类
        //New Line
        tmpChannel->setRawDataCallback([](std::shared_ptr<bb::ByteBuffer> bb) {
            bb->printHex();
        });
        return tmpChannel;
    })
    .withFailedCallback([](int errorcode) {
        std::cerr <<  "withFailedCallback:" << errorcode;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        qDebug() <<  "withConnectionCompletedCallback channelID:" << channel->channelID() ;
    })
    .build();
    requestPtr->connect();

    assert(tmpChannel != nullptr);
    std::string str("this is 1 times request");
    tmpChannel->send(str.data(), str.length());
    return app.exec();
}
