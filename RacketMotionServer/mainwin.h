#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QtCore/QTime>

#include <QThread>
#include <QtConcurrent>
#include <QtConcurrentRun>

#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>

#include <qcustomplot/qcustomplot.h>
#include <racketsensorserver.h>
#include <fitfunctions.h>

#define FPS_NUMS_MEAN 5

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public slots:
    void showState(const QString);
    void rcvSensData(const SensBuffer sensData,
                     const SensBuffer fitData,
                     const int score,
                     const unsigned int scoreCnt,
                     const double transferTime,
                     const double predictionTime);
    void portChanged();
    void fitWinLenChnged();
    void phoneSelected(int);
    void edissonSelected(int);

public:
    explicit MainWin(QWidget *parent = 0);
    ~MainWin();

    // Connects signals
    void connectSignals();
    void setUpPlots();
    void setUpGUI();

private Q_SLOTS:

private:  // functions
    void _setUpPlot(QCustomPlot *plot, const QString timeFormat = "%m:%s",
                    const QString xLabel = "x", const QString yLabel = "y");
    void _appendToPlot(QCustomPlot *plot, const double key, const double value,
                       const std::vector<double> fitKey,
                       const std::vector<double> fitValue,
                       const int scrollRange = 10);
    void _updatePlots(const SensBuffer sensData, const SensBuffer fitData,
                      const int scrollRange = 10);

private:  // variables
    Ui::MainWin *ui;

    RacketSensorServer* _sensServer;
    QList<QCustomPlot*> _plotsList;

    // Variables for processing timing:
    double _FPS;  // fps
    std::vector<double> _fpsVec;  // fps vector for mean value calculation
    double _prevPlotTime;  // previous plot time
};

#endif // MAINWIN_H
