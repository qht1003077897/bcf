#pragma once

#include "iprotocolbuilder.h"
#include "iprotocolparser.h"

class AProtocolBuilder : public bcf::IProtocolBuilder
{
protected:
    virtual bcf::ProtocolType getType()const override
    {
        return bcf::ProtocolType::A;
    };
    virtual std::string build(std::shared_ptr<bcf::AbstractProtocolModel> _model) override
    {
        std::shared_ptr<bcf::AProtocolModel> model = std::dynamic_pointer_cast<bcf::AProtocolModel>(_model);
        if (nullptr != model) {
            return "1";
        }
        return "";
    };
};


class AProtocolParser : public bcf::IProtocolParser
{
public:
    virtual bcf::ProtocolType getType()const override
    {
        return bcf::ProtocolType::A;
    };
    //使用者自己实现parse函数，通过回调返回parser好的数据和parser状态
    virtual void parse(const std::string& data,
                       std::function<void(ParserState, std::shared_ptr<bcf::AbstractProtocolModel>)> callback)
    override
    {
        int seq = std::stoi(data);
        std::shared_ptr<bcf::AProtocolModel> model = std::make_shared<bcf::AProtocolModel>();
        model->seq = seq;
        callback(ParserState::OK, std::move(model));
    };
};
