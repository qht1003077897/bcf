#include "serialchannel_qt.h"

#ifdef BCF_USE_QT_SERIALPORT
#include <QSerialPort>
#include <QDebug>
#include <QTimer>
using namespace bcf;

SerialChannel::SerialChannel(QObject* parent):
    QObject(parent)
{
    m_pSerialPort = new QSerialPort(this);
    setBaudRate(QSerialPort::BaudRate::Baud115200);
    setDataBits(QSerialPort::DataBits::Data8);
    setParity(QSerialPort::Parity::NoParity);
    setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    setStopBits(QSerialPort::StopBits::OneStop);
    connect(m_pSerialPort, &QSerialPort::errorOccurred, this, &SerialChannel::onErrorOccurred);
    usePassiveModel();
}

SerialChannel::~SerialChannel()
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
    }
    m_pSerialPort->deleteLater();
}

void SerialChannel::setPortName(const std::string& name)
{
    m_pSerialPort->setPortName(QString::fromStdString(name));
}

void SerialChannel::setBaudRate(int baudRate)
{
    m_pSerialPort->setBaudRate((QSerialPort::BaudRate)baudRate);
}

void SerialChannel::setDataBits(int dataBits)
{
    m_pSerialPort->setDataBits((QSerialPort::DataBits)dataBits);
}

void SerialChannel::setParity(int parity)
{
    m_pSerialPort->setParity((QSerialPort::Parity)parity);
}

void SerialChannel::setStopBits(int stopBits)
{
    m_pSerialPort->setStopBits((QSerialPort::StopBits)stopBits);
}

void SerialChannel::setFlowControl(int flowControl)
{
    m_pSerialPort->setFlowControl((QSerialPort::FlowControl)flowControl);
}

bool SerialChannel::openInternal()
{
    bool res =  m_pSerialPort->open(QSerialPort::ReadWrite);
    qDebug() << "Open:" << m_pSerialPort->portName() << ":" << res;
    if (!res) {
        if (m_FailCallback) {
            m_FailCallback();
        }
    } else {
        if (m_CompleteCallback) {
            m_CompleteCallback(getSharedFromThis());
        }
    }
    return res;
}

bool SerialChannel::closeInternal()
{
    qDebug() << "Close:" << m_pSerialPort->portName();
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
    }
    return true;
}

int64_t SerialChannel::send(const char* data, uint32_t len)
{
    return m_pSerialPort->write((char*)data, len);
}

bool SerialChannel::isOpen()
{
    if (!m_pSerialPort) {
        return false;
    }

    return m_pSerialPort->isOpen();
}

uint32_t SerialChannel::read(uint8_t* buff, uint32_t len)
{
    return m_pSerialPort->read((char*)buff, len);
}

uint32_t SerialChannel::write(uint8_t* buff, uint32_t len)
{
    return m_pSerialPort->write((char*)buff, len);
}

void SerialChannel::useActiveModel()
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->clear();
    }
    disconnect(m_pSerialPort, &QSerialPort::readyRead, this, &SerialChannel::onReceivedData);
}

void SerialChannel::usePassiveModel()
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->clear();
    }
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &SerialChannel::onReceivedData);
}

ByteBufferPtr SerialChannel::readAll()
{
    QByteArray res = m_pSerialPort->readAll();
    ByteBufferPtr ptr = std::make_shared<bb::ByteBuffer>();
    ptr->putBytes((uint8_t*)res.data(), res.length());
    return ptr;
}

void bcf::SerialChannel::setMaxRecvBufferSize(int maxRecvBufferSize)
{
    m_pSerialPort->setReadBufferSize(maxRecvBufferSize);
}

void SerialChannel::onReceivedData()
{
    auto res = m_pSerialPort->readAll();
    qDebug() << "Received data:" << res;
    ByteBufferPtr ptr = std::make_shared<bb::ByteBuffer>();
    ptr->putBytes((uint8_t*)res.data(), res.length());
    if (m_rawdataCallback) {
        m_rawdataCallback(std::move(ptr));
        return;
    }

    pushData2Bcf(std::move(ptr));
}

void SerialChannel::onErrorOccurred(int error)
{
    if ((QSerialPort::SerialPortError)error == QSerialPort::SerialPortError::NoError) {
        return;
    }

    if (m_errorCallback) {
        m_errorCallback(m_pSerialPort->errorString().toStdString());
    }
}
#endif
