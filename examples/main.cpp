#include <QCoreApplication>
#include "bcf.h"
using namespace std;

#define CHANNEL_ID_SERIALPORT 0

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
    virtual void parse(const
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel>)>& callback)
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
                      .withProtocolBuilders({std::make_shared<ByHeadProtocolBuilder>(),
                                             std::make_shared<UserProtocolBuilder>()})
                      .withProtocolParsers({std::make_shared<ByHeadProtocolParser>()})
    .withAbandonCallback([](std::shared_ptr<bcf::AbstractProtocolModel> model) {})
    .withChannel(CHANNEL_ID_SERIALPORT, []() {
        auto channel = std::make_shared<bcf::SerialChannel>();
        channel->setPortName("COM2");
        return channel;
    })
    .withReceiveData([](std::shared_ptr<bcf::AbstractProtocolModel> model) {
        std::cout <<  "withReceiveData protocolType:"  << model->protocolType() << std::endl;
        if (model->protocolType() == bcf::PackMode::UNPACK_BY_LENGTH_FIELD) {
            auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(model);
            if (bmodel) {
                std::cout <<  "withReceiveData retmodel seq: " << bmodel->seq << std::endl;
                std::cout << "withReceiveData retmodel body:" << bmodel->body() << std::endl;
            }
        }
    })
    .withFailedCallback([]() {
        std::cerr <<  "withFailedCallback"  << std::endl;
    })
    .withConnectionCompletedCallback([](std::shared_ptr<bcf::IChannel> channel) {
        std::cout <<  "withConnectionCompletedCallback channelID:" << channel->channelID() << std::endl;
    })
    .connect();

    std::shared_ptr<bcf::ByHeadProtocolModel> reqmodel = std::make_shared<bcf::ByHeadProtocolModel>();
    reqmodel->seq = bcf::util::getNextSeq();
#define CMD_TEST_LOOP 1
    reqmodel->cmd = CMD_TEST_LOOP;
    reqmodel->setBody(std::string("this is " +  std::to_string(reqmodel->seq) + " times request"));

//    requestPtr->request(reqmodel, [](bcf::ErrorCode code,
//    std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
//        auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(retmodel);
//        if (bmodel) {
//            std::cout << "retmodel seq: " << bmodel->seq << std::endl;
//            std::cout << "retmodel body:" << bmodel->body() << std::endl;
//        }
//    });

    std::string filename = "C:\\Users\\bridge\\Desktop\\image.bin";//902 字节
    requestPtr->sendFileWithYModel(filename, [](int progress) {
        std::cout << "progress: " << progress << std::endl;
    }, [](bcf::TransmitStatus status) {
    });
    return app.exec();
}
