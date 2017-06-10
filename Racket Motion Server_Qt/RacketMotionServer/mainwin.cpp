#include "mainwin.h"
#include "ui_mainwin.h"

//-----------------------------------------------------------------------------
MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWin),
    _dataTable(_generateRandomData(1, 10, 7))
{
    ui->setupUi(this);

    // create layout
    QGridLayout *baseLayout = new QGridLayout();

    // create charts
    QChartView *chartView;
    unsigned int cols = 3;
    unsigned int rows = 3;
    for (unsigned int i = 0; i < cols; i++) {
        for (unsigned int j = 0; j < rows; j++) {
            chartView = new QChartView(_lineChart(_dataTable, "x", "y"));
            baseLayout->addWidget(chartView, j, i);
            _charts << chartView;
        }
    }
    ui->ChartWidget->setLayout(baseLayout);
    _updateCharts();
}

//-----------------------------------------------------------------------------
MainWin::~MainWin()
{
    delete ui;
}

//-----------------------------------------------------------------------------
void MainWin::connectSignals()
{
    // connect(m_themeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUI()));
}

//-----------------------------------------------------------------------------
void MainWin::_updateCharts()
{
    QChart::ChartTheme theme = QChart::ChartThemeLight;

    if (_charts.at(0)->chart()->theme() != theme) {
        foreach (QChartView *chartView, _charts)
            chartView->chart()->setTheme(theme);

        QPalette pal = window()->palette();
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        window()->setPalette(pal);
    }

    foreach (QChartView *chart, _charts)
        chart->setRenderHint(QPainter::Antialiasing, true);

    foreach (QChartView *chartView, _charts)
        chartView->chart()->legend()->hide();
}

//-----------------------------------------------------------------------------
QChart *MainWin::_lineChart(DataTable data, const QString xLabel,
                           const QString yLabel) const
{
    QChart *chart = new QChart();

    QString name("Series ");
    int nameIndex = 0;
    foreach (DataList list, data) {
        QLineSeries *series = new QLineSeries(chart);
        foreach (Data data, list)
            series->append(data.first);
        series->setName(name + QString::number(nameIndex));
        nameIndex++;
        chart->addSeries(series);

        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText(xLabel);
        axisX->setLabelFormat("%.2f");
        axisX->setTickCount(series->count());
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText(yLabel);
        axisY->setLabelFormat("%.2f");
        axisY->setTickCount(series->count());
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }
    return chart;
}

//-----------------------------------------------------------------------------
DataTable MainWin::_generateRandomData(int listCount, int valueMax,
                                      int valueCount) const
{
    DataTable dataTable;

    // set seed for random stuff
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    // generate random data
    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + (qreal)(qrand() % valueMax) / (qreal) valueCount;
            QPointF value((j + (qreal) rand() / (qreal) RAND_MAX) * ((qreal) valueMax / (qreal) valueCount),
                          yValue);
            QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }

    return dataTable;
}
