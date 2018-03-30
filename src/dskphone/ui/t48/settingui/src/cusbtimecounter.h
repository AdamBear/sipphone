#ifndef _C_USB_TIME_COUNTER_H
#define _C_USB_TIME_COUNTER_H

#include <QObject>
#include <QTimer>
#include "usbtimelabel.h"

class CUSBTimeCounter
{
public:
    CUSBTimeCounter();
    virtual ~CUSBTimeCounter();

    virtual void LoadPlayData(long nTotalSeconds, long nCurrentSeconds) = 0;

    long CalculateSeekTimePointByPercent(int nPercent);
    long CalculatePausePointByCurrentTime();

protected:
    //开始数秒，可以从非零开始
    void SetCountTime();
    //停止数秒，没有置零，置零由具体需要实现的地方去实现
    void StopCountTime();

private:
    // 初始化数据
    void InitData();

protected:
    CUSBTimeLabel       m_strTotalTime;
    CUSBTimeLabel       m_strCurrentTime;
    //分割当前时间和总时间的斜杠
    QLabel              m_strSlash;
    long            m_nTotalSeconds;
    long            m_nCurrentSeconds;

    QTimer      m_timer;

private:
};

#endif // _C_USB_TIME_COUNTER_H
