#ifndef MOTUSPLATFROMSOCKT_H
#define MOTUSPLATFROMSOCKT_H

#include <QObject>
#include "motussocket.h"


typedef struct DataToHost
{
    unsigned char nCheckID;
    unsigned char nDOFStatus;
    unsigned char nRev0;
    unsigned char nRev1;

    float attitude[6];			//实际姿态
    float para[6];				//错误代码
    float motor_code[6];		//电机码值
    DataToHost()
    {
        nCheckID=55;
        nDOFStatus=88;
        nRev0=0;
        nRev1=0;
        for(int i=0;i<6;i++)
        {
            attitude[i]=0.0f;
            para[i]=0.0f;
            motor_code[i]=0.0f;
        }
    }
}MotusDataToHost;



typedef struct DataToDOF
{
    unsigned char nCheckID;			//标识位
    unsigned char nCmd;				//状态指令
    unsigned char nAct;				//特效指令
    unsigned char nReserved;	        //保留

    float DOFs[6];			//{横摇，纵倾，航向，前向，侧向，升降}
    float Vxyz[3];			//{前向，侧向，升降}，向右为正，向下为正（速度）
    float Axyz[3];			//...（加速度）
    DataToDOF()
    {
        nCheckID=0;			//标识位
        nCmd=0;				//状态指令
        nAct=0;				//特效指令
        nReserved=0;	        //保留
        for(int i=0;i<6;i++)
        {
            DOFs[i]=0.0f;
            if(i<3)
            {
                Vxyz[i]=0.0f;
                Axyz[i]=0.0f;
            }
        }
    }
}MotusDataToPlatfrom;

class MotusPlatfromSockt : public QObject
{
    Q_OBJECT
public:
    explicit MotusPlatfromSockt(QObject *parent = nullptr);
    void initPara();
    bool getConnect();
    void getRecvHostData(MotusDataToHost &tMotusDataToHost);
    void sendHostData(MotusDataToPlatfrom &mMotusDataToPlatfrom);
    unsigned int getSendCount();
    unsigned int getRecvCount();
    int  getStatus();
private:
    MotusSocket mMotusSocket;
    int platStatus;
    int testStatus;
    int testCount;
    bool testSwitch;
    bool isConnect;
    int ConnectCount;
    unsigned int sendCount;
    unsigned int recvCount;
    MotusDataToHost mMotusDataToHost;
signals:

public slots:
    //接受数据
    void recvData(char *data,int length);
};

#endif // MOTUSPLATFROMSOCKT_H
