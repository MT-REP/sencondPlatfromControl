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

private:
    Ui::MotusInsideCmd *ui;
};

#endif // MOTUSINSIDECMD_H
