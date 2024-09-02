#include <QObject>
#include "base/globaldefine.h"
#include "private/channelmanager.h"
#include "private/protocolbuildermanager.h"
#include "private/protocolparsermanager.h"
#include "private/filetransmithelper.h"
#include "requesthandler.h"

using namespace bcf;

#define TIMER_INTERVAL_MILLS 1'000

Q_DECLARE_METATYPE(std::shared_ptr<AbstractProtocolModel>);
class RequestHandler::RequestHandlerPrivate: public QObject
{
    Q_OBJECT
public:
    RequestHandlerPrivate(RequestHandler* q)
        : q_ptr(q)
        , m_protocolParserManager(std::make_unique<ProtocolParserManager>())
        , m_protocolBuilderManager(std::make_unique<ProtocolBuilderManager>())
        , m_fileTransmitHelper(std::make_unique<FileTransmitHelper>())
        , m_channelManager(std::make_unique<ChannelManager>())
    {
        qRegisterMetaType<std::shared_ptr<AbstractProtocolModel>>("std::shared_ptr<AbstractProtocolModel>");
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
    void request(const std::shared_ptr<AbstractProtocolModel>& model, RequestCallback&& callback);
    void sendFileWithYModel(const std::string& fileName, const ProgressCallback& pcallback,
                            const TransmitStatusCallback& tcallback, int timeoutMills);
    void recvFileWithYModel(const std::string& savePath, const ProgressCallback& pcallback,
                            const TransmitStatusCallback& tcallback, int timeoutMills);
    void connect();

signals:
    void signalParseOver(std::shared_ptr<AbstractProtocolModel>);

private slots:
    void slotTimeout();
    void slotParseOver(std::shared_ptr<AbstractProtocolModel>);

private:
    void setProtocolBuilders(const
                             std::vector<std::shared_ptr<IProtocolBuilder>>& protocolBuilders);
    void setProtocolParsers(const
                            std::vector<std::shared_ptr<IProtocolParser>>& protocolParsers);
    /**
    * @brief 如何区分是发送回复的还是主动上报的？因此全部都是走receive，receive里面根据seq判断转给哪个callback.
    *        如果能匹配到seq,则直接callback,如果匹配不到,则是主动上报的,交给receive直接回调
    */
    void receive(ReceiveCallback&& _callback);

private:
    RequestHandler*                                     q_ptr;

    QTimer                                              m_timer;
    std::atomic_bool                                    m_isexit;
    std::map<int, std::pair<int32_t, RequestCallback>>  m_callbacks;    //key:seq,key:timeout
    ReceiveCallback                                     m_recvCallback;
    std::unique_ptr<ProtocolParserManager>              m_protocolParserManager;
    std::unique_ptr<ProtocolBuilderManager>             m_protocolBuilderManager;
    std::unique_ptr<FileTransmitHelper>                 m_fileTransmitHelper;

    friend class RequestHandlerBuilder;
    ConnectOption                                       m_ConnectOption;
    std::unique_ptr<ChannelManager>                     m_channelManager;
};

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

RequestHandlerBuilder& RequestHandlerBuilder::withChannel(CreateChannelFunc&& func)
{

    int64_t intValue = static_cast<int64_t>(reinterpret_cast<uintptr_t>(this));
    m_ConnectOption.m_channelid = intValue;
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
    //因为RequestHandler的构造函数是私有的，防止从外部构造，所以下面的std::make_shared也是不可用的
    //m_requestHandler = std::make_shared<RequestHandler>();
    m_requestHandler->d_ptr->m_ConnectOption = std::move(m_ConnectOption);
    m_requestHandler->d_ptr->setProtocolBuilders(std::move(m_protocolBuilders));
    m_requestHandler->d_ptr->setProtocolParsers(std::move(m_protocolParsers));
    m_requestHandler->d_ptr->m_channelManager->registerChannel(m_ConnectOption.m_channelid,
                                                               std::move(m_ccfunc));
    return m_requestHandler;
}

RequestHandler::RequestHandler()
    : d_ptr(std::make_unique<RequestHandlerPrivate>(this)) {}

void RequestHandler::request(const std::shared_ptr<AbstractProtocolModel>& model,
                             RequestCallback&& callback)
{
    d_ptr->request(model, std::move(callback));
}

void RequestHandler::sendFileWithYModel(const std::string& fileName,
                                        const ProgressCallback& pcallback,
                                        const TransmitStatusCallback& tcallback, int timeoutMillS)
{
    d_ptr->sendFileWithYModel(fileName, pcallback, tcallback, timeoutMillS);
}

void RequestHandler::recvFileWithYModel(const std::string& savePath,
                                        const ProgressCallback& pcallback,
                                        const TransmitStatusCallback& tcallback, int timeoutMillS)
{
    d_ptr->recvFileWithYModel(savePath, pcallback, tcallback, timeoutMillS);
}

void RequestHandler::connect()
{
    d_ptr->connect();
}

void RequestHandler::RequestHandlerPrivate::request(const std::shared_ptr<AbstractProtocolModel>&
                                                    model,
                                                    RequestCallback&& callback)
{
    const auto channel = m_channelManager->getChannel(m_ConnectOption.m_channelid);
    if (nullptr == channel || !channel->isOpen()) {
        std::cerr << "error,channel is not open" << std::endl;
        (callback)(ErrorCode::CHANNEL_CLOSE, nullptr);
        return;
    }

    const auto buffer = m_protocolBuilderManager->build(model->protocolType(), model);
    if (!buffer) {
        std::cerr << "not support ProtocolModel" << std::endl;
        (callback)(ErrorCode::PROTOCOL_NOT_EXIST, nullptr);
        return;
    }

    if (-2 == channel->send((const char* )buffer->data(), buffer->size())) {
        (callback)(ErrorCode::ERROR_THREAD_AFFINITY, nullptr);
        return;
    }

    m_callbacks.emplace(std::make_pair(model->seq,
                                       std::make_pair(m_ConnectOption.m_timeoutMillSeconds,
                                                      std::move(callback))));
}

void RequestHandler::RequestHandlerPrivate::sendFileWithYModel(const std::string& fileName,
                                                               const ProgressCallback& pcallback, const TransmitStatusCallback& tcallback, int timeoutMills)
{
    const auto channel = m_channelManager->getChannel(m_ConnectOption.m_channelid);
    m_fileTransmitHelper->setTimeOut(timeoutMills);
    m_fileTransmitHelper->startTransmitWithYModel(channel, fileName, std::move(pcallback),
                                                  std::move(tcallback));
}

void RequestHandler::RequestHandlerPrivate::recvFileWithYModel(const std::string& savePath,
                                                               const ProgressCallback& pcallback, const TransmitStatusCallback& tcallback, int timeoutMills)
{
    const auto channel = m_channelManager->getChannel(m_ConnectOption.m_channelid);
    m_fileTransmitHelper->setTimeOut(timeoutMills);
    m_fileTransmitHelper->startRecvWithYModel(channel, savePath, std::move(pcallback),
                                              std::move(tcallback));
};

void RequestHandler::RequestHandlerPrivate::receive(ReceiveCallback&& _callback)
{
    m_recvCallback = std::move(_callback);
    DataCallback call = [ this ](std::shared_ptr<bb::ByteBuffer>
    data) {

        m_protocolParserManager->parseByAll(data, [ = ](ParserState
                                                        state,
        std::shared_ptr<AbstractProtocolModel> model) {
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
                                                                std::vector<std::shared_ptr<IProtocolBuilder>>& protocolBuilders)
{
    for (const auto& p : protocolBuilders) {
        m_protocolBuilderManager->addBuilder(std::move(p));
    }
}

void RequestHandler::RequestHandlerPrivate::setProtocolParsers(const
                                                               std::vector<std::shared_ptr<IProtocolParser>>& protocolParsers)
{
    for (const auto& p : protocolParsers) {
        m_protocolParserManager->addParser(std::move(p));
    }
}

void RequestHandler::RequestHandlerPrivate::connect()
{
    const auto channel = m_channelManager->CreateChannel(m_ConnectOption.m_channelid);
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
    auto it  = m_callbacks.begin();
    while (it != m_callbacks.end()) {
        if ( 0 == it->second.first) {
            //超时时间减到0
            std::cout << "seq: " << it->first << "timeout,remove it!" << std::endl;
            std::bind(it->second.second, ErrorCode::TIME_OUT, nullptr)();
            m_callbacks.erase(it++);
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
    if (nullptr == model) {
        return;
    }

    auto& key = model->seq;
    const auto& itr = m_callbacks.find(key);
    if (itr != m_callbacks.end()) {
        if (nullptr != itr->second.second) {
            itr->second.second(ErrorCode::OK, model);
        }
        m_callbacks.erase(itr);
        return;
    }

    std::cout << "not find seq" << std::endl;
    auto thizz = q_ptr->shared_from_this();
    m_recvCallback(thizz, model);
}

#include "requesthandler.moc"
