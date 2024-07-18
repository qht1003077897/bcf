#pragma once

namespace bcf
{
enum ErrorCode {
    OK = 0x0001,
    CHANNEL_CLOSE = 0x0100,            //通信链路相关错误
    PROTOCOL_NOT_EXIST = 0x0200,       //协议相关错误
    // = 0x0300,                       //其他错误
    TIME_OUT = 0x0300                  //(通信)超时
};

enum FilterType {
    Abandon = 0x0001,                   //Abandon过滤器，可以过滤对端返回的无法被解析的数据，交给过滤器去处理，且允许放行
};
}

#define CHECK_RANGE(value, min, max) \
    static_assert((value) >= (min) && (value) <= (max), "value out of range")
/**
 * 绑定cmd和通道ID的关系。真实的业务场景下，cmd代表业务的唯一ID，则肯定属于某一个通道。如果两个通道有一样的cmd，证明协议设计是不合理的
**/
#define ADD_CMD_TO_BCF(cmd,channelID) \
    { \
        CHECK_RANGE(channelID, bcf::ChannelID::Begin, bcf::ChannelID::End);\
        ChannelManager::getInstance().bindCmdAndChannel((uint32_t)cmd,bcf::ChannelID(channelID));\
    } \
