#pragma once

#include <functional>
#include <memory>
#include <base/bytebuffer.hpp>
#include <abstractprotocolmodel.h>
namespace bcf
{
class IProtocolParser
{
public:

    enum ParserState {
        OK = 0,            //代表整包数据解析成功，解析器返回数据后内部清空数据buffer
        Error = 1,         //代表解析出错，解析器内部清空数据buffer
        Abandon = 2        //当前解析器无法解析的数据,当作垃圾一样丢出去，但解析器内部“可能”还会残留数据buffer（取决于具体的数据包），以等待后面的包能否粘成正常包
    };
    IProtocolParser(bcf::PackEndian endian) : m_endian(endian) {}
    virtual ~IProtocolParser() = default;
    virtual bcf::PackMode getType() const = 0;

    //使用者自己实现parse函数，通过回调返回parser好的数据和parser状态
    virtual void parse(const
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel> model)>&) = 0;

    //嗅探此解析器能否解析buffer,此处要求协议每个报文的起始都是1个字节的协议类型, it is bcf::PackMode
    bool sniff(const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr)
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

protected:
    std::shared_ptr<bb::ByteBuffer> m_buffer = std::make_shared<bb::ByteBuffer>();
    bcf::PackEndian m_endian = bcf::USE_BIG_ENDIAN;
};
}
