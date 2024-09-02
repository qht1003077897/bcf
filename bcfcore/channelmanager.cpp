#include "private/channelmanager.h"

using namespace bcf;

void ChannelManager::addChannel(int64_t id, std::shared_ptr<IChannel> channel)
{
    if (nullptr == channel) {
        return;
    }

    const auto tmpchannel = getChannel(id);
    if (nullptr != tmpchannel) {
        return;
    }

    channel->setChannelID(id);
    m_channels.emplace(id, channel);
}

void ChannelManager::removeChannel(int64_t id)
{
    auto iter = m_channels.find(id);
    if (iter == m_channels.end()) {
        return;
    }

    m_channels.erase(iter);
}

std::shared_ptr<IChannel> ChannelManager::getChannel(int64_t id)
{
    const auto iter = m_channels.find(id);
    if (iter == m_channels.end()) {
        return nullptr;
    }

    return iter->second;
}

void ChannelManager::registerChannel(int64_t type, CreateChannelFunc&& func)
{
    m_channelCreators[type] = std::move(func);
}

std::shared_ptr<bcf::IChannel> ChannelManager::CreateChannel(int64_t id)
{
    std::shared_ptr<IChannel> ch = getChannel(id);
    if (nullptr != ch) {
        return ch;
    }

    auto it = m_channelCreators.find(id);
    if (it != m_channelCreators.end()) {
        std::shared_ptr<bcf::IChannel> channel =  it->second();
        addChannel(id, channel);
        return channel;
    }

    throw std::invalid_argument("Unknown ChannelID");
}
