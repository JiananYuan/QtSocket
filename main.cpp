#include "mainwindow.h"
#include <QApplication>
#include "socket_service.h"
#include <pthread.h>
#define NUM_THREADS 2
void *ReadSession(void* tmp){
    Msession* a = ((Msession*) tmp);
    a->read();
    return (void*)0;
}
int main(int argc, char *argv[])
{
//    qDebug("1");
    auto mserver = Mserver::getInstance();
    struct sockaddr_in clientaddr;
    auto client = mserver->acceptClient(true, clientaddr);
    Msession tmp(client, clientaddr);
    tmp.setStatus(true);
    pthread_t ttmp;
    int ret = pthread_create(&ttmp, NULL, ReadSession, &tmp );
//    QDebug("1");
    QApplication a(argc, argv);



//    Mchatwidget w(0);
//    w.setSession(&tmp);
//    w.start();
//    w.show();
    MainWindow m;
    m.init(&tmp);
    m.show();
//    pthread_t ttmp;
//    int ret = pthread_create(&ttmp, NULL, ReadSession, &tmp );
//    short data[DATA_SIZE_IN_BYTE /2];
//    size_t spear = 0;
//    while(1){
//        if(tmp.fetchData(spear, data)){
//            for (int i = 0;i < DATA_SIZE_IN_BYTE / 2;i++) {
//                printf("%d ", data[i]);
//            }
//            printf("\n\n");
//        }

//    }
    return a.exec();
//    return 0;
}

//struct MsocketQueue{
//    const int size = NUM_THREADS;
//    pthread_t Queue[NUM_THREADS];
//};
