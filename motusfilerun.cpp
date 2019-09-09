#include "motusfilerun.h"
#include "ui_motusfilerun.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
MotusFileRun::MotusFileRun(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusFileRun)
{
    ui->setupUi(this);
    maxValueEdit[0]=ui->maxZongQinLineEdit;
    minValueEdit[0]=ui->minZongQinLineEdit;
    maxSpeedEdit[0]=ui->maxZongQinSpeedLineEdit;
    maxAccEdit[0]=ui->maxZongQinAccLineEdit;

    maxValueEdit[1]=ui->maxHengYaoLineEdit;
    minValueEdit[1]=ui->minHengYaoLineEdit;
    maxSpeedEdit[1]=ui->maxHengYaoSpeedLineEdit;
    maxAccEdit[1]=ui->maxHengYaoAccLineEdit;

    maxValueEdit[2]=ui->maxHangXiangLineEdit;
    minValueEdit[2]=ui->minHangXiangLineEdit;
    maxSpeedEdit[2]=ui->maxHangXiangSpeedLineEdit;
    maxAccEdit[2]=ui->maxHangXiangAccLineEdit;

    maxValueEdit[3]=ui->maxHengYiLineEdit;
    minValueEdit[3]=ui->minHengYiLineEdit;
    maxSpeedEdit[3]=ui->maxHengYiSpeedLineEdit;
    maxAccEdit[3]=ui->maxHengYiAccLineEdit;

    maxValueEdit[4]=ui->maxQianChongLineEdit;
    minValueEdit[4]=ui->minQianChongLineEdit;
    maxSpeedEdit[4]=ui->maxQianChongSpeedLineEdit;
    maxAccEdit[4]=ui->maxQianChongAccLineEdit;

    maxValueEdit[5]=ui->maxShengJiangLineEdit;
    minValueEdit[5]=ui->minShengJiangLineEdit;
    maxSpeedEdit[5]=ui->maxShengJiangSpeedLineEdit;
    maxAccEdit[5]=ui->maxShengJiangAccLineEdit;
    ui->progressBar->setRange(0,10);
    ui->progressBar->setValue(0);
    setInitValue();
}

//析构函数
MotusFileRun::~MotusFileRun()
{
    delete ui;
}

//选择文件
void MotusFileRun::on_selectFileButton_clicked()
{
    //选择文件
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("Txt (*.txt)"));
    //用户选择了文件
    if (!fileName.isNull())
    {
      setInitValue();
      if(readtxtfile(fileName))
      {
          messageView();
      }
    }
    else // 用户取消选择
    {
      QMessageBox::information(this, "Document", "No document", QMessageBox::Ok);
    }
}

//设置初始值
void MotusFileRun::setInitValue()
{
    movieData.clear();
    totalLength=0;
    fileTime=0.f;
    for(int i=0;i<6;i++)
    {
        maxValue[i]=0.f;
        minValue[i]=0.f;
        maxSpeed[i]=0.f;
        maxAcc[i]=0.f;
    }
}

//读取数据文件
bool MotusFileRun::readtxtfile(QString filename)
{
    QFile file;
    file.setFileName(filename);
    float speed;
    float acc;
    unsigned int totalcount=0;
    if(file.exists())
    {
         int i;
         file.open(QIODevice::ReadOnly);
         QTextStream out(&file);
         while(!out.atEnd())
         {
             QString  tempstr;
             QStringList  tempstrlist;
             M_MovieData tempmoviedata;
             tempstr=out.readLine();
             tempstrlist=tempstr.split (",");
             if(tempstrlist.size()!=7&&tempstrlist.size()!=6)
             {
                file.close();
                QMessageBox::information(NULL,"友情提示","文件格式不正确");
                return false;
             }
             for(i=0;i<6;i++)
             {
                 tempmoviedata.att[i]=tempstrlist.at(i).toDouble();
                 if(tempmoviedata.att[i]>maxValue[i])  maxValue[i]=tempmoviedata.att[i];
                 if(tempmoviedata.att[i]<minValue[i])  minValue[i]=tempmoviedata.att[i];
             }
             if(tempstrlist.size()>6)
                 tempmoviedata.speeff=tempstrlist.at(6).toInt();
             movieData.append(tempmoviedata);
             for(i=0;i<6;i++)
             {
                 if(totalcount>=1)
                 {
                     speed=fabsf(100.f*(movieData[totalcount].att[i]-movieData[totalcount-1].att[i]));
                     if(speed>maxSpeed[i]) maxSpeed[i]=speed;
                 }
                 if(totalcount>=2)
                 {
                      acc=fabsf(10000.f*movieData[totalcount].att[i]-20000.f*movieData[totalcount-1].att[i]+10000.f*movieData[totalcount-2].att[i]);
                     if(acc>maxAcc[i]) maxAcc[i]=acc;
                 }

             }
             totalcount++;
         }
         totalLength=movieData.size();
         fileTime=totalLength/100.f;
         file.close();
         return true;
    }else
    {
         return false;
    }
}

//信息显示
void MotusFileRun::messageView()
{
    ui->filePathLineEdit->setText(fileName);
    ui->dataLenghtLineEdit->setText(QString("%1").arg(totalLength));
    ui->runTimeLineEdit->setText(QString("%1s").arg(QString::number(fileTime,'f',2)));
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            maxValueEdit[i]->setText(QString("%1").arg(QString::number(maxValue[i],'f',2)));
            minValueEdit[i]->setText(QString("%1").arg(QString::number(minValue[i],'f',2)));
            maxSpeedEdit[i]->setText(QString("%1").arg(QString::number(maxSpeed[i],'f',2)));
            maxAccEdit[i]->setText(QString("%1").arg(QString::number(maxAcc[i],'f',2)));
        }
        else
        {
            maxValueEdit[i]->setText(QString("%1").arg(QString::number(maxValue[i],'f',2)));
            minValueEdit[i]->setText(QString("%1").arg(QString::number(minValue[i],'f',2)));
            maxSpeedEdit[i]->setText(QString("%1").arg(QString::number(maxSpeed[i],'f',2)));
            maxAccEdit[i]->setText(QString("%1").arg(QString::number(maxAcc[i],'f',2)));
        }
    }
    ui->progressBar->setRange(0,totalLength);
    ui->progressBar->setValue(0);
}

//文件运动按键
void MotusFileRun::on_fileDataButton_clicked()
{
    float data[6]={0,0,0,0,0,0};
    ui->selectFileButton->setEnabled(false);
    ui->fileDataButton->setEnabled(false);
    for(int i=0;i<6;i++)
    {
        if(fabs(maxValue[i])>fabs(minValue[i])) data[i]=fabs(maxValue[i]);
        else data[i]=fabs(minValue[i]);
    }
    emit sendMovieData(movieData,data);
}

//按键是否可用
void MotusFileRun::recvFileRunButton(bool valid)
{
    ui->selectFileButton->setEnabled(valid);
    ui->fileDataButton->setEnabled(valid);
}

//设置进度条数值
void MotusFileRun::recvMovieCount(unsigned int movieCount)
{
    ui->progressBar->setValue(movieCount);
}


