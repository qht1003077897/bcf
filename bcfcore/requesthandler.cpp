#include "requesthandler.h"
#include <channelmanager.h>
#include <globaldefine.h>
#include <protocolbuildermanager.h>
#include <protocolparsermanager.h>
using namespace bcf;

RequestHandler::RequestHandler()
{
    startTimeOut();
    startUserCallbackThread();
}

RequestHandler::~RequestHandler()
{
    m_isexit = true;
    m_cv.notify_all();
    if (m_usercallbackthread.joinable()) {
        m_usercallbackthread.join();
    }
}

RequestHandler& RequestHandler::getInstance()
{
    static RequestHandler instance;
    return instance;
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

    unsigned char* data = new unsigned char[100];
    uint32_t size = ProtocolBuilderManager::getInstance().build(model->protocolType(), model, data);
    callbacks.insert(std::make_pair(model->seq, std::make_pair(timeoutSeconds, std::move(callback))));
    channel->send(data, size);
}

//如何区分是发送回复的还是主动上报的？因此全部都是走receive，receive里面根据业务ID判断转给哪个callback
//如果能匹配到seq,则直接callback,如果匹配不到,则是主动上报的,交给receive调用点进行转发处理
void RequestHandler::receive(ReceiveCallback callback, bcf::ChannelID id)
{
    const auto channel = ChannelManager::getInstance().getChannel(id);
    channel->receive([ =, _callback = std::move(callback)](int code, const unsigned char* data,
    uint32_t len) {

        ProtocolParserManager::getInstance().parseByAll(data,
                                                        len, [ = ](bcf::IProtocolParser::ParserState state,
        std::shared_ptr<bcf::AbstractProtocolModel> model) {
            if (state == IProtocolParser::WaitingStick) {
                printf("wait parsing...,state is WaitingStick");
                return;
            } else if (state == IProtocolParser::Abandon) {
//                emit MessageCenter::getInstance().signalMessageReceived(QString::fromLatin1(
//                                                                            metaObject()->className()),
//                                                                        MessageCenter::MessageID::ProtocolAbandonData, QByteArray::fromStdString(mcuModel.DATA));
                return;
            }

            {
                int key = model->seq;
                const auto& itr = callbacks.find(key);
                if (itr != callbacks.end()) {
                    if (nullptr != itr->second.second) {
                        std::function<void()> func = std::bind(*(itr->second.second), code, model);
                        pushqueueAndNotify(std::move(func));
                    }
                    callbacks.erase(itr);
                    return;
                }
            }

            printf("not find seq,print to logWidget");
            _callback(code, model);
        });
    });
}

void RequestHandler::addInterceptor()
{

}

void RequestHandler::startUserCallbackThread()
{
    m_usercallbackthread = std::thread([this]() {
        while (!m_isexit) {
            std::unique_lock<std::mutex> l(m_mtx);
            m_cv.wait_for(l, std::chrono::seconds(5), [this] {
                std::unique_lock<std::mutex> l(m_mtxuserdata);
                return !m_userqueue.empty();
            });

            {
                std::unique_lock<std::mutex> l(m_mtxuserdata);
                while (!m_userqueue.empty()) {
                    auto  userqueue = popall();
                    for (auto& callback : userqueue) {
                        callback();
                    }
                }
            }
        }
    });
    m_usercallbackthread.detach();
}

void RequestHandler::startTimeOut()
{
    auto timeOutThread = std::thread([this]() {
        while (!m_isexit) {
            std::unique_lock<std::mutex> l(m_mtxuserdata);
            auto it  = callbacks.begin();
            while (it != callbacks.end()) {
                if ( 0 == it->second.first) {
                    //超时时间减到0
                    printf("seq: %d timeout,remove it!", it->first);
                    std::function<void()> func = std::bind((*(it->second.second)), bcf::ErrorCode::TIME_OUT, nullptr);
                    pushqueueAndNotify(std::move(func));
                    callbacks.erase(it++);
                    continue;
                }

                //每秒递减1,直到被callback完成移除或超时移除
                it->second.first --;
                it++;
            }
        }
    });
}

void RequestHandler::pushqueueAndNotify(std::function<void()>&&  data)
{
    std::unique_lock<std::mutex> l(m_mtxuserdata);
    m_userqueue.push_back(std::move(data));
    m_cv.notify_all();
}

std::deque<std::function<void()>> RequestHandler::popall()
{
    std::deque<std::function<void()>> userqueue;
    std::unique_lock<std::mutex> l(m_mtxuserdata);
    std::swap(userqueue, m_userqueue);
    return userqueue;
}
