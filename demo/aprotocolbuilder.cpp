#include "aprotocolbuilder.h"

bcf::ProtocolType AProtocolBuilder::getType() const
{
    return bcf::ProtocolType::A;
}

uint32_t AProtocolBuilder::build(std::shared_ptr<bcf::AbstractProtocolModel> _model,
                                 unsigned char* data)
{
    std::shared_ptr<bcf::AProtocolModel> model = std::dynamic_pointer_cast<bcf::AProtocolModel>(_model);
//    QString buffer;
//    buffer.append(QString("%1").arg(mcuModel.SYNCMODEL, 2, 16, QLatin1Char('0')));
//    QByteArray seq;
//    seq.append(QString("%1").arg(mcuModel.SYNC_SEQ, 4, 16, QLatin1Char('0')));
//    std::reverse(seq.begin(), seq.end());
//    buffer.append(seq.toHex());

//    if (mcuModel.hasSEP) {
//        buffer.append(QString("%1").arg(PROTOCOL_TOKEN_SEP, 2, 16, QLatin1Char('0')));
//        QByteArray subSeq;
//        subSeq.append(QString("%1").arg(mcuModel.SYNC_SUB_PACKET_SEQ, 4, 16, QLatin1Char('0')));
//        std::reverse(subSeq.begin(), subSeq.end());
//        buffer.append(subSeq.toHex());
//    }

//    buffer.append(QString("%1").arg(PROTOCOL_TOKEN_SOI, 2, 16, QLatin1Char('0')));
//    for (int i = 0; i < mcuModel.INSTRN.length(); ++i) {
//        char ch = mcuModel.INSTRN.at(i);
//        buffer.append(QString::number(ch, 16));
//    }
//    if (mcuModel.hasDATA) {
//        buffer.append(QString("%1").arg(PROTOCOL_TOKEN_SOD, 2, 16, QLatin1Char('0')));
//        for (int i = 0; i < mcuModel.DATA.length(); ++i) {
//            char ch = mcuModel.DATA.at(i);
//            buffer.append(QString::number(ch, 16));
//        }
//    }
//    buffer.append(QString("%1").arg(PROTOCOL_TOKEN_SOV, 2, 16, QLatin1Char('0')));
//    QByteArray hex = QByteArray::fromHex(buffer.toUtf8());
//    uint16_t crc16 = CrcUtil::calculateQStringCRC16(hex, hex.size());
//    QByteArray crcArray = QByteArray::number(crc16, 16);
//    std::reverse(crcArray.begin(), crcArray.end());
//    buffer.append(crcArray.toHex());
//    buffer.append(QString("%1").arg(PROTOCOL_TOKEN_EOT, 2, 16, QLatin1Char('0')));
//    logdebug("QString sendData:{}", buffer.toStdString());
//    QByteArray resArray = QByteArray::fromHex(buffer.toUtf8());
//    qDebug() << "QByteArray Hex sendData:" << resArray;
//    return resArray;
    return 1;
}
