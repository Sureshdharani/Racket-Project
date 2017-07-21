#include "racketsensorserver.h"

//-----------------------------------------------------------------------------
RacketSensorServer::RacketSensorServer(QObject *parent,
                                       const quint16 Port) :
    QObject(parent)
{
    _socket = new QUdpSocket(this);
    port = Port;
    _socket->bind(port);

    _sensData = SensBuffer();
    _fitData = SensBuffer();

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

    _appendToBuffer(&_sensData, data);

    if (true) {  // Fit sensor data (do it only if the buffer accumulated enough data):
        // _fitData = _fitSensData(_sensData, fitWinLen);
        emit(sendSensData(_sensData, _fitData));
    }
}

//-----------------------------------------------------------------------------
void RacketSensorServer::_appendToBuffer(SensBuffer *sensData,
                                         const QString data)
{
    // Delete packets from beggining of the buffer
    // if the size exceeds its dimensions:
    if (sensData->size() >= BUFF_SIZE)
        sensData->pop_front();

    // Append new element
    if (!isEdisson) {  // process packets from mobile app
        auto sensPacketPrev = sensData->empty() ? SensPacket() : sensData->back();
        sensData->push_back(processInPacketMobile(data, sensPacketPrev));
    } else {  // process packet from edison
        sensData->push_back(processInPacketEdisson(data));
    }
}

//-----------------------------------------------------------------------------
SensBuffer RacketSensorServer::_fitSensData(const SensBuffer data,
                                             const unsigned int N)
{
    if (data.size() < N)
        return data;

    // Create containers for fit:
    auto fitted = SensBuffer(N);

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

        time.at(j) = data.at(i).t;

        accX.at(j) = data.at(i).acc.x;
        accY.at(j) = data.at(i).acc.y;
        accZ.at(j) = data.at(i).acc.z;

        gyroX.at(j) = data.at(i).gyro.x;
        gyroY.at(j) = data.at(i).gyro.y;
        gyroZ.at(j) = data.at(i).gyro.z;

        thetaX.at(j) = data.at(i).ang.x;
        thetaY.at(j) = data.at(i).ang.y;
        thetaZ.at(j) = data.at(i).ang.z;
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
SensPacket RacketSensorServer::processInPacketMobile(const QString packet,
                                      const SensPacket prevSensPacket)
{
    auto sensPacket = prevSensPacket;
    const QString del3 = ", 3,  ";
    const QString del4 = ", 4,  ";
    const QString del5 = ", 5,  ";
    const QString del = ",";

    if (packet.isEmpty()) return sensPacket;

    QStringList list(packet);

    QStringList acc;
    QStringList gyro;
    QStringList ang;

    list = list.at(0).split(del3, QString::SkipEmptyParts);
    sensPacket.t = list.at(0).toDouble();
    if (list.size() == 1) return sensPacket;
    list = QStringList(list.back());

    list = list.at(0).split(del4, QString::SkipEmptyParts);
    acc = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensPacket.acc.x = acc.at(0).toFloat();
    sensPacket.acc.y = acc.at(1).toFloat();
    sensPacket.acc.z = acc.at(2).toFloat();
    if (list.size() == 1) return sensPacket;
    list = QStringList(list.back());

    list = list.at(0).split(del5, QString::SkipEmptyParts);
    gyro = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensPacket.gyro.x = gyro.at(0).toFloat();
    sensPacket.gyro.y = gyro.at(1).toFloat();
    sensPacket.gyro.z = gyro.at(2).toFloat();
    if (list.size() == 1) return sensPacket;
    list = QStringList(list.back());

    ang = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensPacket.ang.x = ang.at(0).toFloat();
    sensPacket.ang.y = ang.at(1).toFloat();
    sensPacket.ang.z = ang.at(2).toFloat();

    return sensPacket;
}

//-----------------------------------------------------------------------------
SensPacket RacketSensorServer::processInPacketEdisson(const QString data)
{
    // Packet fromat:
    // "s@TimeStamp@accX@accY@accZ@gyroX@gyroY@gyroZ@angX@angY@angZ@;"

    SensPacket p;

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

    p.t = std::stod(p_vec.at(0));

    p.acc.x = std::stof(p_vec.at(1));
    p.acc.y = std::stof(p_vec.at(2));
    p.acc.z = std::stof(p_vec.at(3));

    p.gyro.x = std::stof(p_vec.at(4));
    p.gyro.y = std::stof(p_vec.at(5));
    p.gyro.z = std::stof(p_vec.at(6));

    if (p_vec.size() == 10) {  // use euler angles
        p.ang.x = std::stof(p_vec.at(7));
        p.ang.y = std::stof(p_vec.at(8));
        p.ang.z = std::stof(p_vec.at(9));
    } else if (p_vec.size() == 11) {  // use quaternions
        p.quat.w = std::stof(p_vec.at(7));
        p.quat.x = std::stof(p_vec.at(8));
        p.quat.y = std::stof(p_vec.at(9));
        p.quat.z = std::stof(p_vec.at(10));
    }

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
    *t_x = static_cast<float>(std::atan2(t0, t1));

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