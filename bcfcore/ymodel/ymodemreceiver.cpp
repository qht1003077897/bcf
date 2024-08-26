#include "ymodemreceiver.h"
#include <QDebug>
#include <QByteArray>

#define READ_TIME_MAX   (4)
#define READ_TIME_OUT   (10)
#define WRITE_TIME_OUT  (100)

using namespace  bcf;

YmodemFileReceive::YmodemFileReceive()
{
    setTimeDivide(499);
    setTimeMax(READ_TIME_MAX);
    setErrorMax(999);

    QObject::connect(&readTimer, SIGNAL(timeout()), this, SLOT(slotReadTimeOut()));
}

YmodemFileReceive::~YmodemFileReceive()
{
}

void YmodemFileReceive::setSaveFilePath(const std::string& path)
{
    filePath = QString::fromStdString(path).append("/");
}

void YmodemFileReceive::setTimeOut(int timeMillS)
{
    if (timeMillS < 2000) {
        timeMillS = 2000;//最少2S超时
    }

    //timeMillS = READ_TIME_OUT * (TimeDivide+1) * (READ_TIME_MAX+1);
    int devideMixMax = timeMillS / READ_TIME_OUT / (READ_TIME_MAX + 1);
    setTimeDivide(devideMixMax);
}


void YmodemFileReceive::setChannel(std::shared_ptr<IChannel> _channel)
{
    channel = _channel;
}

std::shared_ptr<IChannel> YmodemFileReceive::getChannel()
{
    return channel;
}



bool YmodemFileReceive::startReceive()
{
    progress = 0;
    status   = StatusEstablish;


    if (nullptr == channel || !channel->isOpen()) {
        std::cout << "channel not open" << std::endl;
        return false;
    }

    readTimer.start(READ_TIME_OUT);
}

void YmodemFileReceive::stopReceive()
{
    file.close();
    abort();
    status = StatusAbort;

    callTransmitStatus(StatusAbort);
}

int YmodemFileReceive::getReceiveProgress()
{
    return progress;
}

YModem::Status YmodemFileReceive::getReceiveStatus()
{
    return status;
}

void YmodemFileReceive::setProgressCallback(const ProgressCallback& callback)
{
    progressCallback = std::move(callback);
}

void YmodemFileReceive::setTransmitStatusCallback(const TransmitStatusCallback& callback)
{
    transmitStatusCallback = std::move(callback);
}

void YmodemFileReceive::slotReadTimeOut()
{
    readTimer.stop();

    receive();

    if ((status == StatusEstablish) || (status == StatusTransmit)) {
        readTimer.start(READ_TIME_OUT);
    }
}

void YmodemFileReceive::callTransmitStatus(Status status)
{
    if (transmitStatusCallback) {
        transmitStatusCallback((TransmitStatus)status);
    }
}

uint32_t YmodemFileReceive::callback(Status status, uint8_t* buff, uint32_t* len)
{
    switch (status) {
        case StatusEstablish: {
                QByteArray b = QByteArray((char*)buff, 133).toHex();
                qDebug() << "recvbuff:" <<  b;
                if (buff[0] != 0) {
                    int  i         =  0;
                    char name[128] = {0};
                    char size[128] = {0};

                    for (int j = 0; buff[i] != 0 && buff[i] != 0x20; i++, j++) {
                        qDebug() << buff[i];
                        name[j] = buff[i];
                    }

                    i++;
                    //SOH 00 FF foo.c 3232 NUL[118] CRCH CRCL
                    //0或者空格,xshell 在文件名称和文件大小中间发送的是空格，即0x20,标准YMODEM协议要求文件名称以'\0'(也就是0)结尾，所以如果要实现和xshell互通，此处要兼容
                    for (int j = 0; buff[i] != 0 && buff[i] != 0x20; i++, j++) {
                        qDebug() << buff[i];
                        size[j] = buff[i];
                    }

                    fileName  = QString::fromLocal8Bit(name);
                    fileSize  = QString(size).toULongLong();
                    fileCount = 0;
                    qDebug() << "StatusEstablish::fileName:" << fileName ;
                    qDebug() << "StatusEstablish::fileSize:" << fileSize ;
                    file.setFileName(filePath + fileName);

                    if (file.open(QFile::WriteOnly) == true) {
                        YmodemFileReceive::status = StatusEstablish;

                        callTransmitStatus(StatusEstablish);

                        return CodeAck;
                    } else {
                        YmodemFileReceive::status = StatusError;

                        callTransmitStatus(StatusError);
                        return CodeCan;
                    }
                } else {
                    YmodemFileReceive::status = StatusError;

                    callTransmitStatus(StatusError);
                    return CodeCan;
                }
            }

        case StatusTransmit: {
                if ((fileSize - fileCount) > *len) {
                    qint64 w = file.write((char*)buff, *len);
                    fileCount += w;
                } else {
                    qint64 w = file.write((char*)buff, fileSize - fileCount);
                    fileCount += w;
                }

                progress = fileSize == 0 ? 0 : (int)(fileCount * 100 / fileSize);
                YmodemFileReceive::status = StatusTransmit;

                callTransmitStatus(StatusTransmit);
                if (progressCallback) {
                    progressCallback(progress);
                }

                return CodeAck;
            }

        case StatusFinish: {
                file.close();

                YmodemFileReceive::status = StatusFinish;

                callTransmitStatus(StatusFinish);

                //结束后，回复ack，以保证发送端正常结束状态
                //收到发送端的第二个EOT之后，发送端回复的ACK和C可能黏住了，导致发送端无法正确认为结束
                char data[1] = {0x06};
                channel->send(data, 1);

                return CodeAck;
            }

        case StatusAbort: {
                file.close();

                YmodemFileReceive::status = StatusAbort;

                callTransmitStatus(StatusAbort);
                return CodeCan;
            }

        case StatusTimeout: {
                YmodemFileReceive::status = StatusTimeout;

                callTransmitStatus(StatusTimeout);
                return CodeCan;
            }

        default: {
                file.close();

                YmodemFileReceive::status = StatusError;

                callTransmitStatus(StatusError);
                return CodeCan;
            }
    }
}

uint32_t YmodemFileReceive::read(uint8_t* buff, uint32_t len)
{
    if (nullptr == channel) {
        return -1;
    }
    return channel->read(buff, len);
}

uint32_t YmodemFileReceive::write(uint8_t* buff, uint32_t len)
{
    if (nullptr == channel) {
        return -1;
    }
    return channel->write(buff, len);
}
