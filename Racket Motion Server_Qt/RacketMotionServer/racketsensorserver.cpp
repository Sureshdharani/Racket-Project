#include "racketsensorserver.h"

//-----------------------------------------------------------------------------
RacketSensorServer::RacketSensorServer(QObject *parent,
                                       const quint16 Port) :
    QObject(parent)
{
    _socket = new QUdpSocket(this);
    port = Port;
    _socket->bind(port);

    connect(_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

//-----------------------------------------------------------------------------
void RacketSensorServer::setListenIPPort(const quint16 Port) {
    _socket->abort();
    port = Port;
    bool isConnected = _socket->bind(port);

    if (!isConnected)
    {
        emit(sendState("Can't lsiten on port "
                       + QString::number(port)));
    }
    else
    {
        emit(sendState("Listen on port "
                       + QString::number(port)));
    }
}

//-----------------------------------------------------------------------------
void RacketSensorServer::readPendingDatagrams()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    // Packet format:
    // "90594.75079, 3,  -0.059,  0.098,  9.826, 4,  -0.000,  0.001,  0.001, 5,  16.191, 12.642,-34.497"
    // "92309.44280, 3,  -0.090,  0.013,  9.746, 4,   0.001,  0.001, -0.001, 5,  17.903, 10.240,-33.698"
    // "92309.37615, 3,  -0.043,  0.033,  9.722, 4,  -0.001, -0.001, -0.001"
    // "92272.20973, 3,  -1.355, -0.098, 10.935"

    _socket->readDatagram(buffer.data(), buffer.size(),
                          &sender, &senderPort);

    QString data(buffer);
    // data.replace(" ", "");

    // qDebug() << "Message from: " << sender.toString();
    // qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << data;
    qDebug() << "--------------------------------";

    // int k = 0;
}
