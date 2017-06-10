#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QtCore/QTime>

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QValueAxis>

#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QChartGlobal>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

QT_CHARTS_USE_NAMESPACE

struct NamedChart {
    QChartView* chart;
    QString name;

    NamedChart(QChartView* newChart, const QString newName) {
       chart = newChart;
       name = newName;
    }
};

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWin(QWidget *parent = 0);
    ~MainWin();

    // Connects signals
    void connectSignals();

private Q_SLOTS:
    void _updateCharts();

private:  // functions
    QChart *_lineChart(DataTable data = DataTable(),
                            const QString xLabel = "",
                            const QString yLabel = "") const;
    DataTable _generateRandomData(int listCount, int valueMax, int valueCount) const;

    QGridLayout *_createChartLayout();

    void _plotData(DataList accx, DataList accy, DataList accz,
                   DataList gyrox, DataList gyroy, DataList gyroz,
                   DataList magx, DataList magy, DataList magz);

    void _plotOnChart(QChartView* chart, DataList Data,
                      const QString xLabel, const QString yLabel);

private:  // variables
    Ui::MainWin *ui;

    // QList<QChartView*> _charts;
    QList<NamedChart> _namedCharts;
    DataTable _dataTable;
};

#endif // MAINWIN_H
