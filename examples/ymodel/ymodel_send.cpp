#include <QCoreApplication>
#include <QDebug>
#include "bcf.h"
using namespace std;

/**
*  @file    : ymodel_send.cpp
*  @brief   : 此demo演示如何使用bcf使用ymodel协议发送文件
*/


int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
    .withChannel([]() {
        auto channel = std::make_shared<bcf::SerialChannel_QT>("COM2");  //使用bcf内部的串口通道类
        return channel;
    })
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback" << std::endl;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        std::cout <<  "withConnectionCompletedCallback channelID:" << channel->channelID() << std::endl;
    })
    .build();
    requestPtr->connect();


    std::string filename = "C:\\Users\\bridge\\Desktop\\image.bin";//902 字节
    requestPtr->sendFileWithYModel(filename, [](int progress) {
        std::cout << "progress: " << progress << std::endl;
    }, [](bcf::TransmitStatus status) {
        std::cout << "status: " << status << std::endl;
    }, 20'000);//配置 20S超时

    return app.exec();
}
