#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "socket_service.h"
#include "FileManager.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 初始化组件，绑定相关socket服务
    bool init(Msession* session);


private slots:
    void onClickStart();
    void onClickStop();
    void writeData(QList<float> data);
private:
    Ui::MainWindow *ui;
    FileManager *fileMgr;
};

#endif // MAINWINDOW_H
