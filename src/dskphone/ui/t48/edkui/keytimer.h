#ifndef _KEY_TIMER_H
#define _KEY_TIMER_H

#include "singletonclassdefine.h"

class CKeyTimer : public QObject
{
    Q_OBJECT
private:
    explicit CKeyTimer(QWidget * parent = 0);
    ~CKeyTimer();

public:
    //静态成员函数获取单例
    static CKeyTimer & GetInstance();
    //开启定时器
    void StartKeyTimer();
    //停止定时器
    void StopKeyTimer();

    bool IsKeyTimerActive() const;

private slots:
    void KeyTimeOut();

private:
    QTimer * m_pTimer;
};

#define _KeyTimer GET_SINGLETON_INSTANCE(CKeyTimer)

#endif