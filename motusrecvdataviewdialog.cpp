#include "motusrecvdataviewdialog.h"
#include "ui_motusrecvdataviewdialog.h"

MotusRecvDataViewDialog::MotusRecvDataViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusRecvDataViewDialog)
{
    ui->setupUi(this);
    ui->dataBox->addItem("Attitudes");
    ui->dataBox->addItem("ErrorCode");
    ui->dataBox->addItem("MotorCode");
    ui->dataBox->addItem("Torque");
    viewIndex=0;
    init();
}

MotusRecvDataViewDialog::~MotusRecvDataViewDialog()
{
    delete ui;
}

//初始化函数
void MotusRecvDataViewDialog::init()
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

void MotusRecvDataViewDialog::on_runButton_clicked()
{
    viewIndex=ui->dataBox->currentIndex();
}

void MotusRecvDataViewDialog::recvNetData(MotusDataToHost & mMotusDataToHost)
{
    switch(viewIndex)
    {
        case 0:{
                    for(int i=0;i<6;i++)
                    {
                        mMotusSingleQwtplot[i].lineView(mMotusDataToHost.Attitudes[i]);
                    }
                    break;
               }
        case 1:{
                    for(int i=0;i<6;i++)
                    {
                        mMotusSingleQwtplot[i].lineView(mMotusDataToHost.ErrorCode[i]);
                    }
                    break;
               }
        case 2:{
                    for(int i=0;i<6;i++)
                    {
                        mMotusSingleQwtplot[i].lineView(mMotusDataToHost.MotorCode[i]);
                    }
                    break;
               }
        case 3:{
                    for(int i=0;i<6;i++)
                    {
                        mMotusSingleQwtplot[i].lineView(mMotusDataToHost.Torque[i]);
                    }
                    break;
               }
    }
}

void MotusRecvDataViewDialog::closeEvent(QCloseEvent* event)
{
    emit sendClose(false);
    delete this;
}
