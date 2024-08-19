#pragma once

#include <vector>
#include <ichannel.h>
#include <bcfexport.h>
#include <base/noncopyable.hpp>

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

    RequestHandlerBuilder& withMaxRecvBufferSize(int maxRecvBufferSize);

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

    void sendFile(const std::string& fileName, const ProgressCallback&, const TransmitStatusCallback&);
    //特定于串口的Ymodel协议
    void sendFileWithYModel(const std::string& fileName, const ProgressCallback&,
                            const TransmitStatusCallback&);

private:
    friend class RequestHandlerBuilder;
    class RequestHandlerPrivate;
    std::unique_ptr<RequestHandlerPrivate> d_ptr;
};
} // namespace bcf
