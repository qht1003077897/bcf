#include <filesystem>
#include "ymodemtransmit.h"

namespace fs = std::filesystem;
using namespace bcf;

#define READ_TIME_OUT   (50)
#define WRITE_TIME_OUT  (100)

YmodemTransmit::YmodemTransmit()
#ifdef BCF_USE_QT_SERIALPORT
    : readTimer(std::make_shared<QTimer>())
#else
    : readTimer(std::make_shared<bcf::Timer>())
#endif
{
#ifdef BCF_USE_QT_SERIALPORT
    QObject::connect(readTimer.get(), &QTimer::timeout, this, &YmodemTransmit::slotReadTimeOut);
#else
    timeOutFunc = std::bind(&YmodemTransmit::slotReadTimeOut, this);
#endif
}

YmodemTransmit::~YmodemTransmit()
{
    delete pfile;
    readTimer->stop();
}

void YmodemTransmit::setFileName(const std::string& name)
{
    fileName = name;
}

void YmodemTransmit::setChannel(std::shared_ptr<IChannel> _channel)
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

    fs::path file_path = fileName;
    // 获取文件名
    fs::path filename = file_path.filename();
    auto fileSize = fs::file_size(file_path);
    //SOH 00 FF NUL[128] CRCH CRCL
    auto fileSizeStr = std::to_string(fileSize);
    if (filename.string().length() + fileSizeStr.length() > (YMODEM_PACKET_SIZE - 2)) {
        std::cerr << "file' length exceed 128" << std::endl;
        return false;
    }

    //MCU底层代码写的有问题，MCU作为接收端没有主动轮询发送C，需要上位机发送端发送一个任意内容（不要超过1个字节）触发C回复
    char data[1] = {'C'};
    channel->send(data, 1);
    setTimeOut(READ_TIME_OUT);
    return true;
}

void YmodemTransmit::stopTransmit()
{
    if (nullptr != pfile) {
        fclose(pfile);
    }
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
        setTimeOut(READ_TIME_OUT);
    }
}

void YmodemTransmit::callTransmitStatus(Status state)
{
    if (transmitStatusCallback) {
        transmitStatusCallback((TransmitStatus)state);
    }
}

void YmodemTransmit::setTimeOut(int timeout)
{
#ifdef BCF_USE_QT_SERIALPORT
    readTimer->start(timeout);
#else
    readTimer->setTimeout(timeOutFunc, timeout);
#endif
}

uint32_t YmodemTransmit::callback(Status _status, uint8_t* buff, uint32_t* len)
{
    switch (_status) {
        case StatusEstablish: {
                pfile = fopen(fileName.data(), "rb");
                if (pfile == nullptr) {
                    status = StatusError;
                    setTimeOut(WRITE_TIME_OUT);
                    return CodeCan;
                }
                fs::path file_path = fileName;
                fs::path fsfilename = file_path.filename();
                std::string filename = fsfilename.string();
                std::cout << "filename: " << filename << std::endl;
                fileSize = fs::file_size(file_path);

                fileCount = 0;
                strncpy((char*)buff, filename.data(), filename.size());
                std::string fileSizeStr = std::to_string(fileSize);
                std::cout << "fileSize: " << fileSizeStr << std::endl;
                strncpy((char*)buff + filename.size() + 1, fileSizeStr.data(), fileSizeStr.length());

                *len = YMODEM_PACKET_SIZE;
                status = StatusEstablish;
                callTransmitStatus(StatusEstablish);
                return CodeAck;
            }

        case StatusTransmit: {
                if (fileSize != fileCount) {
                    //*** 此处勿动，最后一个包不足128 一律按照1024处理，否则MCU校验不通过
                    if ((fileSize - fileCount) > YMODEM_PACKET_SIZE) {
                        fileCount += fread(buff, sizeof(char), YMODEM_PACKET_1K_SIZE, pfile);
                        *len = YMODEM_PACKET_1K_SIZE;
                    } else {
                        fileCount += fread(buff, sizeof(char), YMODEM_PACKET_SIZE, pfile);
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
                fclose(pfile);
                status = StatusFinish;
                setTimeOut(WRITE_TIME_OUT);
                callTransmitStatus(StatusFinish);
                return CodeAck;
            }

        case StatusAbort: {
                fclose(pfile);
                status = StatusAbort;
                setTimeOut(WRITE_TIME_OUT);
                callTransmitStatus(StatusAbort);
                return CodeCan;
            }

        case StatusTimeout: {
                status = StatusTimeout;
                setTimeOut(WRITE_TIME_OUT);
                callTransmitStatus(StatusTimeout);
                return CodeCan;
            }
        case StatusEstablishing: {
                callTransmitStatus(StatusEstablishing);
                return CodeAck;
            }
        default: {
                fclose(pfile);
                status = StatusError;
                setTimeOut(WRITE_TIME_OUT);
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
