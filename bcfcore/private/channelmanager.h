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
    void registerChannel(int64_t, CreateChannelFunc&& func);

    std::shared_ptr<IChannel> CreateChannel(int64_t id);

    std::shared_ptr<IChannel> getChannel(int64_t);

private:
    void addChannel(int64_t, std::shared_ptr<IChannel>);
    void removeChannel(int64_t);

private:
    std::map<int64_t, std::shared_ptr<IChannel>> m_channels;
    std::map<int64_t, CreateChannelFunc> m_channelCreators;
};
}
