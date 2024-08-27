#pragma once

#include <vector>
#include "ichannel.h"
#include "bcfexport.h"
#include "base/noncopyable.hpp"

namespace bcf
{
class IProtocolBuilder;
class IProtocolParser;
class RequestHandler;
class BCF_EXPORT RequestHandlerBuilder: public NonCopyable
{
public:
    virtual ~RequestHandlerBuilder() = default;

    /**
    * @brief 设置当前 RequestHandler 请求对象的request超时时间，超时会返回@enum ErrorCode::TIME_OUT
    * @param [IN]  timeoutMillSeconds  毫秒级的超时时间，默认值位于 @ref base/globaldefine.h @var DEFAULT_TIME_OUT_MILLSCENDS
    */
    RequestHandlerBuilder& withTimeOut(int timeoutMillSeconds);

    /**
    * @brief 设置当前 RequestHandler 每次接收缓冲区的大小
    * @param [IN]  maxRecvBufferSize  毫秒级的超时时间，默认值位于 @ref base/globaldefine.h @var DEFAULT_RECV_BUFFER_SIZE
    */
    RequestHandlerBuilder& withMaxRecvBufferSize(int maxRecvBufferSize);

    /**
    * @brief 注册当前 RequestHandler 所支持的协议构造器,只有被注册的协议构建器，才支持被 request 函数接收和识别，否则request函数将立即回调 @enum ErrorCode::PROTOCOL_NOT_EXIST 错误码
    * 支持注册多个协议构造器，意味着用户在调用@class RequestHandler::request 时可以选择多个协议模型进行通信
    */
    RequestHandlerBuilder& registProtocolBuilders(
        const std::vector<std::shared_ptr<IProtocolBuilder>>& protocolBuilders);

    /**
    * @brief 注册当前 RequestHandler 所支持的协议解析器,只有被注册的协议解析器，才能在收到下位机的数据后参与到解析动作。\n
    * 如果没有注册协议解析器，则request接口直到超时回调 @enum ErrorCode::TIME_OUT
    * 支持注册多个协议解析器，bcf在收到下位机数据后会遍历这些解析器，如果找到可以解包的解析器，则用其进行解包并返回model。如果找不到可以解包的解析器，则超时回调 @enum ErrorCode::TIME_OUT
    */
    RequestHandlerBuilder& registProtocolParsers(
        const std::vector<std::shared_ptr<IProtocolParser>>& protocolParsers);

    /**
    * @brief 设置当前 RequestHandler 对象使用的具体通道类，串口？ TCP？
    * @param [IN]  channelID
    * @param [OUT] CreateChannelFunc  延迟创建channel的回调函数，用户在此回调函数内部返回自己创建的@class IChannel 智能指针，可以使用bcf内部提供的@class SerialChannel_QT 类 \n
    */
    RequestHandlerBuilder& withChannel(int channelID, CreateChannelFunc&&);

    /**
    * @brief 设置连接失败的回调函数
    */
    RequestHandlerBuilder& withFailedCallback(ConnectionFailCallback&& callback);

    /**
    * @brief 设置连接成功的回调函数，如果连接成功，ConnectionCompletedCallback 会返回给用户一个 std::shared_ptr<IChannel> 对象， \n
    * 基于这个对象，用户可以使用其进行原始裸流数据的收发.
    * @ref examples/rawdata.cpp: 保存IChannel指针后,
    * @code
    * channel->send(const char*,uint32_t len);//发送
    * channel->setRawDataCallback(DataCallback&&);//设置原始数据回调
    * channel->isOpen();//查询连接状态
    * @endcode
    * 目前，对于IChannel类，只有上述三个函数是公开的.
    */
    RequestHandlerBuilder& withConnectionCompletedCallback(ConnectionCompletedCallback&& callback);

    /**
    * @brief 设置主动上报数据的回调函数，这个回调返回的数据必定不是经由request请求所返回的，可能是下位机主动push的数据. \n
    * 返回的数据类型是 std::shared_ptr<AbstractProtocolModel>，需要用户转换成 @class RequestHandlerBuilder::registProtocolParsers 接口所设置的协议解析器对应的协议模型。 \n
    * @ref examples/simpleserialport/simpleserialport.cpp
    * @code
    * auto bmodel = std::dynamic_pointer_cast<ByHeadProtocolModel>(model);
    * @endcode
    */
    RequestHandlerBuilder& withReceiveData(ReceiveCallback&&);

    std::shared_ptr<RequestHandler> build();

private:
    ConnectOption                                   m_ConnectOption;
    CreateChannelFunc                               m_ccfunc;
    std::shared_ptr<RequestHandler>                 m_requestHandler;
    std::vector<std::shared_ptr<IProtocolBuilder>>  m_protocolBuilders;
    std::vector<std::shared_ptr<IProtocolParser>>   m_protocolParsers;
};

class BCF_EXPORT RequestHandler: public NonCopyable,
    public std::enable_shared_from_this<RequestHandler>
{
public:
    /**
    * @brief bcf的核心接口. 构建自己的协议Model，请求的结果会异步返回到RequestCallback回调，\n
    * 返回的数据类型是 std::shared_ptr<AbstractProtocolModel>，需要用户转换成 withProtocolParsers 接口所设置的协议解析器对应的协议模型。\n
    * 如果请求超时，即超过了 withTimeOut 所配置的超时时间，则 RequestCallback 返回 @enum ErrorCode::TIME_OUT,且bcf内部会丢弃这个请求
    * @example examples/simpleserialport.cpp
    * @example examples/simpletcpclient.cpp
    */
    void request(const std::shared_ptr<AbstractProtocolModel>& model, RequestCallback&&);

    /**
    * @brief 特定于串口的基于YModel协议的文件发送，支持发送文件给xshell进行验证.
    * @ref examples/ymodel/ymodel_send.cpp
    */
    void sendFileWithYModel(const std::string& fileName, const ProgressCallback&,
                            const TransmitStatusCallback&, int timeoutMillS = DEFAULT_YMODEL_TIME_OUT_MILLSCENDS);

    /**
    * @brief 特定于串口的基于YModel协议的文件接收，支持接收来自xshell的文件或者支持接收来自 sendFileWithYModel 函数的文件.
    * @example examples/ymodel/ymodel_recv.cpp
    */
    void recvFileWithYModel(const std::string& savePath, const ProgressCallback&,
                            const TransmitStatusCallback&, int timeoutMillS = DEFAULT_YMODEL_TIME_OUT_MILLSCENDS);

    /**
    * @brief 建立IO连接(QT 串口是同步连接，即 ConnectionCompletedCallback 回调会立即回调。  QT TCP是异步连接，ConnectionCompletedCallback 不会立即回调。)
    * 因为串口打开设备的动作一般都会快。 而对于tcp客户端，受限于网络、服务器、防火墙等因素，大多数都是异步连接。
    */
    void connect();

private:
    friend class RequestHandlerBuilder;
    RequestHandler();
    class RequestHandlerPrivate;
    std::unique_ptr<RequestHandlerPrivate> d_ptr;
};
} // namespace bcf
