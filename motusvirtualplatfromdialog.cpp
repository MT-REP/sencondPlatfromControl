#include "motusvirtualplatfromdialog.h"
#include "ui_motusvirtualplatfromdialog.h"

MotusVirtualPlatfromDialog::MotusVirtualPlatfromDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusVirtualPlatfromDialog)
{
    ui->setupUi(this);
    recvCount=0;
    sendCount=0;
    init();
    statusPlatInt[0] = 0;          statusPlatStr[0]="寻底初始化中-0";
    statusPlatInt[1]=  1;          statusPlatStr[1]="寻底初始化完成-1";
    statusPlatInt[2] = 2;		   statusPlatStr[2]="底位到中位运行中-2";
    statusPlatInt[3] = 3;		   statusPlatStr[3]="底位到中位运行完成-3";
    statusPlatInt[4] = 4;		   statusPlatStr[4]="正常运行中-4";
    statusPlatInt[5] = 5;		   statusPlatStr[5]="自动运行-5";
    statusPlatInt[6] = 6; 	       statusPlatStr[6]="改变运动中心点-6";
    statusPlatInt[7] = 7;          statusPlatStr[7]="配置驱动器信息-7";
    statusPlatInt[8] = 8;	       statusPlatStr[8]="脚本运行中-8";
    statusPlatInt[9] = 9;	       statusPlatStr[9]="脚本运行完成-9";
    statusPlatInt[10] = 10;		   statusPlatStr[10]="中位到顶位运动中-10";
    statusPlatInt[11] = 11;		   statusPlatStr[11]="中位到顶位运动完成-11";
    statusPlatInt[12] = 12;	       statusPlatStr[12]="中位到底位运动中-12";
    statusPlatInt[12] = 13;		   statusPlatStr[13]="中位到底位运动完成-13";
    statusPlatInt[14] = 14;        statusPlatStr[14]="回中位运行中-14";
    statusPlatInt[15] = 15;        statusPlatStr[15]="回中位运行中完成-15";
    statusPlatInt[16] = 32;        statusPlatStr[16]="手动模式-32";
    statusPlatInt[17] = 33;        statusPlatStr[17]="紧急停止-33";
    statusPlatInt[18] = 55;        statusPlatStr[18]="空闲状态-55";
    statusPlatInt[19] = 88;        statusPlatStr[19]="未连接设备-88";
    for(int i=0;i<20;i++)
        ui->statusBox->addItem(statusPlatStr[i]);
}

//析构函数
MotusVirtualPlatfromDialog::~MotusVirtualPlatfromDialog()
{
    delete ui;
}

//初始化函数
void MotusVirtualPlatfromDialog::init()
{
    QString name[6]={"曲线1","曲线2","曲线3","曲线4","曲线5","曲线6"};
    QColor color[6]={QColor(255,0,0),QColor(255,128,0),QColor(128,255,0),QColor(0,255,0),QColor(128,0,255),QColor(0,255,255)};
    mMotusSingleQwtplot[0].initPara(ui->qwtPlot1);
    mMotusSingleQwtplot[1].initPara(ui->qwtPlot2);
    mMotusSingleQwtplot[2].initPara(ui->qwtPlot3);
    mMotusSingleQwtplot[3].initPara(ui->qwtPlot4);
    mMotusSingleQwtplot[4].initPara(ui->qwtPlot5);
    mMotusSingleQwtplot[5].initPara(ui->qwtPlot6);
    for(int i=0;i<6;i++)
    {
        mMotusSingleQwtplot[i].setCurve(name[i],color[i]);
    }
}

//关闭按键函数
void MotusVirtualPlatfromDialog::closeEvent(QCloseEvent* event)
{
    emit sendClose(false);
    delete this;
}

//运行按键
void MotusVirtualPlatfromDialog::on_runButton_clicked()
{
    QString portstr = ui->controlPortlineEdit->text();
    int port = portstr.toInt();
    mMotusSocket.initSocket(port);
    QString strIp = ui->platfromlineEdit->text();
    portstr = ui->platfromPortlineEdit->text();
    port = portstr.toInt();
    mMotusSocket.setRemoteIpAndPort(strIp,port);
    connect(&mMotusSocket,SIGNAL(sendRecvData(char *,int)),this,SLOT(recvData(char *,int)));
    ui->runButton->setEnabled(false);
    ui->controlLineEdit->setEnabled(false);
    ui->platfromlineEdit->setEnabled(false);
    ui->controlPortlineEdit->setEnabled(false);
    ui->platfromPortlineEdit->setEnabled(false);
}

//接收数据
void MotusVirtualPlatfromDialog::recvData(char data[],int len)
{
    if(len ==  sizeof(MotusDataToPlatfrom))
    {
        memcpy((void*)&mMotusDataToPlatfrom,data,len);
        recvCount++;
        ui->recvCountlineEdit->setText(QString("%1").arg(recvCount));
        ui->mainlineEdit->setText(QString("%1").arg(mMotusDataToPlatfrom.Cmd));
        ui->sonlineEdit->setText(QString("%1").arg(mMotusDataToPlatfrom.SubCmd));
        for(int i=0;i<6;i++)
        {
            if(i>2)
                mMotusSingleQwtplot[i].lineView(mMotusDataToPlatfrom.DOFs[i]*1000.f);
            else
                mMotusSingleQwtplot[i].lineView(mMotusDataToPlatfrom.DOFs[i]);
        }
    }
}

void MotusVirtualPlatfromDialog::on_sendButton_clicked()
{
    int index =ui->statusBox->currentIndex();
    mMotusDataToHost.DOFStatus = statusPlatInt[index];
    mMotusSocket.sendData((char *)&mMotusDataToHost,sizeof(mMotusDataToHost));
    sendCount++;
    ui->sendCountlineEdit->setText(QString("%1").arg(sendCount));
}
