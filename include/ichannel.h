#pragma once

#include <map>
#include <deque>
#include <mutex>
#include <string>
#include <memory>
#include <functional>
#include "bcfexport.h"
#include "base/bytebuffer.hpp"
#include "base/globaldefine.h"

namespace bcf
{
class IChannel;
using DataCallback = std::function<void(std::shared_ptr<bb::ByteBuffer>)>;
using ErrorCallback = std::function<void(const std::string&)>;
using CreateChannelFunc = std::function<std::shared_ptr<IChannel>()>;
using ConnectionCompletedCallback = std::function<void(std::shared_ptr<IChannel>)>;
using ConnectionFailCallback = std::function<void()>;
using ByteBufferPtr = std::shared_ptr<bb::ByteBuffer>;

enum class ChannelState : uint16_t {
    Idel = 0x0001,
    Opened,
    Closed,
    Error,
};

struct ConnectOption {
    ConnectOption& operator=(ConnectOption&& other) noexcept
    {
        if (this == &other) return *this;

        m_timeoutMillSeconds = other.m_timeoutMillSeconds;
        m_maxRecvBufferSize = other.m_maxRecvBufferSize;
        m_channelid = other.m_channelid;
        m_FailCallback = std::move(other.m_FailCallback);
        m_CompleteCallback = std::move(other.m_CompleteCallback);
        m_ReceiveCallback = std::move(other.m_ReceiveCallback);
        return *this;
    }

    int m_channelid;
    int m_timeoutMillSeconds = DEFAULT_TIME_OUT_MILLSCENDS;
    int m_maxRecvBufferSize = DEFAULT_RECV_BUFFER_SIZE;
    ConnectionFailCallback m_FailCallback;
    ConnectionCompletedCallback m_CompleteCallback;
    ReceiveCallback m_ReceiveCallback;
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
    void open();
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
    /**
    * @brief 支持原始裸流数据的接收，不经过requesthandler，用户直接使用ichannel对象注册原始数据回调即可。
    * @note
    * 使用requesthandler进行request请求和使用setRawDataCallback进行原始数据流通信是互斥的。
    * 即:如果设置了RawDataCallback，则经由requesthandler的请求数据也是从此接口返回。所以，
    * 如果不需要使用原始裸流数据了，请给setRawDataCallback接口设置nullptr,@see setRawDataCallback(DataCallback&&)
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
    * @brief 对于串口或者tcp的后端实现而言，一般有两种数据接收方式。
    * 第一种Active即为用户主动触发read和write。passive即为通过库提供的数据回调接口或者信号槽触发。
    * 在使用ymodel发送文件时，需要使用active模式。
    * bcf会在启动发送文件时内部调用，外部用户无需关心这两个函数。
    * @see @class SerialChannel_QT
    */
    virtual void useActiveModel() {}
    virtual void usePassiveModel() {}

    BCF_EXPORT virtual bool isOpen() = 0;
    BCF_EXPORT virtual int64_t send(const char* data, uint32_t len) = 0;

protected:
    virtual void openInternal() = 0;
    virtual bool closeInternal() = 0;

private:
    void startUserCallbackThread();
    void stopUserCallbackThread();
    void pushqueueAndNotify(ByteBufferPtr&&);
    std::deque<ByteBufferPtr> popall();

protected:
    DataCallback                        m_dataCallback;
    DataCallback                        m_rawdataCallback;
    ErrorCallback                       m_errorCallback;
    ConnectionFailCallback              m_FailCallback;
    ConnectionCompletedCallback         m_CompleteCallback;

private:
    int                                 m_channelID = -1;
    ChannelState                        m_state = ChannelState::Idel;
    std::atomic_bool                    m_isexit;
    std::mutex                          m_QueueMtx;
    std::deque<ByteBufferPtr>           m_Queue;
    std::condition_variable             m_QueueCV;
    std::shared_ptr<std::thread>        m_usercallbackthread;
};
}
