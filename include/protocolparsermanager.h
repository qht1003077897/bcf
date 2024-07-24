#pragma once

#include <memory>
#include <map>
#include <bcfexport.h>
#include <abstractprotocolmodel.h>
#include <iprotocolparser.h>

namespace bcf
{
class BCF_EXPORT ProtocolParserManager
{

public:
    void addParser(const std::shared_ptr<IProtocolParser>& parser);
    std::shared_ptr<IProtocolParser> findParse(ProtocolType id);
    void parseByID(bcf::ProtocolType id, const std::string& data,
                   std::function<void(IProtocolParser::ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>);

    ///遍历协议进行解析，哪个解析成功就用哪个返回包
    void parseByAll(const std::string& data,
                    std::function<void(IProtocolParser::ParserState, const std::shared_ptr<AbstractProtocolModel>& model)>);

private:
    std::map<ProtocolType, std::shared_ptr<IProtocolParser>> parsers;
};
}
