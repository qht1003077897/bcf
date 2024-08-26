/**
*  @FileName    : ymodel_recv.cpp
*  @Brief       : 此demo演示如何使用bcf接收文件
*/

#include <QCoreApplication>
#include <QDebug>
#include "bcf.h"
using namespace std;

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
        qDebug() <<  "withConnectionCompletedCallback channelID:" << channel->channelID() ;
    })
    .build();
    requestPtr->connect();

    //New Line
    std::string saveFilePath = "D:/";
    requestPtr->recvFileWithYModel(saveFilePath, [](int progress) {
        qDebug() << "progress: " << progress ;
    }, [](bcf::TransmitStatus status) {
        qDebug() << "status: " << status ;
    }, 20'000);

    return app.exec();
}
