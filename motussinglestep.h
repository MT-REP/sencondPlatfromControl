#ifndef MOTUSSINGLESTEP_H
#define MOTUSSINGLESTEP_H

#include <QDialog>
#include <QLineEdit>
namespace Ui {
class MotusSingleStep;
}

class MotusSingleStep : public QDialog
{
    Q_OBJECT

public:
    explicit MotusSingleStep(QWidget *parent = 0);
    ~MotusSingleStep();
signals:
    void sendSingleStepData(float *pos,float *speed);
public slots:
    void recvSingleStepButton(bool valid);
private slots:
    void on_carryOutButton_clicked();
private:
    Ui::MotusSingleStep *ui;
    QLineEdit *pos[6];
    QLineEdit *speed[6];
    float attpos[6];
    float attspeed[6];
    void initPara();
    void getPara();
};

#endif // MOTUSSINGLESTEP_H
