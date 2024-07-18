#pragma once

#include "iprotocolbuilder.h"

class AProtocolBuilder : public bcf::IProtocolBuilder
{
protected:
    virtual bcf::ProtocolType getType()const override;
    virtual uint32_t build(std::shared_ptr<bcf::AbstractProtocolModel> model,
                           unsigned char* data) override;
};

