#pragma once

#include <vector>
#include <ichannel.h>
#include <bcfexport.h>

namespace bcf
{
class IProtocolParser;
class RequestHandler;
class BCF_EXPORT RequestHandlerBuilder
{
public:
    RequestHandlerBuilder();
    virtual ~RequestHandlerBuilder() = default;

    friend class RequestHandler;
    RequestHandlerBuilder& withTimeOut(int timeoutMillSeconds);

    RequestHandlerBuilder& withTcpAddr(const std::string& ip, int port);

    RequestHandlerBuilder& withSerialPortPortName(const std::string& name);

    RequestHandlerBuilder& withSerialPortBaudRate(BaudRate baudRate);

    RequestHandlerBuilder& withSerialPortDataBits(DataBits dataBits);

    RequestHandlerBuilder& withSerialPortParity(Parity parity);

    RequestHandlerBuilder& withSerialPortStopBits(StopBits stopBits);

    RequestHandlerBuilder& withSerialPortFlowControl(FlowControl flowControl);

    RequestHandlerBuilder& withAbandonCallback(bcf::AbandonCallback&& callback);

    RequestHandlerBuilder& withProtocolParsers(
        const std::vector<std::shared_ptr<bcf::IProtocolParser>>& protocolParsers);

    RequestHandlerBuilder& withChannel(bcf::ChannelID channelid, CreateChannelFunc&&);

    RequestHandlerBuilder& withFailedCallback(bcf::ConnectionFailCallback&& callback);

    RequestHandlerBuilder& withConnectionCompletedCallback(bcf::ConnectionCompletedCallback&& callback);

    RequestHandlerBuilder& withReceiveData(ReceiveCallback&&);

    std::shared_ptr<RequestHandler> asyncConnect();

private:
    std::shared_ptr<RequestHandler> m_requestHandler;
};

class RequestHandler
{
public:
    RequestHandler();
    ~RequestHandler();
    void request(std::shared_ptr<bcf::AbstractProtocolModel> model, std::shared_ptr<RequestCallback>);

private:
    friend class RequestHandlerBuilder;
    class RequestHandlerPrivate;
    std::unique_ptr<RequestHandlerPrivate> d_ptr;
};
} // namespace bcf
