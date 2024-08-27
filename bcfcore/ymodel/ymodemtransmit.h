#pragma once
#include <memory>
#include <QTimer>
#include <QFile>
#include "ichannel.h"
#include "ymodem.h"

namespace bcf
{
class YmodemTransmit : public QObject, public YModem
{
    Q_OBJECT

public:
    explicit YmodemTransmit();
    ~YmodemTransmit();

public:
    void setFileName(const std::string& name);

    void setTimeOut(int timeMills);

    void setChannel(const std::shared_ptr<IChannel>& channel);
    std::shared_ptr<IChannel> getChannel();

    bool startTransmit();
    void stopTransmit();

    int getTransmitProgress();
    YModem::Status getTransmitStatus();

    void setProgressCallback(const ProgressCallback&);
    void setTransmitStatusCallback(const TransmitStatusCallback&);

private:
    /**
    * @brief 给send和receive的延迟缓冲时间，避免立即调用引起的未知异常
    */
    void slotReadTimeOut();
    void callTransmitStatus(Status);
    void startWithTimeOut(int);

protected:
    uint32_t callback(YModem::Status status, uint8_t* buff,
                      uint32_t* len) override;
    uint32_t read(uint8_t* buff, uint32_t len) override;
    uint32_t write(uint8_t* buff, uint32_t len) override;

private:
    int                          timeout = DEFAULT_YMODEL_TIME_OUT_MILLSCENDS;
    int                          progress;
    uint64_t                     fileSize;
    uint64_t                     fileCount;
    YModem::Status               status;
    QFile                        file;
    std::shared_ptr<QTimer>      readTimer;
    std::shared_ptr<IChannel>    channel;
    ProgressCallback             progressCallback;
    TransmitStatusCallback       transmitStatusCallback;
};
}
