#pragma once
#include <memory>
#include "base/platform.hpp"
#include "base/bytebuffer.hpp"
#include "abstractprotocolmodel.h"

namespace bcf
{
class IProtocolBuilder
{
public:
    IProtocolBuilder(PackEndian endian) : m_endian(endian) {}
    virtual ~IProtocolBuilder() = default;
    virtual PackMode getType()const = 0;
    virtual std::shared_ptr<bb::ByteBuffer> build(const std::shared_ptr<AbstractProtocolModel>& model) =
        0;

protected:
    PackEndian m_endian = USE_BIG_ENDIAN;
};
}
