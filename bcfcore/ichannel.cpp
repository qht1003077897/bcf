#include <ichannel.h>
#include <globaldefine.h>
using namespace bcf;

IChannel::~IChannel()
{
    m_isexit = true;
    m_cv.notify_all();
    if (m_usercallbackthread.joinable()) {
        m_usercallbackthread.join();
    }
}

bool IChannel::open()
{
    bool res = openChannel();
    if (res) {
        startUserCallbackThread();
        m_state = ChannelState::Opened;
    }
    return res;
}

void IChannel::close()
{
    closeChannel();
    m_state = ChannelState::Closed;
    stopUserCallbackThread();
}

void IChannel::setDataCallback(DataCallback && dataDallback)
{
    std::unique_lock<std::mutex> l(m_mtxuserdata);
    m_dataCallback = std::move(dataDallback);
}

void IChannel::setErrorCallback(ErrorCallback&& errorCallback)
{
    m_errorCallback = std::move(errorCallback);
}

void IChannel::pushData2Bcf(const std::string& data)
{
    pushqueueAndNotify(data);
}

void IChannel::startUserCallbackThread()
{
    m_usercallbackthread = std::thread([this]() {
        while (!m_isexit &&  (m_state != ChannelState::Opened)) {
            std::unique_lock<std::mutex> l(m_mtx);
            m_cv.wait_for(l, std::chrono::seconds(5), [this] {
                std::unique_lock<std::mutex> l(m_mtxuserdata);
                return !m_dataQueue.empty() || m_isexit || m_state == ChannelState::Closed;
            });

            if (m_isexit || m_state == ChannelState::Closed) {
                return;
            }

            {
                std::unique_lock<std::mutex> l(m_mtxuserdata);
                while (!m_dataQueue.empty()) {
                    auto  dataqueue = popall();
                    for (auto& str : dataqueue) {
                        if (m_dataCallback) {
                            m_dataCallback(str);
                        }
                    }
                }
            }
        }
    });
}

void IChannel::stopUserCallbackThread()
{
    m_cv.notify_all();
    if (m_usercallbackthread.joinable()) {
        m_usercallbackthread.join();
    }
}

void IChannel::pushqueueAndNotify(const std::string& data)
{
    std::unique_lock<std::mutex> l(m_mtxuserdata);
    m_dataQueue.emplace_back(data);
    m_cv.notify_all();
}

std::deque<std::string> IChannel::popall()
{
    std::deque<std::string> userqueue;
    std::unique_lock<std::mutex> l(m_mtxuserdata);
    std::swap(userqueue, m_dataQueue);
    return userqueue;
}
