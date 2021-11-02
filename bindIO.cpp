#include "bindIO.h"
#include <assert.h>
//#include "bindIO.moc"

//class Receiver :  public QIODevice
//{
//    Q_OBJECT
//public:
//    explicit Receiver(QXYSeries * series, QObject *parent = 0);
//protected:
//    qint64 readData(char *data, qint64 maxSize);
//    qint64 writeData(const char *data, qint64 maxSize);
//private:
//    QXYSeries *m_series;
//} ;

//

BindIO::BindIO(QXYSeries * series, Msession * csession)
        : m_series(series)
{
    qRegisterMetaType<QList<float>>("QList<float>");
    this->m_session = csession;
//    float _const_b[] = {0.087589917605869,0,-0.175179835211738,0,0.087589917605869};
//    float _const_a[] = {1,-2.991481782466971,3.345973967555460,-1.711060439517900,0.356625398303518};
    float _const_b[] = {7.962523541803612e-06,0,-1.592504708360722e-05,0,7.962523541803612e-06};
    float _const_a[] = {1,-3.991994973162145,5.976024680942126,-3.976064410920967,0.992034703156509};

    // 初始化滤波器参数
    this->m_filters = new MyFilters(_const_a,_const_b);
}

int BindIO::start(){
    isWorking = true;
    int ret = pthread_create(&ttmp, NULL ,update, this);
    return ret;
}

void BindIO::stop(){
    isWorking = false;
    pthread_join(ttmp, NULL);
//    m_session->closeSession();
}

void * BindIO::update(void *arg){
     BindIO * self = (BindIO *) arg;
     size_t spear = 0;
     short data[MAXSIZE / 2];
     float filteredData[MAXSIZE / 2];

     while(self->isWorking){
         if(self->m_session->fetchData(spear, data)){
             qDebug("%ld", spear);
             for (int var = 0; var < MAXSIZE/2 ; ++var) {
//                filteredData[var] = self->m_filters->filter(data[var]);

                 filteredData[var] = data[var];
//                 qDebug("%3.1f", filteredData[var]);

             }
             QList<float> qdata;
             for (qint64 var = 0; var < MAXSIZE/2; var ++){
                 qdata.append(data[var]);
             }
             emit self->newData(qdata);

             self->writeData(filteredData,MAXSIZE/2);
         }

     }
     return nullptr;
 }
