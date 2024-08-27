#include <QCoreApplication>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include "bcf.h"
using namespace std;

/**
*  @file    : simpletcpserver.cpp
*  @brief   : 使用 @class ByHeadProtocolModel 协议即指定头部长度协议演示一个简单的bcf tcp服务端使用方法
*/

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer(QObject* parent = nullptr) : QTcpServer(parent) {}
protected:
    void incomingConnection(qintptr socketDescriptor) override
    {
        QTcpSocket* clientSocket = new QTcpSocket;
        if (clientSocket->setSocketDescriptor(socketDescriptor)) {
            connect(clientSocket, &QTcpSocket::readyRead, this, &MyTcpServer::readClient);
            connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
        } else {
            delete clientSocket;
        }
    }

private slots:
    void readClient()
    {
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        if (clientSocket) {
            QByteArray res = clientSocket->readAll();
            bcf::ByteBufferPtr ptr = std::make_shared<bb::ByteBuffer>((uint8_t*)res.data(), res.length());

            bcf::ByHeadProtocolParser parser;
            if (parser.sniff(ptr)) {
                parser.parse([&clientSocket](bcf::ParserState state,
                std::shared_ptr<bcf::AbstractProtocolModel> model) {
                    auto bmodel = std::dynamic_pointer_cast<bcf::ByHeadProtocolModel>(model);
                    if (bmodel) {
                        qDebug() << "recv:" << QString::fromStdString(bmodel->body());
                        bcf::ByHeadProtocolBuilder builder;
                        auto amodel = std::make_shared<bcf::ByHeadProtocolModel>();
                        amodel->seq = bmodel->seq;
                        amodel->cmd = bmodel->cmd;//作为服务端,使用和客户端一样的cmd进行回应
                        amodel->setBody("my name is bcf!");
                        qDebug() << "reply:" << QString::fromStdString(amodel->body());
                        auto ptr = builder.build(amodel);
                        clientSocket->write((const char* )ptr->data(), ptr->size());
                    }
                });
            }
        }
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    MyTcpServer server;
    if (!server.listen(QHostAddress::Any, 1234)) {
        qCritical() << "Unable to start the server:" << server.errorString();
        return app.exec();
    }
    qDebug() << "Server started on port" << server.serverPort();
    return app.exec();
}

#include "simpletcpserver.moc"
