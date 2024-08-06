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
};

#define DEFAULT_PACKAGE_MAX_LENGTH  (1 << 21)   // 2M

// UNPACK_BY_DELIMITER
#define PACKAGE_MAX_DELIMITER_BYTES 8

// UNPACK_BY_LENGTH_FIELD
enum PackEndian {
    USE_HOST_ENDIAN = BYTE_ORDER,
    USE_LITTEL_ENDIAN = LITTLE_ENDIAN,
    USE_BIG_ENDIAN    = BIG_ENDIAN,
    USE_NET_ENDIAN = USE_BIG_ENDIAN,
};

class AbstractProtocolModel
{
public:
    uint32_t seq = 0;
    uint16_t cmd = 0x0001;
    PackEndian endian = PackEndian::USE_NET_ENDIAN;
    virtual PackMode protocolType() = 0;
};

class ByHeadProtocolModel : public AbstractProtocolModel
{
public:
    /*
    |************head************|*****body*****|
    |    seq   |  cmd  | length  |     XXX      |
    |    4byte | 2byte | 4bytes  | length bytes |
    |****************************|**************|
    */
    uint32_t length = 0;
    constexpr static uint16_t body_offset = sizeof(seq) + sizeof(cmd) + sizeof(length);

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
