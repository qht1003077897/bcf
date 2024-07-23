#pragma once

#include "iprotocolbuilder.h"

class AProtocolBuilder : public bcf::IProtocolBuilder
{
protected:
    virtual bcf::ProtocolType getType()const override;
    virtual std::string build(std::shared_ptr<bcf::AbstractProtocolModel> model) override;
};

