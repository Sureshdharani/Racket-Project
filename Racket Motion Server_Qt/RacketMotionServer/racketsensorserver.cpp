#include "racketsensorserver.h"

//-----------------------------------------------------------------------------
RacketSensorServer::RacketSensorServer(QObject *parent) :
    QObject(parent)
{
    _socket = new QUdpSocket(this);
    port = 5555;
    ipv4Addr = QHostAddress("127.0.0.1");  // QHostAddress::LocalHost;
    _socket->bind(ipv4Addr, port);

    connect(_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

//-----------------------------------------------------------------------------
void RacketSensorServer::setListenIPPort(const QHostAddress IPv4Addr,
                                         const quint16 Port) {
    _socket->abort();
    port = Port;
    ipv4Addr = IPv4Addr;
    bool isConnected = _socket->bind(ipv4Addr, port);

    if (!isConnected)
    {
        sendState("Can't lsiten from adress "
                  + ipv4Addr.toString()
                  + " on port "
                  + QString::number(port));
    }
    else
    {
        sendState("Listen from adress "
                  + ipv4Addr.toString()
                  + " on port "
                  + QString::number(port));
    }
}

//-----------------------------------------------------------------------------
void RacketSensorServer::readPendingDatagrams()
{
    while (_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = _socket->receiveDatagram();
        sendState("new data");
        // processTheDatagram(datagram);
    }
}
