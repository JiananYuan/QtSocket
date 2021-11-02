#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileMgr = NULL;
}

MainWindow::~MainWindow()
{
    ui->mchat->m_bindIO->stop();
    delete ui;
    disconnect(ui->start, SIGNAL(clicked()), this, SLOT(onClickStart()));
    disconnect(ui->stop, SIGNAL(clicked()), this, SLOT(onClickStop()));
    if (fileMgr){
        disconnect(ui->mchat, SIGNAL(newData(QList<float>)),this, SLOT(writeData(QList<float>)));

        fileMgr = NULL;
    }
}

bool MainWindow::init(Msession* session)
{
    ui->mchat->setSession(session);
    ui->stop->setDisabled(true);
    ui->start->setEnabled(true);
    ui->mchat->start();
// TODO: connect slots and signals
    connect(ui->start, SIGNAL(clicked()), this, SLOT(onClickStart()));
    connect(ui->stop, SIGNAL(clicked()), this, SLOT(onClickStop()));

    return true;

}

void MainWindow::onClickStop()
{
    ui->stop->setEnabled(false);
    ui->start->setEnabled(true);
    disconnect(ui->mchat->m_bindIO, SIGNAL(newData(QList<float>)),this, SLOT(writeData(QList<float>)));
    delete fileMgr;
    fileMgr = NULL;

}

void MainWindow::onClickStart()
{
    //
    ui->stop->setEnabled(true);
    ui->start->setEnabled(false);
    connect(ui->mchat->m_bindIO, SIGNAL(newData(QList<float>)),this, SLOT(writeData(QList<float>)));

}

void MainWindow::writeData(QList<float> data){
    if (fileMgr == NULL){
        fileMgr = new FileManager();
    }
    fileMgr->write(data);

}
