#include "motussavedata.h"
#include "ui_motussavedata.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
MotusSaveData::MotusSaveData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusSaveData)
{
    ui->setupUi(this);
}

//析构函数
MotusSaveData::~MotusSaveData()
{
    delete ui;
}

//保存按键
void MotusSaveData::on_saveDataButton_clicked()
{
    emit sendCarryOut();
}

//数据保存
void MotusSaveData::on_datasaveCheckBox_clicked(bool checked)
{
    bool isSave[6]={false,false,false,false,false,false};
if(checked)
    {
        isSave[0]=ui->zongqingCheckBox->isChecked();
        isSave[1]=ui->hengyaoCheckBox->isChecked();
        isSave[2]=ui->hangxiangCheckBox->isChecked();
        isSave[3]=ui->hengyiCheckBox->isChecked();
        isSave[4]=ui->qianchongCheckBox->isChecked();
        isSave[5]=ui->shengjiangCheckBox->isChecked();
        emit sendDataIsSave(isSave,checked);
    }
}

//数据保存执行函数
void MotusSaveData::recvDataCarryOut(MDataSave &mMDataSave)
{
    ui->datasaveCheckBox->setCheckState(Qt::Unchecked);
    QDateTime time;
    time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy_MM_dd_hh_mm_ss");
    str+=".txt";
    QFile file;
    file.setFileName(str);
    file.open(QIODevice::Text|QIODevice::WriteOnly|QIODevice::Truncate);
    QTextStream out(&file);
    for ( int i=0; i!=mMDataSave.dataList.size(); ++i )
    {
         if(mMDataSave.isSave[0])
         {
             out<<mMDataSave.dataList.at(i).data[0];
             out<<",";
             out<<mMDataSave.dataList.at(i).data[1];
             if(mMDataSave.isSave[1]||mMDataSave.isSave[2]||mMDataSave.isSave[3]||mMDataSave.isSave[4]||mMDataSave.isSave[5])
                out<<",";
         }
         if(mMDataSave.isSave[1])
         {
             out<<mMDataSave.dataList.at(i).data[2];
             out<<",";
             out<<mMDataSave.dataList.at(i).data[3];
             if(mMDataSave.isSave[2]||mMDataSave.isSave[3]||mMDataSave.isSave[4]||mMDataSave.isSave[5])
                out<<",";
         }
         if(mMDataSave.isSave[2])
         {
             out<<mMDataSave.dataList.at(i).data[4];
             out<<",";
             out<<mMDataSave.dataList.at(i).data[5];
             if(mMDataSave.isSave[3]||mMDataSave.isSave[4]||mMDataSave.isSave[5])
                out<<",";
         }
         if(mMDataSave.isSave[3])
         {
             out<<mMDataSave.dataList.at(i).data[6];
             out<<",";
             out<<mMDataSave.dataList.at(i).data[7];
             if(mMDataSave.isSave[4]||mMDataSave.isSave[5])
                out<<",";
         }
         if(mMDataSave.isSave[4])
         {
             out<<mMDataSave.dataList.at(i).data[8];
             out<<",";
             out<<mMDataSave.dataList.at(i).data[9];
             if(mMDataSave.isSave[5])
                out<<",";
         }
         if(mMDataSave.isSave[5])
         {
             out<<mMDataSave.dataList.at(i).data[10];
             out<<",";
             out<<mMDataSave.dataList.at(i).data[11];
         }
         out<<'\n';
    }
    file.close();
    mMDataSave.dataList.clear();
}




























