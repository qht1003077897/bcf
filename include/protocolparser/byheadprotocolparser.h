#pragma once

#include "iprotocolbuilder.h"
#include "iprotocolparser.h"
#include <base/bytebuffer.hpp>
#include <base/endian.hpp>

class ByHeadProtocolBuilder : public bcf::IProtocolBuilder
{
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
        uint32_t bigSeq = model->seq;
        uint32_t bigCmd = model->cmd;
        uint32_t bigLen = model->length;
        if (model->endian == bcf::PackEndian::USE_BIG_ENDIAN) {
            bigSeq = htobe32(model->seq);
            bigCmd = htobe16(model->cmd);
            bigLen = htobe32(model->length);
        }

        auto ptr = std::make_shared<bb::ByteBuffer>(model->body().length() + model->body_offset);

        ptr->putInt(bigSeq);
        ptr->putShort(bigCmd);
        ptr->putInt(bigLen);
        ptr->putBytes((uint8_t*)model->body().c_str(), model->body().length());
        ptr->printHex();
        return ptr;
    };
};

class ByHeadProtocolParser : public bcf::IProtocolParser
{
public:
    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    //使用者自己实现parse函数，回调的目的时因为需要递归callback，解决粘包产生的多包问题
    virtual void parse(const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                       const std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel> model)>& callback)
    override
    {
        byteBufferPtr->printHex();
//        m_buffer->append(data);
//        int totalLength = m_buffer.length();
//        if (totalLength < bcf::ByHeadProtocolModel::body_offset) {
//            qWarning() << "totalLength < " << bcf::ByHeadProtocolModel::body_offset;
//            return;
//        }

//        uint32_t seq = *(uint32_t*) (m_buffer.left(sizeof(uint32_t)).constData());
//        seq = bcf::net::endian::networkToHost32(seq);

//        uint16_t cmd = *(uint16_t*) (m_buffer.mid(sizeof(uint32_t), sizeof(uint16_t)).constData());
//        cmd = bcf::net::endian::networkToHost16(cmd);

//        uint32_t bodylength =  *(uint32_t*) (m_buffer.mid(sizeof(uint32_t) + sizeof(uint16_t),
//                                                          sizeof(uint32_t)).constData());
//        bodylength = bcf::net::endian::networkToHost32(bodylength);

//        const int reqTotalLength = bcf::ByHeadProtocolModel::body_offset + bodylength;
//        if (totalLength < reqTotalLength) {
//            qWarning() << "totalLength <  " << reqTotalLength;
//            return;
//        }

//        const QByteArray body = m_buffer.mid(bcf::ByHeadProtocolModel::body_offset, bodylength);
//        m_buffer.remove(0, reqTotalLength);

//        std::shared_ptr<bcf::ByHeadProtocolModel> model = std::make_shared<bcf::ByHeadProtocolModel>();
//        model->seq = seq;
//        model->cmd = cmd;
//        model->setBody(body.toStdString());
//        callback(ParserState::OK, model);

//        if (m_buffer.isEmpty()) {
//            return;
//        }

//        parse(QByteArray(), callback);
    };

private:
    std::shared_ptr<bb::ByteBuffer> m_buffer = std::make_shared<bb::ByteBuffer>();
};
