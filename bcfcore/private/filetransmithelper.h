﻿#pragma once

#include "bcfexport.h"
#include "ymodel/ymodemtransmit.h"
#include "ymodel/ymodemreceiver.h"

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
            m_ymodemTransmit->setTimeOut(m_timeMillS);
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
    };

    void startRecvWithYModel(std::shared_ptr<IChannel> channel, const std::string& savePath,
                             const ProgressCallback& pcallback, const TransmitStatusCallback& tcallback)
    {
        if (nullptr == channel || !channel->isOpen()) {
            std::cerr << "error,channel is not open" << std::endl;
            return;
        }

        if (nullptr == m_ymodemReceiver) {
            m_ymodemReceiver = std::make_shared<bcf::YmodemFileReceive>();
            m_ymodemReceiver->setTimeOut(m_timeMillS);
        }
        m_ymodemReceiver->setSaveFilePath(savePath);
        m_ymodemReceiver->setTransmitStatusCallback([channel, tc = std::move(tcallback),
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
        m_ymodemReceiver->setProgressCallback(pcallback);
        if (!m_ymodemReceiver->getChannel()) {
            m_ymodemReceiver->setChannel(channel);
        }

        m_channel = channel;
        m_channel->useActiveModel();
        m_ymodemReceiver->startReceive();
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

    void cancelReceiver()
    {
        const auto state = m_ymodemReceiver->getReceiveStatus();
        if (state == YModem::Status::StatusTransmit
                || state == YModem::Status::StatusEstablishing
                || state == YModem::Status::StatusEstablish) {
            m_ymodemReceiver->stopReceive();
        }
    }

    void setTimeOut(int timeMillS)
    {
        m_timeMillS = timeMillS;
    };

private:
    void cleanUp(const std::string& msg)
    {
        std::cout << "cleanUp:" << msg << std::endl;
        m_channel->usePassiveModel();
    }

private:
    int m_timeMillS = DEFAULT_YMODEL_TIME_OUT_MILLSCENDS;
    std::shared_ptr<IChannel> m_channel;
    std::shared_ptr<bcf::YmodemTransmit>    m_ymodemTransmit;
    std::shared_ptr<bcf::YmodemFileReceive> m_ymodemReceiver;
};
}
