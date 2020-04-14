#include "motusplatfromsockt.h"
#include <QDebug>
MotusPlatfromSockt::MotusPlatfromSockt(QObject *parent) : QObject(parent)
{
   isConnect=false;
   ConnectCount=0;
   sendCount=0;
   recvCount=0;
   platStatus=88;
   testStatus=88;
   testCount=0;
   testSwitch=false;
}

//平台参数初始化
void MotusPlatfromSockt::initPara()
{
    mMotusSocket.initSocket(10000);
    mMotusSocket.setRemoteIpAndPort("192.168.0.125",5000);
    connect(&mMotusSocket,SIGNAL(sendRecvData(char*,int)),this,SLOT(recvData(char *,int)));
}

//接受数据
void MotusPlatfromSockt::recvData(char *data,int length)
{
    if(sizeof(MotusDataToHost)==length)
    {
        memcpy(&mMotusDataToHost,data,length);
        if(mMotusDataToHost.nDOFStatus!=platStatus)
        {
            if(testStatus!=mMotusDataToHost.nDOFStatus)
            {
                testStatus=mMotusDataToHost.nDOFStatus;
                testCount=0;
                testSwitch=true;
            }
        }
        if(testSwitch&&(testStatus==mMotusDataToHost.nDOFStatus))
        {
            if(testCount>=10)
            {
                platStatus=testStatus;
                testCount=0;
                testSwitch=false;
            }
            else testCount++;
        }
        isConnect=true;
        ConnectCount=0;
        recvCount++;
    }
}

//得到接收数据
void MotusPlatfromSockt::getRecvHostData(MotusDataToHost &tMotusDataToHost)
{
    memcpy(&tMotusDataToHost,&mMotusDataToHost,sizeof(MotusDataToHost));
}

//得到平台是否连接
bool MotusPlatfromSockt::getConnect()
{
    return isConnect;
}

//得到平台是否连接
int  MotusPlatfromSockt::getStatus()
{
    return platStatus;
}

//发送数据
void MotusPlatfromSockt::sendHostData(MotusDataToPlatfrom &mMotusDataToPlatfrom)
{
    ConnectCount++;
    if(ConnectCount>=100)
    {
        ConnectCount=100;
        isConnect=false;
    }
    if((char*)&mMotusDataToPlatfrom==NULL)
    {
         return;
    }
    sendCount++;
    mMotusSocket.sendData((char*)&mMotusDataToPlatfrom,sizeof(MotusDataToPlatfrom));
}

//发送个数
unsigned int MotusPlatfromSockt::getSendCount()
{
    return sendCount;
}

//接收个数
unsigned int MotusPlatfromSockt::getRecvCount()
{
    return recvCount;
}
























