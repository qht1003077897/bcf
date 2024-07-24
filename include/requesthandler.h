#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <vector>
#include <ichannel.h>
#include <bcfexport.h>
#include <globaldefine.h>
#include <abstractprotocolmodel.h>
#include <protocolparsermanager.h>

namespace bcf
{
class RequestHandler;
class BCF_EXPORT RequestHandlerBuilder
{
public:
    RequestHandlerBuilder();
    virtual ~RequestHandlerBuilder() = default;


    RequestHandlerBuilder& WithTimeOut(int timeoutMillSeconds);

    RequestHandlerBuilder& withAbandonCallback(bcf::AbandonCallback&& callback);
    //指定解析器
    RequestHandlerBuilder& withProtocolParsers(
        const std::vector<std::shared_ptr<bcf::IProtocolParser>>& protocolParsers);

    std::shared_ptr<RequestHandler> build();
private:
    std::shared_ptr<RequestHandler> m_requestHandler;
};

class BCF_EXPORT RequestHandler
{
public:
    RequestHandler();
    ~RequestHandler();
    void request(std::shared_ptr<bcf::AbstractProtocolModel> model, std::shared_ptr<RequestCallback>);
    //指定通道，默认尝试所有协议，可以通过过滤器过滤指定的协议
    void receive(ReceiveCallback, bcf::ChannelID id = bcf::ChannelID::Serial);

private:
    void startTimeOut();

    friend class RequestHandlerBuilder;
    void setTimeOut(int timeoutMillSeconds);
    void setAbandonCallback(bcf::AbandonCallback&& callback);
    void setProtocolParsers(const
                            std::vector<std::shared_ptr<bcf::IProtocolParser>>& protocolParsers);
private:
    std::atomic_bool m_isexit;
    std::mutex m_mtx;
    //key:channelID,key:seq,key:timeout
    std::map<bcf::ChannelID, std::map<int, std::pair<int, std::shared_ptr<RequestCallback>>>> callbacks;
    int m_timeoutMillSeconds = 10'000;

    AbandonCallback m_abandonCallback;
    std::vector<bcf::ProtocolType> m_protocolTypes;
    std::unique_ptr<ProtocolParserManager> protocolParserManager;
};
}
