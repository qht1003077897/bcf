#pragma once

#include <memory>
#include <map>
#include <set>
#include <bcfexport.h>
#include <ichannel.h>

namespace bcf
{

class BCF_EXPORT ChannelManager
{

public:
    static ChannelManager& getInstance();

    void addChannel(bcf::ChannelID, std::shared_ptr<IChannel>);
    void removeChannel(bcf::ChannelID);
    std::shared_ptr<IChannel> getChannel(bcf::ChannelID);

    void bindCmdAndChannel(uint32_t cmd, bcf::ChannelID channelID = ChannelID::Serial);
    bcf::ChannelID getChannelIDOfCmd(uint32_t cmd);
private:
    std::map<bcf::ChannelID, std::shared_ptr<IChannel>> channels;

    //每个cmd所对应的通道id
    std::map<uint32_t, bcf::ChannelID> channelIdOfCmds;
};
}
