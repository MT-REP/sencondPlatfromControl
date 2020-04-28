#ifndef MOTUSRECVDATAVIEWDIALOG_H
#define MOTUSRECVDATAVIEWDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include "motussingleqwtplot.h"
#include "motusplatfromsockt.h"
namespace Ui {
class MotusRecvDataViewDialog;
}

class MotusRecvDataViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MotusRecvDataViewDialog(QWidget *parent = 0);
    ~MotusRecvDataViewDialog();
    //初始化函数
    void init();
    //关闭函数
    void closeEvent(QCloseEvent* event);
signals:
    void sendClose(bool open);
public slots:
    void recvNetData(MotusDataToHost & mMotusDataToHost);
private slots:
    void on_runButton_clicked();

private:
    Ui::MotusRecvDataViewDialog *ui;
    int viewIndex;
    MotusSingleQwtplot mMotusSingleQwtplot[6];
};

#endif // MOTUSRECVDATAVIEWDIALOG_H
