#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MotusTimer;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    MotusTimer *mMotusTimer;
private:
    Ui::MainWindow *ui;
public slots:
    void masterClock(void);//主时钟
};

#endif // MAINWINDOW_H
