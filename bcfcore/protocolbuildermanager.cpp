#include "private/protocolbuildermanager.h"

using namespace bcf;

void ProtocolBuilderManager::addBuilder(const std::shared_ptr<IProtocolBuilder>& newBuilder)
{
    if (newBuilder) {
        m_builders.emplace(std::make_pair(newBuilder->getType(), std::move(newBuilder)));
    }
}

std::shared_ptr<bb::ByteBuffer> ProtocolBuilderManager::build(PackMode id,
                                                              const std::shared_ptr<AbstractProtocolModel>& model)
{
    const auto _build = m_builders.find(id);
    if (_build == m_builders.end()) {
        std::cout << "not find protocol builder,id= " << id << std::endl;
        return nullptr;
    }

    const auto& builder = _build->second;
    return builder->build(model);
}

