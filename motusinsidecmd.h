#ifndef MOTUSINSIDECMD_H
#define MOTUSINSIDECMD_H

#include <QDialog>
#include "motusplatfromsockt.h"
namespace Ui {
class MotusInsideCmd;
}

class MotusInsideCmd : public QDialog
{
    Q_OBJECT

public:
    explicit MotusInsideCmd(QWidget *parent = 0);
    ~MotusInsideCmd();
    bool CheckAppStatus(const QString &appName);
signals:
    //发送数据
    void sendNetDataView(bool view);
    //是否可显示
    void sendNetData(MotusDataToHost&);
private slots:
    //接收串口关闭
    void recvSerialPortOpen(bool open);
    //
    void recvNetDataOpen(bool open);
    //TF
    void on_TFButton_clicked();
    //串口
    void on_serialButton_clicked();
    //网络
    void on_netButton_clicked();
    //虚拟按键
    void on_virtualButton_clicked();
    //接收虚拟开关
    void recvVirtualOpen(bool open);
private:
    Ui::MotusInsideCmd *ui;
    bool serialOpen;
    bool recvNetOpen;
    bool virtualOpen;
};

#endif // MOTUSINSIDECMD_H
