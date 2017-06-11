#include "mainwin.h"
#include "ui_mainwin.h"

//-----------------------------------------------------------------------------
MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWin),
    _dataTable(_generateRandomData(9, 10, 1000))
{
    ui->setupUi(this);
    ui->ChartWidget->setLayout(_createChartLayout());

    _plotData(_dataTable.at(0), _dataTable.at(1), _dataTable.at(2),
              _dataTable.at(3), _dataTable.at(4), _dataTable.at(5),
              _dataTable.at(6), _dataTable.at(7), _dataTable.at(8));
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

    if (_namedCharts.at(0).chart->chart()->theme() != theme) {
        foreach (NamedChart namedChart, _namedCharts)
            namedChart.chart->chart()->setTheme(theme);

        QPalette pal = window()->palette();
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        window()->setPalette(pal);
    }

    foreach (NamedChart namedChart, _namedCharts)
        namedChart.chart->setRenderHint(QPainter::Antialiasing, true);

    foreach (NamedChart namedChart, _namedCharts)
        namedChart.chart->chart()->legend()->hide();
}

//-----------------------------------------------------------------------------
QChart *MainWin::_lineChart(DataTable data, const QString xLabel,
                           const QString yLabel) const
{
    QChart *chart = new QChart();
    int nameIndex = 0;
    foreach (DataList list, data) {
        QLineSeries *series = new QLineSeries(chart);
        foreach (Data data, list)
            series->append(data.first);
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

//-----------------------------------------------------------------------------
QGridLayout *MainWin::_createChartLayout()
{
    // create layout
    QGridLayout *baseLayout = new QGridLayout();

    // create charts
    QChartView *chartView;
    QString chartName;
    QString xLabel = "t, sec.";
    QString yLabel;

    // Acceleration charts:
    chartName = "acc_x";
    yLabel = "a_x, m/s^2";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 1, 0);
    _namedCharts << NamedChart(chartView, chartName);

    chartName = "acc_y";
    yLabel = "a_y, m/s^2";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 2, 0);
    _namedCharts << NamedChart(chartView, chartName);

    chartName = "acc_z";
    yLabel = "a_z, m/s^2";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 3, 0);
    _namedCharts << NamedChart(chartView, chartName);

    // Gyro charts:
    chartName = "gyro_x";
    yLabel = "gyro_x, rad/s";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 1, 1);
    _namedCharts << NamedChart(chartView, chartName);

    chartName = "gyro_y";
    yLabel = "gyro_y, rad/s";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 2, 1);
    _namedCharts << NamedChart(chartView, chartName);

    chartName = "gyro_z";
    yLabel = "gyro_z, rad/s";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 3, 1);
    _namedCharts << NamedChart(chartView, chartName);

    // Magnetic field charts:
    chartName = "mag_x";
    yLabel = "mag_x, uT";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 1, 2);
    _namedCharts << NamedChart(chartView, chartName);

    chartName = "mag_y";
    yLabel = "mag_y, uT";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 2, 2);
    _namedCharts << NamedChart(chartView, chartName);

    chartName = "mag_z";
    yLabel = "mag_z, uT";
    chartView = new QChartView(_lineChart(DataTable(), xLabel, yLabel));
    baseLayout->addWidget(chartView, 3, 2);
    _namedCharts << NamedChart(chartView, chartName);

    return baseLayout;
}

//-----------------------------------------------------------------------------
void MainWin::_plotData(DataList accx, DataList accy, DataList accz,
               DataList gyrox, DataList gyroy, DataList gyroz,
               DataList magx, DataList magy, DataList magz)
{
    const QString xLabel = "t, sec.";
    foreach(NamedChart namedChart, _namedCharts) {
        if (namedChart.name == "acc_x")
            _plotOnChart(namedChart.chart, accx, xLabel, "a_x, m/s^2");
        else if (namedChart.name == "acc_y")
            _plotOnChart(namedChart.chart, accy, xLabel, "a_y, m/s^2");
        else if (namedChart.name == "acc_z")
            _plotOnChart(namedChart.chart, accz, xLabel, "a_z, m/s^2");
        else if (namedChart.name == "gyro_x")
            _plotOnChart(namedChart.chart, gyrox, xLabel, "gyro_x, rad/s");
        else if (namedChart.name == "gyro_y")
            _plotOnChart(namedChart.chart, gyroy, xLabel, "gyro_y, rad/s");
        else if (namedChart.name == "gyro_z")
            _plotOnChart(namedChart.chart, gyroz, xLabel, "gyro_z, rad/s");
        else if (namedChart.name == "mag_x")
            _plotOnChart(namedChart.chart, magx, xLabel, "M_x, uT");
        else if (namedChart.name == "mag_y")
            _plotOnChart(namedChart.chart, magy, xLabel, "M_y, uT");
        else if (namedChart.name == "mag_z")
            _plotOnChart(namedChart.chart, magz, xLabel, "M_z, uT");
        namedChart.chart->chart()->legend()->hide();
        namedChart.chart->setRenderHint(QPainter::Antialiasing, true);
    }
}

//-----------------------------------------------------------------------------
void MainWin::_plotOnChart(QChartView* chart, DataList plotData,
                           const QString xLabel, const QString yLabel)
{
    QLineSeries *series = new QLineSeries(chart);
    foreach (Data data, plotData)
        series->append(data.first);
    chart->chart()->addSeries(series);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText(xLabel);
    axisX->setLabelFormat("%.2f");
    axisX->setTickCount(series->count());
    chart->chart()->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(yLabel);
    axisY->setLabelFormat("%.2f");
    axisY->setTickCount(series->count());
    chart->chart()->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}
