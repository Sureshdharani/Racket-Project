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

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    // Packet format:
    // "90594.75079, 3,  -0.059,  0.098,  9.826, 4,  -0.000,  0.001,  0.001, 5,  16.191, 12.642,-34.497"
    // "92309.37615, 3,  -0.043,  0.033,  9.722, 4,  -0.001, -0.001, -0.001"
    // "92272.20973, 3,  -1.355, -0.098, 10.935"

    _socket->readDatagram(buffer.data(), buffer.size(),
                          &sender, &senderPort);

    QString data(buffer);
    SensDataPacket sensDataPacketPrev = _sensData.empty() ? SensDataPacket() : _sensData.back();
    if (_sensData.size() >= NUM_PACKETS)  // adopt size if it exceeds array size
        _sensData.pop_front();  // delete last element

    // Append new element
    _sensData.push_back(processInputPacket(data, sensDataPacketPrev));

    // Fit sensor data:
    _fitData = _fitSensData(_sensData, fitWinLen);

    emit(sendSensData(_sensData, _fitData));

    /*
    const QString str1 = "90594.75079, 3,  -0.059,  0.098,  9.826, 4,  -0.000,  0.001,  0.001, 5,  16.191, 12.642,-34.497";
    const QString str2 = "92309.37615, 3,  -0.043,  0.033,  9.722, 4,  -0.001, -0.001, -0.001";
    const QString str3 = "92272.20973, 3,  -1.355, -0.098, 10.935";

    qDebug() << "Message: " << data;

    _sensDataPacket = processInputPacket(str1, _sensDataPacketPrev);
    _sensDataPacketPrev = _sensDataPacket;
    qDebug() << "sensDataPacket1: " << _sensDataPacket.toString();

    _sensDataPacket = processInputPacket(str1, _sensDataPacketPrev);
    _sensDataPacketPrev = _sensDataPacket;
    qDebug() << "sensDataPacket2: " << _sensDataPacket.toString();

    _sensDataPacket = processInputPacket(str1, _sensDataPacketPrev);
    _sensDataPacketPrev = _sensDataPacket;
    qDebug() << "sensDataPacket3: " << _sensDataPacket.toString();
    qDebug() << "--------------------------------";
    */
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

    // Pack fitted data to fitted array:
    for(unsigned int i = 0; i < accX.size(); i++) {
        fitted.at(i).acc.x = accX.at(i);
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
