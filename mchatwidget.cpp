#include "mchatwidget.h"
#include "socket_service.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QValueAxis>

#include <QVBoxLayout>

#define _RANGE DATA_SIZE_IN_BYTE
void Mchatwidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter painter(this);
    // 画图
    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(400, 300);
    m_series = new QLineSeries;
    m_chart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, _RANGE);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-32768, 32767);
    axisY->setTitleText("Amplitute");
    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("Data");

//    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->addWidget(chartView);
//    setLayout(mainLayout);
}

Mchatwidget::Mchatwidget(QWidget *parent)
    : QWidget(parent),
      m_chart(0),
      m_series(0),
      m_bindIO(0)
{
    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(800, 600);
    m_series = new QLineSeries;
    m_chart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, _RANGE);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-32768, 32767);
    axisY->setTitleText("Amplitute");
    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("Data");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);

//    m_bindIO = new BindIO(m_series, session);

//    m_bindIO->start();
}

void Mchatwidget::setSession(Msession *session){
    if (m_bindIO != 0) m_bindIO->stop();
    m_bindIO = new BindIO(m_series, session);
}

void Mchatwidget::start(){
    if (m_bindIO == 0) return;
    m_bindIO->start();
}

Mchatwidget::~Mchatwidget(){
    m_bindIO->stop();
}



