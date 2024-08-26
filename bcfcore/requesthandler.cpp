#include <QObject>
#include <base/timer.h>
#include <base/globaldefine.h>
#include <private/channelmanager.h>
#include <private/protocolbuildermanager.h>
#include <private/protocolparsermanager.h>
#include "private/filetransmithelper.h"
#include "requesthandler.h"
using namespace bcf;

#define TIMER_INTERVAL_MILLS 1'000

Q_DECLARE_METATYPE(std::shared_ptr<bcf::AbstractProtocolModel>);
class RequestHandler::RequestHandlerPrivate: public QObject
{
    Q_OBJECT
public:
    RequestHandlerPrivate(RequestHandler* q)
        : q_ptr(q)
        , protocolParserManager(std::make_unique<bcf::ProtocolParserManager>())
        , protocolBuilderManager(std::make_unique<bcf::ProtocolBuilderManager>())
        , m_channelManager(std::make_unique<bcf::ChannelManager>())
        , m_fileTransmitHelper(std::make_unique<bcf::FileTransmitHelper>())
    {
        qRegisterMetaType<std::shared_ptr<bcf::AbstractProtocolModel>>("std::shared_ptr<bcf::AbstractProtocolModel>");
        QObject::connect(&m_timer, &QTimer::timeout, this, &RequestHandlerPrivate::slotTimeout);
        QObject::connect(this, &RequestHandlerPrivate::signalParseOver, this,
                         &RequestHandlerPrivate::slotParseOver);
        m_timer.start(TIMER_INTERVAL_MILLS);
    };
    ~RequestHandlerPrivate()
    {
        m_isexit = true;
        m_timer.stop();
    };
    void request(std::shared_ptr<bcf::AbstractProtocolModel> model, RequestCallback&& callback);
    void sendFile(const std::string& fileName, const ProgressCallback& pcallback,
                  const TransmitStatusCallback& tcallback);
    void sendFileWithYModel(const std::string& fileName, const ProgressCallback& pcallback,
                            const TransmitStatusCallback& tcallback);
    void connect();

signals:
    void signalParseOver(std::shared_ptr<bcf::AbstractProtocolModel>);
private slots:
    void slotTimeout();
    void slotParseOver(std::shared_ptr<bcf::AbstractProtocolModel>);

private:
    void setProtocolBuilders(const
                             std::vector<std::shared_ptr<bcf::IProtocolBuilder>>& protocolBuilders);
    void setProtocolParsers(const
                            std::vector<std::shared_ptr<bcf::IProtocolParser>>& protocolParsers);
    void receive(bcf::ReceiveCallback&& _callback);

private:
    RequestHandler* q_ptr;
    friend class RequestHandlerBuilder;
    std::atomic_bool m_isexit;
    //key:seq,key:timeout
    std::map<int, std::pair<int32_t, bcf::RequestCallback>> callbacks;
    ReceiveCallback m_recvCallback;
    std::unique_ptr<bcf::ProtocolParserManager> protocolParserManager;
    std::unique_ptr<bcf::ProtocolBuilderManager> protocolBuilderManager;
    std::unique_ptr<bcf::ChannelManager> m_channelManager;
    QTimer m_timer;
    std::unique_ptr<bcf::FileTransmitHelper> m_fileTransmitHelper;
    ConnectOption m_ConnectOption;
};

RequestHandlerBuilder::RequestHandlerBuilder() {}

RequestHandlerBuilder& RequestHandlerBuilder::withTimeOut(int timeoutMillSeconds)
{

    m_ConnectOption.m_timeoutMillSeconds = timeoutMillSeconds;
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withMaxRecvBufferSize(int maxRecvBufferSize)
{
    m_ConnectOption.m_maxRecvBufferSize = maxRecvBufferSize;
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::registProtocolBuilders(const
                                                                     std::vector<std::shared_ptr<IProtocolBuilder> >& protocolBuilders)
{
    m_protocolBuilders = std::move(protocolBuilders);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::registProtocolParsers(
    const std::vector<std::shared_ptr<IProtocolParser>>& protocolParsers)
{
    m_protocolParsers = std::move(protocolParsers);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withChannel(int channelID,
                                                          CreateChannelFunc&& func)
{
    m_ConnectOption.m_channelid = channelID;
    m_ccfunc = std::move(func);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withFailedCallback(ConnectionFailCallback&& callback)
{
    m_ConnectOption.m_FailCallback = std::move(callback);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withConnectionCompletedCallback(
    ConnectionCompletedCallback&& callback)
{
    m_ConnectOption.m_CompleteCallback = std::move(callback);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withReceiveData(ReceiveCallback&& callback)
{
    m_ConnectOption.m_ReceiveCallback = std::move(callback);
    return *this;
}

std::shared_ptr<RequestHandler> RequestHandlerBuilder::build()
{
    m_requestHandler = std::shared_ptr<RequestHandler>(new RequestHandler());
    m_requestHandler->d_ptr->m_ConnectOption = std::move(m_ConnectOption);
    m_requestHandler->d_ptr->setProtocolBuilders(std::move(m_protocolBuilders));
    m_requestHandler->d_ptr->setProtocolParsers(std::move(m_protocolParsers));
    m_requestHandler->d_ptr->m_channelManager->registerChannel(m_ConnectOption.m_channelid,
                                                               std::move(m_ccfunc));
    return m_requestHandler;
}

RequestHandler::RequestHandler()
    : d_ptr(std::make_unique<RequestHandlerPrivate>(this))
{
}

void RequestHandler::request(std::shared_ptr<bcf::AbstractProtocolModel> model,
                             RequestCallback&& callback)
{
    d_ptr->request(model, std::move(callback));
}

void RequestHandler::sendFile(const std::string& fileName, const ProgressCallback& pcallback,
                              const TransmitStatusCallback& tcallback)
{
    d_ptr->sendFile(fileName, pcallback, tcallback);
}

void RequestHandler::sendFileWithYModel(const std::string& fileName,
                                        const ProgressCallback& pcallback,
                                        const TransmitStatusCallback& tcallback)
{
    d_ptr->sendFileWithYModel(fileName, pcallback, tcallback);
}

void RequestHandler::connect()
{
    d_ptr->connect();
}

void RequestHandler::RequestHandlerPrivate::request(std::shared_ptr<bcf::AbstractProtocolModel>
                                                    model,
                                                    RequestCallback&& callback)
{
    const auto channel = m_channelManager->getChannel(m_ConnectOption.m_channelid);
    if (nullptr == channel || !channel->isOpen()) {
        std::cerr << "error,channel is not open" << std::endl;
        (callback)(bcf::ErrorCode::CHANNEL_CLOSE, nullptr);
        return;
    }

    auto buffer = protocolBuilderManager->build(model->protocolType(), model);
    if (!buffer) {
        std::cerr << "not support ProtocolModel" << std::endl;
        (callback)(bcf::ErrorCode::PROTOCOL_NOT_EXIST, nullptr);
        return;
    }

    if (-2 == channel->send((const char * )buffer->data(), buffer->size())) {
        (callback)(bcf::ErrorCode::ERROR_THREAD_AFFINITY, nullptr);
        return;
    }

    callbacks.insert(std::make_pair(model->seq,
                                    std::make_pair(m_ConnectOption.m_timeoutMillSeconds,
                                                   std::move(callback))));
}

void RequestHandler::RequestHandlerPrivate::sendFile(const std::string& fileName,
                                                     const ProgressCallback& pcallback, const TransmitStatusCallback& tcallback)
{
    const auto channel = m_channelManager->getChannel(m_ConnectOption.m_channelid);
    m_fileTransmitHelper->startTransmit(channel, fileName, std::move(pcallback), std::move(tcallback));
}

void RequestHandler::RequestHandlerPrivate::sendFileWithYModel(const std::string& fileName,
                                                               const ProgressCallback& pcallback, const TransmitStatusCallback& tcallback)
{
    const auto channel = m_channelManager->getChannel(m_ConnectOption.m_channelid);
    m_fileTransmitHelper->startTransmitWithYModel(channel, fileName, std::move(pcallback),
                                                  std::move(tcallback));
};

////如何区分是发送回复的还是主动上报的？因此全部都是走receive，receive里面根据业务ID判断转给哪个callback
////如果能匹配到seq,则直接callback,如果匹配不到,则是主动上报的,交给receive调用点进行转发处理
void RequestHandler::RequestHandlerPrivate::receive(ReceiveCallback&& _callback)
{
    m_recvCallback = std::move(_callback);
    DataCallback call =  [ = ](std::shared_ptr<bb::ByteBuffer>
    data) {

        protocolParserManager->parseByAll(data, [ = ](bcf::ParserState
                                                      state,
        std::shared_ptr<bcf::AbstractProtocolModel> model) {
            if (nullptr == model) {
                return;
            }

            emit signalParseOver(model);
        });
    };

    const auto channel =  m_channelManager->getChannel(m_ConnectOption.m_channelid);
    channel->setDataCallback(std::move(call));
};

void RequestHandler::RequestHandlerPrivate::setProtocolBuilders(const
                                                                std::vector<std::shared_ptr<bcf::IProtocolBuilder>>& protocolBuilders)
{
    for (const auto& p : protocolBuilders) {
        protocolBuilderManager->addBuilder(std::move(p));
    }
}

void RequestHandler::RequestHandlerPrivate::setProtocolParsers(const
                                                               std::vector<std::shared_ptr<bcf::IProtocolParser>>& protocolParsers)
{
    for (const auto& p : protocolParsers) {
        protocolParserManager->addParser(std::move(p));
    }
}

void RequestHandler::RequestHandlerPrivate::connect()
{
    auto channel = m_channelManager->CreateChannel(m_ConnectOption.m_channelid);
    channel->setMaxRecvBufferSize(m_ConnectOption.m_maxRecvBufferSize);
    channel->setFailedCallback(std::move(m_ConnectOption.m_FailCallback));
    channel->setConnectionCompletedCallback(std::move(m_ConnectOption.m_CompleteCallback));
    if (nullptr != channel) {
        channel->open();
        receive(std::move(m_ConnectOption.m_ReceiveCallback));
    }
}

void RequestHandler::RequestHandlerPrivate::slotTimeout()
{
    auto it  = callbacks.begin();
    while (it != callbacks.end()) {
        if ( 0 == it->second.first) {
            //超时时间减到0

            std::cout << "seq: " << it->first << "timeout,remove it!" << std::endl;
            std::bind(it->second.second, bcf::ErrorCode::TIME_OUT, nullptr)();
            callbacks.erase(it++);
            continue;
        }

        //每秒递减1,直到被callback完成移除或超时移除 //TODO 优化:支持毫秒级定时器
        it->second.first = std::max(0,  it->second.first - 1000);
        it++;
    }
}

void RequestHandler::RequestHandlerPrivate::slotParseOver(std::shared_ptr<AbstractProtocolModel>
                                                          model)
{
    int key = model->seq;
    const auto& itr = callbacks.find(key);
    if (itr != callbacks.end()) {
        if (nullptr != itr->second.second) {
            itr->second.second(bcf::ErrorCode::OK, model);
        }
        callbacks.erase(itr);
        return;
    }

    std::cout << "not find seq" << std::endl;
    auto handler = q_ptr->shared_from_this();
    m_recvCallback(handler, model);
}

#include "requesthandler.moc"
