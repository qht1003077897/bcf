#ifdef USE_BCF_SERIALPORT

#include <QDebug>
#include "serialchannel.h"
using namespace bcf;

SerialChannel::SerialChannel(QObject* parent):
    QObject(parent)
{
    pSerialPort = new QSerialPort(this);
    connect(pSerialPort, &QSerialPort::errorOccurred, this, &SerialChannel::onErrorOccurred);
    connect(pSerialPort, &QSerialPort::readyRead, this, &SerialChannel::onReceivedData);
}


SerialChannel::~SerialChannel()
{
    if (pSerialPort->isOpen()) {
        pSerialPort->close();
    }
    delete pSerialPort;
    pSerialPort = nullptr;
}

bcf::ChannelID SerialChannel::channelID()
{
    return bcf::ChannelID::Serial;
}

bool SerialChannel::openChannel()
{
    bool res =  pSerialPort->open(QSerialPort::ReadWrite);
    qDebug() << "Open:" << pSerialPort->portName() << ":" << res;
    return res;
}

bool SerialChannel::closeChannel()
{
    qDebug() << "Close:" << pSerialPort->portName();
    if (pSerialPort->isOpen()) {
        pSerialPort->close();
    }
    return true;
}

int64_t SerialChannel::send(const unsigned char* data, uint32_t len)
{
    return pSerialPort->write((char*)data, len);
}

bool SerialChannel::isOpen()
{
    if (!pSerialPort) {
        return false;
    }

    return pSerialPort->isOpen();
}

uint32_t SerialChannel::read(uint8_t* buff, uint32_t len)
{
    return pSerialPort->read((char*)buff, len);
}

uint32_t SerialChannel::write(uint8_t* buff, uint32_t len)
{
    return pSerialPort->write((char*)buff, len);
}

uint32_t SerialChannel::readAll(char* buff)
{
    QByteArray res = pSerialPort->readAll();
    strcpy_s(buff, res.length(), res.data());
    return res.length();
}

void SerialChannel::onReceivedData()
{
    auto data = pSerialPort->readAll();
    qDebug() << "onReceivedData:" << data;

    pushData2Bcf(data.toStdString());
}

void SerialChannel::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::SerialPortError::NoError) {
        return;
    }

    if (m_errorCallback) {
        m_errorCallback(pSerialPort->errorString().toStdString());
    }
}

#endif
