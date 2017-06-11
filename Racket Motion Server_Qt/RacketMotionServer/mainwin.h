#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QtCore/QTime>

#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>

#include <qcustomplot/qcustomplot.h>

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public slots:
    void realTimeDataSlot();

public:
    explicit MainWin(QWidget *parent = 0);
    ~MainWin();

    // Connects signals
    void connectSignals();

    void setUpPlots();

private Q_SLOTS:

private:  // functions
    void _setUpPlot(QCustomPlot *plot, const QColor color = QColor(40, 110, 255),
                    const QString timeFormat = "%m:%s",
                    const QString xLabel = "x", const QString yLabel = "y");
    void _plot(QCustomPlot *plot, const double key, const double value);

private:  // variables
    Ui::MainWin *ui;

    QList<QCustomPlot*> _plotsList;
    QTimer _dataTimer;
};

#endif // MAINWIN_H
