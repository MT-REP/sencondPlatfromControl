#include "motusrecvdataviewdialog.h"
#include "ui_motusrecvdataviewdialog.h"

MotusRecvDataViewDialog::MotusRecvDataViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusRecvDataViewDialog)
{
    ui->setupUi(this);
    ui->dataBox->addItem("attitude");
    ui->dataBox->addItem("torque");
}

MotusRecvDataViewDialog::~MotusRecvDataViewDialog()
{
    delete ui;
}

void MotusRecvDataViewDialog::on_runButton_clicked()
{

}
