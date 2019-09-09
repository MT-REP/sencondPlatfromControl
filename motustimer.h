#ifndef MOTUSTIMER_H
#define MOTUSTIMER_H

#include <QObject>
#include <qt_windows.h>
class MotusTimer : public QObject
{
    Q_OBJECT
public:
    explicit MotusTimer(int timer, QObject *parent = nullptr);
    ~MotusTimer();
    void start();   //开始定时器
    void stop();    //停止定时器
private:
    int m_timer;
    int m_id;
    //定时器回调函数
    static void WINAPI CALLBACK mmtimer_proc(uint timerId, uint uMsg, DWORD_PTR user, DWORD_PTR, DWORD_PTR);
signals:
    void timeout(); //信号发送
public slots:
};

#endif // MOTUSTIMER_H
