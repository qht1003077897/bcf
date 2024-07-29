#include "protocolbuildermanager.h"
using namespace bcf;

void ProtocolBuilderManager::addBuilder(std::shared_ptr<IProtocolBuilder> newBuilder)
{
    if (newBuilder) {
        builders.insert(std::make_pair(newBuilder->getType(), newBuilder));
    }
}

std::string ProtocolBuilderManager::build(bcf::ProtocolType id,
                                          std::shared_ptr<AbstractProtocolModel> model)
{
    const auto _build = builders.find(id);
    if (_build == builders.end()) {
        printf("not find id= %d's protocol builder", id);
        return "";
    }

    const auto& builder = _build->second;
    return builder->build(model);
}

