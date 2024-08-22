#pragma once
#include <base/platform.hpp>
#include <string>

namespace bcf
{
//-----------------unpack---------------------------------------------
enum PackMode {
    UNPACK_MODE_NONE        = 0,
    UNPACK_BY_FIXED_LENGTH  = 1,    // Not recommended 定长解包
    UNPACK_BY_DELIMITER     = 2,    // Suitable for text protocol   分隔符解包
    UNPACK_BY_LENGTH_FIELD  = 3,    // Suitable for binary protocol head+body
    UNPACK_BY_USER  = 4,            // 如何扩展: UNPACK_BY_USER,UNPACK_BY_USER+1 UNPACK_BY_USER+2
    UNPACK_BY_USERMAX = 100
};

class AbstractProtocolModel
{
public:
    uint8_t type = PackMode::UNPACK_MODE_NONE;
    uint32_t seq = 0;

    virtual PackMode protocolType() = 0;
};

class ByHeadProtocolModel : public AbstractProtocolModel
{
public:
    /**
    bcf要求【协议类型】和【会话唯一序列号】必须存在,seq后面的内容可以自定义
    协议类型\会话唯一序列号\业务ID\body长度
    |*****************head*******************|*****body*****|
    |    type  |   seq   |  cmd    | length  |     XXX      |
    |    1byte |  4byte  | 4byte   | 4bytes  | length bytes |
    |****************************************|**************|
    **/
    uint32_t cmd = 0;
    uint32_t length = 0;
    constexpr static uint16_t body_offset = sizeof(type) + sizeof(seq) + sizeof(cmd) + sizeof(length);

    void setBody(const std::string& body)
    {
        m_body = body;
        length = m_body.length();
    }
    const std::string& body()
    {
        return m_body;
    };
    virtual PackMode protocolType() override
    {
        return PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    //e.g. json or std::string
private:
    std::string m_body;
};

class ByDelimiterProtocolModel : public AbstractProtocolModel
{

    virtual PackMode protocolType() override
    {
        return PackMode::UNPACK_BY_DELIMITER;
    };
};

class ByFixedProtocolModel : AbstractProtocolModel
{

    virtual PackMode protocolType() override
    {
        return PackMode::UNPACK_BY_FIXED_LENGTH;
    };
};
}
