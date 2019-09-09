#include "motussocket.h"
#include <QMessageBox>
MotusSocket::MotusSocket(QObject *parent) : QObject(parent)
{
    isInit=false;
    remoteaddr=NULL;
    remotePort=5000;
}

//网络初始化
void MotusSocket::initSocket(int bindPort)
{
      //创建UdpSocket
      motusUdpServer = new QUdpSocket(this);
      //绑定端口号
      if(motusUdpServer->bind(QHostAddress::Any, bindPort))
      {
          //网络数据接收
          connect(motusUdpServer, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
          //初始化完成
          isInit=true;
      }
      else
      {
          QMessageBox box;
          box.setText("绑定的端口号被占用。");
          box.exec();
      }
}

//设置远程IP和端口
void MotusSocket::setRemoteIpAndPort(QString remoteIP,int remotePort)
{
    if(remoteaddr==NULL)
    {
        remoteaddr=new QHostAddress();
        remoteaddr->setAddress(remoteIP);
    }
    else
    {
        remoteaddr->setAddress(remoteIP);
    }
    this->remotePort=remotePort;
}

//接收数据
void MotusSocket::readPendingDatagrams()
{
     while(motusUdpServer->hasPendingDatagrams())//判断是否有待接收数据
     {
         QByteArray data;
         data.resize(motusUdpServer->pendingDatagramSize());
         motusUdpServer->readDatagram(data.data(),data.size());//读取数据
         emit sendRecvData(data.data(),data.size());
     }
}

//发送数据
void MotusSocket::sendData(char data[],int length)
{
    if(remoteaddr!=NULL&&isInit)
    {
        //发送数据
        motusUdpServer->writeDatagram(data,length,*remoteaddr,remotePort);
    }
}

//关闭Socket
void MotusSocket::closeSocket()
{
    if(isInit)
    {
        motusUdpServer->close();
    }
    if(remoteaddr!=NULL)
    {
        remoteaddr->clear();
    }
}

//析构函数
MotusSocket::~MotusSocket()
{
    //关闭Socket
    closeSocket();
}




