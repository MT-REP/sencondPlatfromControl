#ifndef MOTUSFILERUN_H
#define MOTUSFILERUN_H

#include <QDialog>
#include <QLineEdit>
typedef struct MovieData{
    float att[6];
    int speeff;
    MovieData()
    {
        for(int i=0;i<6;i++)
        {
           att[i]=0.0f;
        }
        speeff=0;
    }
}M_MovieData;


namespace Ui {
class MotusFileRun;
}

class MotusFileRun : public QDialog
{
    Q_OBJECT

public:
    explicit MotusFileRun(QWidget *parent = 0);
    ~MotusFileRun();
signals:
    void sendMovieData(QList<M_MovieData>&movieData,float *data);
public slots:
    void recvFileRunButton(bool valid);
    void recvMovieCount(unsigned int movieCount);
private slots:
    void on_selectFileButton_clicked();
    void on_fileDataButton_clicked();
private:
    Ui::MotusFileRun *ui;
    QList<M_MovieData> movieData; //文件数据
    float maxValue[6];
    float minValue[6];
    float maxSpeed[6];
    float maxAcc[6];
    unsigned int  totalLength;
    float fileTime;
    QString fileName;
    QLineEdit *maxValueEdit[6];
    QLineEdit *minValueEdit[6];
    QLineEdit *maxSpeedEdit[6];
    QLineEdit *maxAccEdit[6];
    void setInitValue();
    bool readtxtfile(QString filename);
    void messageView();
};

#endif // MOTUSFILERUN_H
