#include <channelmanager.h>
#include <base/globaldefine.h>
#include <base/timer.h>
#include <protocolbuildermanager.h>
#include <protocolparsermanager.h>
#include "filetransmithelper.h"
#include "requesthandler.h"
using namespace bcf;

class RequestHandler::RequestHandlerPrivate
{
public:
    RequestHandlerPrivate()
        : protocolParserManager(std::make_unique<bcf::ProtocolParserManager>())
        , protocolBuilderManager(std::make_unique<bcf::ProtocolBuilderManager>())
        , m_channelManager(std::make_unique<bcf::ChannelManager>())
        , m_timer(new bcf::Timer())
        , m_fileTransmitHelper(std::make_unique<bcf::FileTransmitHelper>())
    {
        startTimeOut();
    };
    ~RequestHandlerPrivate()
    {
        m_isexit = true;
        m_timer->stop();
    };
    void request(std::shared_ptr<bcf::AbstractProtocolModel> model, RequestCallback&& callback);
    void sendFile(const std::string& fileName, const ProgressCallback& pcallback,
                  const TransmitStatusCallback& tcallback);
    void sendFileWithYModel(const std::string& fileName, const ProgressCallback& pcallback,
                            const TransmitStatusCallback& tcallback);
private:
    void startTimeOut();
    void setAbandonCallback(bcf::AbandonCallback && callback);
    void setProtocolBuilders(const
                             std::vector<std::shared_ptr<bcf::IProtocolBuilder>>& protocolBuilders);
    void setProtocolParsers(const
                            std::vector<std::shared_ptr<bcf::IProtocolParser>>& protocolParsers);
    void receive(bcf::ReceiveCallback&& _callback);
    void connect();

private:
    friend class RequestHandlerBuilder;
    std::atomic_bool m_isexit;
    std::mutex m_mtx;
    //key:seq,key:timeout
    std::map<int, std::pair<int32_t, bcf::RequestCallback>> callbacks;
    bcf::AbandonCallback m_abandonCallback;
    std::unique_ptr<bcf::ProtocolParserManager> protocolParserManager;
    std::unique_ptr<bcf::ProtocolBuilderManager> protocolBuilderManager;
    std::unique_ptr<bcf::ChannelManager> m_channelManager;
    std::shared_ptr<bcf::Timer> m_timer;
    std::unique_ptr<bcf::FileTransmitHelper> m_fileTransmitHelper;
    ConnectOption m_ConnectOption;
};

RequestHandlerBuilder::RequestHandlerBuilder()
    : m_requestHandler(std::make_shared<RequestHandler>()) {}

RequestHandlerBuilder& RequestHandlerBuilder::withTimeOut(int timeoutMillSeconds)
{
    m_requestHandler->d_ptr->m_ConnectOption.m_timeoutMillSeconds = timeoutMillSeconds;
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withMaxRecvBufferSize(int maxRecvBufferSize)
{
    m_requestHandler->d_ptr->m_ConnectOption.m_maxRecvBufferSize = maxRecvBufferSize;
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withAbandonCallback(bcf::AbandonCallback&& callback)
{
    m_requestHandler->d_ptr->setAbandonCallback(std::move(callback));
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withProtocolBuilders(const
                                                                   std::vector<std::shared_ptr<IProtocolBuilder> >& protocolBuilders)
{
    m_requestHandler->d_ptr->setProtocolBuilders(protocolBuilders);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withProtocolParsers(
    const std::vector<std::shared_ptr<IProtocolParser>>& protocolParsers)
{
    m_requestHandler->d_ptr->setProtocolParsers(protocolParsers);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withChannel(int channelID,
                                                          CreateChannelFunc&& func)
{
    m_requestHandler->d_ptr->m_ConnectOption.m_channelid = channelID;
    m_requestHandler->d_ptr->m_channelManager->registerChannel(channelID, std::move(func));
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withFailedCallback(ConnectionFailCallback&& callback)
{
    m_requestHandler->d_ptr->m_ConnectOption.m_FailCallback = std::move(callback);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withConnectionCompletedCallback(
    ConnectionCompletedCallback&& callback)
{
    m_requestHandler->d_ptr->m_ConnectOption.m_CompleteCallback = std::move(callback);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withReceiveData(ReceiveCallback&& callback)
{
    m_requestHandler->d_ptr->m_ConnectOption.m_ReceiveCallback = std::move(callback);
    return *this;
}

std::shared_ptr<RequestHandler> RequestHandlerBuilder::connect()
{
    m_requestHandler->d_ptr->connect();
    return m_requestHandler;
}

RequestHandler::RequestHandler()
    : d_ptr(std::make_unique<RequestHandlerPrivate>())
{
}

RequestHandler::~RequestHandler()
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

    auto res = protocolBuilderManager->build(model->protocolType(), model);
    {
        std::unique_lock<std::mutex> l(m_mtx);
        callbacks.insert(std::make_pair(model->seq,
                                        std::make_pair(m_ConnectOption.m_timeoutMillSeconds,
                                                       std::move(callback))));
    }

    auto buf = std::make_unique<uint8_t[]>(res->size());
    res->getBytes(buf.get(), res->size());
    channel->send((const char*)buf.get(), res->size());
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
    DataCallback call =  [ =, tmpCallback = std::move(_callback)](std::shared_ptr<bb::ByteBuffer>
    data) {

        protocolParserManager->parseByAll(data, [ = ](bcf::IProtocolParser::ParserState
                                                      state,
        std::shared_ptr<bcf::AbstractProtocolModel> model) {
            if (state == IProtocolParser::Abandon) {
                if (nullptr != m_abandonCallback) {
                    m_abandonCallback(model);
                }
                return;
            }

            if (nullptr == model) {
                return;
            }

            {
                int key = model->seq;
                std::unique_lock<std::mutex> l(m_mtx);
                const auto& itr = callbacks.find(key);
                if (itr != callbacks.end()) {
                    if (nullptr != itr->second.second) {
                        itr->second.second(bcf::ErrorCode::OK, model);
                    }
                    callbacks.erase(itr);
                    return;
                }
            }

            std::cout << "not find seq" << std::endl;
            tmpCallback(model);
        });
    };

    const auto channel =  m_channelManager->getChannel(m_ConnectOption.m_channelid);
    channel->setDataCallback(std::move(call));
};


void RequestHandler::RequestHandlerPrivate::setAbandonCallback(AbandonCallback &&callback)
{
    m_abandonCallback = std::move(callback);
}

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
    auto channel =  m_channelManager->CreateChannel(m_ConnectOption.m_channelid);
    channel->setMaxRecvBufferSize(m_ConnectOption.m_maxRecvBufferSize);
    channel->setFailedCallback(std::move(m_ConnectOption.m_FailCallback));
    channel->setConnectionCompletedCallback(std::move(m_ConnectOption.m_CompleteCallback));
    if (nullptr != channel) {
        channel->open();
        receive(std::move(m_ConnectOption.m_ReceiveCallback));
    }
}

void RequestHandler::RequestHandlerPrivate::startTimeOut()
{
    m_timer->setInterval([this]() {
        {
            std::unique_lock<std::mutex> l(m_mtx);
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
    }, 1000);
}
