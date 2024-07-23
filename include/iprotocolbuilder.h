#pragma once
#include <memory>
#include <abstractprotocolmodel.h>

namespace bcf
{
class IProtocolBuilder
{
public:
    virtual ProtocolType getType()const = 0;
    virtual std::string build(std::shared_ptr<AbstractProtocolModel> model) = 0;
};
}
