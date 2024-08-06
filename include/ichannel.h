#pragma once

#include <functional>
#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <deque>
#include <bcfexport.h>
#include <base/bytebuffer.hpp>
#include <base/globaldefine.h>

namespace bcf
{
class IChannel;
using DataCallback = std::function<void(std::shared_ptr<bb::ByteBuffer>)>;
using ErrorCallback = std::function<void(const std::string&)>;
using GenChannelIDFunc = std::function<int()>;
using CreateChannelFunc = std::function<std::shared_ptr<bcf::IChannel>()>;
using ConnectionCompletedCallback = std::function<void(std::shared_ptr<bcf::IChannel>)>;
using ConnectionFailCallback = std::function<void()>;
using ByteBufferPtr = std::shared_ptr<bb::ByteBuffer>;

enum class ChannelState {
    Idel = 0x01,
    Opened,
    Closed,
    Error,
};

struct ConnectOption {
    int m_timeoutMillSeconds = DEFAULT_TIME_OUT_MILLSCENDS;
    int m_maxRecvBufferSize = DEFAULT_RECV_BUFFER_SIZE;
    int m_channelid;//bcf自动生成
    bcf::ConnectionFailCallback m_FailCallback;
    bcf::ConnectionCompletedCallback m_CompleteCallback;
    bcf::ReceiveCallback m_ReceiveCallback;
};

class BCF_EXPORT IChannel: public std::enable_shared_from_this<IChannel>
{
public:
    virtual ~IChannel();

    inline void setChannelID(int channelID)
    {
        m_channelID = channelID;
    };
    virtual int channelID() const
    {
        return m_channelID;
    };
    virtual bool isOpen() = 0;
    virtual bool open();
    virtual void close();

    virtual int64_t send(const char* data, uint32_t len) = 0;
    //底层具体的通道每收到一次数据，调用此函数交给bcf来迁移到用户线程进行转发
    virtual void pushData2Bcf(ByteBufferPtr&&);
    virtual void setMaxRecvBufferSize(int size)
    {
        (void)size;
    };
    virtual void setDataCallback(DataCallback&&);
    virtual void setErrorCallback(ErrorCallback&&);
    virtual void setFailedCallback(ConnectionFailCallback&& callback);
    virtual void setConnectionCompletedCallback(
        ConnectionCompletedCallback&& callback);
    std::shared_ptr<IChannel> getSharedFromThis()
    {
        return shared_from_this();
    }
    virtual uint32_t read(unsigned char* buff, uint32_t len)
    {
        return -1;
    };
    virtual uint32_t write(unsigned char* buff, uint32_t len)
    {
        return -1;
    };

    virtual ByteBufferPtr readAll()
    {
        return nullptr;
    };

protected:
    //必须重写
    virtual bool openInternal() = 0;
    virtual bool closeInternal() = 0;

private:
    void startUserCallbackThread();
    void stopUserCallbackThread();
    void pushqueueAndNotify(ByteBufferPtr&&);
    std::deque<ByteBufferPtr> popall();

protected:
    DataCallback m_dataCallback;
    ErrorCallback m_errorCallback;
    bcf::ConnectionFailCallback m_FailCallback;
    bcf::ConnectionCompletedCallback m_CompleteCallback;

private:
    std::deque<ByteBufferPtr> m_Queue;
    std::atomic_bool m_isexit;
    std::mutex m_QueueMtx;
    std::condition_variable m_QueueCV;
    std::shared_ptr<std::thread> m_usercallbackthread;
    int m_channelID = -1;

    bcf::ChannelState m_state = ChannelState::Idel;

};
}
