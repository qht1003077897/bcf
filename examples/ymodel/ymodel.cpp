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

    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .registProtocolBuilders({std::make_shared<ByHeadProtocolBuilder>()})
                      .registProtocolParsers({std::make_shared<ByHeadProtocolParser>()})
    .withAbandonCallback([](std::shared_ptr<bcf::AbstractProtocolModel> model) {})
    .withChannel(CHANNEL_ID_SERIALPORT, []() {
        auto channel = std::make_shared<bcf::SerialChannel_QT>();  //使用bcf内部的串口通道类
        channel->setPortName("COM2");
        return channel;
    })
    .withReceiveData([](std::shared_ptr<bcf::AbstractProtocolModel> model) {
        if (model->protocolType() == bcf::PackMode::UNPACK_BY_LENGTH_FIELD) {
            auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(model);
            if (bmodel) {
                qDebug() <<  "withReceiveData retmodel seq: " << bmodel->seq;
                qDebug() << "withReceiveData retmodel body:" << QString::fromStdString(bmodel->body());
            }
        }
    })
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback"  ;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        qDebug() <<  "withConnectionCompletedCallback channelID:" << channel->channelID() ;
    })
    .build();
    requestPtr->connect();

    std::string filename = "C:\\Users\\bridge\\Desktop\\image.bin";//902 字节
    requestPtr->sendFileWithYModel(filename, [](int progress) {
        qDebug() << "progress: " << progress ;
    }, [](bcf::TransmitStatus status) {
        qDebug() << "status: " << status ;
    });

    return app.exec();
}
