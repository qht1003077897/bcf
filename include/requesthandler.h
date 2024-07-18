#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <deque>
#include <ichannel.h>
#include <bcfexport.h>
#include <abstractprotocolmodel.h>

namespace bcf
{

using RequestCallback = std::function<void(int, std::shared_ptr<bcf::AbstractProtocolModel>)>;
using ReceiveCallback = RequestCallback;

class BCF_EXPORT RequestHandler
{
public:
    static RequestHandler& getInstance();

    void request(std::shared_ptr<bcf::AbstractProtocolModel> model, std::shared_ptr<RequestCallback>);
    void receive(ReceiveCallback, bcf::ChannelID id = bcf::ChannelID::Serial);

    void addInterceptor();
private:

    RequestHandler();
    ~RequestHandler();
    void startUserCallbackThread();
    void startTimeOut();
    void pushqueueAndNotify(std::function<void()>&& data);
    std::deque<std::function<void()>> popall();


private:
    std::deque<std::function<void()>> m_userqueue;
    std::atomic_bool m_isexit;
    std::mutex m_mtx;
    std::mutex m_mtxuserdata;
    std::condition_variable m_cv;
    std::thread m_usercallbackthread;

    //key:seq,value:first为超时时间
    std::map<int, std::pair<int, std::shared_ptr<RequestCallback>>> callbacks;
    int timeoutSeconds = 10;
};
}
