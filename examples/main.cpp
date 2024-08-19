#include <QCoreApplication>
#include <QDebug>
#include "bcf.h"
using namespace std;

#define CHANNEL_ID_SERIALPORT 0

bcf::PackMode userPackMode = static_cast<bcf::PackMode>(bcf::PackMode::UNPACK_BY_USER + 1);
class UserProtocolModel : public bcf::AbstractProtocolModel
{
    virtual bcf::PackMode protocolType() override
    {
        return userPackMode;
    };

    //your extend fields  e.g. ByHeadProtocolModel
};

class UserProtocolBuilder : public bcf::IProtocolBuilder
{
protected:
    virtual bcf::PackMode getType()const override
    {
        return userPackMode;
    };
    virtual std::shared_ptr<bb::ByteBuffer> build(std::shared_ptr<bcf::AbstractProtocolModel> _model)
    override
    {
        //your build...  e.g. ByHeadProtocolBuilder
        return nullptr;
    };
};

class UserProtocolParser : public bcf::IProtocolParser
{
public:
    virtual bcf::PackMode getType()const override
    {
        return userPackMode;
    };
    virtual void parse(const
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel>)>& callback)
    override
    {
        std::shared_ptr<bcf::ByHeadProtocolModel> model = std::make_shared<bcf::ByHeadProtocolModel>();
        //your parse...  e.g. ByHeadProtocolParser
        callback(ParserState::OK, model);
    };
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    auto requestPtr = bcf::RequestHandlerBuilder()
                      .withTimeOut(10'000)
                      .withProtocolBuilders({std::make_shared<ByHeadProtocolBuilder>(),
                                             std::make_shared<UserProtocolBuilder>()})
                      .withProtocolParsers({std::make_shared<ByHeadProtocolParser>()})
    .withAbandonCallback([](std::shared_ptr<bcf::AbstractProtocolModel> model) {})
    .withChannel(CHANNEL_ID_SERIALPORT, []() {
        auto channel = std::make_shared<bcf::SerialChannel>();
//        channel->setRawDataCallback([](std::shared_ptr<bb::ByteBuffer> bb) {
//            bb->printHex();
//        });
        channel->setPortName("COM2");
        return channel;
    })
    .withReceiveData([](std::shared_ptr<bcf::AbstractProtocolModel> model) {
        qDebug() <<  "withReceiveData protocolType:"  << model->protocolType();
        if (model->protocolType() == bcf::PackMode::UNPACK_BY_LENGTH_FIELD) {
            auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(model);
            if (bmodel) {
                qDebug() <<  "withReceiveData retmodel seq: " << bmodel->seq ;
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
    .connect();

    std::shared_ptr<bcf::ByHeadProtocolModel> reqmodel = std::make_shared<bcf::ByHeadProtocolModel>();
    reqmodel->seq = bcf::util::getNextSeq();
#define CMD_TEST_LOOP 1
    reqmodel->cmd = CMD_TEST_LOOP;
    reqmodel->setBody(std::string("this is " +  std::to_string(reqmodel->seq) + " times request"));

    requestPtr->request(reqmodel, [](bcf::ErrorCode code,
    std::shared_ptr<bcf::AbstractProtocolModel> retmodel) {
        auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(retmodel);
        if (bmodel) {
            qDebug() << "retmodel seq: " << bmodel->seq ;
            qDebug() << "retmodel body:" <<  QString::fromStdString(bmodel->body());
        }
    });

//    std::string filename = "C:\\Users\\bridge\\Desktop\\image.bin";//902 字节
//    requestPtr->sendFileWithYModel(filename, [](int progress) {
//        qDebug() << "progress: " << progress ;
//    }, [](bcf::TransmitStatus status) {
//    });
    return app.exec();
}
