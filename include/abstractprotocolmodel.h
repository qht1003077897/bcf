#pragma once

#include <string>

namespace bcf
{

enum ProtocolType {
    None = 0,
    A = 1,
    B = 2,
    //...
};

struct AbstractProtocolModel {
    uint32_t seq = 1;
    uint32_t cmd = 0x01;
    virtual ProtocolType protocolType()
    {
        return ProtocolType::None;
    };
};


//e.g.: A、B协议
struct AProtocolModel : AbstractProtocolModel {

    virtual ProtocolType protocolType() override
    {
        return ProtocolType::A;
    };
    //e.g. json
    std::string data;
};

struct BProtocolModel : AbstractProtocolModel {

    virtual ProtocolType protocolType() override
    {
        return ProtocolType::B;
    };

    uint32_t ack = 0x00;
    uint32_t end = 0x00;
};
}
