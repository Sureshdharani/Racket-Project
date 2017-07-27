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

    _FPS = 0;  // fps
    _prevPlotTime = 0;  // previous plot time
    _fpsVec = std::vector<double>();

    setUpPlots();

    // Start and run racket sensor client
    _sensServer = new RacketSensorServer(this);
    _sensServer->fitWinLen = ui->fitWinLenLnEd->text().toInt();

    if (ui->isPhoneChBox->isChecked())
        _sensServer->isEdisson = false;

    if (ui->isEdissonChBox->isChecked())
        _sensServer->isEdisson = true;

    connectSignals();

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    // connect(&_dataTimer, SIGNAL(timeout()), this, SLOT(realTimeDataSlot()));
    // _dataTimer.start(0); // Interval 0 means to refresh as fast as possible
    // _prevProcessTimePoint = QTime::currentTime().elapsed();
    // _prevPlotTimePoint = QTime::currentTime().elapsed();
    // _prevTimeStamp = 0;
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
    connect(_sensServer, SIGNAL(sendState(const QString)), this,
            SLOT(showState(const QString)));

    connect(_sensServer,
            SIGNAL(sendSensData(const SensBuffer, const SensBuffer)),
            this, SLOT(rcvSensData(const SensBuffer, const SensBuffer)));

    // Line edit objects
    connect(ui->localPortLnEd, SIGNAL(editingFinished()), this,
            SLOT(portChanged()));

    connect(ui->fitWinLenLnEd, SIGNAL(editingFinished()), this,
            SLOT(fitWinLenChnged()));

    connect(ui->isPhoneChBox, SIGNAL(stateChanged(int)), this,
            SLOT(phoneSelected(int)));

    connect(ui->isEdissonChBox, SIGNAL(stateChanged(int)), this,
            SLOT(edissonSelected(int)));
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
void MainWin::fitWinLenChnged() {
    _sensServer->fitWinLen = ui->fitWinLenLnEd->text().toInt();
}

//-----------------------------------------------------------------------------
void MainWin::rcvSensData(const SensBuffer sensData,
                          const SensBuffer fitData) {
    static QTime time(QTime::currentTime());
    double t0 = time.elapsed();  // in milliseconds
    double dtPlot = t0 - _prevPlotTime;

    _updatePlots(sensData, fitData, ui->pltBufSzLnEd->text().toInt());

    // Update plot information:
    ui->plotTimeMSLabel->setText(QString::number(dtPlot, 'f', 2));
    ui->totDataPtLabel->setText(QString::number(ui->wid11->graph(0)->data()->size()));

    ui->transferTimeMSLabel->setText(QString::number(-1, 'f', 2));
    ui->procTimeMSLabel->setText(QString::number(-1, 'f', 2));

    _FPS = 1000.0 / dtPlot;
    _fpsVec.push_back(_FPS);
    if (_fpsVec.size() >= FPS_NUMS_MEAN) {
        ui->FPSLabel->setText(QString::number(MathFit::mean(_fpsVec), 'f', 2));
        _fpsVec.clear();
    }

    _prevPlotTime = t0;
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
}

//-----------------------------------------------------------------------------
void MainWin::_appendToPlot(QCustomPlot *plot,
                            const double key,
                            const double value,
                            const std::vector<double> fitKey,
                            const std::vector<double> fitValue,
                            const int scrollRange)
{
  // add data to lines:
  plot->graph(0)->addData(key, value);

  // rescale value (vertical) axis to fit the current data:
  plot->graph(0)->rescaleValueAxis(true);

  // append fitted data:
  if (!fitValue.empty()) {
      plot->graph(1)->data()->clear();
      for (unsigned int i = 0; i < fitKey.size(); i++)
          plot->graph(1)->addData(fitKey.at(i), fitValue.at(i));
      plot->graph(1)->rescaleValueAxis(true);
  }

  // make key axis range scroll with the data:
  plot->xAxis->setRange(key, scrollRange, Qt::AlignRight);
  plot->replot();
}

//-----------------------------------------------------------------------------
void MainWin::_updatePlots(const SensBuffer sensData, const SensBuffer fitData,
                           const int scrollRange)
{
    auto t = sensData.back().t;
    auto packet = sensData.back();

    unsigned int N = fitData.size();
    std::vector<double> tFit(N);
    std::vector<double> accX(N), accY(N), accZ;
    std::vector<double> gyroX, gyroY(N), gyroZ(N);
    std::vector<double> angX, angY(N), angZ;

    for(unsigned int i = 0; i < tFit.size(); i++) {
        tFit.at(i) = fitData.at(i).t;

        accX.at(i) = fitData.at(i).acc.x;
        accY.at(i) = fitData.at(i).acc.y;

        gyroY.at(i) = fitData.at(i).gyro.y;
        gyroZ.at(i) = fitData.at(i).gyro.z;

        angY.at(i) = fitData.at(i).ang.y;
    }
    _appendToPlot(ui->wid11, t, packet.acc.x, tFit, accX, scrollRange);
    _appendToPlot(ui->wid21, t, packet.acc.y, tFit, accY, scrollRange);
    _appendToPlot(ui->wid31, t, packet.acc.z, tFit, accZ, scrollRange);

    _appendToPlot(ui->wid12, t, packet.gyro.x, tFit, gyroX, scrollRange);
    _appendToPlot(ui->wid22, t, packet.gyro.y, tFit, gyroY, scrollRange);
    _appendToPlot(ui->wid32, t, packet.gyro.z, tFit, gyroZ, scrollRange);

    _appendToPlot(ui->wid13, t, packet.ang.x, tFit, angX, scrollRange);
    _appendToPlot(ui->wid23, t, packet.ang.y, tFit, angY, scrollRange);
    _appendToPlot(ui->wid33, t, packet.ang.z, tFit, angZ, scrollRange);
}
