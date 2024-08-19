#include <ichannel.h>
#include <base/globaldefine.h>
#include <base/exception.hpp>
using namespace bcf;

IChannel::~IChannel()
{
    m_isexit = true;
    m_QueueCV.notify_all();
    if (m_usercallbackthread != nullptr && m_usercallbackthread->joinable()) {
        m_usercallbackthread->join();
    }
}

bool IChannel::open()
{
    bool res = openInternal();
    if (res) {
        startUserCallbackThread();
        m_state = ChannelState::Opened;
    }
    return res;
}

void IChannel::close()
{
    closeInternal();
    m_state = ChannelState::Closed;
    stopUserCallbackThread();
}

void IChannel::setDataCallback(DataCallback&& callback)
{
    std::unique_lock<std::mutex> l(m_QueueMtx);
    m_dataCallback = std::move(callback);
}

void IChannel::setRawDataCallback(DataCallback&& callback)
{
    m_rawdataCallback = std::move(callback);
}

void IChannel::setErrorCallback(ErrorCallback&& errorCallback)
{
    m_errorCallback = std::move(errorCallback);
}

void IChannel::setFailedCallback(ConnectionFailCallback &&callback)
{
    m_FailCallback = std::move(callback);
}

void IChannel::setConnectionCompletedCallback(ConnectionCompletedCallback &&callback)
{
    m_CompleteCallback = std::move(callback);
}

void IChannel::pushData2Bcf(ByteBufferPtr&& data)
{
    pushqueueAndNotify(std::move(data));
}

void IChannel::startUserCallbackThread()
{
    if (m_usercallbackthread != nullptr) {
        throw BcfCommonException("connect thread already started");
    }

    m_usercallbackthread = std::make_shared<std::thread>([this]() {
        while (!m_isexit &&  (m_state != ChannelState::Closed)) {
            std::mutex mtx;
            std::unique_lock<std::mutex> l(mtx);
            m_QueueCV.wait_for(l, std::chrono::seconds(5), [this] {
                std::unique_lock<std::mutex> l(m_QueueMtx);
                return !m_Queue.empty() || m_isexit || m_state == ChannelState::Closed;
            });

            if (m_isexit || m_state == ChannelState::Closed) {
                return;
            }

            {
                std::unique_lock<std::mutex> l(m_QueueMtx, std::try_to_lock);
                while (!m_Queue.empty()) {
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
    if (m_usercallbackthread == nullptr) {
        return;
    }

    m_QueueCV.notify_all();
    if (m_usercallbackthread->joinable()) {
        m_usercallbackthread->join();
    }
    m_usercallbackthread = nullptr;
}

void IChannel::pushqueueAndNotify(ByteBufferPtr&& data)
{
    std::lock_guard<std::mutex> l(m_QueueMtx);
    m_Queue.emplace_back(std::move(data));
    m_QueueCV.notify_all();
}

std::deque<ByteBufferPtr> IChannel::popall()
{
    std::deque<ByteBufferPtr> userqueue;
    std::unique_lock<std::mutex> l(m_QueueMtx, std::try_to_lock);
    std::swap(userqueue, m_Queue);
    return userqueue;
}
