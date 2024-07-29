#pragma once

#include <memory>
#include <functional>
#include <map>
#include <bcfexport.h>
#include <ichannel.h>
#include <noncopyable.hpp>
namespace bcf
{
class BCF_EXPORT ChannelManager: public bcf::NonCopyable
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
