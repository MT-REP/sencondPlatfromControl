#ifndef MOTUSSERIALDIALOG_H
#define MOTUSSERIALDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QCloseEvent>
#include "motussingleqwtplot.h"
namespace Ui {
class MotusSerialDialog;
}

class MotusSerialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MotusSerialDialog(QWidget *parent = 0);
    ~MotusSerialDialog();
    void init();
    void closeEvent(QCloseEvent* event);
signals:
    void sendClose(bool open);
private slots:
    void on_runButton_clicked();
    //接收单片机的数据
    void receiveInfo();
private:
    Ui::MotusSerialDialog *ui;
    QSerialPort mQSerialPort;
    QString splitStr;
    MotusSingleQwtplot mMotusSingleQwtplot[6];
};

#endif // MOTUSSERIALDIALOG_H
