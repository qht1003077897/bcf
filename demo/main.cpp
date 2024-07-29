#include <abstractprotocolmodel.h>
#include "aprotocolbuilder.h"
#include "requesthandler.h"
#include "serialchannel.h"
#include <QCoreApplication>
using namespace std;

int main(int argc, char* argv[])
{

    setbuf(stdout, NULL);

    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .withProtocolBuilders({std::make_shared<AProtocolBuilder>()})
                      .withProtocolParsers({std::make_shared<AProtocolParser>()})
    .withAbandonCallback([](std::shared_ptr<bcf::AbstractProtocolModel> model) {})
    .withChannel(bcf::CHANNEL_ID_SERIALPORT, (bcf::CreateChannelFunc)[]() {
        auto channel = std::make_shared<bcf::SerialChannel>();
        channel->setPortName("COM6");
        return channel;
    })
    .withReceiveData([](bcf::ErrorCode code, std::shared_ptr<bcf::AbstractProtocolModel> model) {
        printf("code:%d \n", code);
        printf("protocolType:%d \n", model->protocolType());
    })
    .withFailedCallback([]() {})
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel>
    channel) {
        printf("withConnectionCompletedCallback channelID:%d \n", channel->channelID());
    })
    .connect();

    std::shared_ptr<bcf::AbstractProtocolModel> reqmodel = std::make_shared<bcf::AProtocolModel>();
    reqmodel->seq = 1;
    requestPtr->request(reqmodel, [](bcf::ErrorCode code,
    std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
        printf( "retmodel code:%d,\n", code) ;
        if (retmodel) {
            printf("retmodel seq:%d,\n", retmodel->seq) ;
        }
    });
    return app.exec();
}
