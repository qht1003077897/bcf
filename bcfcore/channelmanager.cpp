#include "private/channelmanager.h"
using namespace bcf;

void ChannelManager::addChannel(int id, std::shared_ptr<IChannel> channel)
{
    if (nullptr == channel) {
        return;
    }

    const auto tmpchannel = getChannel(id);
    if (nullptr != tmpchannel) {
        return;
    }

    channel->setChannelID(id);
    channels.emplace(id, channel);
}

void ChannelManager::removeChannel(int id)
{
    auto iter = channels.find(id);
    if (iter == channels.end()) {
        return;
    }

    channels.erase(iter);
}

std::shared_ptr<IChannel> ChannelManager::getChannel(int id)
{
    const auto iter = channels.find(id);
    if (iter == channels.end()) {
        return nullptr;
    }

    return iter->second;
}

void ChannelManager::registerChannel(int type, CreateChannelFunc func)
{
    channelCreators[type] = func;
}

std::shared_ptr<bcf::IChannel> ChannelManager::CreateChannel(int id)
{
    std::shared_ptr<IChannel> ch = getChannel(id);
    if (nullptr != ch) {
        return ch;
    }

    auto it = channelCreators.find(id);
    if (it != channelCreators.end()) {
        std::shared_ptr<bcf::IChannel> channel =  it->second();
        addChannel(id, channel);
        return channel;
    }
    throw std::invalid_argument("Unknown ChannelID");
}
