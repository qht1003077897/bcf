﻿#include <QCoreApplication>
#include "bcf.h"
#include "customprotocol.h"
using namespace std;
using namespace bcf;

/**
 *  @brief 这是演示发送者！
 *  下面的例子中演示了如何自定义自己的协议，如果你不想使用bcf内置的支持的协议，你可以参考下面的代码定义自己的通信协议。
 *  在下面的例子中，我们参考@class ByHeadProtocolModel，只是将cmd拆分为2个2byte的字段，将body的内容用json格式代替。
 *  其实大多数用户层协议都是这个格式，例子的目的只是告诉使用者如何自定义和自定义的要求。
 *  @note bcf要求报文中【协议类型】和【会话唯一序列号】必须存在,且位于开头,seq后面的内容可以自定义
*/

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .registProtocolBuilders({std::make_shared<CustomProtocolBuilder>(),
                                               std::make_shared<bcf::ByHeadProtocolBuilder>()})
                      .registProtocolParsers({std::make_shared<CustomProtocolParser> (),
                                              std::make_shared<bcf::ByHeadProtocolParser>()})
#ifdef BCF_USE_QT_SERIALPORT
    .withChannel([]() {
        return std::make_shared<bcf::SerialChannel_QT>("COM2");
    })
#endif
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback" << std::endl;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        std::cout <<  "withConnectionCompletedCallback channelID:" << channel->channelID() << std::endl;
    })
    .build();
    requestPtr->connect();

    auto reqmodel = std::make_shared<CustomProtocolModel>();
    reqmodel->seq = bcf::util::getNextSeq();
    reqmodel->what = WHAT_NAME;
    reqmodel->action = ACTION_GET_NAME;
    std::string jsonString = "{\"msgid\":1,\"content\":\"what's your name?\"}";
    reqmodel->setBody(jsonString);

    requestPtr->request(reqmodel, [](bcf::ErrorCode code,
    std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
        auto bmodel = std::dynamic_pointer_cast<CustomProtocolModel>(retmodel);
        if (bmodel) {
            std::cout << "retmodel seq: " << bmodel->seq << std::endl;
            std::cout << "retmodel what: " << bmodel->what << std::endl;
            std::cout << "retmodel action: " << bmodel->action << std::endl;
            std::cout << "retmodel body: " <<  bmodel->body() << std::endl;
        }
    });

    return app.exec();
}
