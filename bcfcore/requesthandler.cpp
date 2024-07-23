#include "requesthandler.h"
#include <channelmanager.h>
#include <globaldefine.h>
#include <protocolbuildermanager.h>
#include <protocolparsermanager.h>
using namespace bcf;


RequestHandlerBuilder::RequestHandlerBuilder()
    : m_requestHandler(std::make_shared<RequestHandler>()) {};

RequestHandlerBuilder& RequestHandlerBuilder::WithTimeOut(int timeoutMillSeconds)
{
    m_requestHandler->setTimeOut(timeoutMillSeconds);
    return *this;
}

RequestHandlerBuilder& RequestHandlerBuilder::withAbandonCallback(bcf::AbandonCallback&& callback)
{
    m_requestHandler->setAbandonCallback(std::move(callback));
    return *this;
}

std::shared_ptr<RequestHandler> RequestHandlerBuilder::build()
{
    return m_requestHandler;
}

RequestHandler::RequestHandler()
{
    startTimeOut();
}

RequestHandler::~RequestHandler()
{
    m_isexit = true;
}

void RequestHandler::request(std::shared_ptr<bcf::AbstractProtocolModel> model,
                             std::shared_ptr<RequestCallback> callback)
{
    const auto channel = ChannelManager::getInstance().getChannel(
                             ChannelManager::getInstance().getChannelIDOfCmd(model->cmd));
    if (nullptr == channel || !channel->isOpen()) {
        printf("error,channel is not open");
        (*callback)(bcf::ErrorCode::CHANNEL_CLOSE, nullptr);
        return;
    }

    std::string res = ProtocolBuilderManager::getInstance().build(model->protocolType(), model);
    {
        std::unique_lock<std::mutex> l(m_mtx);
        callbacks.insert(std::make_pair(model->seq, std::make_pair(m_timeoutMillSeconds,
                                                                   std::move(callback))));
    }
    channel->send((unsigned char*)res.c_str(), res.length());
}

//如何区分是发送回复的还是主动上报的？因此全部都是走receive，receive里面根据业务ID判断转给哪个callback
//如果能匹配到seq,则直接callback,如果匹配不到,则是主动上报的,交给receive调用点进行转发处理
void RequestHandler::receive(ReceiveCallback _callback, bcf::ChannelID id)
{
    bcf::DataCallback call =  [ =, callback = std::move(_callback)](const std::string dataStr) {

        ProtocolParserManager::getInstance().parseByAll(dataStr, [ = ](bcf::IProtocolParser::ParserState
                                                                       state,
        std::shared_ptr<bcf::AbstractProtocolModel> model) {
            if (state == IProtocolParser::WaitingStick) {
                printf("wait parsing...,state is WaitingStick");
                return;
            } else if (state == IProtocolParser::Abandon) {
                if (nullptr != m_abandonCallback) {
                    m_abandonCallback(model);
                }
                return;
            }

            {
                int key = model->seq;
                std::unique_lock<std::mutex> l(m_mtx);
                const auto& itr = callbacks.find(key);
                if (itr != callbacks.end()) {
                    if (nullptr != itr->second.second) {
                        std::bind(*(itr->second.second), bcf::ErrorCode::OK, model)();
                    }
                    callbacks.erase(itr);
                    return;
                }
            }

            printf("not find seq,print to logWidget");
            callback(bcf::ErrorCode::UNOWNED_DATA, model);
        });
    };

    const auto channel = ChannelManager::getInstance().getChannel(id);
    channel->setDataCallback(std::move(call));
}

void RequestHandler::addInterceptor()
{

}

void RequestHandler::setTimeOut(int timeoutMillSeconds)
{
    m_timeoutMillSeconds = timeoutMillSeconds;
}

void RequestHandler::setAbandonCallback(AbandonCallback &&callback)
{
    m_abandonCallback = std::move(callback);
}

void RequestHandler::startTimeOut()
{
    auto timeOutThread = std::thread([this]() {
        while (!m_isexit) {
            std::unique_lock<std::mutex> l(m_mtx);
            auto it  = callbacks.begin();
            while (it != callbacks.end()) {
                if ( 0 == it->second.first) {
                    //超时时间减到0
                    printf("seq: %d timeout,remove it!", it->first);
                    std::bind(*(it->second.second), bcf::ErrorCode::TIME_OUT, nullptr)();
                    callbacks.erase(it++);
                    continue;
                }

                //每秒递减1,直到被callback完成移除或超时移除 //TODO 优化:支持毫秒级定时器
                it->second.first = std::max(0,  it->second.first - 1000);
                it++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
    timeOutThread.detach();
}
