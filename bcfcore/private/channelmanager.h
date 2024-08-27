#pragma once

#include <map>
#include <memory>
#include <functional>
#include "ichannel.h"
#include "base/noncopyable.hpp"

namespace bcf
{
class ChannelManager: public NonCopyable
{
public:
    void registerChannel(int, CreateChannelFunc&& func);

    std::shared_ptr<IChannel> CreateChannel(int id);

    std::shared_ptr<IChannel> getChannel(int);

private:
    void addChannel(int, std::shared_ptr<IChannel>);
    void removeChannel(int);

private:
    std::map<int, std::shared_ptr<IChannel>> m_channels;
    std::map<int, CreateChannelFunc> m_channelCreators;
};
}
