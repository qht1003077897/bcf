#pragma once

#include <memory>
#include <functional>
#include <map>
#include <bcfexport.h>
#include <ichannel.h>

namespace bcf
{
class BCF_EXPORT ChannelManager
{
public:
    void registerChannel(bcf::ChannelID type, CreateChannelFunc func);

    std::shared_ptr<bcf::IChannel> CreateChannel(bcf::ChannelID id);

    std::shared_ptr<IChannel> getChannel(bcf::ChannelID);
private:
    void addChannel(bcf::ChannelID, std::shared_ptr<IChannel>);
    void removeChannel(bcf::ChannelID);

private:
    std::map<bcf::ChannelID, std::shared_ptr<IChannel>> channels;
    std::map<bcf::ChannelID, CreateChannelFunc> channelCreators;
};
}
