#pragma once
#include <memory>
#include <base/bytebuffer.hpp>
#include <abstractprotocolmodel.h>

namespace bcf
{
class IProtocolBuilder
{
public:
    virtual ~IProtocolBuilder() = default;
    virtual PackMode getType()const = 0;
    virtual std::shared_ptr<bb::ByteBuffer> build(std::shared_ptr<AbstractProtocolModel> model) = 0;
};
}
