#pragma once
#include <config.h>

#ifdef BCF_USE_QT_TCP
#include <QObject>
#include <ichannel.h>
#include <bcfexport.h>
#include <base/globaldefine.h>

class QTcpSocket;
namespace bcf
{
class BCF_EXPORT TCPClientChannel_QT : public QObject, public IChannel
{
    Q_OBJECT

public:
    explicit TCPClientChannel_QT(const std::string& ip, int port, QObject* parent = nullptr);
    virtual ~TCPClientChannel_QT();

    void setAddr(const std::string& ip, int port);

private slots:
    void onConnected();
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
    QTcpSocket* m_pTcpClient = nullptr;
    std::string m_ip;
    int m_port;
};
}// namespace bcf
#endif //BCF_USE_QT_TCP
