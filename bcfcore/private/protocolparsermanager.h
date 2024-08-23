#pragma once

#include <memory>
#include <map>
#include <abstractprotocolmodel.h>
#include <iprotocolparser.h>
#include <base/noncopyable.hpp>

namespace bcf
{
class ProtocolParserManager: public bcf::NonCopyable
{
public:
    void addParser(const std::shared_ptr<IProtocolParser>& parser);
    std::shared_ptr<IProtocolParser> findParse(PackMode id);
    void parseByID(bcf::PackMode id, const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                   std::function<void(bcf::ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>);

    ///遍历协议进行解析，哪个解析成功就用哪个返回包
    void parseByAll(const std::shared_ptr<bb::ByteBuffer>& byteBufferPtr,
                    std::function<void(bcf::ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>);

private:
    std::map<PackMode, std::shared_ptr<IProtocolParser>> parsers;
};
}
