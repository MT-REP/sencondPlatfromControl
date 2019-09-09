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
    void sendSinData(int totaltime,float *value,float *fre, float *phase);
    void sendSinStop();
public slots:
    void recvTimeView(float timeView);
    void recvSinInterrupt(void);
private:
    QLineEdit *value[6];
    QLineEdit *fre[6];
    QLineEdit *phase[6];
private slots:
    void on_calButton_clicked();
    void on_calbutton_clicked();
    void on_sinStartButton_clicked();
    void on_buffStopButton_clicked();

private:
    Ui::MotusSine *ui;
};

#endif // MOTUSSINE_H
