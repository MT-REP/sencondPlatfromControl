#include "motusinsidecmd.h"
#include "ui_motusinsidecmd.h"
#include "motusserialdialog.h"
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
}

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

//
void MotusInsideCmd::on_netButton_clicked()
{

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
