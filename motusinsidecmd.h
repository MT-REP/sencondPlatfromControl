#ifndef MOTUSINSIDECMD_H
#define MOTUSINSIDECMD_H

#include <QDialog>

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
private slots:
    void recvSerialPortOpen(bool open);
    //TF
    void on_TFButton_clicked();
    //串口
    void on_serialButton_clicked();
    //网络
    void on_netButton_clicked();
private:
    Ui::MotusInsideCmd *ui;
    bool serialOpen;
};

#endif // MOTUSINSIDECMD_H
