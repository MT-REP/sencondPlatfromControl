#ifndef MOTUSPLATFROMSOCKT_H
#define MOTUSPLATFROMSOCKT_H

#include <QObject>
#include "motussocket.h"

//typedef struct DataToHost
//{
//    unsigned char nCheckID;
//    unsigned char nDOFStatus;
//    unsigned char nRev0;
//    unsigned char nRev1;
//    float attitude[6];			//实际姿态
//    float para[6];				//错误代码
//    float motor_code[6];		    //电机码值
//    DataToHost()
//    {
//        nCheckID=55;
//        nDOFStatus=88;
//        nRev0=0;
//        nRev1=0;
//        for(int i=0;i<6;i++)
//        {
//            attitude[i]=0.0f;
//            para[i]=0.0f;
//            motor_code[i]=0.0f;
//        }
//    }
//}MotusDataToHost;

typedef struct DataToHost			//总共108字节
{
    unsigned char CheckID;		//标识位 固定为55
    unsigned char DOFStatus;	//下位机状态
    unsigned char DI;			//数字输入
    unsigned char Rev1;			//保留位
    float Attitudes[6];         //实际姿态
    float ErrorCode[6];         //错误代码
    float MotorCode[6];         //电机码值
    float Torque[6];            //电机力矩
    unsigned int Version;		//下位机固件版本
    unsigned int Time;			//时间戳
    DataToHost()
    {
        CheckID=55;
        DOFStatus=255;
        DI=0;
        Rev1=0;
        Version=0;
        Time=0;
        for(int i=0;i<6;i++)
        {
          Attitudes[i]=0.f;
          ErrorCode[i]=0.f;
          MotorCode[i]=0.f;
          Torque[i]=0.f;
        }
    }
}MotusDataToHost;

//typedef struct DataToDOF
//{
//    unsigned char nCheckID;			//标识位
//    unsigned char nCmd;				//状态指令
//    unsigned char nAct;				//特效指令
//    unsigned char nReserved;	        //保留
//    float DOFs[6];			        //{横摇，纵倾，航向，前向，侧向，升降}
//    float Vxyz[3];			        //{前向，侧向，升降}，向右为正，向下为正（速度）
//    float Axyz[3];			        //...（加速度）
//    DataToDOF()
//    {
//        nCheckID=0;			         //标识位
//        nCmd=0;				         //状态指令
//        nAct=0;				         //特效指令
//        nReserved=0;	                 //保留
//        for(int i=0;i<6;i++)
//        {
//            DOFs[i]=0.0f;
//            if(i<3)
//            {
//                Vxyz[i]=0.0f;
//                Axyz[i]=0.0f;
//            }
//        }
//    }
//}MotusDataToPlatfrom;

typedef struct DataToDOF		//总共180字节
{
    unsigned char CheckID;		//标识位 固定为55
    unsigned char Cmd;		    //状态指令
    unsigned char SubCmd;		//子状态指令//1开始2缓停
    unsigned char FileNum;		//本地文件运行文件编号
    unsigned char DO;			//数字量输出
    unsigned char CyChoose;	    //运动缸选择，按位置1
    unsigned short JogSpeed;	//手动模式运行速度
    float DOFs[6];	    //随动模式姿态：纵倾，横摇，航向，横移，前冲，升降
    float Vxyz[3];	    //洗出算法三轴角速度
    float Axyz[3];	    //洗出算法三轴位移加速度
    float Amp[6];		//指令运行正弦幅值
    float Fre[6];		//指令运行正弦频率
    float Pha[6];		//指令运行正弦相位
    float Pos[6];		//指令运行单步目标位置
    float Spd[6];		//指令运行单步运行速度
    unsigned int Time;  //时间戳
    DataToDOF()
    {
        CheckID=55;
        Cmd=0;
        SubCmd=0;
        FileNum=0;
        DO=0;
        CyChoose=0;
        JogSpeed=0;
        Time=0;
        for(int i=0;i<6;i++)
        {
            if(i<3)
            {
                Vxyz[i]=0.f;	//洗出算法三轴角速度
                Axyz[i]=0.f;	//洗出算法三轴位移加速度
            }
            DOFs[i]=0.f;	//随动模式姿态：纵倾，横摇，航向，横移，前冲，升降
            Amp[i]=0.f;		//指令运行正弦幅值
            Fre[i]=0.f;		//指令运行正弦频率
            Pha[i]=0.f;		//指令运行正弦相位
            Pos[i]=0.f;		//指令运行单步目标位置
            Spd[i]=0.f;		//指令运行单步运行速度
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
    int courseStatus;
    int courseCount;
    bool courseSwitch;
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
