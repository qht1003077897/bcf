#pragma once

#include "iprotocolbuilder.h"
#include "iprotocolparser.h"
#include <QDebug>
#include <QByteArray>
#include <endian.hpp>

class ByHeadProtocolBuilder : public bcf::IProtocolBuilder
{
protected:
    virtual bcf::PackMode getType()const override
    {
        return bcf::PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    virtual QByteArray build(std::shared_ptr<bcf::AbstractProtocolModel> _model) override
    {
        std::shared_ptr<bcf::ByHeadProtocolModel> model =
            std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(_model);
        if (nullptr == model) {
            return "";
        }
        uint32_t bigSeq = bcf::net::endian::hostToNetwork32(model->seq);
        uint16_t bigCmd = bcf::net::endian::hostToNetwork16(model->cmd);
        uint32_t bigLen = bcf::net::endian::hostToNetwork32(model->length);

        QByteArray msg;
        msg.append(reinterpret_cast<const char*>(&bigSeq), sizeof(bigSeq));
        msg.append(reinterpret_cast<const char*>(&bigCmd), sizeof(bigCmd));
        msg.append(reinterpret_cast<const char*>(&bigLen), sizeof(bigLen));
        QByteArray bodyArray(model->body().c_str(), model->body().length());
        msg.append(bodyArray);
        qDebug() << "build data:" << msg;
        return msg;
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
    virtual void parse(const QByteArray& data,
                       const std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel>)>& callback)
    override
    {
        qDebug() << "parse data:" << data;
        m_buffer.append(data);
        int totalLength = m_buffer.length();
        if (totalLength < bcf::ByHeadProtocolModel::body_offset) {
            qWarning() << "totalLength < " << bcf::ByHeadProtocolModel::body_offset;
            return;
        }

        uint32_t seq = *(uint32_t*) (m_buffer.left(sizeof(uint32_t)).constData());
        seq = bcf::net::endian::networkToHost32(seq);

        uint16_t cmd = *(uint16_t*) (m_buffer.mid(sizeof(uint32_t), sizeof(uint16_t)).constData());
        cmd = bcf::net::endian::networkToHost16(cmd);

        uint32_t bodylength =  *(uint32_t*) (m_buffer.mid(sizeof(uint32_t) + sizeof(uint16_t),
                                                          sizeof(uint32_t)).constData());
        bodylength = bcf::net::endian::networkToHost32(bodylength);

        const int reqTotalLength = bcf::ByHeadProtocolModel::body_offset + bodylength;
        if (totalLength < reqTotalLength) {
            qWarning() << "totalLength <  " << reqTotalLength;
            return;
        }

        const QByteArray body = m_buffer.mid(bcf::ByHeadProtocolModel::body_offset, bodylength);
        m_buffer.remove(0, reqTotalLength);

        std::shared_ptr<bcf::ByHeadProtocolModel> model = std::make_shared<bcf::ByHeadProtocolModel>();
        model->seq = seq;
        model->cmd = cmd;
        model->setBody(body.toStdString());
        callback(ParserState::OK, model);

        if (m_buffer.isEmpty()) {
            return;
        }

        parse(QByteArray(), callback);
    };

private:
    QByteArray m_buffer;
};
