#ifndef _USB_PROCESSBAR_H
#define _USB_PROCESSBAR_H

#include "uikernel/qprocessbarutility.h"
#include "cusbtimecounter.h"

class CUSBProcessBar : public CProcessBarUtility, public CUSBTimeCounter
{
    Q_OBJECT

public:
    CUSBProcessBar(QWidget * parent = 0);
    virtual ~CUSBProcessBar();

    void InitData();

    void SetTimeValue(long nTotalSeconds, long nCurrentSeconds);

    long GetCurrentSeconds(long nTotalSeconds);

    int GetEmitValue();

    virtual void LoadPlayData(long nTotalSeconds, long nCurrentSeconds) { }

    virtual bool ProcessKeyEvent(QObject * pObject, QEvent * pEvent);

signals:
    void ProcessBarSwift(bool bIsLeftKeyPress, bool bPlay);

private Q_SLOTS:
    //长按的计时
    void OnLongPressTimeOut();

private:
    void SetLongPressTimerStart();
    void StopLongPressTimer();

private:
    //长按计时
    QTimer      m_LongPressTimer;
    //记录左键被按还是右键被按
    bool        m_bIsLeftKeyPress;
};

#endif // _USB_PROCESSBAR_H
