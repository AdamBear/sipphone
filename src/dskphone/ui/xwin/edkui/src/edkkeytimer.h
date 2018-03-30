#ifndef _KEY_TIMER_H
#define _KEY_TIMER_H

#include "singletonclassdefine.h"
#include "xwindow/xWindow.h"

class CEdkKeyTimer: public xTimerHandler
{
private:
    explicit CEdkKeyTimer();
    ~CEdkKeyTimer();

public:
    //静态成员函数获取单例
    static CEdkKeyTimer & GetInstance();
    //开启定时器
    void StartKeyTimer();
    //停止定时器
    void StopKeyTimer();

private:
    void OnLongKeyTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    xThreadTimer        m_timerKey;
};

#define _EDKKeyTimer GET_SINGLETON_INSTANCE(CEdkKeyTimer)

#endif
