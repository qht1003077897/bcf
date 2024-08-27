#pragma once

#include "base/bytebuffer.hpp"
#include "base/endian.hpp"
#include "iprotocolbuilder.h"
#include "iprotocolparser.h"

namespace bcf
{
/**
* @brief 对于任何被bcf使用的自定义协议,要求【协议类型】和【会话唯一序列号】必须存在,seq后面的内容可以自定义
* @details
* \a【协议类型     type】: 供协议解析器来使用，因为支持多个解析器，所以需要根据此字段探测能否被其中的一个解析器所解析.
* \a【会话唯一序列号 seq】： 因为收到的是异步消息，所以bcf需要根据这个seq判断转发给哪个callback.
* \a【业务ID       cmd】： 只是"指定头部长度协议"所需要的业务识别号，代表业务类型，bcf不关心，但是业务上应该关心是数据具体的哪个业务，比如是请求数据？还是删除数据？
* @see
* \a|*****************head*******************|*****body*****|
* \a|    type  |   seq   |  cmd    | length  |     XXX      |
* \a|    1byte |  4byte  | 4byte   | 4bytes  | length bytes |
* \a|****************************************|**************|
**/
class ByHeadProtocolModel : public AbstractProtocolModel
{
public:
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

class ByHeadProtocolBuilder : public IProtocolBuilder
{
public:
    ByHeadProtocolBuilder(PackEndian endian = PackEndian::USE_BIG_ENDIAN): IProtocolBuilder(
            endian) {};

    virtual PackMode getType()const override
    {
        return PackMode::UNPACK_BY_LENGTH_FIELD;
    };
    virtual std::shared_ptr<bb::ByteBuffer> build(const std::shared_ptr<AbstractProtocolModel>&
                                                  _model)
    override
    {
        std::shared_ptr<ByHeadProtocolModel> model =
            std::dynamic_pointer_cast<ByHeadProtocolModel>(_model);
        if (nullptr == model) {
            return nullptr;
        }

        uint8_t type = getType();
        uint32_t bigSeq = model->seq;
        uint32_t bigCmd = model->cmd;
        uint32_t bigLen = model->length;
        if (m_endian == PackEndian::USE_BIG_ENDIAN) {
            bigSeq = htobe32(model->seq);
            bigCmd = htobe32(model->cmd);
            bigLen = htobe32(model->length);
        } else if (m_endian == PackEndian::USE_LITTEL_ENDIAN) {
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

class ByHeadProtocolParser : public IProtocolParser
{
public:
    ByHeadProtocolParser(PackEndian endian = PackEndian::USE_BIG_ENDIAN): IProtocolParser(
            endian) {};

    virtual PackMode getType()const override
    {
        return PackMode::UNPACK_BY_LENGTH_FIELD;
    };

    /**
    * @brief 协议解码器实现纯虚的parse函数，回调的目的是因为假如出现粘包、拆包现象，可以递归callback，每callback一次则代表完整的一帧，剩余的数据被缓存在协议解码器内部
    */
    virtual void parse(const
                       std::function<void(ParserState state, std::shared_ptr<AbstractProtocolModel> model)>& callback)
    override
    {
        int totalLength = m_buffer->size();
        if (totalLength < ByHeadProtocolModel::body_offset) {
            std::cout << "totalLength:" << totalLength << "<" << ByHeadProtocolModel::body_offset <<
                      std::endl;
            return;
        }

        uint8_t type = m_buffer->getChar();
        uint32_t seq = m_buffer->getInt();
        uint16_t cmd = m_buffer->getInt();
        uint32_t bodylength = m_buffer->getInt();

        if (m_endian == PackEndian::USE_BIG_ENDIAN) {
            seq = be32toh(seq);
            cmd = be32toh(cmd);
            bodylength = be32toh(bodylength);
        } else if (m_endian == PackEndian::USE_LITTEL_ENDIAN) {
            seq = le32toh(seq);
            cmd = le32toh(cmd);
            bodylength = le32toh(bodylength);
        }

        const int reqTotalLength = ByHeadProtocolModel::body_offset + bodylength;
        if (totalLength < reqTotalLength) {
            std::cout << "totalLength < " << reqTotalLength;
            return;
        }

        bb::ByteBuffer body = m_buffer->mid(bodylength);

        std::shared_ptr<ByHeadProtocolModel> model = std::make_shared<ByHeadProtocolModel>();
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
