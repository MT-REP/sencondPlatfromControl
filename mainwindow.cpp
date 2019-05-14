#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "motustimer.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //mMotusTimer=new MotusTimer(10);
    //connect(mMotusTimer,SIGNAL(timeout()),this,SLOT(masterClock()));
    //mMotusTimer->start();
    QMessageBox box;
    box.setText("连接播放器的IP出现\r\n网络错误");
    box.exec();
}

//析构函数
MainWindow::~MainWindow()
{
    delete ui;
}

//主时钟
void MainWindow::masterClock(void)
{
    //static int count=0;
    //count++;
    //ui->lineEdit->setText(QString("%1").arg(count));

}
