#pragma once

#include <map>
#include <memory>
#include "base/noncopyable.hpp"
#include "abstractprotocolmodel.h"
#include "iprotocolparser.h"

namespace bcf
{
class ProtocolParserManager: public NonCopyable
{
public:
    void addParser(const std::shared_ptr<IProtocolParser>& parser);
    std::shared_ptr<IProtocolParser> findParse(PackMode id);
    void parseByID(PackMode id, const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                   std::function<void(ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>);
    /**
    * @brief 遍历使用协议解码器进行解析，哪个解码器解析成功就用哪个返回包(取决于解码器的@class IProtocolParser::sniff 函数)
    */
    void parseByAll(const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                    std::function<void(ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>);

private:
    std::map<PackMode, std::shared_ptr<IProtocolParser>> m_parsers;
};
}
