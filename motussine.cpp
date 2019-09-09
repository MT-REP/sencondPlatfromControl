#include "motussine.h"
#include "ui_motussine.h"
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>

MotusSine::MotusSine(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusSine)
{
    ui->setupUi(this);
    QRegExp rx("^[0-9]+[.]?[0-9]+$");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->totalTimeEdit->setValidator(new QIntValidator(0, 9999, this));
    //限制输入格式
    ui->accEdit->setValidator(validator);
    ui->accedit->setValidator(validator);
    ui->freEdit->setValidator(validator);
    ui->freedit->setValidator(validator);
    //赋值
    value[0]=ui->zongqingvalueEdit;
    fre[0]=ui->zongqingfreEdit;
    phase[0]=ui->zongqingphaseEdit;

    value[1]=ui->hengyaovalueEdit;
    fre[1]=ui->hengyaofreEdit;
    phase[1]=ui->hengyaophaseEdit;

    value[2]=ui->hangxiangvalueEdit;
    fre[2]=ui->hangxiangfreEdit;
    phase[2]=ui->hangxiangphaseEdit;

    value[3]=ui->hengyivalueEdit;
    fre[3]=ui->hengyifreEdit;
    phase[3]=ui->hengyiphaseEdit;

    value[4]=ui->qianchongvalueEdit;
    fre[4]=ui->qianchongfreEdit;
    phase[4]=ui->qianchongphaseEdit;

    value[5]=ui->shengjiangvalueEdit;
    fre[5]=ui->shengjiangfreEdit;
    phase[5]=ui->shengjiangphaseEdit;

    for(int i=0;i<6;i++)
    {
        //输入限制
        value[i]->setValidator(validator);
        fre[i]->setValidator(validator);
        phase[i]->setValidator(validator);
        //赋值初始值
        value[i]->setText(QString("%1").arg(0));
        fre[i]->setText(QString("%1").arg(0.1f));
        phase[i]->setText(QString("%1").arg(0));
    }
    ui->buffStopButton->setEnabled(false);
}

//析构函数
MotusSine::~MotusSine()
{
    delete ui;
}

//速度 加速度
void MotusSine::on_calButton_clicked()
{
    float value=ui->valueEdit->text().toFloat();
    float fre=ui->freEdit->text().toFloat();
    ui->speedEdit->setText(QString("%1").arg(QString::number(6.2832f*value*fre,'f',2)));
    ui->accEdit->setText(QString("%1").arg(QString::number(39.4786f*value*fre*fre,'f',2)));
}

//幅值 频率
void MotusSine::on_calbutton_clicked()
{
    float speed=ui->speededit->text().toFloat();
    float acc=ui->accedit->text().toFloat();
    ui->freedit->setText(QString("%1").arg(QString::number(acc/(speed*6.2832f),'f',2)));
    ui->valueedit->setText(QString("%1").arg(QString::number(speed*speed/acc,'f',2)));
}

//正弦开始
void MotusSine::on_sinStartButton_clicked()
{
    float valueEdit[6];
    float freEdit[6];
    float phaseEdit[6];
    int totaltime;
    totaltime=ui->totalTimeEdit->text().toInt();
    for(int i=0;i<6;i++)
    {
        valueEdit[i]=value[i]->text().toFloat();
        freEdit[i]=fre[i]->text().toFloat();
        phaseEdit[i]=phase[i]->text().toFloat();
    }
    ui->sinStartButton->setEnabled(false);
    ui->buffStopButton->setEnabled(true);
    emit sendSinData(totaltime,valueEdit,freEdit, phaseEdit);
}

//正弦停止
void MotusSine::on_buffStopButton_clicked()
{
    ui->buffStopButton->setEnabled(false);
    emit sendSinStop();
}

//接受时间显示
 void MotusSine::recvTimeView(float timeView)
 {
    int judge=int(timeView*100.f);
    if(judge==0)
    {
        ui->sinStartButton->setEnabled(true);
        ui->buffStopButton->setEnabled(false);
    }
    ui->bufferTimeEdit->setText(QString("%1").arg(QString::number(timeView,'f',2)));
 }

 //正弦中断
 void MotusSine::recvSinInterrupt(void)
 {
     ui->sinStartButton->setEnabled(true);
     ui->buffStopButton->setEnabled(false);
     ui->bufferTimeEdit->setText("0");
 }
