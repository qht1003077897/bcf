#include "channelmanager.h"
using namespace bcf;

void ChannelManager::addChannel(bcf::ChannelID id, std::shared_ptr<IChannel> channel)
{
    if (nullptr == channel) {
        return;
    }

    const auto tmpchannel = getChannel(id);
    if (nullptr != tmpchannel) {
        return;
    }

    channels.emplace(id, channel);
}

void ChannelManager::removeChannel(bcf::ChannelID id)
{
    auto iter = channels.find(id);
    if (iter == channels.end()) {
        return;
    }

    channels.erase(iter);
}

std::shared_ptr<IChannel> ChannelManager::getChannel(bcf::ChannelID id)
{
    const auto iter = channels.find(id);
    if (iter == channels.end()) {
        return nullptr;
    }

    return iter->second;
}

void ChannelManager::registerChannel(bcf::ChannelID type, CreateChannelFunc func)
{
    channelCreators[type] = func;
}

std::shared_ptr<bcf::IChannel> ChannelManager::CreateChannel(bcf::ChannelID id)
{
    auto it = channelCreators.find(id);
    if (it != channelCreators.end()) {
        std::shared_ptr<bcf::IChannel> channel =  it->second();
        addChannel(id, channel);
        return channel;
    }
    throw std::invalid_argument("Unknown ChannelID");
}
