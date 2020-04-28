#include "motusinsidecmd.h"
#include "ui_motusinsidecmd.h"
#include "motusserialdialog.h"
#include "motusrecvdataviewdialog.h"
#include "motusvirtualplatfromdialog.h"
#include <QProcess>
#include <QDir>
#include <QStringList>
#include <QDebug>
MotusInsideCmd::MotusInsideCmd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusInsideCmd)
{
    ui->setupUi(this);
    serialOpen=false;
    recvNetOpen=false;
    virtualOpen=false;
}

//析构函数
MotusInsideCmd::~MotusInsideCmd()
{
    delete ui;
}

//Tf配卡功能
void MotusInsideCmd::on_TFButton_clicked()
{
    if(CheckAppStatus("MotusConfiguration.exe"))
        return;
    QString strdir =QDir::currentPath();
    strdir+="\\MotusConfiguration.exe";
    QProcess::startDetached(strdir,QStringList());
}

bool MotusInsideCmd::CheckAppStatus(const QString &appName)
{
    QProcess process;
    //执行tasklist程序
    process.start("tasklist" ,QStringList()<<"/FI"<<"imagename eq " +appName);
    //阻塞5秒等待tasklist程序执行完成，超过五秒则直接返回
    process.waitForFinished(5000);
    //把tasklist程序读取到的进程信息输出到字符串中
    QString outputStr = QString::fromLocal8Bit(process.readAllStandardOutput());
    if(outputStr.contains(appName))
    {
        //用完记得把process关闭了，否则如果重新调用这个函数可以会失败
        process.close();
        return true;
    }
    else
    {
        process.close();
        return false;
    }
}

//串口打印窗口
void MotusInsideCmd::on_serialButton_clicked()
{
    if(!serialOpen)
    {
        serialOpen=true;
        MotusSerialDialog *mMotusSerialDialog= new MotusSerialDialog(this);
        connect(mMotusSerialDialog,SIGNAL(sendClose(bool)),this,SLOT(recvSerialPortOpen(bool)));
        mMotusSerialDialog->show();
    }
}

//判断串口窗口是否关闭
void MotusInsideCmd::recvSerialPortOpen(bool open)
{
    serialOpen=open;
}

//网络接收按键
void MotusInsideCmd::on_netButton_clicked()
{
    if(!recvNetOpen)
    {
        recvNetOpen=true;
        emit sendNetDataView(true);
        MotusRecvDataViewDialog *mMotusRecvDataViewDialog= new MotusRecvDataViewDialog(this);
        connect(mMotusRecvDataViewDialog,SIGNAL(sendClose(bool)),this,SLOT(recvNetDataOpen(bool)));
        connect(this,SIGNAL(sendNetData(MotusDataToHost&)),mMotusRecvDataViewDialog,SLOT(recvNetData(MotusDataToHost&)));
        mMotusRecvDataViewDialog->show();
    }
}

//接收数据开关
void MotusInsideCmd::recvNetDataOpen(bool open)
{
    recvNetOpen=open;
    emit sendNetDataView(false);
}


//虚拟下位机
void MotusInsideCmd::on_virtualButton_clicked()
{
    if(!virtualOpen)
    {
        virtualOpen=true;
        MotusVirtualPlatfromDialog *mMotusVirtualPlatfromDialog= new MotusVirtualPlatfromDialog(this);
        connect(mMotusVirtualPlatfromDialog,SIGNAL(sendClose(bool)),this,SLOT(recvVirtualOpen(bool)));
        mMotusVirtualPlatfromDialog->show();
    }
}

//接收开关
void MotusInsideCmd::recvVirtualOpen(bool open)
{
    virtualOpen=open;
}
