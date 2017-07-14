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
    _fitData = FitSensData();

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

    // Fit sensor data (do it only if the buffer accumulated enough data):
    // _fitData = _fitSensData(_sensData, fitWinLen);

    emit(sendSensData(_sensData, _fitData));
}

//-----------------------------------------------------------------------------
FitSensData RacketSensorServer::_fitSensData(const SensData data,
                                             const unsigned int N)
{
    if (data.size() < N)
        return static_cast<FitSensData>(data);

    // Create containers for fit:
    FitSensData fitted = FitSensData(N);

    std::vector<double> time(N);
    std::vector<double> accX(N);
    std::vector<double> accY(N);
    std::vector<double> accZ(N);

    std::vector<double> gyroX(N);
    std::vector<double> gyroY(N);
    std::vector<double> gyroZ(N);

    std::vector<double> magX(N);
    std::vector<double> magY(N);
    std::vector<double> magZ(N);

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

        magX.at(j) = data.at(i).mag.x;
        magY.at(j) = data.at(i).mag.y;
        magZ.at(j) = data.at(i).mag.z;
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
    QStringList mag;

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

    mag = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensDataPacket.mag.x = mag.at(0).toDouble();
    sensDataPacket.mag.y = mag.at(1).toDouble();
    sensDataPacket.mag.z = mag.at(2).toDouble();

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

    p.mag.x = std::stof(p_vec.at(7));
    p.mag.y = std::stof(p_vec.at(8));
    p.mag.z = std::stof(p_vec.at(9));

    return p;
}
