#include "mainwin.h"
#include "ui_mainwin.h"

//-----------------------------------------------------------------------------
MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);
    setUpGUI();

    // Save plots in the structure:
    _plotsList.append(ui->wid11);
    _plotsList.append(ui->wid12);
    _plotsList.append(ui->wid13);

    _plotsList.append(ui->wid21);
    _plotsList.append(ui->wid22);
    _plotsList.append(ui->wid23);

    _plotsList.append(ui->wid31);
    _plotsList.append(ui->wid32);
    _plotsList.append(ui->wid33);

    setUpPlots();

    // Start and run racket sensor client
    _sensServer = new RacketSensorServer(this);

    if (ui->isPhoneChBox->isChecked())
        _sensServer->isEdisson = false;

    if (ui->isEdissonChBox->isChecked())
        _sensServer->isEdisson = true;

    connectSignals();

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    // connect(&_dataTimer, SIGNAL(timeout()), this, SLOT(realTimeDataSlot()));
    // _dataTimer.start(0); // Interval 0 means to refresh as fast as possible
    _prevProcessTimePoint = QTime::currentTime().elapsed();
    _prevPlotTimePoint = QTime::currentTime().elapsed();
    _prevTimeStamp = 0;
}

//-----------------------------------------------------------------------------
MainWin::~MainWin()
{
    delete _sensServer;
    delete ui;
}

//-----------------------------------------------------------------------------
void MainWin::connectSignals()
{
    bool isCon = false;
    isCon = connect(_sensServer, SIGNAL(sendState(const QString)),
                    this, SLOT(showState(const QString)));
    // Q_ASSERT(!isCon);

    isCon = connect(_sensServer, SIGNAL(sendSensData(const SensData,
                                                     const SensData)),
                    this, SLOT(rcvSensData(const SensData,
                                           const SensData)));
    // Q_ASSERT(!isCon);

    // Line edit objects
    isCon = connect(ui->localPortLnEd, SIGNAL(editingFinished()),
                    this, SLOT(portChanged()));
    // Q_ASSERT(!isCon);

    isCon = connect(ui->fitWinLenLnEd, SIGNAL(editingFinished()),
                            this, SLOT(fitWinLenChnged()));
    // Q_ASSERT(!isCon);

    isCon = connect(ui->isPhoneChBox, SIGNAL(stateChanged(int)),
                            this, SLOT(phoneSelected(int)));
    // Q_ASSERT(!isCon);

    isCon = connect(ui->isEdissonChBox, SIGNAL(stateChanged(int)),
                            this, SLOT(edissonSelected(int)));
    // Q_ASSERT(!isCon);
}

//-----------------------------------------------------------------------------
void MainWin::portChanged()
{
    emit(_sensServer->setListenIPPort(ui->localPortLnEd->text()));
}

//-----------------------------------------------------------------------------
void MainWin::edissonSelected(int newState) {
    if (newState == Qt::Checked) {
        ui->isPhoneChBox->setCheckState(Qt::Unchecked);
        ui->isEdissonChBox->setCheckState(Qt::Checked);
        _sensServer->isEdisson = true;
    }
}

//-----------------------------------------------------------------------------
void MainWin::phoneSelected(int newState) {
    if (newState == Qt::Checked) {
        ui->isPhoneChBox->setCheckState(Qt::Checked);
        ui->isEdissonChBox->setCheckState(Qt::Unchecked);
        _sensServer->isEdisson = false;
    }
}

//-----------------------------------------------------------------------------
void MainWin::fitWinLenChnged()
{
    _sensServer->fitWinLen = ui->fitWinLenLnEd->text().toInt();
}

//-----------------------------------------------------------------------------
void MainWin::rcvSensData(const SensData sensData, const SensData fitData)
{
    // calculate frames per second:
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // time elapsed since start, in seconds
    double t1 = time.elapsed();

    static double lastFpsKey;
    static int frameCount;
    double dtPlot = t1 - _prevPlotTimePoint;

    // Plot only each PLOT_TIME_MS:
    if (dtPlot >= ui->pltUpTimeMSLnEd->text().toInt())  // in ms
    {
        const unsigned int scrollRange = ui->pltBufSzLnEd->text().toInt();
        // QtConcurrent::run(this, &MainWin::_appendToPlot, ui->wid11,
        // sensDataPacket.timeStamp, sensDataPacket.acc.x, scrollRange);
        _updatePlots(sensData, fitData, scrollRange);

        ++frameCount;
        _prevPlotTimePoint = t1;

        ui->plotTimeMSLabel->setText(QString::number(dtPlot, 'f', 2));
        ui->FPSLabel->setText(QString::number(frameCount/(key-lastFpsKey), 'f', 2));
        ui->totDataPtLabel->setText(QString::number(ui->wid11->graph(0)->data()->size()));
        ui->transferTimeMSLabel->setText(
                    QString::number(
                        (sensData.back().timeStamp - _prevTimeStamp) * 1000, 'f', 2));
        ui->procTimeMSLabel->setText(QString::number(t1 - _prevProcessTimePoint, 'f', 2));
    }
    /*
    ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2, Processing time: %3 ms, "
                  "Transfer Time: %4 ms, Plot Time: %5")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->wid11->graph(0)->data()->size())
          .arg(t1 - _prevProcessTimePoint, 0, 'f', 2)
          .arg((sensData.timeStamp - _prevTimeStamp) * 1000, 0, 'f', 4)
          .arg(dtPlot, 0, 'f', 4), 0);
   */


   // update timer variables
   lastFpsKey = key;
   frameCount = 0;
   _prevProcessTimePoint = t1;
   _prevTimeStamp = sensData.back().timeStamp;
}

//-----------------------------------------------------------------------------
void MainWin::showState(const QString state)
{
    QMessageBox::critical(this, "State notification", state, 1, 0);
}

//-----------------------------------------------------------------------------
void MainWin::setUpGUI()
{
    ui->localPortLnEd->setValidator(new QIntValidator(1, 65536, this));
    ui->pltBufSzLnEd->setValidator(new QIntValidator(5, 1000, this));
    ui->pltUpTimeMSLnEd->setValidator(new QIntValidator(1, 1000, this));
    ui->fitWinLenLnEd->setValidator(new QIntValidator(10, 1000, this));
}


//-----------------------------------------------------------------------------
void MainWin::setUpPlots()
{
    const QString timeFormat = "%h:%m:%s";  // "%h:%m:%s:%z"
    const QString timeAxisLabel = "t, h:m:s";

    // acceleration plots:
    _setUpPlot(ui->wid11, timeFormat, timeAxisLabel, "a_x, m/s^2");
    _setUpPlot(ui->wid21, timeFormat, timeAxisLabel, "a_y, m/s^2");
    _setUpPlot(ui->wid31, timeFormat, timeAxisLabel, "a_z, m/s^2");

    // gyro plots:
    _setUpPlot(ui->wid12, timeFormat, timeAxisLabel, "w_x, rad/s");
    _setUpPlot(ui->wid22, timeFormat, timeAxisLabel, "w_y, rad/s");
    _setUpPlot(ui->wid32, timeFormat, timeAxisLabel, "w_z, rad/s");

    // magnetic field plots:
    _setUpPlot(ui->wid13, timeFormat, timeAxisLabel, "O_x, rad");
    _setUpPlot(ui->wid23, timeFormat, timeAxisLabel, "O_y, rad");
    _setUpPlot(ui->wid33, timeFormat, timeAxisLabel, "O_z, rad");
}

//-----------------------------------------------------------------------------
void MainWin::_setUpPlot(QCustomPlot *plot, const QString timeFormat,
                         const QString xLabel, const QString yLabel)
{
    QColor c1 = QColor(40, 110, 255);
    QColor c2 = QColor(255, 110, 40);

    plot->addGraph(); // c1
    plot->addGraph(); // c2
    plot->graph(0)->setPen(QPen(c1));
    plot->graph(1)->setPen(QPen(c2));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat(timeFormat);
    plot->xAxis->setTicker(timeTicker);
    plot->axisRect()->setupFullAxesBox();
    plot->yAxis->setRange(-1, 1);
    plot->xAxis->setLabel(xLabel);
    plot->yAxis->setLabel(yLabel);

    // make left and bottom axes transfer their ranges to right and top axes:
    // connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), plot->xAxis2, SLOT(setRange(QCPRange)));
    // connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), plot->yAxis2, SLOT(setRange(QCPRange)));
}

//-----------------------------------------------------------------------------
void MainWin::_appendToPlot(QCustomPlot *plot,
                            const double key, const double value,
                            std::vector<double> t, std::vector<double> fit,
                            const int scrollRange)
{
  // add data to lines:
  plot->graph(0)->addData(key, value);

  // rescale value (vertical) axis to fit the current data:
  plot->graph(0)->rescaleValueAxis(true);

  // append fitted data:
  plot->graph(1)->data()->clear();
  for (unsigned int i = 0; i < fit.size(); i++)
      plot->graph(1)->addData(t.at(i), fit.at(i));
  plot->graph(1)->rescaleValueAxis(true);

  // make key axis range scroll with the data:
  plot->xAxis->setRange(key, scrollRange, Qt::AlignRight);
  plot->replot();
}

//-----------------------------------------------------------------------------
void MainWin::_updatePlots(const SensData sensData, const SensData fitData,
                           const int scrollRange)
{
    const double t = sensData.back().timeStamp;
    const SensDataPacket packet = sensData.back();

    // = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    std::vector<double> tFitData;
    std::vector<double> accX;
    std::vector<double> accY;
    std::vector<double> accZ;

    std::vector<double> gyroX;
    std::vector<double> gyroY;
    std::vector<double> gyroZ;

    std::vector<double> thetaX;
    std::vector<double> thetaY;
    std::vector<double> thetaZ;

    foreach(SensDataPacket fd, fitData) {
        accX.push_back(fd.acc.x);
        accY.push_back(fd.acc.y);
        accZ.push_back(fd.acc.z);

        gyroX.push_back(fd.gyro.x);
        gyroY.push_back(fd.gyro.y);
        gyroZ.push_back(fd.gyro.z);

        thetaX.push_back(fd.theta.x);
        thetaY.push_back(fd.theta.y);
        thetaZ.push_back(fd.theta.z);
        tFitData.push_back(fd.timeStamp);
    }
    // std::reverse(d.begin(), d.end());

    _appendToPlot(ui->wid11, t, packet.acc.x, tFitData, accX, scrollRange);
    _appendToPlot(ui->wid21, t, packet.acc.y, tFitData, accY, scrollRange);
    _appendToPlot(ui->wid31, t, packet.acc.z, tFitData, accZ, scrollRange);

    _appendToPlot(ui->wid12, t, packet.gyro.x, tFitData, gyroX, scrollRange);
    _appendToPlot(ui->wid22, t, packet.gyro.y, tFitData, gyroY, scrollRange);
    _appendToPlot(ui->wid32, t, packet.gyro.z, tFitData, gyroZ, scrollRange);

    _appendToPlot(ui->wid13, t, packet.theta.x, tFitData, thetaX, scrollRange);
    _appendToPlot(ui->wid23, t, packet.theta.y, tFitData, thetaY, scrollRange);
    _appendToPlot(ui->wid33, t, packet.theta.z, tFitData, thetaZ, scrollRange);
}
