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
    _isFirstPacket = true;
    _startTime = 0;

    fitWinLen = 100;
    isEdisson = true;

    _plotCnt = 0;
    _fitSampleCnt = 0;
    _isProcess = true;
    _saveCnt = 0;

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
void RacketSensorServer::readPendingDatagrams() {
    #ifdef LOG
    if (!_isProcess) {
        QDir dir;
        emit(sendState("Data is saved to: " + dir.absolutePath()));
        return;
    }
    #endif

    // When data comes in
    QByteArray buffer;
    buffer.resize(_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    _socket->readDatagram(buffer.data(), buffer.size(),
                          &sender, &senderPort);

    QString data(buffer);

    _appendToBuffer(&_sensData, data);

    // Process fit data buffer by moving fit window over the sensor buffer:
    _fitSampleCnt++;
    if (_fitSampleCnt >= fitWinLen) {
        // Copy last fitWinLen points from sensor buffer:
        for (unsigned int i = _sensData.size() - fitWinLen; i < _sensData.size(); i++)
            _fitData.push_back(_sensData.at(i));

        // Fit the window:
        _fitData = _fit(_fitData);

        // Reset counter:
         _fitSampleCnt = 0;
    }

    // Plot the data according to samples to plot:
    _plotCnt++;
    if (_plotCnt >= SAMPLE_PLOT) {
        emit(sendSensData(_sensData, _fitData));
        _plotCnt = 0;
    }

    #ifdef LOG
    _saveCnt++;
    if (_saveCnt > 1000) {
        _saveBuffer(_sensData);
        _isProcess = false;
        _saveCnt = 0;
    }
    #endif
}

//-----------------------------------------------------------------------------
void RacketSensorServer::_appendToBuffer(SensBuffer *sensData,
                                         const QString data)
{
    // Delete packets from beggining of the buffer
    // if the size exceeds its dimensions:
    if (sensData->size() >= MAX_BUFF_SIZE)
        sensData->pop_front();

    // Append new element
    SensPacket p;
    if (!isEdisson) {  // process packets from mobile app
        auto sensPacketPrev = sensData->empty() ? SensPacket() : sensData->back();
        p = processInPacketMobile(data, sensPacketPrev);
    } else {  // process packet from edison
        if (sensData->empty()) {
            _isFirstPacket = true;
        } else {
            _isFirstPacket = false;
        }
        p = processInPacketEdisson(data, _isFirstPacket);
    }

    sensData->push_back(p);
}

//-----------------------------------------------------------------------------
SensBuffer RacketSensorServer::_fit(const SensBuffer fitData) {
    // Create containers for fit:
    unsigned int N = fitData.size();
    auto fitted = SensBuffer(N);

    std::vector<double> time(N);
    std::vector<double> accX(N), accY(N), accZ;
    std::vector<double> gyroX, gyroY(N), gyroZ(N);
    std::vector<double> angX, angY(N), angZ;

    for(unsigned int i = 0; i < time.size(); i++) {
        time.at(i) = fitData.at(i).t;

        accX.at(i) = fitData.at(i).acc.x;
        accY.at(i) = fitData.at(i).acc.y;

        gyroY.at(i) = fitData.at(i).gyro.y;
        gyroZ.at(i) = fitData.at(i).gyro.z;

        angY.at(i) = fitData.at(i).ang.y;
    }

    // Fit the data:
    accX = MathFit::fitGauss1b(time, accX);
    accY = MathFit::fitGauss1b(time, accY);
    // accZ = MathFit::fitGauss1b(time, accZ);

    // gyroX = MathFit::fitGauss1b(time, gyroX);
    gyroY = MathFit::fitGauss1b(time, gyroY);
    gyroZ = MathFit::fitGauss1b(time, gyroZ);

    angY = MathFit::fitGauss1b(time, angY);

    // Pack fitted data to fitted array:
    for(unsigned int i = 0; i < time.size(); i++) {
        fitted.at(i).t = time.at(i);
        fitted.at(i).acc.x = accX.at(i);
        fitted.at(i).acc.y = accY.at(i);

        fitted.at(i).gyro.y = gyroY.at(i);
        fitted.at(i).gyro.z = gyroZ.at(i);

        fitted.at(i).ang.y = angY.at(i);
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
SensPacket RacketSensorServer::processInPacketEdisson(const QString data,
                                                      const bool isFirstPacket)
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

    if (isFirstPacket) {  // set start time to zero
        _startTime = std::stod(p_vec.at(0));
    }

    p.t = std::stod(p_vec.at(0));
    p.t = p.t - _startTime;

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
void RacketSensorServer::_saveBuffer(const SensBuffer data) {
    std::ofstream logFile;
    std::string del = "\t";
    logFile.open ("BufferLog.txt");
    logFile << "t" << del
            << "accX" << del << "accY" << del << "accZ" << del
            << "gyroX" << del << "gyroY" << del << "gyroZ" << del
            << "angX" << del << "angY" << del << "angZ" << del
            << "\n";
    for (unsigned int i = 0; i < data.size(); i++) {
        logFile << data.at(i).t << del
                << data.at(i).acc.x << del
                << data.at(i).acc.y << del
                << data.at(i).acc.z << del
                << data.at(i).gyro.x << del
                << data.at(i).gyro.y << del
                << data.at(i).gyro.z << del
                << data.at(i).ang.x << del
                << data.at(i).ang.y << del
                << data.at(i).ang.z << del
                << "\n";
    }
   logFile.close();
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
