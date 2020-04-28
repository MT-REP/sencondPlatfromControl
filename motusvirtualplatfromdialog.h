#ifndef MOTUSVIRTUALPLATFROMDIALOG_H
#define MOTUSVIRTUALPLATFROMDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include "motussocket.h"
#include "motussingleqwtplot.h"
#include "motusplatfromsockt.h"
namespace Ui {
class MotusVirtualPlatfromDialog;
}

class MotusVirtualPlatfromDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MotusVirtualPlatfromDialog(QWidget *parent = 0);
    ~MotusVirtualPlatfromDialog();
    void closeEvent(QCloseEvent* event);
    //初始化函数
    void init();
signals:
    void sendClose(bool open);
public slots:
    void recvData(char *data,int len);
private slots:
    void on_runButton_clicked();
    void on_sendButton_clicked();
private:
    Ui::MotusVirtualPlatfromDialog *ui;
    //曲线图形
    MotusSingleQwtplot mMotusSingleQwtplot[6];
    //接收数据SOCKET
    MotusSocket mMotusSocket;
    //接收结构体
    MotusDataToPlatfrom mMotusDataToPlatfrom;
    //发送结构体
    MotusDataToHost mMotusDataToHost;
    //接收个数
    int recvCount;
    //发送个数
    int sendCount;
    QString statusPlatStr[20];
    int statusPlatInt[20];
};

#endif // MOTUSVIRTUALPLATFROMDIALOG_H
