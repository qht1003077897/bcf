﻿#include <QCoreApplication>
#include <QTimer>
#include "bcf.h"
using namespace std;

/**
*  @file    : simpletcpclient.cpp
*  @brief   : 使用 @class ByHeadProtocolModel 协议即指定头部长度协议演示一个简单的bcf tcp客户端使用方法
*/
#define CMD_REQ_NAME 0x01         //模拟一个请求对方名字的cmd，在真实的业务环境中请和对端协商定义唯一的int值即可

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    bool connect = false;
    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .registProtocolBuilders({std::make_shared<bcf::ByHeadProtocolBuilder>()})
                      .registProtocolParsers({std::make_shared<bcf::ByHeadProtocolParser>()})
#ifdef BCF_USE_QT_TCP
    .withChannel([]() {
        auto channel =
            std::make_shared<bcf::TCPClientChannel_QT>("127.0.0.1", 1234); //使用bcf内部的串口通道类
        return channel;
    })
#endif
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback" << std::endl;
    })
    .withConnectionCompletedCallback([&connect](std::shared_ptr<bcf::IChannel> channel) {
        std::cout <<  "withConnectionCompletedCallback channelID:" << channel->channelID() << std::endl;
        connect = true;
    })
    .build();
    requestPtr->connect();

    //因为是异步连接，所以延迟
    QTimer::singleShot(1000, [&connect, &requestPtr]() {
        if (connect) {
            std::shared_ptr<bcf::ByHeadProtocolModel> reqmodel = std::make_shared<bcf::ByHeadProtocolModel>();
            reqmodel->seq = bcf::util::getNextSeq();
            reqmodel->cmd = CMD_REQ_NAME;
            reqmodel->setBody(
                std::string("what's your name?"));
            requestPtr->request(reqmodel, [](bcf::ErrorCode code,
            std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
                auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(retmodel);
                if (bmodel) {
                    std::cout << "retmodel seq:" << bmodel->seq << std::endl;
                    std::cout << "retmodel body:" <<  bmodel->body() << std::endl;
                }
            });
        }
    });

    return app.exec();
}
