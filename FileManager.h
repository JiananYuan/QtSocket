#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QFile>
#include <QTextStream>
class FileManager{
/*
 *
 */
private:
    QFile csvfile;
    QTextStream out;
public:
//    QDateTime time = QDateTime::currentDateTime();
//        QString dateTime = time.toString("MM-dd-hh-mm");
//        QString str= QString("Data%1.dat").arg(dateTime);

    FileManager();
    ~FileManager();
    bool write(QList<float> data);


};

#endif // FILEMANAGER_H
