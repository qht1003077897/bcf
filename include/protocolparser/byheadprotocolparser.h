#pragma once

#include "iprotocolbuilder.h"
#include "iprotocolparser.h"
#include <base/bytebuffer.hpp>
#include <base/endian.hpp>

class ByHeadProtocolBuilder : public bcf::IProtocolBuilder
{
public:
    ByHeadProtocolBuilder(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN): m_endian(
            endian) {};

protected:
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
        uint16_t bigCmd = model->cmd;
        uint32_t bigLen = model->length;
        if (m_endian == bcf::PackEndian::USE_BIG_ENDIAN) {
            bigSeq = htobe32(model->seq);
            bigCmd = htobe16(model->cmd);
            bigLen = htobe32(model->length);
        } else if (m_endian == bcf::PackEndian::USE_LITTEL_ENDIAN) {
            bigSeq = htole32(model->seq);
            bigCmd = htole16(model->cmd);
            bigLen = htole32(model->length);
        }

        auto ptr = std::make_shared<bb::ByteBuffer>(model->body().length() + model->body_offset);

        ptr->put(type);
        ptr->putInt(bigSeq);
        ptr->putShort(bigCmd);
        ptr->putInt(bigLen);
        ptr->putBytes((uint8_t*)model->body().c_str(), model->body().length());
#ifndef NDEBUG
        ptr->printHex();
#endif
        return ptr;
    };
private:
    bcf::PackEndian m_endian;
};

class ByHeadProtocolParser : public bcf::IProtocolParser
{
public:
    ByHeadProtocolParser(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN): m_endian(endian) {};

    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };

    bool sniff(const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr) override
    {
        m_buffer->put(byteBufferPtr.get());

#ifndef NDEBUG
        m_buffer->printHex();
#endif
        uint8_t type = m_buffer->peek();
        if (type != getType()) {
            return false;
        }

        return true;
    };

    //使用者自己实现parse函数，回调的目的时因为需要递归callback，解决粘包产生的多包问题
    virtual void parse(const
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel> model)>& callback)
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
        uint16_t cmd = m_buffer->getShort();
        uint32_t bodylength = m_buffer->getInt();

        if (m_endian == bcf::PackEndian::USE_BIG_ENDIAN) {
            seq = be32toh(seq);
            cmd = be16toh(cmd);
            bodylength = be32toh(bodylength);
        } else if (m_endian == bcf::PackEndian::USE_LITTEL_ENDIAN) {
            seq = le32toh(seq);
            cmd = le16toh(cmd);
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

private:
    bcf::PackEndian m_endian;
    std::shared_ptr<bb::ByteBuffer> m_buffer = std::make_shared<bb::ByteBuffer>();
};
