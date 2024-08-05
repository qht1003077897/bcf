﻿#pragma once

#include <vector>
#include <ichannel.h>
#include <bcfexport.h>
#include <noncopyable.hpp>

namespace bcf
{
class IProtocolBuilder;
class IProtocolParser;
class RequestHandler;
class BCF_EXPORT RequestHandlerBuilder: public bcf::NonCopyable
{
public:
    RequestHandlerBuilder();
    virtual ~RequestHandlerBuilder() = default;

    friend class RequestHandler;
    RequestHandlerBuilder& withTimeOut(int timeoutMillSeconds);

    RequestHandlerBuilder& useBigEndian(bool ussbigendian = true);

    RequestHandlerBuilder& withAbandonCallback(bcf::AbandonCallback&& callback);

    RequestHandlerBuilder& withProtocolBuilders(
        const std::vector<std::shared_ptr<bcf::IProtocolBuilder>>& protocolBuilders);

    RequestHandlerBuilder& withProtocolParsers(
        const std::vector<std::shared_ptr<bcf::IProtocolParser>>& protocolParsers);

    RequestHandlerBuilder& withChannel(int channelID, CreateChannelFunc&&);

    RequestHandlerBuilder& withFailedCallback(bcf::ConnectionFailCallback&& callback);

    RequestHandlerBuilder& withConnectionCompletedCallback(bcf::ConnectionCompletedCallback&& callback);

    RequestHandlerBuilder& withReceiveData(ReceiveCallback&&);

    std::shared_ptr<RequestHandler> connect();

private:
    std::shared_ptr<RequestHandler> m_requestHandler;
};

class BCF_EXPORT RequestHandler: public bcf::NonCopyable
{
public:
    RequestHandler();
    ~RequestHandler();
    void request(std::shared_ptr<bcf::AbstractProtocolModel> model, RequestCallback&&);

private:
    friend class RequestHandlerBuilder;
    class RequestHandlerPrivate;
    std::unique_ptr<RequestHandlerPrivate> d_ptr;
};
} // namespace bcf
