#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include "bcf.h"
#include "customprotocol.h"
using namespace std;
using namespace bcf;

#define CHANNEL_ID_SERIALPORT 0

/**
 *  这是演示接收者！ 先运行customprotocol_recv.cpp, 再运行customprotocol_send.cpp
 *
 *  下面的例子中演示了如何自定义自己的协议，如果你不想使用bcf内置的支持的协议，你可以参考 customprotocol.h 定义自己的通信协议。
 *  在下面的例子中，我们参考 ByHeadProtocolModel，只是将cmd拆分为2个2byte的字段，将body的内容用json格式代替。
 *  其实大多数用户层协议都是这个格式，例子的目的只是告诉使用者如何自定义和自定义的要求。
 *  >NOTE: bcf要求报文中【协议类型】和【会话唯一序列号】必须存在,且位于开头,seq后面的内容可以自定义
*/
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .registProtocolBuilders({std::make_shared<CustomProtocolBuilder>()})
                      .registProtocolParsers({std::make_shared<CustomProtocolParser>()})
#ifdef BCF_USE_QT_SERIALPORT
    .withChannel(CHANNEL_ID_SERIALPORT, []() {
        auto channel = std::make_shared<bcf::SerialChannel_QT>("COM3");
        return channel;
    })
#endif
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback" << std::endl;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        qDebug() <<  "withConnectionCompletedCallback channelID:" << channel->channelID() ;
    })
    .withReceiveData([](std::shared_ptr<bcf::RequestHandler> handler,
                        std::shared_ptr<bcf::AbstractProtocolModel>
    model) {
        qDebug() <<  "withReceiveData protocolType:"  << model->protocolType();
        if (model->protocolType() == customPackMode) {
            auto bmodel = std::dynamic_pointer_cast<CustomProtocolModel>(model);
            if (bmodel) {
                if (bmodel->what == WHAT_NAME && bmodel->action == ACTION_GET_NAME) {
                    std::string resultJsonString = "{\"msgid\":1,\"content\":\"my name is bcf!\"}";
                    auto reqmodel = std::make_shared<CustomProtocolModel>();
                    reqmodel->seq = bcf::util::getNextSeq();
                    reqmodel->what = bmodel->what;
                    reqmodel->action = bmodel->action;
                    reqmodel->setBody(resultJsonString);
                    handler->request(reqmodel, [](bcf::ErrorCode code,
                    std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
                    });

                }
            }
        }
    })
    .build();
    requestPtr->connect();

    return app.exec();
}
