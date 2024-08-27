#pragma once

#include <map>
#include <memory>
#include "base/noncopyable.hpp"
#include "abstractprotocolmodel.h"
#include "iprotocolbuilder.h"

namespace bcf
{
class ProtocolBuilderManager: public NonCopyable
{
public:
    void addBuilder(const std::shared_ptr<IProtocolBuilder>& newBuilder);
    std::shared_ptr<bb::ByteBuffer> build(PackMode id,
                                          const std::shared_ptr<AbstractProtocolModel>& model);

private:
    std::map<PackMode, std::shared_ptr<IProtocolBuilder>> m_builders;
};
}
