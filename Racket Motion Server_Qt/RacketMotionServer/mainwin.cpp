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
    connect(_sensServer, SIGNAL(sendState(const QString)),
            this, SLOT(showState(const QString)));
    connect(_sensServer, SIGNAL(sendSensData(const SensData)),
            this, SLOT(rcvSensData(const SensData)));

    // Line edit objects
    connect(ui->localPortLnEd, SIGNAL(editingFinished()),
            this, SLOT(portChanged()));
}

//-----------------------------------------------------------------------------
void MainWin::portChanged()
{
    emit(_sensServer->setListenIPPort(ui->localPortLnEd->text()));
}

//-----------------------------------------------------------------------------
void MainWin::rcvSensData(const SensData sensData)
{
    // calculate frames per second:
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // time elapsed since start, in seconds
    double t1 = time.elapsed();

    static double lastFpsKey;
    static int frameCount;
    double dtPlot = t1 - _prevPlotTimePoint;

    // Plot only each PLOT_TIME_MS:
    if (dtPlot >= PLOT_TIME_MS)  // in ms
    {
        const unsigned int scrollRange = ui->potBufSzLnEd->text().toInt();
        _appendToPlot(ui->wid11, sensData.timeStamp, sensData.accX, scrollRange);
        _appendToPlot(ui->wid21, sensData.timeStamp, sensData.accY, scrollRange);
        _appendToPlot(ui->wid31, sensData.timeStamp, sensData.accZ, scrollRange);

        _appendToPlot(ui->wid12, sensData.timeStamp, sensData.gyroX, scrollRange);
        _appendToPlot(ui->wid22, sensData.timeStamp, sensData.gyroY, scrollRange);
        _appendToPlot(ui->wid32, sensData.timeStamp, sensData.gyroZ, scrollRange);

        _appendToPlot(ui->wid13, sensData.timeStamp, sensData.magX, scrollRange);
        _appendToPlot(ui->wid23, sensData.timeStamp, sensData.magY, scrollRange);
        _appendToPlot(ui->wid33, sensData.timeStamp, sensData.magZ, scrollRange);

        ++frameCount;
        _prevPlotTimePoint = t1;

        ui->plotTimeMSLabel->setText(QString::number(dtPlot, 'f', 2));
        ui->FPSLabel->setText(QString::number(frameCount/(key-lastFpsKey), 'f', 2));
        ui->totDataPtLabel->setText(QString::number(ui->wid11->graph(0)->data()->size()));
        ui->transferTimeMSLabel->setText(
                    QString::number(
                        (sensData.timeStamp - _prevTimeStamp) * 1000, 'f', 2));
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
   _prevTimeStamp = sensData.timeStamp;
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
    ui->potBufSzLnEd->setValidator(new QIntValidator(5, 1000, this));
}


//-----------------------------------------------------------------------------
void MainWin::setUpPlots()
{
    const QString timeFormat = "%h:%m:%s";  // "%h:%m:%s:%z"
    const QString timeAxisLabel = "t, h:m:s";

    // acceleration plots:
    _setUpPlot(ui->wid11, QColor(40, 110, 255), timeFormat, timeAxisLabel, "a_x, m/s^2");
    _setUpPlot(ui->wid21, QColor(40, 110, 255), timeFormat, timeAxisLabel, "a_y, m/s^2");
    _setUpPlot(ui->wid31, QColor(40, 110, 255), timeFormat, timeAxisLabel, "a_z, m/s^2");

    // gyro plots:
    _setUpPlot(ui->wid12, QColor(40, 110, 255), timeFormat, timeAxisLabel, "w_x, rad/s");
    _setUpPlot(ui->wid22, QColor(40, 110, 255), timeFormat, timeAxisLabel, "w_y, rad/s");
    _setUpPlot(ui->wid32, QColor(40, 110, 255), timeFormat, timeAxisLabel, "w_z, rad/s");

    // magnetic field plots:
    _setUpPlot(ui->wid13, QColor(40, 110, 255), timeFormat, timeAxisLabel, "M_x, uT");
    _setUpPlot(ui->wid23, QColor(40, 110, 255), timeFormat, timeAxisLabel, "M_y, uT");
    _setUpPlot(ui->wid33, QColor(40, 110, 255), timeFormat, timeAxisLabel, "M_z, uT");
}

//-----------------------------------------------------------------------------
void MainWin::_setUpPlot(QCustomPlot *plot, const QColor color,
                         const QString timeFormat,
                         const QString xLabel, const QString yLabel)
{
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(color));

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
void MainWin::_appendToPlot(QCustomPlot *plot, const double key,
                            const double value, const int scrollRange)
{
  // add data to lines:
  plot->graph(0)->addData(key, value);

  // rescale value (vertical) axis to fit the current data:
  plot->graph(0)->rescaleValueAxis(true);

  // make key axis range scroll with the data:
  plot->xAxis->setRange(key, scrollRange, Qt::AlignRight);
  plot->replot();
}

//-----------------------------------------------------------------------------
void MainWin::realTimeDataSlot()
{
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.0001) // at most add point every 0.1 ms
    {
      foreach(QCustomPlot* plot, _plotsList)
        _appendToPlot(plot, key, qSin(key)
                      + qrand()/(double)RAND_MAX*1*qSin(key/0.3843));

      lastPointKey = key;
    }

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(ui->wid11->graph(0)->data()->size())
            , 0);
      lastFpsKey = key;
      frameCount = 0;
    }
}
