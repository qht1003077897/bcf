#pragma once
#include <memory>
#include <QByteArray>
#include <abstractprotocolmodel.h>

namespace bcf
{
class IProtocolBuilder
{
public:
    virtual ~IProtocolBuilder() = default;
    virtual PackMode getType()const = 0;
    virtual QByteArray build(std::shared_ptr<AbstractProtocolModel> model) = 0;
};
}
