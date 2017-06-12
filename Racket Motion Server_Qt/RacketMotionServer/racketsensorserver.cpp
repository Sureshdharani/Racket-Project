#include "racketsensorserver.h"

//-----------------------------------------------------------------------------
RacketSensorServer::RacketSensorServer(QObject *parent,
                                       const quint16 Port) :
    QObject(parent)
{
    _socket = new QUdpSocket(this);
    port = Port;
    _socket->bind(port);

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
    _sensData = processInputPacket(data, _sensDataPrev);
    _sensDataPrev = _sensData;

    emit(sendSensData(_sensData));

    /*
    const QString str1 = "90594.75079, 3,  -0.059,  0.098,  9.826, 4,  -0.000,  0.001,  0.001, 5,  16.191, 12.642,-34.497";
    const QString str2 = "92309.37615, 3,  -0.043,  0.033,  9.722, 4,  -0.001, -0.001, -0.001";
    const QString str3 = "92272.20973, 3,  -1.355, -0.098, 10.935";

    qDebug() << "Message: " << data;

    _sensData = processInputPacket(str1, _sensDataPrev);
    _sensDataPrev = _sensData;
    qDebug() << "SensData1: " << _sensData.toString();

    _sensData = processInputPacket(str1, _sensDataPrev);
    _sensDataPrev = _sensData;
    qDebug() << "SensData2: " << _sensData.toString();

    _sensData = processInputPacket(str1, _sensDataPrev);
    _sensDataPrev = _sensData;
    qDebug() << "SensData3: " << _sensData.toString();
    qDebug() << "--------------------------------";
    */
}

//-----------------------------------------------------------------------------
SensData RacketSensorServer::processInputPacket(const QString packet,
                                                const SensData prevSensData)
{
    SensData sensData = prevSensData;
    const QString del3 = ", 3,  ";
    const QString del4 = ", 4,  ";
    const QString del5 = ", 5,  ";
    const QString del = ",";

    if (packet.isEmpty()) return sensData;

    QStringList list(packet);

    QStringList acc;
    QStringList gyro;
    QStringList mag;

    list = list.at(0).split(del3, QString::SkipEmptyParts);
    sensData.timeStamp = list.at(0).toDouble();
    if (list.size() == 1) return sensData;
    list = QStringList(list.back());

    list = list.at(0).split(del4, QString::SkipEmptyParts);
    acc = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensData.accX = acc.at(0).toDouble();
    sensData.accY = acc.at(1).toDouble();
    sensData.accZ = acc.at(2).toDouble();
    if (list.size() == 1) return sensData;
    list = QStringList(list.back());

    list = list.at(0).split(del5, QString::SkipEmptyParts);
    gyro = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensData.gyroX = gyro.at(0).toDouble();
    sensData.gyroY = gyro.at(1).toDouble();
    sensData.gyroZ = gyro.at(2).toDouble();
    if (list.size() == 1) return sensData;
    list = QStringList(list.back());

    mag = QString(list.at(0)).replace(" ", "").split(del, QString::SkipEmptyParts);
    sensData.magX = mag.at(0).toDouble();
    sensData.magY = mag.at(1).toDouble();
    sensData.magZ = mag.at(2).toDouble();

    return sensData;
}
