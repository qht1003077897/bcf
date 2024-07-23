#pragma once

#include <functional>
#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <deque>
#include <globaldefine.h>

namespace bcf
{

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

class __declspec(dllexport) IChannel
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
    virtual void setDataCallback(bcf::DataCallback&&);

    virtual uint32_t read(unsigned char* buff, uint32_t len)
    {
        return -1;
    };
    virtual uint32_t write(unsigned char* buff, uint32_t len)
    {
        return -1;
    };

    virtual const char* readAll()
    {
        return nullptr;
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

private:
    std::deque<std::string> m_dataQueue;
    std::atomic_bool m_isexit;
    std::mutex m_mtx;
    std::mutex m_mtxuserdata;
    std::condition_variable m_cv;
    std::thread m_usercallbackthread;

    bcf::ChannelState m_state = ChannelState::Idel;
    bcf::DataCallback m_dataCallback;
};
}
