#ifndef MOTUSSAVEDATA_H
#define MOTUSSAVEDATA_H

#include <QDialog>
#include <QList>

typedef struct DataArr
{
    float data[12];
    DataArr()
    {
        for(int i=0;i<12;i++)
        {
            data[i]=0.0f;
        }
    }
}MDataArr;

typedef struct DataSave
{
    bool isopen;
    bool isSave[6];
    QList<MDataArr>dataList;
    DataSave()
    {
        isopen=false;
        for(int i=0;i<6;i++)
        {
            isSave[i]=false;
        }
    }
}MDataSave;


namespace Ui {
class MotusSaveData;
}

class MotusSaveData : public QDialog
{
    Q_OBJECT

public:
    explicit MotusSaveData(QWidget *parent = 0);
    ~MotusSaveData();
signals:
    //是否保存数据
    void sendDataIsSave(bool *isswitch,bool issave);
    //数据保存执行
    void sendCarryOut();
public slots:
    void recvDataCarryOut(MDataSave &mMDataSave);
private slots:
    void on_saveDataButton_clicked();
    void on_datasaveCheckBox_clicked(bool checked);
private:
    Ui::MotusSaveData *ui;
};

#endif // MOTUSSAVEDATA_H
