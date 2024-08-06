#pragma once

#include <memory>
#include <map>
#include <bcfexport.h>
#include <abstractprotocolmodel.h>
#include <iprotocolbuilder.h>
#include <base/noncopyable.hpp>

namespace bcf
{
class BCF_EXPORT ProtocolBuilderManager: public bcf::NonCopyable
{
public:
    void addBuilder(std::shared_ptr<IProtocolBuilder> newBuilder);
    std::shared_ptr<bb::ByteBuffer> build(bcf::PackMode id,
                                          std::shared_ptr<AbstractProtocolModel> model);

private:
    std::map<PackMode, std::shared_ptr<IProtocolBuilder>> builders;
};
}
