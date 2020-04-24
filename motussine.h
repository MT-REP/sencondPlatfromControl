#ifndef MOTUSSINE_H
#define MOTUSSINE_H

#include <QDialog>
#include <QLineEdit>
namespace Ui {
class MotusSine;
}

class MotusSine : public QDialog
{
    Q_OBJECT

public:
    explicit MotusSine(QWidget *parent = 0);
    ~MotusSine();
signals:
    void sendSingleStepData(float *pos,float *speed);
    void sendSinData(float *pos,float *spd,float *value,float *fre, float *phase);
    void sendSinStop();
private:
    //单步运动 对话框和
    QLineEdit *pos[6];
    QLineEdit *speed[6];
    float attpos[6];
    float attspeed[6];
    //正弦运动
    QLineEdit *value[6];
    QLineEdit *fre[6];
    QLineEdit *phase[6];
    //得到参数
    void getPara();
private slots:
    void on_calButton_clicked();
    void on_calbutton_clicked();
    void on_sinStartButton_clicked();
    void on_buffStopButton_clicked();
    void on_carryOutButton_clicked();
private:
    Ui::MotusSine *ui;
};

#endif // MOTUSSINE_H
