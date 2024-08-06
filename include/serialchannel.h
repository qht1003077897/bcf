﻿#ifdef BCF_USE_SERIALPORT
#pragma once
#include <QObject>
#include <ichannel.h>
#include <bcfexport.h>
#include <base/globaldefine.h>

class QSerialPort;
namespace bcf
{
class BCF_EXPORT SerialChannel : public QObject, public IChannel
{
    Q_OBJECT

public:
    explicit SerialChannel(QObject* parent = nullptr);
    virtual ~SerialChannel();

    void setPortName(const std::string& name);

    void setBaudRate(int baudRate);

    void setDataBits(int dataBits);

    void setParity(int parity);

    void setStopBits(int stopBits);

    void setFlowControl(int flowControl);


private slots:
    void onReceivedData();
    void onErrorOccurred(int error);

protected:
    bool openInternal() override;
    bool closeInternal() override;
    bool isOpen() override;
    int64_t send(const char* data, uint32_t len) override;
    uint32_t read(uint8_t* buff, uint32_t len) override;
    uint32_t write(uint8_t* buff, uint32_t len) override;

    ByteBufferPtr readAll() override;
    void setMaxRecvBufferSize(int maxRecvBufferSize) override;
private:
    QSerialPort* m_pSerialPort = nullptr;
};
}       // namespace bcf
#endif  //BCF_USE_SERIALPORT
