#include "racketsensorserver.h"

//-----------------------------------------------------------------------------
RacketSensorServer::RacketSensorServer(QObject *parent,
                                       const quint16 Port) :
    QObject(parent)
{
    _socket = new QUdpSocket(this);
    port = Port;
    _socket->bind(port);

    _sensData = SensData(NUM_PACKETS);
    _fitData = SensData();

    fitWinLen = 100;
    isEdisson = true;

    connect(_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

//-----------------------------------------------------------------------------
void RacketSensorServer::setListenIPPort(const quint16 Port) {
    _socket->abort();
    port = Port;
    bool isConnected = _socket->bind(port);
    if (!isConnected)
        emit(sendState("Can't lsiten on port " + QString::number(port)));
}

//-----------------------------------------------------------------------------
void RacketSensorServer::readPendingDatagrams()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    _socket->readDatagram(buffer.data(), buffer.size(),
                          &sender, &senderPort);

    QString data(buffer);

    // Append new element
    if (!isEdisson) {  // process packets from app
        SensDataPacket sensDataPacketPrev = _sensData.empty() ? SensDataPacket() : _sensData.back();
        _sensData.push_back(processInputPacket(data, sensDataPacketPrev));
    } else {  // process packet from edison
        _sensData.push_back(processInPacket(data));
    }

    if (true) {  // Fit sensor data (do it only if the buffer accumulated enough data):
        _fitData = _fitSensData(_sensData, fitWinLen);
        emit(sendSensData(_sensData, _fitData));
    }
}

//-----------------------------------------------------------------------------
SensData RacketSensorServer::_fitSensData(const SensData data,
                                             const unsigned int N)
{
    if (data.size() < N)
        return data;

    // Create containers for fit:
    SensData fitted = SensData(N);

    std::vector<double> time(N);
    std::vector<double> accX(N);
    std::vector<double> accY(N);
    std::vector<double> accZ(N);

    std::vector<double> gyroX(N);
    std::vector<double> gyroY(N);
    std::vector<double> gyroZ(N);

    std::vector<double> thetaX(N);
    std::vector<double> thetaY(N);
    std::vector<double> thetaZ(N);

    // Cut last N data points from data:
    unsigned int j = 0;
    for(unsigned int i = data.size()-1; i > data.size()-1-N; i--) {
        fitted.at(j) = data.at(i);

        time.at(j) = data.at(i).timeStamp;

        accX.at(j) = data.at(i).acc.x;
        accY.at(j) = data.at(i).acc.y;
        accZ.at(j) = data.at(i).acc.z;

        gyroX.at(j) = data.at(i).gyro.x;
        gyroY.at(j) = data.at(i).gyro.y;
        gyroZ.at(j) = data.at(i).gyro.z;

        thetaX.at(j) = data.at(i).theta.x;
        thetaY.at(j) = data.at(i).theta.y;
        thetaZ.at(j) = data.at(i).theta.z;
        j++;
    }

    // Fit the data:
    accX = MathFit::fitNormal(time, accX);
    accY = MathFit::fitNormal(time, accY);
    accZ = MathFit::fitNormal(time, accZ);

    gyroX = MathFit::fitNormal(time, gyroX);
    gyroY = MathFit::fitNormal(time, gyroY);
    gyroZ = MathFit::fitNormal(time, gyroZ);

    // Pack fitted data to fitted array:
    for(unsigned int i = 0; i < accX.size(); i++) {
        fitted.at(i).acc.x = accX.at(i);
        fitted.at(i).acc.y = accY.at(i);
        fitted.at(i).acc.z = accZ.at(i);

        fitted.at(i).gyro.x = gyroX.at(i);
        fitted.at(i).gyro.y = gyroY.at(i);
        fitted.at(i).gyro.z = gyroZ.at(i);
    }

    return fitted;
}

//-----------------------------------------------------------------------------
SensDataPacket RacketSensorServer::processInputPacket(const QString packet,
                                         const SensDataPacket prevSensDataPacket)
{
    SensDataPacket sensDataPacket = prevSensDataPacket;
    const QString del3 = ", 3,  ";
    const QString del4 = ", 4,  ";
    const QString del5 = ", 5,  ";
    const QString del = ",";

    if (packet.isEmpty()) return sensDataPacket;

    QStringList list(packet);

    QStringList acc;
    QStringList gyro;
    QStringList theta;

    list = list.at(0).split(del3, QString::SkipEmptyParts);
    sensDataPacket.timeStamp = list.at(0).toDouble();
    if (list.size() == 1) return sensDataPacket;
    list = QStringList(list.back());

    list = list.at(0).split(del4, QString::SkipEmptyParts);
    acc = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensDataPacket.acc.x = acc.at(0).toDouble();
    sensDataPacket.acc.y = acc.at(1).toDouble();
    sensDataPacket.acc.z = acc.at(2).toDouble();
    if (list.size() == 1) return sensDataPacket;
    list = QStringList(list.back());

    list = list.at(0).split(del5, QString::SkipEmptyParts);
    gyro = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensDataPacket.gyro.x = gyro.at(0).toDouble();
    sensDataPacket.gyro.y = gyro.at(1).toDouble();
    sensDataPacket.gyro.z = gyro.at(2).toDouble();
    if (list.size() == 1) return sensDataPacket;
    list = QStringList(list.back());

    theta = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensDataPacket.theta.x = theta.at(0).toDouble();
    sensDataPacket.theta.y = theta.at(1).toDouble();
    sensDataPacket.theta.z = theta.at(2).toDouble();

    return sensDataPacket;
}

//-----------------------------------------------------------------------------
SensDataPacket RacketSensorServer::processInPacket(const QString data)
{
    // Packet fromat:
    // "s@TimeStamp@accX@accY@accZ@gyroX@gyroY@gyroZ@w@x@y@z@;"

    SensDataPacket p;

    std::string packet = data.toStdString();

    int posBeg = packet.find_first_of("s@");
    int posEnd = packet.find_last_of("@;");
    packet = packet.substr(posBeg + 2, posEnd - 2);

    std::vector<std::string> p_vec;

    posBeg = 0;
    std::string del = "@";
    for (size_t i = 0; i < packet.size(); ++i) {
        if (packet.substr(i,1).compare(del) == 0) {
            posEnd = packet.find_first_of("@", posBeg);
            p_vec.push_back(packet.substr(posBeg,
                                          std::abs(posEnd - posBeg)));
            posBeg = i + 1;
        }
    }

    p.timeStamp = std::stod(p_vec.at(0));

    p.acc.x = std::stof(p_vec.at(1));
    p.acc.y = std::stof(p_vec.at(2));
    p.acc.z = std::stof(p_vec.at(3));

    p.gyro.x = std::stof(p_vec.at(4));
    p.gyro.y = std::stof(p_vec.at(5));
    p.gyro.z = std::stof(p_vec.at(6));

    float q_w = std::stof(p_vec.at(7));
    float q_x = std::stof(p_vec.at(8));
    float q_y = std::stof(p_vec.at(9));
    float q_z = std::stof(p_vec.at(10));
    _quat2euler(q_w, q_x, q_y, q_z,
                &(p.theta.x), &(p.theta.y), &(p.theta.z));

    return p;
}

//-----------------------------------------------------------------------------
void RacketSensorServer::_quat2euler(const float q_w, const float q_x,
                                     const float q_y, const float q_z,
                                     float *t_x, float *t_y, float *t_z)
{
    double ysqr = q_y * q_y;

    // roll (x-axis rotation)
    double t0 = +2.0 * (q_w * q_x + q_y * q_z);
    double t1 = +1.0 - 2.0 * (q_x * q_x + ysqr);
    *t_x = std::atan2(t0, t1);

    // pitch (y-axis rotation)
    double t2 = +2.0 * (q_w * q_y - q_z * q_x);
    t2 = ((t2 > 1.0) ? 1.0 : t2);
    t2 = ((t2 < -1.0) ? -1.0 : t2);
    *t_y = static_cast<float>(std::asin(t2));

    // yaw (z-axis rotation)
    double t3 = +2.0 * (q_w * q_z + q_x * q_y);
    double t4 = +1.0 - 2.0 * (ysqr + q_z * q_z);
    *t_z = static_cast<float>(std::atan2(t3, t4));
}
