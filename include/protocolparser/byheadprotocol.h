#pragma once

#include "iprotocolbuilder.h"
#include "iprotocolparser.h"
#include <base/bytebuffer.hpp>
#include <base/endian.hpp>
namespace bcf
{
class ByHeadProtocolModel : public bcf::AbstractProtocolModel
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
    virtual bcf::PackMode protocolType() override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    //e.g. json or std::string
private:
    std::string m_body;
};

class ByHeadProtocolBuilder : public bcf::IProtocolBuilder
{
public:
    ByHeadProtocolBuilder(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN): IProtocolBuilder(
            endian) {};

    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    virtual std::shared_ptr<bb::ByteBuffer> build(std::shared_ptr<bcf::AbstractProtocolModel> _model)
    override
    {
        std::shared_ptr<bcf::ByHeadProtocolModel> model =
            std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(_model);
        if (nullptr == model) {
            return nullptr;
        }
        uint8_t type = getType();
        uint32_t bigSeq = model->seq;
        uint32_t bigCmd = model->cmd;
        uint32_t bigLen = model->length;
        if (m_endian == bcf::PackEndian::USE_BIG_ENDIAN) {
            bigSeq = htobe32(model->seq);
            bigCmd = htobe32(model->cmd);
            bigLen = htobe32(model->length);
        } else if (m_endian == bcf::PackEndian::USE_LITTEL_ENDIAN) {
            bigSeq = htole32(model->seq);
            bigCmd = htole32(model->cmd);
            bigLen = htole32(model->length);
        }

        auto ptr = std::make_shared<bb::ByteBuffer>(model->body().length() + model->body_offset);

        ptr->put(type);
        ptr->putInt(bigSeq);
        ptr->putInt(bigCmd);
        ptr->putInt(bigLen);
        ptr->putBytes((uint8_t*)model->body().c_str(), model->body().length());
        return ptr;
    };
};

class ByHeadProtocolParser : public bcf::IProtocolParser
{
public:
    ByHeadProtocolParser(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN): IProtocolParser(
            endian) {};

    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };

    //使用者自己实现parse函数，回调的目的时因为需要递归callback，解决粘包产生的多包问题
    virtual void parse(const
                       std::function<void(ParserState state, std::shared_ptr<bcf::AbstractProtocolModel> model)>& callback)
    override
    {
        int totalLength = m_buffer->size();
        if (totalLength < bcf::ByHeadProtocolModel::body_offset) {
            std::cout << "totalLength:" << totalLength << "<" << bcf::ByHeadProtocolModel::body_offset <<
                      std::endl;
            return;
        }

        uint8_t type = m_buffer->getChar();
        uint32_t seq = m_buffer->getInt();
        uint16_t cmd = m_buffer->getInt();
        uint32_t bodylength = m_buffer->getInt();

        if (m_endian == bcf::PackEndian::USE_BIG_ENDIAN) {
            seq = be32toh(seq);
            cmd = be32toh(cmd);
            bodylength = be32toh(bodylength);
        } else if (m_endian == bcf::PackEndian::USE_LITTEL_ENDIAN) {
            seq = le32toh(seq);
            cmd = le32toh(cmd);
            bodylength = le32toh(bodylength);
        }

        const int reqTotalLength = bcf::ByHeadProtocolModel::body_offset + bodylength;
        if (totalLength < reqTotalLength) {
            std::cout << "totalLength <  " << reqTotalLength;
            return;
        }

        bb::ByteBuffer body = m_buffer->mid(bodylength);

        std::shared_ptr<bcf::ByHeadProtocolModel> model = std::make_shared<bcf::ByHeadProtocolModel>();
        model->type = type;
        model->seq = seq;
        model->cmd = cmd;
        model->setBody(std::string(body.begin(), body.end()));
        callback(ParserState::OK, model);

        if (0 == m_buffer->bytesRemaining()) {
            m_buffer->clear();
            return;
        }

        parse(callback);
    };
};
}
