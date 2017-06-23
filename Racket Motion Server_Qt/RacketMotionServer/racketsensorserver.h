#ifndef RACKETSENSORSERVER_H
#define RACKETSENSORSERVER_H

#include <QUdpSocket>
#include <deque>

#define NUM_PACKETS 10000  // number of packets from sensro to safe

// Represents 3D vector
struct Vec3D {
    float x = 0;
    float y = 0;
    float z = 0;

    QString toString(const char prec = 'f', const int numDigit = 6) {
        QString str = "";
        str.append("[X: " + QString::number(x, prec, numDigit) + "; ");
        str.append("Y: " + QString::number(y, prec, numDigit) + "; ");
        str.append("Z: " + QString::number(z, prec, numDigit) + "]");
        return str;
    }
};

// Represents sensor data packet
struct SensDataPacket {
    double timeStamp = 0;

    Vec3D acc;
    Vec3D gyro;
    Vec3D mag;

    QString toString(const char prec = 'f', const int numDigit = 6)
    {
        QString str;
        str.append("{");
        str.append("timeStamp: " + QString::number(timeStamp, prec, numDigit) + "; ");
        str.append("acc: " + acc.toString() + "; ");
        str.append("gyro: " + gyro.toString() + "; ");
        str.append("mag: " + mag.toString());
        str.append("}");
        return str;
    }
};

// Sensor data FIFO:
typedef std::deque<SensDataPacket> SensData;     // raw sensor data
typedef std::deque<SensDataPacket> FitSensData;  // fitted sensor data

class RacketSensorServer : public QObject
{
    Q_OBJECT

public:  // functions
    explicit RacketSensorServer(QObject *parent = 0,
                                const quint16 Port = 5554);
    SensDataPacket processInputPacket(const QString packet,
                                      const SensDataPacket prevSensDataPacket);

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
    /* Fits the sensor data and returns new filled array
     *  Input:
     *      data - sensor data
     *      N - window size to fit
     *  Return:
     *      Fitted sensor data
     */
    FitSensData _fitSensData(const SensData data, const unsigned int N = 100);

private:  // variables

    QUdpSocket* _socket;
    SensData _sensData;    // raw sensor data
    FitSensData _fitData;  // fitted sensor data
};

#endif // RACKETSENSORCLIENT_H
