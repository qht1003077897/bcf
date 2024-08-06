#include <abstractprotocolmodel.h>
#include "protocolparser/byheadprotocolparser.h"
#include "requesthandler.h"
#include "serialchannel.h"
#include <QCoreApplication>
#include <QDebug>

using namespace std;
constexpr int CHANNEL_ID_SERIALPORT = 0;

class UserProtocolModel : public bcf::AbstractProtocolModel
{
    bcf::PackMode userPackMode = static_cast<bcf::PackMode>(bcf::PackMode::UNPACK_BY_USER + 1);
    virtual bcf::PackMode protocolType() override
    {
        return userPackMode;
    };
};

class UserProtocolBuilder : public bcf::IProtocolBuilder
{
protected:
    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    virtual std::shared_ptr<bb::ByteBuffer> build(std::shared_ptr<bcf::AbstractProtocolModel> _model)
    override
    {
        return nullptr;
    };
};

class UserProtocolParser : public bcf::IProtocolParser
{
public:
    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    virtual void parse(const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                       const std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel>)>& callback)
    override
    {
        std::shared_ptr<bcf::ByHeadProtocolModel> model = std::make_shared<bcf::ByHeadProtocolModel>();
        callback(ParserState::OK, model);
    };
};

int main(int argc, char* argv[])
{

    setbuf(stdout, NULL);

    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .withProtocolBuilders({std::make_shared<ByHeadProtocolBuilder>(), std::make_shared<UserProtocolBuilder>()})
                      .withProtocolParsers({std::make_shared<ByHeadProtocolParser>()})
    .withAbandonCallback([](std::shared_ptr<bcf::AbstractProtocolModel> model) {})
#ifdef BCF_USE_SERIALPORT
    .withChannel(CHANNEL_ID_SERIALPORT, (bcf::CreateChannelFunc)[]() {
        auto channel = std::make_shared<bcf::SerialChannel>();
        channel->setPortName("COM2");
        return channel;
    })
#endif
    .withReceiveData([](bcf::ErrorCode code, std::shared_ptr<bcf::AbstractProtocolModel> model) {
        printf("code:%d \n", code);
        printf("protocolType:%d \n", model->protocolType());
    })
    .withFailedCallback([]() {
        printf("withFailedCallback \n");
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        printf("withConnectionCompletedCallback channelID:%d \n", channel->channelID());
    })
    .connect();

    std::shared_ptr<bcf::ByHeadProtocolModel> reqmodel = std::make_shared<bcf::ByHeadProtocolModel>();
    reqmodel->seq = 1;
    reqmodel->cmd = 1;
    reqmodel->setBody(std::string("this is " +  std::to_string(reqmodel->seq) + " times request"));

    requestPtr->request(reqmodel, [](bcf::ErrorCode code,
    std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
        printf( "retmodel code:%d \n", code) ;
        if (retmodel) {
            printf("retmodel seq:%d \n", retmodel->seq) ;
        }
    });
    return app.exec();
}
