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
    accXMin = ACC_X_MIN_TRESHHOLD;
    fitWinStepInPercentOfLen = 1;

    _plotCnt = 0;
    _fitSampleCnt = 0;
    _isProcess = true;
    _saveCnt = 0;
    _scoreCnt = 0;
    _score = -1;

    _transferTime = 0;
    _predictionTime = 0;

    // LDA Coeffitients:
    /*
    [[  4.29152573e-01   2.39417880e-03  -4.87487628e-03   3.78098592e+00
        5.72766662e-03   8.76812229e-03   1.51705065e+00   7.11886179e-01
        2.86129860e-03   2.65704906e-02  -5.79301334e-01   1.43178097e-03
        -1.15422108e-03   6.04679361e-03   1.35714229e-02   5.49566637e-04
        -6.14553399e-03  -9.66999931e-03   2.27829264e-03   3.60328976e-04
        -4.73265230e-03  -2.11558324e-02   1.77403736e-01  -1.07398512e-02
        -1.95671325e-02  -9.74312910e-03]]
    [-7.98102013]
    */
    _coeffs.w = {4.29152573e-01, 2.39417880e-03, -4.87487628e-03, 3.78098592e+00,
                 5.72766662e-03, 8.76812229e-03,  1.51705065e+00, 7.11886179e-01,
                 2.86129860e-03, 2.65704906e-02, -5.79301334e-01, 1.43178097e-03,
                -1.15422108e-03, 6.04679361e-03,  1.35714229e-02, 5.49566637e-04,
                -6.14553399e-03, -9.66999931e-03, 2.27829264e-03, 3.60328976e-04,
                -4.73265230e-03, -2.11558324e-02, 1.77403736e-01, -1.07398512e-02,
                -1.95671325e-02, -9.74312910e-03};
    _coeffs.b = -7.98102013;

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
    buffer.resize(_socket->pendingDatagramSize()+100);

    QHostAddress sender;
    quint16 senderPort;

    _socket->readDatagram(buffer.data(), buffer.size(),
                          &sender, &senderPort);

    QString data(buffer);

    _appendToBuffer(&_sensData, data);

    // Process fit data buffer by moving fit window over the sensor buffer:
    _fitSampleCnt++;
    size_t treshold = static_cast<size_t>(fitWinStepInPercentOfLen *
                                          static_cast<float>(fitWinLen));
    if (_fitSampleCnt >= treshold) {
        // Copy last fitWinLen points from sensor buffer:
       _fitData.clear();
        size_t n = _sensData.size() < fitWinLen ? _fitSampleCnt : fitWinLen;
        for (unsigned int i = _sensData.size() - n; i < _sensData.size(); i++)
            _fitData.push_back(_sensData.at(i));

        // Fit and predict the window:
        int score = -1;
        _fitData = _fitPredict(_fitData, &score);

        // Increase score counter if prediction was correct:
        _score = score;
        if (score > 0) _scoreCnt++;

        // Reset counter:
        _fitSampleCnt = 0;
    }

    // Plot the data according to samples to plot:
    _plotCnt++;
    if (_plotCnt >= SAMPLE_PLOT) {
        emit(sendSensData(_sensData, _fitData,
                          _score, _scoreCnt,
                          _transferTime, _predictionTime));
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
        if (sensData->empty())
            _isFirstPacket = true;
        else
            _isFirstPacket = false;
        bool isBad = false;
        size_t badCnt = 0;
        p = processInPacketEdisson(data, &isBad, &badCnt, _isFirstPacket);
        if (isBad)
            _correctBadPacket(&p, sensData->back(), badCnt);
    }
    sensData->push_back(p);
}

//-----------------------------------------------------------------------------
void RacketSensorServer::_correctBadPacket(SensPacket *badPacket,
                                           const SensPacket prevPacket,
                                           const size_t badCnt) {
    switch(badCnt) {
        case 0:
            badPacket->t = prevPacket.t;
            break;
        case 1:
            badPacket->acc.x = prevPacket.acc.x;
            break;
        case 2:
            badPacket->acc.y = prevPacket.acc.y;
            break;
        case 3:
            badPacket->acc.z = prevPacket.acc.z;
            break;
        case 4:
            badPacket->gyro.x = prevPacket.gyro.x;
            break;
        case 5:
            badPacket->gyro.y = prevPacket.gyro.y;
            break;
        case 6:
            badPacket->gyro.z = prevPacket.gyro.z;
            break;
        case 7:
            badPacket->ang.x = prevPacket.ang.x;
            break;
        case 8:
            badPacket->ang.y = prevPacket.ang.y;
            break;
        case 9:
            badPacket->ang.z = prevPacket.ang.z;
            break;
        case 10:
            badPacket->ang.x = prevPacket.ang.x;
            badPacket->ang.z = prevPacket.ang.z;
            badPacket->ang.z = prevPacket.ang.z;
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
SensBuffer RacketSensorServer::_fitPredict(const SensBuffer fitData,
                                           int *score) {
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
    G1bPar accYPar, gyroZPar, angYPar;
    G2bPar accXPar, gyroYPar;
    accXPar = MathFit::fitG2b(time, accX, MAX_ITER);  // G2b
    accYPar = MathFit::fitG1b(time, accY, MAX_ITER);  // G1b

    gyroYPar = MathFit::fitG2b(time, gyroY, MAX_ITER);  // G2b
    gyroZPar = MathFit::fitG1b(time, gyroZ, MAX_ITER);  // G1b

    angYPar = MathFit::fitG1b(time, angY, MAX_ITER);  // G1b

    // Predict:
    *score = _predict(accYPar, gyroZPar, angYPar, accXPar, gyroYPar);

    // Detection treshold:
    double accXMin = *std::min_element(accX.begin(), accX.end());
    if (accXMin > this->accXMin) *score = -1;

    // Pack fitted data to fitted array:
    double t = 0;
    for(unsigned int i = 0; i < time.size(); i++) {
        t = time.at(i);
        fitted.at(i).t = t;
        fitted.at(i).acc.x = MathFit::G2b(t, accXPar);
        fitted.at(i).acc.y = MathFit::G1b(t, accYPar);

        fitted.at(i).gyro.y = MathFit::G2b(t, gyroYPar);
        fitted.at(i).gyro.z = MathFit::G1b(t, gyroZPar);

        fitted.at(i).ang.y = MathFit::G1b(t, angYPar);
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
                                                      bool *isBad,
                                                      size_t *badCnt,
                                                      const bool isFirstPacket)
{
    // Packet fromat:
    // "s@TimeStamp@accX@accY@accZ@gyroX@gyroY@gyroZ@angX@angY@angZ@;"
    *isBad = false;
    SensPacket p;
    std::string packet = data.toStdString();
    try {

        const std::string del = "@";

        int posBeg = packet.find_first_of("s@");
        int posEnd = packet.find_last_of(";");
        packet = packet.substr(posBeg + 2, posEnd);

        if (packet.substr(packet.size()-1,1).compare(del) != 0)
            packet.append(del);

        std::vector<std::string> p_vec;
        posBeg = 0;
        for (size_t i = 0; i < packet.size(); ++i) {
            if (packet.substr(i,1).compare(del) == 0) {
                posEnd = packet.find_first_of(del, posBeg);
                p_vec.push_back(packet.substr(posBeg,
                                              std::abs(posEnd - posBeg)));
                posBeg = i + 1;
            }
        }

        (*badCnt) = 0;
        if (isFirstPacket)  // set start time to zero
            _startTime = std::stod(p_vec.at(0));

        p.t = std::stod(p_vec.at(0));
        p.t = p.t - _startTime;

        (*badCnt)++;
        p.acc.x = std::stof(p_vec.at(1));
        (*badCnt)++;
        p.acc.y = std::stof(p_vec.at(2));
        (*badCnt)++;
        p.acc.z = std::stof(p_vec.at(3));

        (*badCnt)++;
        p.gyro.x = std::stof(p_vec.at(4));
        (*badCnt)++;
        p.gyro.y = std::stof(p_vec.at(5));
        (*badCnt)++;
        p.gyro.z = std::stof(p_vec.at(6));

        if (p_vec.size() == 10) {  // use euler angles
            (*badCnt)++;
            p.ang.x = std::stof(p_vec.at(7)) * 180.0 / M_PI;
            (*badCnt)++;
            p.ang.y = std::stof(p_vec.at(8)) * 180.0 / M_PI;
            (*badCnt)++;
            p.ang.z = std::stof(p_vec.at(9)) * 180.0 / M_PI;
        } else if (p_vec.size() == 11) {  // use quaternions
            (*badCnt)++;
            p.quat.w = std::stof(p_vec.at(7));
            (*badCnt)++;
            p.quat.x = std::stof(p_vec.at(8));
            (*badCnt)++;
            p.quat.y = std::stof(p_vec.at(9));
            (*badCnt)++;
            p.quat.z = std::stof(p_vec.at(10));

            _quat2euler(p.quat.w, p.quat.x, p.quat.y, p.quat.z,
                        &p.ang.x, &p.ang.y, &p.ang.z);
        }

    } catch (const std::exception &ex) {
        *isBad = true;
        return p;
    } catch (...) {
        *isBad = true;
        return p;
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
                                     float *t_x, float *t_y, float *t_z) {
    double ysqr = q_y * q_y;

    // roll (x-axis rotation)
    double t0 = +2.0 * (q_w * q_x + q_y * q_z);
    double t1 = +1.0 - 2.0 * (q_x * q_x + ysqr);
    *t_x = static_cast<float>(std::atan2(t0, t1)* 180.0 / M_PI);

    // pitch (y-axis rotation)
    double t2 = +2.0 * (q_w * q_y - q_z * q_x);
    t2 = ((t2 > 1.0) ? 1.0 : t2);
    t2 = ((t2 < -1.0) ? -1.0 : t2);
    *t_y = static_cast<float>(std::asin(t2)* 180.0 / M_PI);

    // yaw (z-axis rotation)
    double t3 = +2.0 * (q_w * q_z + q_x * q_y);
    double t4 = +1.0 - 2.0 * (ysqr + q_z * q_z);
    *t_z = static_cast<float>(std::atan2(t3, t4) * 180.0 / M_PI);
}

//-----------------------------------------------------------------------------
int RacketSensorServer::_predict(const G1bPar &accYPar, const G1bPar &gyroZPar,
                                 const G1bPar &angYPar, const G2bPar &accXPar,
                                 const G2bPar &gyroYPar) {
    // G2bPar:
    // p(0) = b
    // p(1) = a1
    // p(2) = m1
    // p(3) = s1
    // p(4) = a2
    // p(5) = m2
    // p(6) = s2

    // G1bPar:
    // p(0) = b
    // p(1) = a
    // p(2) = m
    // p(3) = s

    // Create features vector:
    // [
    //  A1 m1 s1 A2 m2 s2 b  // accXPar
    //  A1 m1 s1 b           // accYPar
    //  A1 m1 s1 b           // gyroZPar
    //  A1 m1 s1 A2 m2 s2 b  // gyroYPar
    //  A1 m1 s1 b           // angYPar
    // ]
    std::vector<double> fVec(NUM_FEATURES);

    // accXPar:
    fVec.at(0) = accXPar(1);  // a1
    fVec.at(1) = accXPar(2);  // m1
    fVec.at(2) = accXPar(3);  // s1
    fVec.at(3) = accXPar(4);  // a2
    fVec.at(4) = accXPar(5);  // m2
    fVec.at(5) = accXPar(6);  // s2
    fVec.at(6) = accXPar(0);  // b

    // accYPar:
    fVec.at(7) = accYPar(1);   // a1
    fVec.at(8) = accYPar(2);   // m1
    fVec.at(9) = accYPar(3);   // s1
    fVec.at(10) = accYPar(0);  // b

    // gyroZPar:
    fVec.at(11) = gyroZPar(1);  // a1
    fVec.at(12) = gyroZPar(2);  // m1
    fVec.at(13) = gyroZPar(3);  // s1
    fVec.at(14) = gyroZPar(0);  // b

    // gyroYPar:
    fVec.at(15) = gyroYPar(1);  // a1
    fVec.at(16) = gyroYPar(2);  // m1
    fVec.at(17) = gyroYPar(3);  // s1
    fVec.at(18) = gyroYPar(4);  // a2
    fVec.at(19) = gyroYPar(5);  // m2
    fVec.at(20) = gyroYPar(6);  // s2
    fVec.at(21) = gyroYPar(0);  // b

    // angYPar:
    fVec.at(22) = angYPar(1);  // a1
    fVec.at(23) = angYPar(2);  // m1
    fVec.at(24) = angYPar(3);  // s1
    fVec.at(25) = angYPar(0);  // b

    double res = _dot(fVec, _coeffs.w) + _coeffs.b;
    int score = -1;
    if (res > 0) score = 1;
    return score;
}

//-----------------------------------------------------------------------------
double RacketSensorServer::_dot(std::vector<double> v1,
                                std::vector<double> v2) {
    double dot = 0;
    if (v1.size() != v2.size()) return dot;

    for (size_t i = 0; i < v1.size(); i++)
        dot += v1.at(i) * v2.at(i);
    return dot;
}
