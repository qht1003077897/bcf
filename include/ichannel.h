#pragma once
#pragma warning(push)
#pragma warning(disable: 4251)

#include <functional>
#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <deque>
#include <bcfexport.h>
#include <globaldefine.h>

namespace bcf
{
class IChannel;
using DataCallback = std::function<void(const std::string&)>;
using ErrorCallback = DataCallback;
using CreateChannelFunc = std::function<std::shared_ptr<bcf::IChannel>()>;
using ConnectionCompletedCallback = std::function<void(std::shared_ptr<bcf::IChannel>)>;
using ConnectionFailCallback = std::function<void()>;

enum ChannelID {
    Begin = 0x01,
    Serial = Begin,
    TCP = 0x02,
    End = TCP
};

enum class ChannelState {
    Idel = 0x01,
    Opened,
    Closed,
    Error,
};

struct TCPConnectOption {
    std::string ip;
    int port = 0;
};

struct SerialPortConnectOption {
    std::string portName;
    BaudRate baudRate = BaudRate::Baud115200;
    DataBits dataBits = DataBits::Data8;
    Parity parity = Parity::NoParity;
    StopBits stopBits = StopBits::OneStop;
    FlowControl flowControl = FlowControl::NoFlowControl;
};

struct ConnectOption {
    TCPConnectOption m_TCPConnectOption;
    SerialPortConnectOption m_SerialPortConnectOption;
    int m_timeoutMillSeconds = 10'000;
    bcf::ChannelID m_channelid;
    bcf::ConnectionFailCallback m_FailCallback;
    bcf::ConnectionCompletedCallback m_CompleteCallback;
    bcf::ReceiveCallback m_ReceiveCallback;
};

class BCF_EXPORT IChannel
{
public:
    virtual ~IChannel();

    virtual bcf::ChannelID channelID() = 0;
    virtual bool isOpen() = 0;
    virtual bool open();
    virtual void close();

    virtual int64_t send(const unsigned char* data, uint32_t len) = 0;
    //底层具体的通道每收到一次数据，调用此函数交给bcf来迁移到用户线程进行转发
    virtual void pushData2Bcf(const std::string&);
    virtual void setDataCallback(DataCallback&&);
    virtual void setErrorCallback(ErrorCallback&&);

    virtual uint32_t read(unsigned char* buff, uint32_t len)
    {
        return -1;
    };
    virtual uint32_t write(unsigned char* buff, uint32_t len)
    {
        return -1;
    };

    virtual uint32_t readAll(char* buff)
    {
        return -1;
    };

protected:
    //必须重写
    virtual bool openChannel() = 0;
    virtual bool closeChannel() = 0;

private:
    void startUserCallbackThread();
    void stopUserCallbackThread();
    void pushqueueAndNotify(const std::string& data);
    std::deque<std::string> popall();

protected:
    DataCallback m_dataCallback;
    ErrorCallback m_errorCallback;

private:
    std::deque<std::string> m_dataQueue;
    std::atomic_bool m_isexit;
    std::mutex m_mtx;
    std::mutex m_mtxuserdata;
    std::condition_variable m_cv;
    std::thread m_usercallbackthread;

    bcf::ChannelState m_state = ChannelState::Idel;

};
}
