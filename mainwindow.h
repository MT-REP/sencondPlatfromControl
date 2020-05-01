#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "motustimer.h"
#include "motusplatfromsockt.h"
#include "motusfilerun.h"
#include "motussine.h"
#include "motusqwtplot.h"
#include "motussavedata.h"
#include "motuscylinder.h"
#include "motusinsidecmd.h"

#define MotusRelase

//平台工作状态指令
enum M_nCmd
{
    S_CMD_Check=1,
    S_CMD_Back2MID=2,		//底回中立位
    S_CMD_ToMerg=3,			//紧急停机
    S_CMD_FindZero=4,		//寻零位
    S_CMD_JOG=5,			//单缸手动
    S_CMD_Work=6,			//由低位上升到中位
    S_CMD_Stop=7,			//由中位回落到低位
    S_CMD_ChaConf=8,		//配置驱动器信息
    S_CMD_RUN=9,			//正常运行
    S_CMD_CenterPoint = 10,	//改变运动中心点
    S_CMD_AutoMove = 11,	//自动运行
    S_CMD_ToTop = 12,		//由中位运动到顶位
    S_CMD_RomRun = 13,		//脚本运行
    S_CMD_Idle=22,			//指令空闲
};

//平台工作状态指令
enum M_Function
{
    FunctionNull=0,
    MovieFuntion=1, //文件功能
    SingleStepFunction=2,//单步运动
    SinFunction=3,//正弦运动
};


//平台返回状态
enum R_cStatus
{
    R_STATUS_Initting = 0,      //寻底初始化中
    R_STATUS_Inited = 1,        //寻底初始化完成
    R_STATUS_Working = 2,		//底位到中位运行中
    R_STATUS_Worked = 3,		//底位到中位运行完成
    R_STATUS_Run = 4,		    //正常运行中
    R_STATUS_AUTOMOVE = 5,		//自动运行
    R_STATUS_CenterPoint = 6, 	//改变运动中心点
    R_STATUS_ChaConf = 7,       //配置驱动器信息
    R_STATUS_RomRuning = 8,	    //脚本运行中
    R_STATUS_RomRuned = 9,	    //脚本运行完成
    R_STATUS_Topping = 10,		//中位到顶位运动中
    R_STATUS_Topped = 11,		//中位到顶位运动完成
    R_STATUS_Stopping = 12,		//中位到底位运动中
    R_STATUS_Stopped = 13,		//中位到底位运动完成
    R_STATUS_Back2Miding = 14,  //回中位运行中
    R_STATUS_Back2Mided = 15,   //回中位运行中完成
    R_STATUS_JOG = 32,         	//手动模式
    R_STATUS_ToMerg = 33,       //紧急停止
    R_STATUS_Idle = 55,        	//空闲状态
    R_STATUS_Disconnect = 88,   //未连接设备
};

//文件
typedef  struct FileDataStruct
{
    QList<M_MovieData> movieData; //文件数据
    int movieCount;
    unsigned char status;//0没有动作 1正在动作 2动作完成
    FileDataStruct()
    {
        movieCount=0;
        status=0;
    }
}MotusFileData;

//单步运动
typedef  struct singleStepStruct
{
    float midAttu[6];
    float AttuSpeed[6];
    unsigned char status;//0没有动作 1正在动作 2动作完成
    singleStepStruct()
    {
        for(int i=0;i<6;i++)
        {
            midAttu[i]=0.0f;
            AttuSpeed[i]=0.0f;
        }
        status=0;
    }
}MotusSingleStepData;

//正弦运动
typedef  struct sinStruct
{
    float valueAttu[6];
    float freAttu[6];
    float phaseAttu[6];
    unsigned char status;    //0没有动作 1正在动作 2动作完成
    sinStruct()
    {
        for(int i=0;i<6;i++)
        {
            valueAttu[i]=0.f;
            freAttu[i]=0.f;
            phaseAttu[i]=0.f;
        }
        status=0;
    }
}MotusSinStruct;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    MotusTimer *mMotusTimer; //定时器函数
    MotusQwtplot mMotusAngleQwtplot;
    MotusQwtplot mMotusPlaceQwtplot;
    MotusPlatfromSockt mMotusPlatfromSockt; //平台发送UDP对象
    MotusFileRun *mMotusFileRun;       //文件运动对象
    MotusSine *mMotusSine;             //正弦运动对象
    MotusSaveData *mMotusSaveData;     //数据保存对象
    MDataSave mMDataSave;              //数据保存结构体
    MotusCylinder *mMotusCylinder;     //单缸运动对象
    MotusInsideCmd *mMotusInsideCmd;   //特殊功能界面
    MotusDataToPlatfrom sendStruct;    //发送结构体对象
    MotusDataToHost recvStrcut;        //接收结构体对象
    MotusFileData mMotusFileData;      //数据动作结构体
    MotusSingleStepData mMotusSingleStepData;
    MotusSinStruct mMotusSinStruct;
    unsigned char function;            //功能
    int Cmd;                           //平台主命令
    int sonCmd;                        //平台子命令
    int platStatus;                    //平台状态
    bool keyEnable[5];
    bool netDataView;
    //平台状态显示
    void statusView();
    //界面显示
    void interfaceView();
    //文件运动
    void fileRunFunction();    
    //单步运动
    void singleStepFunction();
    //正弦运动
    void sinFunction();
    //清除参数
    void clearSinData();
    //清除数据
    void clearMovieData(void);
    //清除单步数据
    void clearSingleStepData();
    //按键设置可用
    void keySetEnable();
signals:
    void sendSingleStepButton(bool valid);
    //设置文件运动运动的个数
    void sendMovieCount(unsigned int movieCount);
    //设置文件运动按键是否可用
    void setFileRunButton(bool valid);
    //数据的保存
    void sendDataCarryOut(MDataSave &);
    //发送接收数据
    void sendNetData(MotusDataToHost &mMotusDataToHost);
public slots:
    //主时钟
    void masterClock(void);
    //接收文件运动数据
    void recvMovieData(QList<M_MovieData>&movieData,float *data);
    //接收单步运动数据
    void recvSingleStepData(float *pos,float *speed);
    //接收数据正弦
    void recvSinData(float *pos,float *spd,float *value,float *fre, float *phase);
    //正弦停止
    void recvSinStop();
    //数据是否保存
    void recvDataIsSave(bool *iswitch,bool isave);
    //执行函数
    void recvCarryOut();
    //手动指令
    void recvHandCmd(int cmd);
    //电动缸选择
    void recvHandMerve(unsigned char merve);
    //电动缸运动速度
    void recvHandValue(float data);
    //数据是否可显示
    void recvNetDataView(bool view);
private slots:
    void on_startStopButton_clicked();
    void on_searchBottomButton_clicked();
    void on_riseButton_clicked();
    void on_runButton_clicked();
    void on_midButton_clicked();
    void on_downButton_clicked();
};

#endif // MAINWINDOW_H
