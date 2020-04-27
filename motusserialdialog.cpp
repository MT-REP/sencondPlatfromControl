#include "motusserialdialog.h"
#include "ui_motusserialdialog.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QMessageBox>
MotusSerialDialog::MotusSerialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusSerialDialog)
{
    ui->setupUi(this);

    QList<QSerialPortInfo> ablePorts = QSerialPortInfo::availablePorts();
    for(int i=0;i<ablePorts.size();i++)
    {
        ui->comBox->addItem(ablePorts.at(i).portName());
    }
    ui->splitBox->addItem("|");
    ui->splitBox->addItem(",");
    init();
}

MotusSerialDialog::~MotusSerialDialog()
{
    if(mQSerialPort.isOpen())//如果串口已经打开了 先给他关闭了
    {
        mQSerialPort.clear();
        mQSerialPort.close();
    }
    delete ui;
}

//初始化函数
void MotusSerialDialog::init()
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

void MotusSerialDialog::on_runButton_clicked()
{
    QString mserialPortName = ui->comBox->currentText();
    splitStr = ui ->splitBox->currentText();
    if(mserialPortName.isNull())
    {
        QMessageBox::information(NULL,"友情提示","无串口可用");
    }
    mQSerialPort.setPortName(mserialPortName);
    //用ReadWrite 的模式尝试打开串口
    if(mQSerialPort.open(QIODevice::ReadWrite))
    {
        //设置波特率和读写方向
        mQSerialPort.setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);
        //数据位为8位
        mQSerialPort.setDataBits(QSerialPort::Data8);
        //无流控制
        mQSerialPort.setFlowControl(QSerialPort::NoFlowControl);
        //无校验位
        mQSerialPort.setParity(QSerialPort::NoParity);
        //一位停止位
        mQSerialPort.setStopBits(QSerialPort::OneStop);
        //连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
        connect(&mQSerialPort,SIGNAL(readyRead()),this,SLOT(receiveInfo()));
        ui->runButton->setEnabled(false);
        QMessageBox::information(NULL,"友情提示","串口打开成功");
    }
}

//接收单片机的数据
void MotusSerialDialog::receiveInfo()
{
    while (mQSerialPort.canReadLine())
    {
       QByteArray info = mQSerialPort.readLine();
       QString data = info;
       QString tempStr;
       QStringList strlist = data.split(splitStr);
       for(int i =0;i<strlist.size()&&i<6;i++)
       {
           tempStr = strlist.at(i);
           mMotusSingleQwtplot[i].lineView(tempStr.toFloat());
       }
       break;
    }
}

void MotusSerialDialog::closeEvent(QCloseEvent* event)
{
    emit sendClose(false);
    delete this;
}





