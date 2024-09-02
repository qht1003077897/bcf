#pragma once
#include <stdint.h>
#include <memory>
#include <stdexcept>

namespace bcf
{
enum PackMode : uint8_t {
    UNPACK_MODE_NONE        = 0,
    UNPACK_BY_FIXED_LENGTH  = 1,    // Not recommended,定长协议报文,BCF 未实现 (Not implemented)
    UNPACK_BY_DELIMITER     = 2,    // Not recommended,分隔符协议报文,BCF 未实现 (Not implemented)
    UNPACK_BY_LENGTH_FIELD  = 3,    // recommended,Suitable for binary protocol head+body, such as : ByHeadProtocolModel
    UNPACK_BY_USER  = 4,            // how to expand: UNPACK_BY_USER,UNPACK_BY_USER+1 UNPACK_BY_USER+2,such as : examples/customprotocol.h
    UNPACK_BY_USERMAX = 100
};

class AbstractProtocolModel
{
public:
    uint8_t type = PackMode::UNPACK_MODE_NONE;
    uint32_t seq = 0;

    virtual PackMode protocolType() = 0;
};


class ByDelimiterProtocolModel : public AbstractProtocolModel
{

    virtual PackMode protocolType() override
    {
        return PackMode::UNPACK_BY_DELIMITER;
    };
};
}
