﻿#pragma once

#include <memory>
#include <functional>
#include <map>
#include <ichannel.h>
#include <base/noncopyable.hpp>
namespace bcf
{
class ChannelManager: public bcf::NonCopyable
{
public:
    void registerChannel(int, CreateChannelFunc func);

    std::shared_ptr<bcf::IChannel> CreateChannel(int id);

    std::shared_ptr<IChannel> getChannel(int);
private:
    void addChannel(int, std::shared_ptr<IChannel>);
    void removeChannel(int);

private:
    std::map<int, std::shared_ptr<IChannel>> channels;
    std::map<int, CreateChannelFunc> channelCreators;
};
}