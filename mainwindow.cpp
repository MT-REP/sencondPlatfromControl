#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //
    function=0;
    //平台命令
    Cmd=0;
    sonCmd=0;
    //平台未连接
    platStatus=88;
    //定时器初始化
    mMotusTimer=new MotusTimer(10);
    //槽函数绑定
    connect(mMotusTimer,SIGNAL(timeout()),this,SLOT(masterClock()));
    //平台Socket初始化
    mMotusPlatfromSockt.initPara();
    //文件运动对象
    mMotusFileRun=new MotusFileRun(this);
    //正弦运动对象
    mMotusSine =new MotusSine(this);
    //数据保存
    mMotusSaveData =new MotusSaveData(this);
    //单缸运动
    mMotusCylinder =new MotusCylinder(this);//单缸运动对象
    //特殊功能
    mMotusInsideCmd = new MotusInsideCmd(this);
    //删除标签页
    ui->functionTabWidget->removeTab(0);
    ui->functionTabWidget->removeTab(0);

    ui->functionTabWidget->insertTab(0,mMotusSine,"正弦单步运动");
    connect(mMotusSine,SIGNAL(sendSinData(float *,float *,float *,float *, float *)),this,SLOT(recvSinData(float *,float *,float *,float *, float *)));
    connect(mMotusSine,SIGNAL(sendSingleStepData(float *,float *)),this,SLOT(recvSingleStepData(float *,float *)));
    connect(mMotusSine,SIGNAL(sendSinStop()),this,SLOT(recvSinStop()));

    ui->functionTabWidget->insertTab(1,mMotusFileRun,"文件运动");
    connect(mMotusFileRun,SIGNAL(sendMovieData(QList<M_MovieData>&,float *)),this,SLOT(recvMovieData(QList<M_MovieData>&,float *)));
    connect(this,SIGNAL(sendMovieCount(unsigned int)),mMotusFileRun,SLOT(recvMovieCount(unsigned int)));
    connect(this,SIGNAL(setFileRunButton(bool)),mMotusFileRun,SLOT(recvFileRunButton(bool)));

    ui->functionTabWidget->insertTab(2,mMotusSaveData,"数据保存");
    connect(mMotusSaveData,SIGNAL(sendDataIsSave(bool *,bool )),this,SLOT(recvDataIsSave(bool *,bool )));
    connect(mMotusSaveData,SIGNAL(sendCarryOut()),this,SLOT(recvCarryOut()));
    connect(this,SIGNAL(sendDataCarryOut(MDataSave &)),mMotusSaveData,SLOT(recvDataCarryOut(MDataSave &)));

    ui->functionTabWidget->insertTab(3,mMotusCylinder,"单缸运动");
    connect(mMotusCylinder,SIGNAL(sendHandCmd(int)),this,SLOT(recvHandCmd(int)));
    connect(mMotusCylinder,SIGNAL(sendHandMerve(unsigned char )),this,SLOT(recvHandMerve(unsigned char )));
    connect(mMotusCylinder,SIGNAL(sendHandValue(float)),this,SLOT(recvHandValue(float)));

    ui->functionTabWidget->insertTab(4,mMotusInsideCmd,"辅助功能");

    mMotusAngleQwtplot.initPara(ui->angleQwtPlot);
    mMotusAngleQwtplot.setXMinMAX(-35,35,10);
    QString name[3]={"纵倾","横摇","航向"};
    QColor color[3]={QColor(255,0,0),QColor(255,128,0),QColor(128,255,0)};
    mMotusAngleQwtplot.setCurve(name,color);

    mMotusPlaceQwtplot.initPara(ui->displacementQwtPlot);
    mMotusPlaceQwtplot.setXMinMAX(-650,650,100);
    QString placename[3]={"横移","前冲","升降"};
    QColor placecolor[3]={QColor(0,255,0),QColor(128,0,255),QColor(0,255,255)};
    mMotusPlaceQwtplot.setCurve(placename,placecolor);

#ifndef MotusDebug
    ui->searchBottomButton->setEnabled(false);  keyEnable[0]=false;
    ui->riseButton->setEnabled(false);          keyEnable[1]=false;
    ui->runButton->setEnabled(false);           keyEnable[2]=false;
    ui->midButton->setEnabled(false);           keyEnable[3]=false;
    ui->downButton->setEnabled(false);          keyEnable[4]=false;
#endif
}

//析构函数
MainWindow::~MainWindow()
{
    delete ui;
    mMotusTimer->stop();
}

//主时钟
void MainWindow::masterClock(void)
{
    //得到接收数据
    mMotusPlatfromSockt.getRecvHostData(recvStrcut);
    //得到平台状态
    platStatus=mMotusPlatfromSockt.getStatus();
    sendStruct.Cmd=Cmd;
    sendStruct.SubCmd=sonCmd;
    switch(function)
    {
        case MovieFuntion: {fileRunFunction();break;}
        case SingleStepFunction: {singleStepFunction();break;}
        case SinFunction: {sinFunction();break;}
    default: break;
    }
    statusView(); //状态显示
    interfaceView();
    if(sendStruct.Cmd==S_CMD_Back2MID||sendStruct.Cmd==S_CMD_Work||sendStruct.Cmd==S_CMD_Stop)
    {
        for(int i=0;i<6;i++)
        {
            sendStruct.DOFs[i]=0.f;
        }
    }
    //数据保存
    if(mMDataSave.isopen&&platStatus!=R_STATUS_Disconnect)
    {
        MDataArr tMDataArr;
        for(int i=0;i<6;i++)
        {
            if(mMDataSave.isSave[i])
           {
                tMDataArr.data[2*i]=sendStruct.DOFs[i];
                tMDataArr.data[2*i+1]=recvStrcut.Attitudes[i];
            }
        }
        mMDataSave.dataList.push_back(tMDataArr);
    }
    mMotusPlatfromSockt.sendHostData(sendStruct);
}

//文件运动
void MainWindow::fileRunFunction()
{
    if(mMotusFileData.movieCount<mMotusFileData.movieData.size())
    {
        for(int i=0;i<6;i++)
        {
            if(i<3)
                sendStruct.DOFs[i]=mMotusFileData.movieData[mMotusFileData.movieCount].att[i];
            else
                sendStruct.DOFs[i]=mMotusFileData.movieData[mMotusFileData.movieCount].att[i]/1000.f;
        }
        mMotusFileData.movieCount++;
    }
    else
    {
        mMotusFileData.status=2;
        emit setFileRunButton(true);
    }
    emit sendMovieCount(mMotusFileData.movieCount);
}

//单步运动
void MainWindow::singleStepFunction()
{
   sonCmd=1;
   for (int i = 0; i < 6; i++)
   {
       sendStruct.Amp[i]=0.f;		//指令运行正弦幅值
       sendStruct.Fre[i]=0.f;		//指令运行正弦频率
       sendStruct.Pha[i]=0.f;		//指令运行正弦相位
       sendStruct.Pos[i]=mMotusSingleStepData.midAttu[i];		//指令运行单步目标位置
       sendStruct.Spd[i]=mMotusSingleStepData.AttuSpeed[i];		//指令运行单步运行速度
   }
}

//正弦运动
void MainWindow::sinFunction()
{
    sonCmd=2;
    for (int i = 0; i < 6; i++)
    {
        sendStruct.Amp[i]=mMotusSinStruct.valueAttu[i];		//指令运行正弦幅值
        sendStruct.Fre[i]=mMotusSinStruct.freAttu[i];		//指令运行正弦频率
        sendStruct.Pha[i]=mMotusSinStruct.phaseAttu[i];		//指令运行正弦相位
        sendStruct.Pos[i]=mMotusSingleStepData.midAttu[i];		//指令运行单步目标位置
        sendStruct.Spd[i]=mMotusSingleStepData.AttuSpeed[i];		//指令运行单步运行速度
    }
}

//平台状态显示
void MainWindow::statusView()
{
    //判断平台是否连接 平台连接亮绿色 否则亮绿色
    if(mMotusPlatfromSockt.getConnect())
    {
        ui->platConnButton->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(0, 255, 0);");
    }
    else
    {
        platStatus=R_STATUS_Disconnect;
        ui->platConnButton->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(85, 85, 0);");
    }
    //发送指令
    ui->sendCmdEdit->setText(QString("%1").arg(Cmd));
    //平台状态判断
    switch(platStatus)
    {
       case R_STATUS_Initting:     {ui->recvStatusEdit->setText("寻底初始化中-0");break;}
       case R_STATUS_Inited:       {ui->recvStatusEdit->setText("寻底初始化完成-1");break;}
       case R_STATUS_Working:      {ui->recvStatusEdit->setText("底位到中位运行中-2");break;}
       case R_STATUS_Worked:       {ui->recvStatusEdit->setText("底位到中位运行完成-3");break;}
       case R_STATUS_Run:          {ui->recvStatusEdit->setText("正常运行中-4");break;}
       case R_STATUS_AUTOMOVE:     {ui->recvStatusEdit->setText("自动运行-5");break;}
       case R_STATUS_CenterPoint:  {ui->recvStatusEdit->setText("改变运动中心点-6");break;}
       case R_STATUS_ChaConf:      {ui->recvStatusEdit->setText("配置驱动器信息-7");break;}
       case R_STATUS_RomRuning:    {ui->recvStatusEdit->setText("脚本运行中-8");break;}
       case R_STATUS_RomRuned:     {ui->recvStatusEdit->setText("脚本运行完成-9");break;}
       case R_STATUS_Topping:      {ui->recvStatusEdit->setText("中位到顶位运动中-10");break;}
       case R_STATUS_Topped:       {ui->recvStatusEdit->setText("中位到顶位运动完成-11");break;}
       case R_STATUS_Stopping:     {ui->recvStatusEdit->setText("中位到底位运动中-12");break;}
       case R_STATUS_Stopped:      {ui->recvStatusEdit->setText("中位到底位运动完成-13");break;}
       case R_STATUS_Back2Miding:  {ui->recvStatusEdit->setText("回中位运行中-14");break;}
       case R_STATUS_Back2Mided:   {ui->recvStatusEdit->setText("回中位运行中完成-15");break;}
       case R_STATUS_JOG:          {ui->recvStatusEdit->setText("手动模式-32");break;}
       case R_STATUS_ToMerg:       {ui->recvStatusEdit->setText("紧急停止-33");break;}
       case R_STATUS_Idle:         {ui->recvStatusEdit->setText("空闲状态-55");break;}
       case R_STATUS_Disconnect:   {ui->recvStatusEdit->setText("未连接设备-88");break;}
    }
    //发送个数
    ui->sendCountEdit->setText(QString("%1").arg(mMotusPlatfromSockt.getSendCount()));
    //接收个数
    ui->recvCountEdit->setText(QString("%1").arg(mMotusPlatfromSockt.getRecvCount()));

}

//界面动态处理
void MainWindow::interfaceView()
{
    static int totalcount=0;
    totalcount++;
    if(totalcount%50==0)
    {
        #ifndef  MotusDebug
        if(platStatus!=R_STATUS_Run)
        {
            clearSinData();
            clearMovieData();
            clearSingleStepData();
            ui->functionTabWidget->setTabEnabled(0,false);
            ui->functionTabWidget->setTabEnabled(1,false);
            ui->functionTabWidget->setTabEnabled(2,false);
        }
        else if(platStatus==R_STATUS_Run)
        {
            if(mMotusSinStruct.status==1)
            {
                ui->functionTabWidget->setTabEnabled(1,false);
                ui->functionTabWidget->setTabEnabled(2,false);
            }
            else if(mMotusFileData.status==1)
            {
                ui->functionTabWidget->setTabEnabled(0,false);
                ui->functionTabWidget->setTabEnabled(1,false);
            }
            else if(mMotusSingleStepData.status==1)
            {
                ui->functionTabWidget->setTabEnabled(0,false);
                ui->functionTabWidget->setTabEnabled(2,false);
            }
            else
            {
                ui->functionTabWidget->setTabEnabled(0,true);
                ui->functionTabWidget->setTabEnabled(1,true);
                ui->functionTabWidget->setTabEnabled(2,true);
            }
        }
        //按键显示
        switch(platStatus)
        {
           case R_STATUS_Initting: {   keyEnable[0]=false; keyEnable[1]=false; break;   }
           case R_STATUS_Inited:   {   keyEnable[0]=false; keyEnable[1]=true; break;    }
           case R_STATUS_Working:  {   keyEnable[0]=false; keyEnable[1]=false; break;   }
           case R_STATUS_Worked:   {   keyEnable[2]=true;  break;                       }
           case R_STATUS_Run:
           case R_STATUS_AUTOMOVE:
           case R_STATUS_CenterPoint:
           case R_STATUS_ChaConf:
           case R_STATUS_RomRuning:
           case R_STATUS_RomRuned:
           case R_STATUS_Topped:   { keyEnable[2]=false; keyEnable[3]=true;keyEnable[4]=true; break; }
           case R_STATUS_Stopping: { keyEnable[2]=false; keyEnable[3]=false;keyEnable[4]=false; break; }
           case R_STATUS_Stopped:  { keyEnable[1]=true; break;   }
           case R_STATUS_Back2Miding:{ keyEnable[2]=false; keyEnable[3]=false;keyEnable[4]=false; break; }
           case R_STATUS_Back2Mided: { keyEnable[2]=true; keyEnable[3]=false;keyEnable[4]=true; break; }
           case R_STATUS_Idle:       { keyEnable[0]=true; keyEnable[1]=true; break;    }
           case R_STATUS_JOG:
           case R_STATUS_ToMerg:
           case R_STATUS_Disconnect: {keyEnable[0]=false; keyEnable[1]=false;keyEnable[2]=false; keyEnable[3]=false;keyEnable[4]=false; break; }
        }
        keySetEnable();
        #endif
        /////////////////////////////////////////返回数据显示/////////////////////////////////////
        //纵倾
        ui->zongqingtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[0],'f',2)));
        ui->zongqingrEdit->setText(QString("%1").arg(QString::number(recvStrcut.Attitudes[0],'f',2)));
        //横摇
        ui->hengyaotEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[1],'f',2)));
        ui->hengyaorEdit->setText(QString("%1").arg(QString::number(recvStrcut.Attitudes[1],'f',2)));
        //航向
        ui->hangxiangtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[2],'f',2)));
        ui->hangxiangrEdit->setText(QString("%1").arg(QString::number(recvStrcut.Attitudes[2],'f',2)));
        //横移
        ui->hengyitEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[3]*1000.f,'f',2)));
        ui->hengyirEdit->setText(QString("%1").arg(QString::number(recvStrcut.Attitudes[3]*1000.f,'f',2)));
        //前冲
        ui->qianchongtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[4]*1000.f,'f',2)));
        ui->qianchongrEdit->setText(QString("%1").arg(QString::number(recvStrcut.Attitudes[4]*1000.f,'f',2)));
        //升降
        ui->shengjiangtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[5]*1000.f,'f',2)));
        ui->shengjiangrEdit->setText(QString("%1").arg(QString::number(recvStrcut.Attitudes[5]*1000.f,'f',2)));
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////状态显示////////////////////////////////////////////////////
        //1号
        ui->para1Edit->setText(QString("%1").arg(QString::number(recvStrcut.ErrorCode[0],'f',2)));
        ui->code1Edit->setText(QString("%1").arg(QString::number(recvStrcut.MotorCode[0],'f',2)));
        //2号
        ui->para2Edit->setText(QString("%1").arg(QString::number(recvStrcut.ErrorCode[1],'f',2)));
        ui->code2Edit->setText(QString("%1").arg(QString::number(recvStrcut.MotorCode[1],'f',2)));
        //3号
        ui->para3Edit->setText(QString("%1").arg(QString::number(recvStrcut.ErrorCode[2],'f',2)));
        ui->code3Edit->setText(QString("%1").arg(QString::number(recvStrcut.MotorCode[2],'f',2)));
        //4号
        ui->para4Edit->setText(QString("%1").arg(QString::number(recvStrcut.ErrorCode[3],'f',2)));
        ui->code4Edit->setText(QString("%1").arg(QString::number(recvStrcut.MotorCode[3],'f',2)));
        //5号
        ui->para5Edit->setText(QString("%1").arg(QString::number(recvStrcut.ErrorCode[4],'f',2)));
        ui->code5Edit->setText(QString("%1").arg(QString::number(recvStrcut.MotorCode[4],'f',2)));
        //6号
        ui->para6Edit->setText(QString("%1").arg(QString::number(recvStrcut.ErrorCode[5],'f',2)));
        ui->code6Edit->setText(QString("%1").arg(QString::number(recvStrcut.MotorCode[5],'f',2)));
        ///////////////////////////////////////////////////////////////////////////////////////////////
    }
    float data[3];
    data[0]=sendStruct.DOFs[0];
    data[1]=sendStruct.DOFs[1];
    data[2]=sendStruct.DOFs[2];
    float tdata[3];
    tdata[0]=sendStruct.DOFs[3]*1000.f;
    tdata[1]=sendStruct.DOFs[4]*1000.f;
    tdata[2]=sendStruct.DOFs[5]*1000.f;
    //画曲线
    mMotusAngleQwtplot.lineView(data);
    mMotusPlaceQwtplot.lineView(tdata);
}

void MainWindow::keySetEnable()
{
//    static bool oldkey[5]={false,false,false,false,false};
//    //qDebug()<<oldkey[0]<<oldkey[1]<<oldkey[2]<<oldkey[3]<<oldkey[4];
//    if(oldkey[0]!=keyEnable[0])
//    {
//        if(keyEnable[0]) ui->searchBottomButton->setEnabled(true);
//        else ui->searchBottomButton->setEnabled(false);
//        oldkey[0]=keyEnable[0];
//    }
//    if(oldkey[1]!=keyEnable[1])
//    {
//        if(keyEnable[1]) ui->riseButton->setEnabled(true);
//        else ui->riseButton->setEnabled(false);
//        oldkey[1]=keyEnable[1];
//    }
//    if(oldkey[2]!=keyEnable[2])
//    {
//        if(keyEnable[2]) ui->runButton->setEnabled(true);
//        else ui->runButton->setEnabled(false);
//        oldkey[2]=keyEnable[2];
//    }
//    if(oldkey[3]!=keyEnable[3])
//    {
//        if(keyEnable[3]) ui->midButton->setEnabled(true);
//        else ui->midButton->setEnabled(false);
//        oldkey[3]=keyEnable[3];
//    }
//    if(oldkey[4]!=keyEnable[4])
//    {
//        if(keyEnable[4]) ui->downButton->setEnabled(true);
//        else ui->downButton->setEnabled(false);
//        oldkey[4]=keyEnable[4];
//    }
//    repaint();
}

//开始按键
void MainWindow::on_startStopButton_clicked()
{
    static int  startStopFlag=0;
    if(startStopFlag==0)
    {
        startStopFlag=1;
        mMotusTimer->start();
        ui->startStopButton->setText("停止");
    }
    else
    {
        startStopFlag=0;
        mMotusTimer->stop();
        ui->startStopButton->setText("开始");
    }
}

//寻底函数
void MainWindow::on_searchBottomButton_clicked()
{
    Cmd=S_CMD_FindZero;//4
}

//上升函数
void MainWindow::on_riseButton_clicked()
{
    Cmd=S_CMD_Work;//6
}

//运行函数
void MainWindow::on_runButton_clicked()
{
    Cmd=S_CMD_RUN;//9
}

//中立位函数
void MainWindow::on_midButton_clicked()
{
    Cmd=S_CMD_Back2MID;//2
}

//落下函数
void MainWindow::on_downButton_clicked()
{
    Cmd=S_CMD_Stop;//7
}

//文件运动信号槽函数
void MainWindow::recvMovieData(QList<M_MovieData>&movieData,float *data)
{
    Cmd=S_CMD_RUN;
    mMotusFileData.movieData=movieData;
    mMotusFileData.status=1;
    mMotusFileData.movieCount=0;
    mMotusAngleQwtplot.dynamicXMinMAX(data);
    mMotusPlaceQwtplot.dynamicXMinMAX(&data[3]);
    function=MovieFuntion;
}

//文件运动结束
void MainWindow::clearMovieData()
{
    mMotusFileData.status=2;
    mMotusFileData.movieCount=0;
    function=FunctionNull;
    emit setFileRunButton(true);
    emit sendMovieCount(mMotusFileData.movieCount);
}

//单步运动
void MainWindow::recvSingleStepData(float *pos,float *speed)
{
    float value[6]={0,0,0,0,0,0};
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            mMotusSingleStepData.midAttu[i]=pos[i];
            mMotusSingleStepData.AttuSpeed[i]=speed[i];
            value[i]=mMotusSingleStepData.midAttu[i];

        }
        else
        {
            mMotusSingleStepData.midAttu[i]=pos[i]/1000.f;
            mMotusSingleStepData.AttuSpeed[i]=speed[i];
            value[i]=mMotusSingleStepData.midAttu[i];
        }
    }
    mMotusSingleStepData.status=1;
    mMotusAngleQwtplot.dynamicXMinMAX(value);
    mMotusPlaceQwtplot.dynamicXMinMAX(&value[3]);
    Cmd=S_CMD_AutoMove;
    sonCmd=0;
    function=SingleStepFunction;
}

//停止单步数据
void MainWindow::clearSingleStepData()
{
    for(int i=0;i<6;i++)
    {
        mMotusSingleStepData.midAttu[i]=0.f;
        mMotusSingleStepData.AttuSpeed[i]=0.f;
    }
    mMotusSingleStepData.status=2;
    sonCmd=0;
    function=FunctionNull;
}

//正弦开始
void MainWindow::recvSinData(float *pos,float *spd,float *value,float *fre, float *phase)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            mMotusSingleStepData.midAttu[i]=pos[i];
            mMotusSingleStepData.AttuSpeed[i]=spd[i];
            mMotusSinStruct.valueAttu[i]=value[i];
        }
        else
        {
            mMotusSingleStepData.midAttu[i]=pos[i]/1000.f;
            mMotusSingleStepData.AttuSpeed[i]=spd[i];
            mMotusSinStruct.valueAttu[i]=value[i]/1000.f;
        }
        mMotusSinStruct.freAttu[i]=fre[i];
        mMotusSinStruct.phaseAttu[i]=phase[i];
    }
    mMotusSinStruct.status=1;
    mMotusAngleQwtplot.dynamicXMinMAX(value);
    mMotusPlaceQwtplot.dynamicXMinMAX(&value[3]);
    Cmd=S_CMD_AutoMove;
    sonCmd=0;
    function=SinFunction;
}

//正弦停止
void MainWindow::recvSinStop()
{
    sonCmd=3;
}

//清除正弦数据
void MainWindow::clearSinData()
{
    mMotusSinStruct.status=2;
    for(int i=0;i<6;i++)
    {
        mMotusSingleStepData.midAttu[i]=0.f;
        mMotusSingleStepData.AttuSpeed[i]=0.f;
        mMotusSinStruct.valueAttu[i]=0.f;		//指令运行正弦幅值
        mMotusSinStruct.freAttu[i]=0.f;		//指令运行正弦频率
        mMotusSinStruct.phaseAttu[i]=0.f;
    }
    sonCmd=0;
    function=FunctionNull;
}

//接收数据是否保存
void MainWindow::recvDataIsSave(bool *iswitch,bool isave)
{
   for(int i=0;i<6;i++)
   {
       mMDataSave.isSave[i]=iswitch[i];
   }
   mMDataSave.isopen=isave;
}

//数据保存执行
void MainWindow::recvCarryOut()
{
    mMDataSave.isopen=false;
    emit sendDataCarryOut(mMDataSave);
}

//接收手动指令
void MainWindow::recvHandCmd(int cmd)
{
    Cmd=cmd;
}

//电动缸选择
void MainWindow::recvHandMerve(unsigned char merve)
{
    sendStruct.CyChoose=merve;
}

//电动缸运行速度
void MainWindow::recvHandValue(float data)
{
    sendStruct.JogSpeed=data;
}





































































