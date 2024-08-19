#pragma once

#include <memory>
#include "bcfexport.h"
#include "ichannel.h"
#include "ymodem.h"
#ifdef BCF_USE_QT_SERIALPORT
    #include <QTimer>
#elif
    #include "base/timer.h"
#endif

namespace bcf
{
class BCF_EXPORT YmodemTransmit  :
#ifdef BCF_USE_QT_SERIALPORT
    public QObject,
    public YModem
{
    Q_OBJECT
#elif
    public YModem {
#endif
public:
    explicit YmodemTransmit();
    ~YmodemTransmit();

public:
    void setFileName(const std::string & name);

    void setChannel(std::shared_ptr<IChannel> channel);
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
    void setTimeOut(int);

protected:
    virtual uint32_t callback(YModem::Status status, uint8_t* buff,
                              uint32_t* len) override;
    virtual uint32_t read(uint8_t* buff, uint32_t len) override;
    virtual uint32_t write(uint8_t* buff, uint32_t len) override;

private:
    int                          progress;
    uint64_t                     fileSize;
    uint64_t                     fileCount;
    YModem::Status               status;

    FILE*                        pfile;

#ifdef BCF_USE_QT_SERIALPORT
    std::shared_ptr<QTimer>      readTimer;
#else
    std::shared_ptr<bcf::Timer>  readTimer;
#endif
    std::shared_ptr<IChannel>    channel;
    std::string                  fileName;
    ProgressCallback             progressCallback;
    TransmitStatusCallback       transmitStatusCallback;
    std::function<void()>        timeOutFunc;
};
}
