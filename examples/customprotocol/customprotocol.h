#include "bcf.h"
using namespace std;
using namespace bcf;

#define WHAT_NAME          0x0001
#define ACTION_GET_NAME    0x0010
#define ACTION_SET_NAME    0x0020

static bcf::PackMode customPackMode = static_cast<bcf::PackMode>(bcf::PackMode::UNPACK_BY_USER + 1);

/**
 *  @brief
 *  下面的例子中演示了如何自定义自己的协议,如果你不想使用bcf内置的支持的协议,你可以参考@ref examples/customprotocol/customprotocol.h 定义自己的通信协议。
 *  在下面的例子中,我们参考@class ByHeadProtocolModel,只是将cmd拆分为2个2byte的字段,将body的内容用json格式代替。
 *  其实大多数用户层协议都是这个格式,例子的目的只是告诉使用者如何自定义和自定义的要求。
 *  @note bcf要求报文中【协议类型】和【会话唯一序列号】必须存在,且位于开头,seq后面的内容可以自定义 @see bcf::ByHeadProtocolModel
*/
class CustomProtocolModel : public bcf::AbstractProtocolModel
{
public:

    bcf::PackMode protocolType() override
    {
        return customPackMode;
    };

    /**
    * bcf要求【协议类型】和【会话唯一序列号】必须存在,seq后面的内容可以自定义 @see bcf::ByHeadProtocolModel
    * 协议类型\会话唯一序列号\业务类型\当前业务类型下的对应的具体操作\body长度
    * |*****************head****************************|*****body*****|
    * |    type  |   seq   | what  | action   | length  |     XXX      |
    * |    1byte |  4byte  | 2byte |  2byte   | 4bytes  | length bytes |
    * |*************************************************|**************|
    **/
    uint16_t what = 0;
    uint16_t action = 0;
    uint32_t length = 0;
    constexpr static uint16_t body_offset = sizeof(type) + sizeof(seq) + sizeof(what) + sizeof(
                                                action) + sizeof(length);

    void setBody(const std::string& body)
    {
        m_body = body;
        length = m_body.length();
    }
    const std::string& body()
    {
        return m_body;
    };
private:
    std::string m_body;
};

class CustomProtocolBuilder : public bcf::IProtocolBuilder
{
public:
    CustomProtocolBuilder(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN): IProtocolBuilder(
            endian) {};
public:
    bcf::PackMode getType()const override
    {
        return customPackMode;
    };
    std::shared_ptr<bb::ByteBuffer> build(const std::shared_ptr<bcf::AbstractProtocolModel>& _model)
    override
    {
        //your build...  e.g. ByHeadProtocolBuilder
        auto model = std::dynamic_pointer_cast<CustomProtocolModel>(_model);
        if (nullptr == model) {
            return nullptr;
        }
        uint8_t type = getType();
        uint32_t bigSeq = model->seq;
        uint16_t bigWhat = model->what;
        uint16_t bigAction = model->action;
        uint32_t bigLen = model->length;
        if (m_endian == bcf::PackEndian::USE_BIG_ENDIAN) {
            bigSeq = htobe32(model->seq);
            bigWhat = htobe16(model->what);
            bigAction = htobe16(model->action);
            bigLen = htobe32(model->length);
        } else if (m_endian == bcf::PackEndian::USE_LITTEL_ENDIAN) {
            bigSeq = htole32(model->seq);
            bigWhat = htole16(model->what);
            bigAction = htole16(model->action);
            bigLen = htole32(model->length);
        }

        auto ptr = std::make_shared<bb::ByteBuffer>(model->body().length() + model->body_offset);

        ptr->put(type);
        ptr->putInt(bigSeq);
        ptr->putShort(bigWhat);
        ptr->putShort(bigAction);
        ptr->putInt(bigLen);
        ptr->putBytes((uint8_t*)model->body().c_str(), model->body().length());
#ifndef NDEBUG
        ptr->printHex();
#endif
        return ptr;
    };
};

class CustomProtocolParser : public bcf::IProtocolParser
{
public:
    CustomProtocolParser(bcf::PackEndian endian = bcf::PackEndian::USE_BIG_ENDIAN): IProtocolParser(
            endian) {};

    bcf::PackMode getType()const override
    {
        return customPackMode;
    };

    void parse(const
               std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel>)>& callback)
    override
    {
        auto model = std::make_shared<CustomProtocolModel>();
        //your parse...  e.g. ByHeadProtocolParser
        int totalLength = m_buffer->size();
        if (totalLength < bcf::ByHeadProtocolModel::body_offset) {
            std::cout << "totalLength:" << totalLength << "<" << bcf::ByHeadProtocolModel::body_offset <<
                      std::endl;
            return;
        }

        uint8_t type = m_buffer->getChar();
        uint32_t seq = m_buffer->getInt();
        uint16_t what = m_buffer->getShort();
        uint16_t action = m_buffer->getShort();
        uint32_t bodylength = m_buffer->getInt();

        if (m_endian == bcf::PackEndian::USE_BIG_ENDIAN) {
            seq = be32toh(seq);
            what = be16toh(what);
            action = be16toh(action);
            bodylength = be32toh(bodylength);
        } else if (m_endian == bcf::PackEndian::USE_LITTEL_ENDIAN) {
            seq = le32toh(seq);
            what = le16toh(what);
            action = le16toh(action);
            bodylength = le32toh(bodylength);
        }

        const int reqTotalLength = bcf::ByHeadProtocolModel::body_offset + bodylength;
        if (totalLength < reqTotalLength) {
            std::cout << "totalLength <  " << reqTotalLength;
            return;
        }

        bb::ByteBuffer body = m_buffer->mid(bodylength);

        model->type = type;
        model->seq = seq;
        model->what = what;
        model->action = action;
        model->setBody(std::string(body.begin(), body.end()));
        callback(ParserState::OK, model);

        if (0 == m_buffer->bytesRemaining()) {
            m_buffer->clear();
            return;
        }

        parse(callback);
    };
};
