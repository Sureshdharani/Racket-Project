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

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public slots:
    void showState(const QString);
    void rcvSensData(const SensData sensData, const FitSensData fitData);
    void portChanged();
    void fitWinLenChnged();

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
                       std::vector<double> t, std::vector<double> fit,
                       const int scrollRange = 10);
    void _updatePlots(const SensData sensData, const FitSensData fitData,
                      const int scrollRange = 10);

private:  // variables
    Ui::MainWin *ui;

    RacketSensorServer* _sensServer;
    QList<QCustomPlot*> _plotsList;
    double _prevProcessTimePoint;
    double _prevPlotTimePoint;
    double _prevTimeStamp;
};

#endif // MAINWIN_H
