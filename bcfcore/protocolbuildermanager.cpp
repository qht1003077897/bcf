#include "protocolbuildermanager.h"
using namespace bcf;

ProtocolBuilderManager& ProtocolBuilderManager::getInstance()
{
    static ProtocolBuilderManager instance;
    return instance;
}


void ProtocolBuilderManager::addBuilder(std::shared_ptr<IProtocolBuilder> newBuilder)
{
    if (newBuilder) {
        builders.insert(std::make_pair(newBuilder->getType(), newBuilder));
    }
}

uint32_t ProtocolBuilderManager::build(bcf::ProtocolType id,
                                       std::shared_ptr<AbstractProtocolModel> model, unsigned char* data)
{
    const auto _build = builders.find(id);
    if (_build == builders.end()) {
        printf("not find id= %d's protocol builder", id);
        return -1;
    }

    const auto& builder = _build->second;
    return builder->build(model, data);
}

