#pragma once

#include "bcfexport.h"
#include "ymodel/ymodemtransmit.h"

namespace bcf
{
class BCF_EXPORT FileTransmitHelper
{
public:
    void startTransmit(std::shared_ptr<IChannel> channel, const std::string& fileName,
                       const ProgressCallback& pcallback, const TransmitStatusCallback& tcallback)
    {
        if (nullptr == channel || !channel->isOpen()) {
            std::cerr << "error,channel is not open" << std::endl;
            return;
        }

        m_channel = channel;
    }

    void startTransmitWithYModel(std::shared_ptr<IChannel> channel, const std::string& fileName,
                                 const ProgressCallback& pcallback, const TransmitStatusCallback& tcallback)
    {
        if (nullptr == channel || !channel->isOpen()) {
            std::cerr << "error,channel is not open" << std::endl;
            return;
        }

        if (nullptr == m_ymodemTransmit) {
            m_ymodemTransmit = std::make_shared<bcf::YmodemTransmit>();
        }
        m_ymodemTransmit->setFileName(fileName);
        m_ymodemTransmit->setTransmitStatusCallback([channel, tc = std::move(tcallback),
        this](TransmitStatus status) {
            switch (status) {
                case StatusEstablishing:
                    break;
                case StatusEstablish:
                    break;
                case StatusTransmit:
                    break;
                case StatusFinish:
                    cleanUp(("StatusFinish"));
                    break;
                case StatusAbort:
                    cleanUp(("StatusAbort"));
                    break;
                case StatusTimeout:
                    cleanUp(("StatusTimeout"));
                    break;
                case StatusError:
                    cleanUp(("StatusError"));
                    break;
            }
            tc(status);
        });
        m_ymodemTransmit->setProgressCallback(pcallback);
        if (!m_ymodemTransmit->getChannel()) {
            m_ymodemTransmit->setChannel(channel);
        }

        m_channel = channel;
        m_channel->useActiveModel();
        m_ymodemTransmit->startTransmit();
    }

    void cancelTransmit()
    {
        const auto state = m_ymodemTransmit->getTransmitStatus();
        if (state == YModem::Status::StatusTransmit
                || state == YModem::Status::StatusEstablishing
                || state == YModem::Status::StatusEstablish) {
            m_ymodemTransmit->stopTransmit();
        }
    }

private:
    void cleanUp(const std::string& msg)
    {
        std::cout << "cleanUp:" << msg << std::endl;
        m_channel->usePassiveModel();
    }

private:
    std::shared_ptr<IChannel> m_channel;
    std::shared_ptr<bcf::YmodemTransmit> m_ymodemTransmit;
};
}
