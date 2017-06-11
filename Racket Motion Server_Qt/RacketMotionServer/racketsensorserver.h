#ifndef RACKETSENSORSERVER_H
#define RACKETSENSORSERVER_H

#include <QUdpSocket>

struct SensData {
    double timeStamp = 0;

    double accX = 0;
    double accY = 0;
    double accZ = 0;

    double gyroX = 0;
    double gyroY = 0;
    double gyroZ = 0;

    double magX = 0;
    double magY = 0;
    double magZ = 0;
};

class RacketSensorServer : public QObject
{
    Q_OBJECT

public:  // functions
    explicit RacketSensorServer(QObject *parent = 0,
                                const quint16 Port = 5555);

public:  // variables
    quint16 port;

public slots:
    void setListenIPPort(const quint16 Port);
    void readPendingDatagrams();

signals:
    void sendState(const QString errorDescr);

private:  // functions

private:  // variables

    QUdpSocket* _socket;
    SensData _sensData;
    SensData _sensDataPrev;
};

#endif // RACKETSENSORCLIENT_H
