#include "channelmanager.h"
using namespace bcf;

ChannelManager& ChannelManager::getInstance()
{
    static ChannelManager instance;
    return instance;
}

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

void ChannelManager::bindCmdAndChannel(uint32_t cmd, ChannelID channelID)
{
    channelIdOfCmds[cmd] = channelID;
}

ChannelID ChannelManager::getChannelIDOfCmd(uint32_t cmd)
{
    return channelIdOfCmds[cmd];
}
