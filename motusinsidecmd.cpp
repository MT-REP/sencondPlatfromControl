#include "motusinsidecmd.h"
#include "ui_motusinsidecmd.h"

MotusInsideCmd::MotusInsideCmd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusInsideCmd)
{
    ui->setupUi(this);
}

MotusInsideCmd::~MotusInsideCmd()
{
    delete ui;
}
