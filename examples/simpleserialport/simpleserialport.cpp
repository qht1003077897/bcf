#include <QCoreApplication>
#include "bcf.h"
using namespace std;

/**
*  @file      : simpleserialport.cpp
*  @brief     : 使用 @class ByHeadProtocolModel 协议即指定头部长度协议演示一个简单的bcf 串口使用方法
*/

#define CMD_REQ_NAME 0x01         //模拟一个请求对方名字的cmd，在真实的业务环境中请和对端协商定义唯一的int值即可

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .registProtocolBuilders({std::make_shared<bcf::ByHeadProtocolBuilder>()})
                      .registProtocolParsers({std::make_shared<bcf::ByHeadProtocolParser>()})
    .withChannel([]() {
        auto channel = std::make_shared<bcf::SerialChannel_QT>("COM2");  //使用bcf内部的串口通道类
        return channel;
    })
    .withReceiveData([](std::shared_ptr<bcf::RequestHandler> handler,
    std::shared_ptr<bcf::AbstractProtocolModel> model) {
        if (model->protocolType() == bcf::PackMode::UNPACK_BY_LENGTH_FIELD) {
            auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(model);
            if (bmodel) {
                std::cout <<  "withReceiveData retmodel seq: " << bmodel->seq << std::endl;
                std::cout << "withReceiveData retmodel body:" << bmodel->body() << std::endl;
            }
        }
    })
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback" << std::endl;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        std::cout <<  "withConnectionCompletedCallback channelID:" << channel->channelID() << std::endl;
    })
    .build();
    requestPtr->connect();
    //串口是同步连接

    std::shared_ptr<bcf::ByHeadProtocolModel> reqmodel = std::make_shared<bcf::ByHeadProtocolModel>();
    reqmodel->seq = bcf::util::getNextSeq();
    reqmodel->cmd = CMD_REQ_NAME;
    reqmodel->setBody(std::string("this is " +  std::to_string(reqmodel->seq) + " times request"));

    requestPtr->request(reqmodel, [](bcf::ErrorCode code,
    std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
        auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(retmodel);
        if (bmodel) {
            std::cout << "retmodel seq:" << bmodel->seq << std::endl;
            std::cout << "retmodel body:" << bmodel->body() << std::endl;
        }
    });

    return app.exec();
}
