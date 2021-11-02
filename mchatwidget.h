#ifndef MCHATWIDGET_H
#define MCHATWIDGET_H
#include "bindIO.h"
#include "socket_service.h"

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Receiver;


class Mchatwidget : public QWidget
{
    Q_OBJECT

public:
    Mchatwidget(QWidget *parent = 0);
    void setSession(Msession *session);
    void start();
    ~Mchatwidget();
    BindIO *m_bindIO;
protected:
    void paintEvent(QPaintEvent *event)Q_DECL_OVERRIDE; //绘图
private:
    QChart *m_chart;
    QLineSeries *m_series;

};
#endif // MCHATWIDGET_H
