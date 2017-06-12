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

    QString toString()
    {
        QString str;
        const char prec = 'f';
        const int numDigit = 6;
        str.append("{");
        str.append("timeStamp: " + QString::number(timeStamp, prec, numDigit) + "; ");

        str.append("acc: ");
        str.append("X: " + QString::number(accX, prec, numDigit) + ", ");
        str.append("Y: " + QString::number(accY, prec, numDigit) + ", ");
        str.append("Z: " + QString::number(accZ, prec, numDigit) + "; ");

        str.append("gyro: ");
        str.append("X: " + QString::number(gyroX, prec, numDigit) + ", ");
        str.append("Y: " + QString::number(gyroY, prec, numDigit) + ", ");
        str.append("Z: " + QString::number(gyroZ, prec, numDigit) + "; ");

        str.append("mag: ");
        str.append("X: " + QString::number(magX, prec, numDigit) + ", ");
        str.append("Y: " + QString::number(magY, prec, numDigit) + ", ");
        str.append("Z: " + QString::number(magZ, prec, numDigit));
        str.append("}");

        return str;
    }
};

class RacketSensorServer : public QObject
{
    Q_OBJECT

public:  // functions
    explicit RacketSensorServer(QObject *parent = 0,
                                const quint16 Port = 5554);
    SensData processInputPacket(const QString packet,
                                const SensData prevSensData);

public:  // variables
    quint16 port;

public slots:
    void setListenIPPort(const quint16 Port);
    void setListenIPPort(const QString Port) { setListenIPPort(Port.toInt()); };
    void readPendingDatagrams();

signals:
    void sendState(const QString errorDescr);
    void sendSensData(const SensData sensData);

private:  // functions

private:  // variables

    QUdpSocket* _socket;
    SensData _sensData;
    SensData _sensDataPrev;
};

#endif // RACKETSENSORCLIENT_H
