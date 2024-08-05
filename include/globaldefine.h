#pragma once

#include <functional>
#include <memory>
#include <abstractprotocolmodel.h>

namespace bcf
{
enum ErrorCode {
    OK = 0x0000,
    CHANNEL_CLOSE = 0x0001,            //通道错误
    PROTOCOL_NOT_EXIST = 0x0002,       //协议错误
    TIME_OUT = 0x0003,                 //(通信)超时
    UNOWNED_DATA = 0x0004,             //没有找到seq的数据，可能时底层主动上报的数据
};

using RequestCallback =
    std::function<void(bcf::ErrorCode, std::shared_ptr<bcf::AbstractProtocolModel>)>;
using ReceiveCallback = RequestCallback;
using AbandonCallback =
    std::function<void(std::shared_ptr<bcf::AbstractProtocolModel>)>;
}//namespace bcf
