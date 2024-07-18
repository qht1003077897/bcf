#pragma once
#include <memory>
#include <abstractprotocolmodel.h>

namespace bcf
{
class IProtocolBuilder
{
public:
    virtual ProtocolType getType()const = 0;
    virtual uint32_t build(std::shared_ptr<AbstractProtocolModel> model, unsigned char* data) = 0;
};
}
