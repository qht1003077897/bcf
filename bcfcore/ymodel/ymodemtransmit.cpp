#include <QFileInfo>
#include "ymodemtransmit.h"

using namespace bcf;

#define READ_TIME_MAX   (4)
#define READ_TIME_OUT   (10)
#define WRITE_TIME_OUT  (100)

YmodemTransmit::YmodemTransmit()
    : readTimer(std::make_shared<QTimer>())
{
    setTimeDivide(499);
    setTimeMax(READ_TIME_MAX);
    setErrorMax(999);

    QObject::connect(readTimer.get(), &QTimer::timeout, this, &YmodemTransmit::slotReadTimeOut);
}

YmodemTransmit::~YmodemTransmit()
{
    file.close();
    readTimer->stop();
}

void YmodemTransmit::setFileName(const std::string& name)
{
    file.setFileName(QString::fromStdString(name));
}

void YmodemTransmit::setTimeOut(int timeMills)
{
    if (timeMills < 2000) {
        timeMills = 2000;//最少2S超时
    }

    //timeMills = READ_TIME_OUT * (TimeDivide+1) * (READ_TIME_MAX+1);
    int devideMixMax = timeMills / READ_TIME_OUT / (READ_TIME_MAX + 1);
    setTimeDivide(devideMixMax);
}

void YmodemTransmit::setChannel(const std::shared_ptr<IChannel>& _channel)
{
    channel = _channel;
}

std::shared_ptr<IChannel> YmodemTransmit::getChannel()
{
    return channel;
}

bool YmodemTransmit::startTransmit()
{
    progress = 0;
    status   = StatusEstablish;

    if (nullptr == channel || !channel->isOpen()) {
        std::cout << "channel not open" << std::endl;
        return false;
    }

    QFileInfo fileInfo(file);
    QByteArray baFileName = fileInfo.fileName().toLocal8Bit();
    QByteArray baFileSize = QByteArray::number(fileInfo.size());
    //SOH 00 FF NUL[128] CRCH CRCL
    if (baFileName.length() + baFileSize.length() > (YMODEM_PACKET_SIZE - 2)) {
        std::cerr << "file' length exceed 128" << std::endl;
        return false;
    }

    //MCU底层代码写的有问题，MCU作为接收端没有主动轮询发送C，需要上位机发送端发送一个任意内容（不要超过1个字节）触发C回复
    char data[1] = {'C'};
    channel->send(data, 1);
    startWithTimeOut(READ_TIME_OUT);
    return true;
}

void YmodemTransmit::stopTransmit()
{
    file.close();
    abort();
    status = StatusAbort;
    readTimer->stop();
    callTransmitStatus(status);
}

int YmodemTransmit::getTransmitProgress()
{
    return progress;
}

YModem::Status YmodemTransmit::getTransmitStatus()
{
    return status;
}

void YmodemTransmit::setProgressCallback(const ProgressCallback& callback)
{
    progressCallback = std::move(callback);
}

void YmodemTransmit::setTransmitStatusCallback(const TransmitStatusCallback& callback)
{
    transmitStatusCallback = std::move(callback);
}

void YmodemTransmit::slotReadTimeOut()
{
    readTimer->stop();

    transmit();

    if ((status == StatusEstablish) || (status == StatusTransmit)) {
        startWithTimeOut(READ_TIME_OUT);
    }
}

void YmodemTransmit::callTransmitStatus(Status state)
{
    if (transmitStatusCallback) {
        transmitStatusCallback((TransmitStatus)state);
    }
}

void YmodemTransmit::startWithTimeOut(int timeout)
{
    readTimer->start(timeout);
}

uint32_t YmodemTransmit::callback(Status _status, uint8_t* buff, uint32_t* len)
{
    switch (_status) {
        case StatusEstablish: {
                if (file.open(QFile::ReadOnly)) {
                    QFileInfo fileInfo(file);

                    fileSize  = fileInfo.size();
                    std::cout << "fileSize: " << fileSize << std::endl;
                    fileCount = 0;

                    QByteArray baFileName = fileInfo.fileName().toLocal8Bit();
                    QByteArray baFileSize = QByteArray::number(fileInfo.size());
                    strncpy((char*)buff, baFileName.data(), baFileName.size());
                    strncpy((char*)buff + baFileName.size() + 1, baFileSize.data(), baFileSize.size());

                    *len = YMODEM_PACKET_SIZE;

                    status = StatusEstablish;
                    callTransmitStatus(StatusEstablish);
                    return CodeAck;
                } else {
                    status = StatusError;
                    readTimer->start(WRITE_TIME_OUT);
                    return CodeCan;
                }
            }

        case StatusTransmit: {
                if (fileSize != fileCount) {
                    //*** 此处勿动，最后一个包不足128 一律按照1024处理，否则MCU校验不通过
                    if ((fileSize - fileCount) > YMODEM_PACKET_SIZE) {
                        fileCount += file.read((char*)buff, YMODEM_PACKET_1K_SIZE);
                        *len = YMODEM_PACKET_1K_SIZE;
                    } else {
                        fileCount += file.read((char*)buff, YMODEM_PACKET_SIZE);
                        * len = YMODEM_PACKET_SIZE;
                    }
                    progress = (int)(fileCount * 100 / fileSize);
                    status = StatusTransmit;
                    if (progressCallback) {
                        progressCallback(progress);
                    }
                    callTransmitStatus(StatusTransmit);
                    return CodeAck;
                } else {
                    status = StatusTransmit;
                    callTransmitStatus(StatusTransmit);
                    return CodeEot;
                }
            }

        case StatusFinish: {
                file.close();
                status = StatusFinish;
                startWithTimeOut(WRITE_TIME_OUT);
                callTransmitStatus(StatusFinish);
                return CodeAck;
            }

        case StatusAbort: {
                file.close();
                status = StatusAbort;
                startWithTimeOut(WRITE_TIME_OUT);
                callTransmitStatus(StatusAbort);
                return CodeCan;
            }

        case StatusTimeout: {
                status = StatusTimeout;
                startWithTimeOut(WRITE_TIME_OUT);
                callTransmitStatus(StatusTimeout);
                return CodeCan;
            }
        case StatusEstablishing: {
                callTransmitStatus(StatusEstablishing);
                return CodeAck;
            }
        default: {
                file.close();
                status = StatusError;
                startWithTimeOut(WRITE_TIME_OUT);
                return CodeCan;
            }
    }
}

uint32_t YmodemTransmit::read(uint8_t* buff, uint32_t len)
{
    if (nullptr == channel) {
        return -1;
    }
    return channel->read(buff, len);
}

uint32_t YmodemTransmit::write(uint8_t* buff, uint32_t len)
{
    if (nullptr == channel) {
        return -1;
    }
    return channel->write(buff, len);
}
