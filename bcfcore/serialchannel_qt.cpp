#include "SerialChannel_qt.h"

#ifdef BCF_USE_QT_SERIALPORT
#include <QSerialPort>
#include <QDebug>
#include <QTimer>
using namespace bcf;

SerialChannel_QT::SerialChannel_QT(const std::string& name, QObject* parent):
    QObject(parent)
{
    m_pSerialPort = new QSerialPort(this);
    setPortName(name);
    setBaudRate(QSerialPort::BaudRate::Baud115200);
    setDataBits(QSerialPort::DataBits::Data8);
    setParity(QSerialPort::Parity::NoParity);
    setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    setStopBits(QSerialPort::StopBits::OneStop);
    connect(m_pSerialPort, &QSerialPort::errorOccurred, this, &SerialChannel_QT::onErrorOccurred);
    usePassiveModel();
}

SerialChannel_QT::~SerialChannel_QT()
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
    }
    m_pSerialPort->deleteLater();
}

void SerialChannel_QT::setPortName(const std::string& name)
{
    m_pSerialPort->setPortName(QString::fromStdString(name));
}

void SerialChannel_QT::setBaudRate(int baudRate)
{
    m_pSerialPort->setBaudRate((QSerialPort::BaudRate)baudRate);
}

void SerialChannel_QT::setDataBits(int dataBits)
{
    m_pSerialPort->setDataBits((QSerialPort::DataBits)dataBits);
}

void SerialChannel_QT::setParity(int parity)
{
    m_pSerialPort->setParity((QSerialPort::Parity)parity);
}

void SerialChannel_QT::setStopBits(int stopBits)
{
    m_pSerialPort->setStopBits((QSerialPort::StopBits)stopBits);
}

void SerialChannel_QT::setFlowControl(int flowControl)
{
    m_pSerialPort->setFlowControl((QSerialPort::FlowControl)flowControl);
}

bool SerialChannel_QT::openInternal()
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

bool SerialChannel_QT::closeInternal()
{
    qDebug() << "Close:" << m_pSerialPort->portName();
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
    }
    return true;
}

int64_t SerialChannel_QT::send(const char* data, uint32_t len)
{
    return m_pSerialPort->write((char*)data, len);
}

bool SerialChannel_QT::isOpen()
{
    if (!m_pSerialPort) {
        return false;
    }

    return m_pSerialPort->isOpen();
}

uint32_t SerialChannel_QT::read(uint8_t* buff, uint32_t len)
{
    return m_pSerialPort->read((char*)buff, len);
}

uint32_t SerialChannel_QT::write(uint8_t* buff, uint32_t len)
{
    return m_pSerialPort->write((char*)buff, len);
}

void SerialChannel_QT::useActiveModel()
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->clear();
    }
    disconnect(m_pSerialPort, &QSerialPort::readyRead, this, &SerialChannel_QT::onReceivedData);
}

void SerialChannel_QT::usePassiveModel()
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->clear();
    }
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &SerialChannel_QT::onReceivedData);
}

ByteBufferPtr SerialChannel_QT::readAll()
{
    QByteArray res = m_pSerialPort->readAll();
    ByteBufferPtr ptr = std::make_shared<bb::ByteBuffer>();
    ptr->putBytes((uint8_t*)res.data(), res.length());
    return ptr;
}

void bcf::SerialChannel_QT::setMaxRecvBufferSize(int maxRecvBufferSize)
{
    m_pSerialPort->setReadBufferSize(maxRecvBufferSize);
}

void SerialChannel_QT::onReceivedData()
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

void SerialChannel_QT::onErrorOccurred(int error)
{
    if ((QSerialPort::SerialPortError)error == QSerialPort::SerialPortError::NoError) {
        return;
    }

    if (m_errorCallback) {
        m_errorCallback(m_pSerialPort->errorString().toStdString());
    }
}
#endif
