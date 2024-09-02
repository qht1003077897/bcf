#ifdef BCF_USE_QT_TCP
#include <QThread>
#include <QTcpSocket>
#include "tcpclientchannel_qt.h"
#include "base/exception.hpp"

using namespace bcf;

TCPClientChannel_QT::TCPClientChannel_QT(const QString& ip, int port, QObject* parent):
    QObject(parent)
{
    m_pTcpClient = new QTcpSocket(this);
    setAddr(ip, port);
    connect(m_pTcpClient, &QTcpSocket::connected, this, &TCPClientChannel_QT::onConnected);
    connect(m_pTcpClient, &QTcpSocket::errorOccurred, this, &TCPClientChannel_QT::onErrorOccurred);
    usePassiveModel();
}

TCPClientChannel_QT::~TCPClientChannel_QT()
{
    if (m_pTcpClient->isOpen()) {
        m_pTcpClient->close();
    }
}

void TCPClientChannel_QT::setAddr(const QString& ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void TCPClientChannel_QT::onConnected()
{
    if (m_CompleteCallback) {
        m_CompleteCallback(getSharedFromThis());
    }
}

void TCPClientChannel_QT::openInternal()
{
    m_pTcpClient->connectToHost(m_ip, m_port);
}

bool TCPClientChannel_QT::closeInternal()
{
    if (m_pTcpClient->isOpen()) {
        m_pTcpClient->close();
    }
    return true;
}

int64_t TCPClientChannel_QT::send(const char* data, uint32_t len)
{
    QThread* thread = QThread::currentThread();
    if (!thread) {
        throw BcfCommonException("TCPClientChannel_QT::send can only be used with QThread");
        return -2;
    }

    return m_pTcpClient->write((char*)data, len);
}

bool TCPClientChannel_QT::isOpen()
{
    if (!m_pTcpClient) {
        return false;
    }

    return m_pTcpClient->isOpen();
}

uint32_t TCPClientChannel_QT::read(uint8_t* buff, uint32_t len)
{
    return m_pTcpClient->read((char*)buff, len);
}

uint32_t TCPClientChannel_QT::write(uint8_t* buff, uint32_t len)
{
    return m_pTcpClient->write((char*)buff, len);
}

void TCPClientChannel_QT::useActiveModel()
{
    disconnect(m_pTcpClient, &QTcpSocket::readyRead, this, &TCPClientChannel_QT::onReceivedData);
}

void TCPClientChannel_QT::usePassiveModel()
{
    connect(m_pTcpClient, &QTcpSocket::readyRead, this, &TCPClientChannel_QT::onReceivedData);
}

ByteBufferPtr TCPClientChannel_QT::readAll()
{
    QByteArray res = m_pTcpClient->readAll();
    ByteBufferPtr ptr = std::make_shared<bb::ByteBuffer>();
    ptr->putBytes((uint8_t*)res.data(), res.length());
    return ptr;
}

void bcf::TCPClientChannel_QT::setMaxRecvBufferSize(int maxRecvBufferSize)
{
    m_pTcpClient->setReadBufferSize(maxRecvBufferSize);
}

void TCPClientChannel_QT::onReceivedData()
{
    auto res = m_pTcpClient->readAll();
    ByteBufferPtr ptr = std::make_shared<bb::ByteBuffer>();
    ptr->putBytes((uint8_t*)res.data(), res.length());
    if (m_rawdataCallback) {
        m_rawdataCallback(std::move(ptr));
        return;
    }

    pushData2Bcf(std::move(ptr));
}

void TCPClientChannel_QT::onErrorOccurred(int error)
{
    if (m_FailCallback) {
        m_FailCallback();
    }

    if (m_errorCallback) {
        m_errorCallback(m_pTcpClient->errorString().toStdString());
    }
}
#endif
