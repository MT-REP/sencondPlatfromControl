#ifndef MOTUSSOCKET_H
#define MOTUSSOCKET_H

#include <QObject>
#include <QUdpSocket>

class MotusSocket : public QObject
{
    Q_OBJECT
public:
    explicit MotusSocket(QObject *parent = nullptr);//构造函数
    ~MotusSocket();//析构函数
    void initSocket(int bindPort);//绑定端口
    void setRemoteIpAndPort(QString remoteIP,int remotePort);//设置远程IP和端口
    void sendData(char data[],int length);
private:
    QUdpSocket *motusUdpServer;//网络接收Socket
    QHostAddress *remoteaddr;  //远程地址
    int remotePort;            //远程地址端口
    bool isInit;//是否初始化完成
    void closeSocket();//关闭Socket
signals:

public slots:
    void readPendingDatagrams();
};

#endif // MOTUSSOCKET_H
