#ifndef __HANDSET_JUDDER_H__
#define __HANDSET_JUDDER_H__

#include <QtGui>

#define TIMER_HANDSET_JUDDER    80
#define TIMER_HANDFREE_JUDDER   200

class CKeyJudder : public QObject
{
    Q_OBJECT
public:
    CKeyJudder(void);
    virtual ~CKeyJudder(void);

public:
    //设置按键事件防抖
    bool setKeyCheck(int iKey, bool bPress);

protected:
    //设置手柄防抖检测定时器
    void DelayHandsetCheck(int nMillionSeconds = 500);
    //设置手柄按键事件检测
    bool setHandsetCheck(int iKey, bool bPress);
    //设置免提按键事件检测
    bool setHandfreeCheck(int iKey, bool bPress);

protected slots:
    //手柄防抖检测定时器超时处理槽
    void slotCheckHandset();

private:
    //手柄防抖检测定时器
    QTimer                  m_timerHandsetCheck;
    //是否超时重传按键
    bool                    m_bDelayResendKey;
    //当前手柄按键
    int                     m_nHookKey;
    //手柄检测中手柄状态是否成对
    bool                    m_bHookPairs;
};

#endif //__HANDSET_JUDDER_H__
