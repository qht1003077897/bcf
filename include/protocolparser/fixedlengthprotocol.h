#pragma once
#include <string>
#include "iprotocolbuilder.h"
#include "iprotocolparser.h"
#include <base/globaldefine.h>
#include <base/bytebuffer.hpp>
#include <base/endian.hpp>

namespace bcf
{
class FixedLengthProtocolModel : public bcf::AbstractProtocolModel
{
public:
    PackMode protocolType() override
    {
        return PackMode::UNPACK_BY_FIXED_LENGTH;
    };

    /**
    bcf要求【协议类型】和【会话唯一序列号】必须存在,seq后面的内容可以自定义
    协议类型\会话唯一序列号\body
    对于定长协议，我们可以支持用户配置长度，bcf默认单包 1024 bytes,用户配置范围为：1-2048(不包含type和seq)，即总长度：6-2053
    如果配置太大，浪费带宽，因为不足的长度需要填充0。如果太小，可能不够用。如果2048都不够用，只能说明当前通信不适用此协议。
    如果使用定长协议，请务必遵守约定，发送方在规定长度内发送。如果发生tcp粘包，由解析器负责处理。
    如果不遵守定长约定，则失去了使用此协议的意义。何不使用变长协议？e.g. ByHeadProtocolModel
    |*********head*******|***************body***************|
    |    type  |   seq   |               XXX                |
    |    1byte |  4byte  |            1019 bytes            |
    |********************|**********************************|
    **/
    constexpr static uint16_t body_offset = sizeof(type) + sizeof(seq);

    void setBody(const std::string& body)
    {
        if (body.length() > m_maxbodylength) {
            throw std::out_of_range("body.length() > frameLength:" + std::to_string(m_maxbodylength));
        } else if (body.length() < m_maxbodylength) {
            int remainLength = m_maxbodylength - body.length();

        }
        m_body = body;
        m_maxbodylength = m_body.length();
    }
    const std::string& body()
    {
        return m_body;
    };
    //e.g. json or std::string
private:
    //只能通过父类的create函数创建
    FixedLengthProtocolModel(int frameLength)
    {
        if (frameLength <= 0 || frameLength > 2048) {
            throw std::invalid_argument("frameLength not allowed,frameLength should with in 1-2048");
        }
        m_maxbodylength = frameLength;
    }
    int m_maxbodylength = DEFAULT_FIXED_LENGTH_FRAME_SIZE - body_offset;
    std::string m_body;
};
class FixedLengthProtocolBuilder : public bcf::IProtocolBuilder
{
public:
    FixedLengthProtocolBuilder(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN):
        IProtocolBuilder(
            endian) {};

protected:
    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_FIXED_LENGTH;
    };
    virtual std::shared_ptr<bb::ByteBuffer> build(std::shared_ptr<bcf::AbstractProtocolModel> _model)
    override
    {
        std::shared_ptr<bcf::FixedLengthProtocolModel> model =
            std::dynamic_pointer_cast<bcf::FixedLengthProtocolModel>(_model);
        if (nullptr == model) {
            return nullptr;
        }

        return nullptr;
    };
};

class ByHeadProtocolParser : public bcf::IProtocolParser
{
public:
    ByHeadProtocolParser(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN): IProtocolParser(
            endian) {};

    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_FIXED_LENGTH;
    };

    //使用者自己实现parse函数，回调的目的时因为需要递归callback，解决粘包产生的多包问题
    virtual void parse(const
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel> model)>& callback)
    override
    {

    };
};
}
