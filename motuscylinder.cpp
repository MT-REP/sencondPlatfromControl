#include "motuscylinder.h"
#include "ui_motuscylinder.h"
#include "mainwindow.h"
MotusCylinder::MotusCylinder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusCylinder)
{
    ui->setupUi(this);
    addMark=false;
    subMark=false;
}

//析构函数
MotusCylinder::~MotusCylinder()
{
    delete ui;
}

//手动按键
void MotusCylinder::on_handButton_clicked()
{
    emit sendHandCmd(S_CMD_JOG);
    #ifdef MotusRelase
        ui->handButton->setEnabled(false);
    #endif
}


//确认按键
void MotusCylinder::on_sureButton_clicked()
{
    unsigned char merve=0;
    if(ui->checkBox1->isChecked()) merve+=0x01;
    if(ui->checkBox2->isChecked()) merve+=0x02;
    if(ui->checkBox3->isChecked()) merve+=0x04;
    if(ui->checkBox4->isChecked()) merve+=0x08;
    if(ui->checkBox5->isChecked()) merve+=0x10;
    if(ui->checkBox6->isChecked()) merve+=0x20;
    emit sendHandMerve(merve);
}

void MotusCylinder::forbiddenButton()//手动，加减按钮不可操作
{
    ui->handButton->setEnabled(false);
    ui->addButton->setEnabled(false);
    ui->subButton->setEnabled(false);
}

//加号按键
void MotusCylinder::on_addButton_clicked()
{
    float data=ui->lineEdit->text().toFloat();
    if(!subMark)
        return;
    if(!addMark)
    {
        emit sendHandValue(data);
        addMark=true;
    }
    else
    {
        emit sendHandValue(0);
        ui->lineEdit->setText("0");
        addMark=false;
    }

}

//减号按键
void MotusCylinder::on_subButton_clicked()
{
    float data=ui->lineEdit->text().toFloat();
    if(!addMark)
        return;
    if(!subMark)
    {
        emit sendHandValue(-1.f*data);
        subMark=true;
    }
    else
    {
        emit sendHandValue(0);
        ui->lineEdit->setText("0");
        subMark=false;
    }
}

