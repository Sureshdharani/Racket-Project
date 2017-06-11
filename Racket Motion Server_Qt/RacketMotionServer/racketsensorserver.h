#ifndef RACKETSENSORSERVER_H
#define RACKETSENSORSERVER_H

#include <QUdpSocket>
#include <QNetworkDatagram>

class RacketSensorServer : public QObject
{
    Q_OBJECT

public:  // functions
    explicit RacketSensorServer(QObject *parent = 0);

public:  // variables
    quint16 port;
    QHostAddress ipv4Addr;

public slots:
    void setListenIPPort(const QHostAddress IPv4Addr, const quint16 Port);
    void readPendingDatagrams();

signals:
    void sendState(const QString errorDescr = "No Error");

private:  // functions

private:  // variables

    QUdpSocket* _socket;
};

#endif // RACKETSENSORCLIENT_H
