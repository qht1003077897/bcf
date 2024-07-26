#pragma once
#include <functional>
#include <memory>
#include <abstractprotocolmodel.h>

namespace bcf
{
enum ErrorCode {
    OK = 0x0001,
    CHANNEL_CLOSE = 0x0100,            //通信链路相关错误
    PROTOCOL_NOT_EXIST = 0x0200,       //协议相关错误
    // = 0x0300,                       //其他错误
    TIME_OUT = 0x0300,                 //(通信)超时
    UNOWNED_DATA = 0x0400,             //没有找到seq的数据，可能时底层主动上报的数据
};

enum BaudRate {
    Baud1200 = 1200,
    Baud2400 = 2400,
    Baud4800 = 4800,
    Baud9600 = 9600,
    Baud19200 = 19200,
    Baud38400 = 38400,
    Baud57600 = 57600,
    Baud115200 = 115200,
    UnknownBaud = -1
};

enum DataBits {
    Data5 = 5,
    Data6 = 6,
    Data7 = 7,
    Data8 = 8,
    UnknownDataBits = -1
};

enum Parity {
    NoParity = 0,
    EvenParity = 2,
    OddParity = 3,
    SpaceParity = 4,
    MarkParity = 5,
    UnknownParity = -1
};

enum StopBits {
    OneStop = 1,
    OneAndHalfStop = 3,
    TwoStop = 2,
    UnknownStopBits = -1
};

enum FlowControl {
    NoFlowControl,
    HardwareControl,
    SoftwareControl,
    UnknownFlowControl = -1
};

using RequestCallback =
    std::function<void(bcf::ErrorCode, std::shared_ptr<bcf::AbstractProtocolModel>)>;
using ReceiveCallback = RequestCallback;
using AbandonCallback =
    std::function<void(std::shared_ptr<bcf::AbstractProtocolModel>)>;
}
