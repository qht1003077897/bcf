#ifdef BCF_USE_SERIALPORT
#pragma once
#include <QObject>
#include <ichannel.h>
#include <bcfexport.h>
#include <globaldefine.h>

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
    virtual bool openInternal() override;
    virtual bool closeInternal() override;
    virtual bool isOpen() override;
    virtual int64_t send(const unsigned char* data, uint32_t len) override;
    virtual uint32_t read(uint8_t* buff, uint32_t len) override;
    virtual uint32_t write(uint8_t* buff, uint32_t len) override;

    virtual QByteArray readAll() override;
private:
    QSerialPort* m_pSerialPort = nullptr;
};
}       // namespace bcf
#endif  //BCF_USE_SERIALPORT
