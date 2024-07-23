#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <ichannel.h>
#include <bcfexport.h>
#include <globaldefine.h>
#include <abstractprotocolmodel.h>

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
    void addInterceptor();

private:
    void startUserCallbackThread();
    void startTimeOut();
    void pushqueueAndNotify(std::function<void()>&& data);
    std::deque<std::function<void()>> popall();

    friend class RequestHandlerBuilder;
    void setTimeOut(int timeoutMillSeconds);
    void setAbandonCallback(bcf::AbandonCallback&& callback);
private:
    std::atomic_bool m_isexit;
    std::mutex m_mtx;
    //key:seq,value:first为超时时间
    std::map<int, std::pair<int, std::shared_ptr<RequestCallback>>> callbacks;
    int m_timeoutMillSeconds = 10'000;

    AbandonCallback m_abandonCallback;
};
}
