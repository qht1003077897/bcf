#pragma once

#include <functional>
#include <string>

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
    virtual bcf::ChannelID channelID() = 0;

    virtual bool open() = 0;
    virtual bool isOpen() = 0;
    virtual void close() = 0;

    virtual int64_t send(const unsigned char* data, uint32_t len) = 0;
    virtual void receive(std::function<void(int code, const unsigned char* data, uint32_t len)>) = 0;
    virtual void onError(std::function<void (std::string)>) = 0;

    virtual uint32_t read(unsigned char* buff, uint32_t len) = 0;
    virtual uint32_t write(unsigned char* buff, uint32_t len) = 0;

    virtual const char* readAll() = 0;
};
}
