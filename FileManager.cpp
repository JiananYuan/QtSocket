#include "FileManager.h"
#include <QDateTime>
#include <QTextStream>

FileManager::FileManager(){
    QDateTime time = QDateTime::currentDateTime();
    QString dateTime = time.toString("MM-dd-hh-mm-ss");
    QString str= QString("D:/qt_program/foot_vibration/collected_data/Data%1.csv").arg(dateTime);
    this->csvfile.setFileName(str);

    if (!csvfile.open(QIODevice::WriteOnly)){
        return;
    }

    out.setDevice(&csvfile);
    if (out.device() == &csvfile)
        printf("1: init successfully.");
}

FileManager::~FileManager(){
    out << "\r\n";
    out.flush();
    csvfile.close();
}

bool FileManager::write(QList<float> data){
    for(int i =0; i < data.length(); i++){
        out << data[i] <<",";
    }
}
