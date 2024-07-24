#pragma once

#include "iprotocolbuilder.h"
#include "iprotocolparser.h"

class AProtocolBuilder : public bcf::IProtocolBuilder
{
protected:
    virtual bcf::ProtocolType getType()const override;
    virtual std::string build(std::shared_ptr<bcf::AbstractProtocolModel> model) override;
};


class AProtocolParser : public bcf::IProtocolParser
{
public:
    virtual bcf::ProtocolType getType()const override;
    //使用者自己实现parse函数，通过回调返回parser好的数据和parser状态
    virtual void parse(const std::string& data,
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel> model)>) override;
};
