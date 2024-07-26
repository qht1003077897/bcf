#pragma once
#ifdef USE_BCF_SERIALPORT

#include <QObject>
#include <QSerialPort>
#include <ichannel.h>
#include <bcfexport.h>
#include <globaldefine.h>

namespace bcf
{

class BCF_EXPORT SerialChannel : public QObject, public IChannel
{
    Q_OBJECT

public:
    explicit SerialChannel(QObject* parent = nullptr);
    virtual ~SerialChannel();

private slots:
    void onReceivedData();
    void onErrorOccurred(QSerialPort::SerialPortError error);

protected:
    virtual bcf::ChannelID channelID() override;
    virtual bool openChannel() override;
    virtual bool closeChannel() override;
    virtual bool isOpen() override;
    virtual int64_t send(const unsigned char* data, uint32_t len) override;
    virtual uint32_t read(uint8_t* buff, uint32_t len) override;
    virtual uint32_t write(uint8_t* buff, uint32_t len) override;

    virtual uint32_t readAll(char* buff) override;

private:
    bool bReadyReadConnected = true;
    QSerialPort* pSerialPort = nullptr;
};
}
#endif
