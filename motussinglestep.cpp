#include "motussinglestep.h"
#include "ui_motussinglestep.h"

#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>

MotusSingleStep::MotusSingleStep(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusSingleStep)
{
    ui->setupUi(this);
    pos[0]=ui->zongqingposEdit;
    speed[0]=ui->zongqingspeedEdit;

    pos[1]=ui->hengyaoposEdit;
    speed[1]=ui->hengyaospeedEdit;

    pos[2]=ui->hangxiangposEdit;
    speed[2]=ui->hangxiangspeedEdit;

    pos[3]=ui->hengyiposEdit;
    speed[3]=ui->hengyispeedEdit;

    pos[4]=ui->qianchongposEdit;
    speed[4]=ui->qianchongspeedEdit;

    pos[5]=ui->shengjiangposEdit;
    speed[5]=ui->shengjiangspeedEdit;
    for(int i=0;i<6;i++)
    {
        attpos[i]=0.f;
        attspeed[i]=0.f;
    }
    initPara();
}

//初始化参数
void MotusSingleStep::initPara()
{
    QRegExp rx("^-?[0-9]+[.]?[0-9]+$");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    for(int i=0;i<6;i++)
    {
       pos[i]->setValidator(validator);
       speed[i]->setValidator(validator);
       pos[i]->setText(QString("%1").arg(0));
       if(i<3)
            speed[i]->setText(QString("%1").arg(1));
       else
            speed[i]->setText(QString("%1").arg(10));
    }
}

//得到参数
void MotusSingleStep::getPara()
{
    for(int i=0;i<6;i++)
    {
        attpos[i]=pos[i]->text().toFloat();
        attspeed[i]=speed[i]->text().toFloat()*3*3.1416f;
    }
}


//析构函数
MotusSingleStep::~MotusSingleStep()
{
    delete ui;
}

void MotusSingleStep::on_carryOutButton_clicked()
{
    getPara();
    emit sendSingleStepData(attpos,attspeed);
}

//设置按键是否可用
void MotusSingleStep::recvSingleStepButton(bool valid)
{
    ui->carryOutButton->setVisible(valid);
}






























