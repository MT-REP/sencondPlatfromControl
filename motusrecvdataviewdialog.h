#ifndef MOTUSRECVDATAVIEWDIALOG_H
#define MOTUSRECVDATAVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class MotusRecvDataViewDialog;
}

class MotusRecvDataViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MotusRecvDataViewDialog(QWidget *parent = 0);
    ~MotusRecvDataViewDialog();

private slots:
    void on_runButton_clicked();

private:
    Ui::MotusRecvDataViewDialog *ui;
};

#endif // MOTUSRECVDATAVIEWDIALOG_H
