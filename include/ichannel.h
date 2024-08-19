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
    int m_channelid;
    bcf::ConnectionFailCallback m_FailCallback;
    bcf::ConnectionCompletedCallback m_CompleteCallback;
    bcf::ReceiveCallback m_ReceiveCallback;
};

class IChannel: public std::enable_shared_from_this<IChannel>
{
public:
    virtual ~IChannel();

    inline void setChannelID(int channelID)
    {
        m_channelID = channelID;
    };
    inline int channelID() const
    {
        return m_channelID;
    };
    bool open();
    void close();

    /**
    * @brief 底层具体的通道每收到一次数据，调用此函数交给bcf来迁移到用户线程进行转发
    */
    void pushData2Bcf(ByteBufferPtr&&);
    virtual void setMaxRecvBufferSize(int size)
    {
        (void)size;
    };
    void setDataCallback(DataCallback&&);
    /*!
    * @brief 支持原始裸流数据的接收，不经过requesthandler，用户直接使用ichannel对象注册原始数据回调即可。
    * >NOTES: \n
    * 使用requesthandler进行request请求和使用setRawDataCallback进行原始数据流通信是互斥的。
    * 即:如果设置了RawDataCallback，则经由requesthandler的请求数据也是从此接口返回。所以，
    * 如果不需要使用原始裸流数据了，请给setRawDataCallback接口设置nullptr
    * @example
    * channel->setRawDataCallback(nullptr);
    */
    BCF_EXPORT void setRawDataCallback(DataCallback&&);
    void setErrorCallback(ErrorCallback&&);
    void setFailedCallback(ConnectionFailCallback&& callback);
    void setConnectionCompletedCallback(ConnectionCompletedCallback&& callback);

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
    /**
    * @brief 对于串口或者tcp的后端实现而言，一般有两种数据接收方式。\n
    * 第一种Active即为用户主动触发read和write。passive即为通过库提供的数据回调接口或者信号槽触发。
    * 在使用ymodel发送文件时，需要使用active模式。
    * 一般不用关心这两个函数。
    */
    virtual void useActiveModel() {}
    virtual void usePassiveModel() {}

    BCF_EXPORT virtual bool isOpen() = 0;
    BCF_EXPORT virtual int64_t send(const char* data, uint32_t len) = 0;

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
    DataCallback m_rawdataCallback;
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
