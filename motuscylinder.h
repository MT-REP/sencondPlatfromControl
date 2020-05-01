#ifndef MOTUSCYLINDER_H
#define MOTUSCYLINDER_H

#include <QDialog>

namespace Ui {
class MotusCylinder;
}

class MotusCylinder : public QDialog
{
    Q_OBJECT

public:
    explicit MotusCylinder(QWidget *parent = 0);
    void forbiddenButton();
    ~MotusCylinder();
signals:
    void sendHandCmd(int cmd);
    void sendHandMerve(unsigned char merve);
    void sendHandValue(float data);
public slots:

private slots:
    void on_handButton_clicked();
    void on_sureButton_clicked();
    void on_addButton_clicked();
    void on_subButton_clicked();
private:
    Ui::MotusCylinder *ui;
    bool addMark;
    bool subMark;
};

#endif // MOTUSCYLINDER_H
