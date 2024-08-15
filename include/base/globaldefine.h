#pragma once

#include <functional>
#include <memory>

//接收缓冲区默认大小
#define DEFAULT_RECV_BUFFER_SIZE 16*1024

//默认的异步chllback回调超时时间，此超时时间不是tcp的alive时间，而是在此时间内，对应的seq没有回复，则认为此条请求超时，则对应的callback会被丢弃
#define DEFAULT_TIME_OUT_MILLSCENDS 10'000


namespace bcf
{
namespace
{
inline uint32_t& getCurrentSeq()
{
    static uint32_t seq = 0;
    return seq;
}
}
namespace util
{
inline static uint32_t getNextSeq()
{
    uint32_t& seq = getCurrentSeq();
    seq++;
    if (seq >= UINT_MAX) {
        seq = 0;
    }
    return seq;
}
}//namespace bcf::util

enum ErrorCode {
    OK = 0x0000,
    CHANNEL_CLOSE = 0x0001,            //通道错误
    PROTOCOL_NOT_EXIST = 0x0002,       //协议错误
    TIME_OUT = 0x0003,                 //(通信)超时
    UNKOWNED_DATA = 0x0004,             //没有找到seq的数据，可能时底层主动上报的数据
};

class AbstractProtocolModel;
using RequestCallback =
    std::function<void(bcf::ErrorCode, std::shared_ptr<bcf::AbstractProtocolModel>)>;
using ReceiveCallback = std::function<void(std::shared_ptr<bcf::AbstractProtocolModel>)>;
using AbandonCallback =
    std::function<void(std::shared_ptr<bcf::AbstractProtocolModel>)>;
}//namespace bcf
