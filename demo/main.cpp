#include <iostream>
#include <abstractprotocolmodel.h>
#include "aprotocolbuilder.h"
#include "requesthandler.h"
using namespace std;

int main()
{
    auto abandonCallBack = [](std::shared_ptr<bcf::AbstractProtocolModel> model) {
        cout << "protocolType:" << model->protocolType() << endl;
    };


    std::shared_ptr<bcf::IProtocolParser> aProtocolParser = std::make_shared<AProtocolParser>();
    bcf::RequestHandlerBuilder builder;
    auto handler = builder
                   .WithTimeOut(100'000)
                   .withAbandonCallback(abandonCallBack)
                   .withProtocolParsers({aProtocolParser})
                   .build();

    handler->receive([](bcf::ErrorCode code, std::shared_ptr<bcf::AbstractProtocolModel> model) {
        cout << "code:" << code << endl;
        cout << "protocolType:" << model->protocolType() << endl;
    },
    bcf::ChannelID::TCP);

    return 0;
}
