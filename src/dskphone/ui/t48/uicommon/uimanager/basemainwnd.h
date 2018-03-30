#ifndef _CBASE_MAINWND_H_
#define _CBASE_MAINWND_H_

#include <QtGui>
#include <ETLLib/ETLLib.hpp>

class CBaseDialog;
class CBaseMainWnd : public QDialog
{
    Q_OBJECT
public:
    CBaseMainWnd();
    virtual ~CBaseMainWnd();

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent)                  = 0;

    // 更新窗口
    virtual void UpdateWnd(CBaseDialog * pWnd)                                   = 0;

    // 返回Idle前的预操作,由UIManager调用
    virtual void PreReturnToIdle() = 0;
};

#endif //_BASE_MAINWND_H_
