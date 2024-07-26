#include <iostream>
#include <abstractprotocolmodel.h>
#include "aprotocolbuilder.h"
#include "requesthandler.h"
#include "serialchannel.h"
#include <QCoreApplication>
using namespace std;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    std::shared_ptr<bcf::RequestHandler> request
        = bcf::RequestHandlerBuilder()
          .withTimeOut(100'000)
          .withProtocolParsers({std::make_shared<AProtocolParser>()})
    .withAbandonCallback([](std::shared_ptr<bcf::AbstractProtocolModel> model) {
        cout << "protocolType:" << model->protocolType() << endl;
    })
    .withChannel(bcf::ChannelID::Serial, []() {
        return std::make_shared<bcf::SerialChannel>();
    })
    .withSerialPortPortName("COM2")
    .withReceiveData([](bcf::ErrorCode code, std::shared_ptr<bcf::AbstractProtocolModel> model) {
        cout << "code:" << code << endl;
        cout << "protocolType:" << model->protocolType() << endl;
    })
    .asyncConnect();

//    request->request();
    return app.exec();
}
