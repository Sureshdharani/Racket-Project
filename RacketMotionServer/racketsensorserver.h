#ifndef RACKETSENSORSERVER_H
#define RACKETSENSORSERVER_H

#include <QUdpSocket>
#include <deque>
#include <fitfunctions.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFF_SIZE 1000  // sensor packets buffer size
#define SAMPLE_PLOT 100  // eacht n-th sample to plot


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

// Represents 4D vector
struct Vec4D {
    float w = 0;
    float x = 0;
    float y = 0;
    float z = 0;

    QString toString(const char prec = 'f', const int numDigit = 6) {
        QString str = "";
        str.append("[W: " + QString::number(w, prec, numDigit) + "; ");
        str.append("X: " + QString::number(x, prec, numDigit) + "; ");
        str.append("Y: " + QString::number(y, prec, numDigit) + "; ");
        str.append("Z: " + QString::number(z, prec, numDigit) + "]");
        return str;
    }
};

// Represents sensor data packet
struct SensPacket {
    double t = 0;  // time stamp

    Vec3D acc;  // acceleration
    Vec3D gyro; // gyroscope
    Vec3D ang;  // orientation angle
    Vec4D quat;  // quaternion

    QString toString(const char prec = 'f', const int numDigit = 6)
    {
        QString str;
        str.append("{");
        str.append("timeStamp: " + QString::number(t, prec, numDigit) + "; ");
        str.append("acc: " + acc.toString() + "; ");
        str.append("gyro: " + gyro.toString() + "; ");
        str.append("ang: " + ang.toString());
        str.append("quaternion: " + quat.toString());
        str.append("}");
        return str;
    }
};

// Sensor data FIFO:
typedef std::deque<SensPacket> SensBuffer;     // raw sensor data buffer

class RacketSensorServer : public QObject
{
    Q_OBJECT

public:  // functions
    explicit RacketSensorServer(QObject *parent = 0,
                                const quint16 Port = 5554);

    // Processes packet from mobile app:
    // Packet format:
    // "90594.75079, 3,  -0.059,  0.098,  9.826, 4,  -0.000,  0.001,  0.001, 5,  16.191, 12.642,-34.497"
    // "92309.37615, 3,  -0.043,  0.033,  9.722, 4,  -0.001, -0.001, -0.001"
    // "92272.20973, 3,  -1.355, -0.098, 10.935"
    SensPacket processInPacketMobile(const QString packet,
                                      const SensPacket prevPacket);

    // Processes packet from edisson:
    // Packet fromat:
    // "s@TimeStamp@accX@accY@accZ@gyroX@gyroY@gyroZ@w@x@y@z@;"
    SensPacket processInPacketEdisson(const QString data, const bool isFirstPacket = false);

public:  // variables
    bool isEdisson;
    quint16 port;
    unsigned int fitWinLen;  // fit window length

public slots:
    void setListenIPPort(const quint16 Port);
    void setListenIPPort(const QString Port) { setListenIPPort(Port.toInt()); };
    void readPendingDatagrams();

signals:
    void sendState(const QString errorDescr);
    void sendSensData(const SensBuffer sensData, const SensBuffer fitData);

private:  // private functions
    // Fits sensor data:
    SensBuffer _fit(const SensBuffer fitData);
    static void _quat2euler(const float q_w, const float q_x,
                            const float q_y, const float q_z,
                            float *t_x, float *t_y, float *t_z);
    void _appendToBuffer(SensBuffer *sensData, const QString data);

private:  // private variables
    bool _isFirstPacket;  // first packet flag
    double _startTime;
    QUdpSocket* _socket;
    SensBuffer _sensData;    // raw sensor data
    SensBuffer _fitData;  // fitted sensor data
    unsigned int _plotCnt;  // plotting counter
    unsigned int _fitSampleCnt;  // fit sample counter
};

#endif // RACKETSENSORCLIENT_H
