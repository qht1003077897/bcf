#include <QCoreApplication>
#include "bcf.h"
using namespace std;

/**
*  @file    : ymodel_recv.cpp
*  @brief   : 此demo演示如何使用bcf接收文件
*/

#define CHANNEL_ID_SERIALPORT 0   //作为通道的唯一id，在真实的业务环境中随意定义为任意int即可

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
    .withChannel(CHANNEL_ID_SERIALPORT, []() {
        auto channel = std::make_shared<bcf::SerialChannel_QT>("COM3");  //使用bcf内部的串口通道类
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

    //New Line
    std::string saveFilePath = "D:/";
    requestPtr->recvFileWithYModel(saveFilePath, [](int progress) {
        std::cout << "progress: " << progress << std::endl;
    }, [](bcf::TransmitStatus status) {
        std::cout << "status: " << status << std::endl;
    }, 20'000);//配置 20S超时

    return app.exec();
}
