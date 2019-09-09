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
    nCmd=0;
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
    //单步运动对象
    mMotusSingleStep =new MotusSingleStep(this);
    //删除标签页
    ui->functionTabWidget->removeTab(0);
    ui->functionTabWidget->removeTab(0);
    ui->functionTabWidget->insertTab(0,mMotusSine,"正弦运动");
    connect(mMotusSine,SIGNAL(sendSinData(int,float *,float *, float *)),this,SLOT(recvSinData(int,float *,float *, float *)));
    connect(mMotusSine,SIGNAL(sendSinStop()),this,SLOT(recvSinStop()));
    connect(this,SIGNAL(sendTimeView(float)),mMotusSine,SLOT(recvTimeView(float)));
    connect(this,SIGNAL(sendSinInterrupt()),mMotusSine,SLOT(recvSinInterrupt()));

    ui->functionTabWidget->insertTab(1,mMotusSingleStep,"单步运行");
    connect(mMotusSingleStep,SIGNAL(sendSingleStepData(float *,float *)),this,SLOT(recvSingleStepData(float *,float *)));
    connect(this,SIGNAL(sendSingleStepButton(bool )),mMotusSingleStep,SLOT(recvSingleStepButton(bool )));

    ui->functionTabWidget->insertTab(2,mMotusFileRun,"文件运动");
    connect(mMotusFileRun,SIGNAL(sendMovieData(QList<M_MovieData>&,float *)),this,SLOT(recvMovieData(QList<M_MovieData>&,float *)));
    connect(this,SIGNAL(sendMovieCount(unsigned int)),mMotusFileRun,SLOT(recvMovieCount(unsigned int)));
    connect(this,SIGNAL(setFileRunButton(bool)),mMotusFileRun,SLOT(recvFileRunButton(bool)));
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

    ui->searchBottomButton->setEnabled(false);  keyEnable[0]=false;
    ui->riseButton->setEnabled(false);          keyEnable[1]=false;
    ui->runButton->setEnabled(false);           keyEnable[2]=false;
    ui->midButton->setEnabled(false);           keyEnable[3]=false;
    ui->downButton->setEnabled(false);          keyEnable[4]=false;
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
    mMotusPlatfromSockt.getRecvHostData(recvStrcut);
    platStatus=mMotusPlatfromSockt.getStatus();
    sendStruct.nCmd=nCmd;
    switch(function)
    {
        case MovieFuntion: {fileRunFunction();break;}
        case SingleStepFunction: {singleStepFunction();break;}
        case SinFunction: {sinFunction();break;}
    default: break;
    }
    statusView();
    interfaceView();
    if(sendStruct.nCmd==S_CMD_Back2MID||sendStruct.nCmd==S_CMD_Work||sendStruct.nCmd==S_CMD_Stop)
    {
        for(int i=0;i<6;i++)
        {
            sendStruct.DOFs[i]=0.f;
        }
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
    for (int i = 0; i < 6; i++)
    {
        if (!mMotusSingleStepData.isFinish[i]&&(mMotusSingleStepData.stepCount < mMotusSingleStepData.totalCount[i]))
        {
            sendStruct.DOFs[i]= ((mMotusSingleStepData.endAttu[i] / 2.f) + (mMotusSingleStepData.endAttu[i] / 2.f)*cos(mMotusSingleStepData.AttuSpeed[i] / (mMotusSingleStepData.absAttu[i] / 2.f)*mMotusSingleStepData.stepCount/ 100.f ));
            sendStruct.DOFs[i]= sendStruct.DOFs[i] + ((mMotusSingleStepData.midAttu[i] / 2.f) - (mMotusSingleStepData.midAttu[i]  / 2.f)*cos(mMotusSingleStepData.AttuSpeed[i] / (mMotusSingleStepData.absAttu[i] / 2.f)*mMotusSingleStepData.stepCount/ 100.f));
        }
        else
        {
            sendStruct.DOFs[i]=mMotusSingleStepData.midAttu[i];
            mMotusSingleStepData.isFinish[i]=true;
        }
    }
    if(mMotusSingleStepData.isFinish[0]&&mMotusSingleStepData.isFinish[1]&&mMotusSingleStepData.isFinish[2]
       &&mMotusSingleStepData.isFinish[3]&&mMotusSingleStepData.isFinish[4]&&mMotusSingleStepData.isFinish[5])
    {
        mMotusSingleStepData.status=2;
    }
    mMotusSingleStepData.stepCount++;
}

//正弦运动
void MainWindow::sinFunction()
{
    if (mMotusSinStruct.stepCount < 2000)
        mMotusSinStruct.Kslow= mMotusSinStruct.stepCount / 2000.f;
    else if (mMotusSinStruct.stepCount< (mMotusSinStruct.inputview + 20) * 100)
        mMotusSinStruct.Kslow = 1;
    else if (mMotusSinStruct.stepCount >= (mMotusSinStruct.inputview + 20) * 100
             && mMotusSinStruct.stepCount <= ((mMotusSinStruct.inputview + 20) * 100 + 2000))
        mMotusSinStruct.Kslow = (mMotusSinStruct.inputview* 100 + 4000 - mMotusSinStruct.stepCount) / 2000.f;
    else
        {
            mMotusSinStruct.status=2;
            mMotusSinStruct.Kslow = 0.f;
        }
    if (mMotusSinStruct.bufferStop)
    {
        if ((mMotusSinStruct.stepCount  - mMotusSinStruct.BufferStopCount) < 1000)
            mMotusSinStruct.Kslow = mMotusSinStruct.Kslow *(1-(mMotusSinStruct.stepCount-mMotusSinStruct.BufferStopCount)/1000.f);
        else
        {
            mMotusSinStruct.Kslow = 0.f;
        }
    }
    if (mMotusSinStruct.Kslow  > 1) mMotusSinStruct.Kslow  = 1;
    if (mMotusSinStruct.Kslow  < 0) mMotusSinStruct.Kslow  = 0;

    sendStruct.DOFs[0]= mMotusSinStruct.Kslow * mMotusSinStruct.valueAttu[0] * sin(6.283*mMotusSinStruct.freAttu[0] * mMotusSinStruct.stepCount / 100.f + mMotusSinStruct.phaseAttu[0]);
    sendStruct.DOFs[1]= mMotusSinStruct.Kslow * mMotusSinStruct.valueAttu[1] * sin(6.283*mMotusSinStruct.freAttu[1] * mMotusSinStruct.stepCount / 100.f + mMotusSinStruct.phaseAttu[1]);
    sendStruct.DOFs[2]= mMotusSinStruct.Kslow * mMotusSinStruct.valueAttu[2] * sin(6.283*mMotusSinStruct.freAttu[2] * mMotusSinStruct.stepCount / 100.f + mMotusSinStruct.phaseAttu[2]);
    sendStruct.DOFs[3]= mMotusSinStruct.Kslow * mMotusSinStruct.valueAttu[3] * sin(6.283*mMotusSinStruct.freAttu[3] * mMotusSinStruct.stepCount / 100.f + mMotusSinStruct.phaseAttu[3]);
    sendStruct.DOFs[4]= mMotusSinStruct.Kslow * mMotusSinStruct.valueAttu[4] * sin(6.283*mMotusSinStruct.freAttu[4] * mMotusSinStruct.stepCount / 100.f + mMotusSinStruct.phaseAttu[4]);
    sendStruct.DOFs[5]= mMotusSinStruct.Kslow * mMotusSinStruct.valueAttu[5] * sin(6.283*mMotusSinStruct.freAttu[5] * mMotusSinStruct.stepCount / 100.f + mMotusSinStruct.phaseAttu[5]);

    mMotusSinStruct.stepCount++;
    mMotusSinStruct.timeview -= 0.01f;
    if (mMotusSinStruct.timeview < 0)
    {
        mMotusSinStruct.timeview  = 0;
        mMotusSinStruct.status=2;
        mMotusSinStruct.Kslow = 0.f;
    }
    emit sendTimeView(mMotusSinStruct.timeview);
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
    ui->sendCmdEdit->setText(QString("%1").arg(nCmd));
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
        /////////////////////////////////////////返回数据显示/////////////////////////////////////
        //纵倾
        ui->zongqingtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[0],'f',2)));
        ui->zongqingrEdit->setText(QString("%1").arg(QString::number(recvStrcut.attitude[0],'f',2)));
        //横摇
        ui->hengyaotEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[1],'f',2)));
        ui->hengyaorEdit->setText(QString("%1").arg(QString::number(recvStrcut.attitude[1],'f',2)));
        //航向
        ui->hangxiangtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[2],'f',2)));
        ui->hangxiangrEdit->setText(QString("%1").arg(QString::number(recvStrcut.attitude[2],'f',2)));
        //横移
        ui->hengyitEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[3]*1000.f,'f',2)));
        ui->hengyirEdit->setText(QString("%1").arg(QString::number(recvStrcut.attitude[3]*1000.f,'f',2)));
        //前冲
        ui->qianchongtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[4]*1000.f,'f',2)));
        ui->qianchongrEdit->setText(QString("%1").arg(QString::number(recvStrcut.attitude[4]*1000.f,'f',2)));
        //升降
        ui->shengjiangtEdit->setText(QString("%1").arg(QString::number(sendStruct.DOFs[5]*1000.f,'f',2)));
        ui->shengjiangrEdit->setText(QString("%1").arg(QString::number(recvStrcut.attitude[5]*1000.f,'f',2)));
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////状态显示////////////////////////////////////////////////////
        //1号
        ui->para1Edit->setText(QString("%1").arg(QString::number(recvStrcut.para[0],'f',2)));
        ui->code1Edit->setText(QString("%1").arg(QString::number(recvStrcut.motor_code[0],'f',2)));
        //2号
        ui->para2Edit->setText(QString("%1").arg(QString::number(recvStrcut.para[1],'f',2)));
        ui->code2Edit->setText(QString("%1").arg(QString::number(recvStrcut.motor_code[1],'f',2)));
        //3号
        ui->para3Edit->setText(QString("%1").arg(QString::number(recvStrcut.para[2],'f',2)));
        ui->code3Edit->setText(QString("%1").arg(QString::number(recvStrcut.motor_code[2],'f',2)));
        //4号
        ui->para4Edit->setText(QString("%1").arg(QString::number(recvStrcut.para[3],'f',2)));
        ui->code4Edit->setText(QString("%1").arg(QString::number(recvStrcut.motor_code[3],'f',2)));
        //5号
        ui->para5Edit->setText(QString("%1").arg(QString::number(recvStrcut.para[4],'f',2)));
        ui->code5Edit->setText(QString("%1").arg(QString::number(recvStrcut.motor_code[4],'f',2)));
        //6号
        ui->para6Edit->setText(QString("%1").arg(QString::number(recvStrcut.para[5],'f',2)));
        ui->code6Edit->setText(QString("%1").arg(QString::number(recvStrcut.motor_code[5],'f',2)));
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
    static bool oldkey[5]={false,false,false,false,false};
    //qDebug()<<oldkey[0]<<oldkey[1]<<oldkey[2]<<oldkey[3]<<oldkey[4];
    if(oldkey[0]!=keyEnable[0])
    {
        if(keyEnable[0]) ui->searchBottomButton->setEnabled(true);
        else ui->searchBottomButton->setEnabled(false);
        oldkey[0]=keyEnable[0];
    }
    if(oldkey[1]!=keyEnable[1])
    {
        if(keyEnable[1]) ui->riseButton->setEnabled(true);
        else ui->riseButton->setEnabled(false);
        oldkey[1]=keyEnable[1];
    }
    if(oldkey[2]!=keyEnable[2])
    {
        if(keyEnable[2]) ui->runButton->setEnabled(true);
        else ui->runButton->setEnabled(false);
        oldkey[2]=keyEnable[2];
    }
    if(oldkey[3]!=keyEnable[3])
    {
        if(keyEnable[3]) ui->midButton->setEnabled(true);
        else ui->midButton->setEnabled(false);
        oldkey[3]=keyEnable[3];
    }
    if(oldkey[4]!=keyEnable[4])
    {
        if(keyEnable[4]) ui->downButton->setEnabled(true);
        else ui->downButton->setEnabled(false);
        oldkey[4]=keyEnable[4];
    }
    repaint();
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
    nCmd=S_CMD_FindZero;//4
}

//上升函数
void MainWindow::on_riseButton_clicked()
{
    nCmd=S_CMD_Work;//6
}

//运行函数
void MainWindow::on_runButton_clicked()
{
    nCmd=S_CMD_RUN;//9
}

//中立位函数
void MainWindow::on_midButton_clicked()
{
    nCmd=S_CMD_Back2MID;//2
}

//落下函数
void MainWindow::on_downButton_clicked()
{
    nCmd=S_CMD_Stop;//7
}

//信号槽函数
void MainWindow::recvMovieData(QList<M_MovieData>&movieData,float *data)
{
    mMotusFileData.movieData=movieData;
    mMotusFileData.status=1;
    mMotusFileData.movieCount=0;
    mMotusAngleQwtplot.dynamicXMinMAX(data);
    mMotusPlaceQwtplot.dynamicXMinMAX(&data[3]);
    function=MovieFuntion;
}

//清除文件运动
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
        mMotusSingleStepData.endAttu[i]=QString::number(recvStrcut.attitude[i],'f',2).toFloat();
        if(i<3)
        {
            mMotusSingleStepData.midAttu[i]=pos[i];
            mMotusSingleStepData.AttuSpeed[i]=speed[i];
            if(fabs(mMotusSingleStepData.endAttu[i])>fabs(mMotusSingleStepData.midAttu[i]))
                value[i]=fabs(mMotusSingleStepData.endAttu[i]);
            else  value[i]=fabs(mMotusSingleStepData.midAttu[i]);

        }
        else
        {
            mMotusSingleStepData.midAttu[i]=pos[i]/1000.f;
            mMotusSingleStepData.AttuSpeed[i]=speed[i]/1000.f;
            if(fabs(mMotusSingleStepData.endAttu[i])>fabs(mMotusSingleStepData.midAttu[i]))
                value[i]=fabs(mMotusSingleStepData.endAttu[i])*1000.f;
            else  value[i]=fabs(mMotusSingleStepData.midAttu[i])*1000.f;
        }
        mMotusSingleStepData.absAttu[i]=fabsf((mMotusSingleStepData.endAttu[i]-mMotusSingleStepData.midAttu[i])*60000.f)/10000.f;
        mMotusSingleStepData.totalCount[i]=3.1416f/(mMotusSingleStepData.AttuSpeed[i] / (mMotusSingleStepData.absAttu[i]/ 2.f))*100.f;
        if(mMotusSingleStepData.totalCount[i]>0) mMotusSingleStepData.isFinish[i]=false;
        else mMotusSingleStepData.isFinish[i]=true;
    }
    mMotusSingleStepData.stepCount=0;
    mMotusSingleStepData.status=1;
    mMotusAngleQwtplot.dynamicXMinMAX(value);
    mMotusPlaceQwtplot.dynamicXMinMAX(&value[3]);
    function=SingleStepFunction;
}

//清除单步数据
void MainWindow::clearSingleStepData()
{
    mMotusSingleStepData.stepCount=0;
    mMotusSingleStepData.status=2;
    function=FunctionNull;
}

//正弦开始
void MainWindow::recvSinData(int totaltime,float *value,float *fre, float *phase)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
            mMotusSinStruct.valueAttu[i]=value[i];
        else
            mMotusSinStruct.valueAttu[i]=value[i]/1000.f;
        mMotusSinStruct.freAttu[i]=fre[i];
        mMotusSinStruct.phaseAttu[i]=phase[i]/360.f*(2.f*3.1415926f);
    }
    mMotusSinStruct.timeview=totaltime+40;
    mMotusSinStruct.inputview=totaltime;
    mMotusSinStruct.status=1;
    mMotusSinStruct.stepCount=0;
    mMotusSinStruct.bufferStop=false;
    mMotusSinStruct.Kslow=0.f;
    mMotusSinStruct.BufferStopCount=0;
    mMotusAngleQwtplot.dynamicXMinMAX(value);
    mMotusPlaceQwtplot.dynamicXMinMAX(&value[3]);
    function=SinFunction;
}

//正弦停止
void MainWindow::recvSinStop()
{
    mMotusSinStruct.timeview=10;
    mMotusSinStruct.bufferStop=true;
    mMotusSinStruct.BufferStopCount=mMotusSinStruct.stepCount;
}

//清除正弦数据
void MainWindow::clearSinData()
{
    mMotusSinStruct.status=2;
    mMotusSinStruct.stepCount=0;
    mMotusSinStruct.bufferStop=false;
    mMotusSinStruct.Kslow=0.f;
    mMotusSinStruct.BufferStopCount=0;
    function=FunctionNull;
    emit sendSinInterrupt();
}











































































