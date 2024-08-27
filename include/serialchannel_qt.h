#pragma once
#include <QObject>
#include "ichannel.h"
#include "bcfexport.h"

class QSerialPort;
namespace bcf
{
class BCF_EXPORT SerialChannel_QT : public QObject, public IChannel
{
    Q_OBJECT

public:
    explicit SerialChannel_QT(const QString& name, QObject* parent = nullptr);
    virtual ~SerialChannel_QT();

    void setPortName(const QString& name);

    void setBaudRate(int baudRate);

    void setDataBits(int dataBits);

    void setParity(int parity);

    void setStopBits(int stopBits);

    void setFlowControl(int flowControl);

private slots:
    void onReceivedData();
    void onErrorOccurred(int error);

protected:
    void openInternal() override;
    bool closeInternal() override;
    bool isOpen() override;
    int64_t send(const char* data, uint32_t len) override;
    uint32_t read(uint8_t* buff, uint32_t len) override;
    uint32_t write(uint8_t* buff, uint32_t len) override;
    void useActiveModel() override;
    void usePassiveModel() override;
    ByteBufferPtr readAll() override;
    void setMaxRecvBufferSize(int maxRecvBufferSize) override;
private:
    QSerialPort* m_pSerialPort = nullptr;
};
}// namespace bcf
