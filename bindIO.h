#ifndef RECEIVERIODEVICE_H
#define RECEIVERIODEVICE_H

#include "socket_service.h"
#include <pthread.h>
#include <QIODevice>
#include "assert.h"
#include <QtCharts/QXYSeries>
#include <QMetaType>

QT_CHARTS_BEGIN_NAMESPACE
class QXYSeries;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

#define FilterSize 5
#define MAXSIZE DATA_SIZE_IN_BYTE

class MyFilters{
    //b =[1 -0.9375], a = [1 0]
protected:
    float a[FilterSize];
    float b[FilterSize];
    double buffx[FilterSize];
    double buffy[FilterSize];
public:
    MyFilters(float* atmp, float* btmp) {
        for (int i = 0; i < FilterSize; i++) {
            a[i] = atmp[i];
            b[i] = btmp[i];
        }
    }
    ~MyFilters(){}

    float filter(float x)   {
        double tmpy = b[0] * x;
        int var;
        for (var = FilterSize - 1; var > 0 ; var--) {
            tmpy += b[var] * buffx[var-1] - a[var] * buffy[var-1];

            buffx[var] = buffx[var - 1];
            buffy[var] = buffy[var - 1];
        }
        buffx[var] = x;
        buffy[var] = tmpy;
        return (float)tmpy;
    }
};



class BindIO : public QObject,public DataCenter {

    Q_OBJECT
private:
    QXYSeries *m_series;
    pthread_t ttmp;
    MyFilters *m_filters;
    bool isWorking = false;
signals:
    void newData(QList<float> data);
public:
    explicit BindIO(QXYSeries * series, Msession * csession);

    int start();

    void stop();

    static void * update(void *arg);

    template<typename T>
    qint64 writeData(const T *data, qint64 maxSize){
//        assert(maxSize == 5000);
        qint64 range = DATA_SIZE_IN_BYTE;
        QVector<QPointF> oldPoints = m_series->pointsVector();
        QVector<QPointF> points;
        int resolution = 1;
        if( oldPoints.count() < range){
            points = m_series->pointsVector();
        } else {
            for (int i = maxSize/resolution; i < oldPoints.count() ; i++) {
                points.append(QPoint(i - maxSize / resolution, oldPoints.at(i).y()));
            }
        }

        qint64 size = points.count();
        for (int k = 0;k < maxSize/resolution; k++) {
            points.append(QPointF(k + size,data[k * resolution]));
        }


        m_series->replace(points);
        return maxSize;
    }
};



#endif // RECEIVERIODEVICE_H
