#pragma once

#include "inputbutton.h"
#include "uikernel/quipushbutton.h"

class CLongPressButton: public CInputButton
{
    Q_OBJECT

public:
    CLongPressButton(QWidget* v_parent = 0);
    virtual ~CLongPressButton(void);

    virtual bool InputButtonMousePress(const QPoint& mousePressPoint);
    virtual bool InputButtonMouseRelease(const QPoint& mouseReleasePoint);

    virtual void ResetInputButtonDown();

signals:
    void SignalLongPress(int iKeyCode);

protected slots:
    virtual void OnLongPressBtnPressed();
    virtual void OnLongPressBtnReleased();

protected:
    // 启动计时器
    virtual void startTimer(int iMSecond);

    // 停止计时器
    virtual void stopTimer();

    virtual void timerEvent(QTimerEvent* pEvent);

protected:
    //定时器
    int m_nTimerID;
};
