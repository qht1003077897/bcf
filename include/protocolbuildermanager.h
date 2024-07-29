#pragma once

#include <memory>
#include <map>
#include <bcfexport.h>
#include <abstractprotocolmodel.h>
#include <iprotocolbuilder.h>
#include <noncopyable.hpp>

namespace bcf
{
class BCF_EXPORT ProtocolBuilderManager: public bcf::NonCopyable
{
public:
    void addBuilder(std::shared_ptr<IProtocolBuilder> newBuilder);
    std::string build(bcf::ProtocolType id, std::shared_ptr<AbstractProtocolModel> model);

private:
    std::map<ProtocolType, std::shared_ptr<IProtocolBuilder>> builders;
};
}
