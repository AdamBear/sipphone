#ifndef _E911_LOCATION_CONTROLLER_H
#define _E911_LOCATION_CONTROLLER_H

#include "config.h"
#if IF_FEATURE_GENBAND_E911
#include "messagebox/modmessagebox.h"

class CE911LocationController : public QObject,  public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    static CE911LocationController& GetInstance();

private:
    CE911LocationController(QObject* pParent = NULL);
    virtual ~CE911LocationController();

public:
    LRESULT OnE911Message(msgObject& objMessage);

    void ProcessIdleEvent();

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

    void ResetE911Location();

    void ExitE911SetUI(int nCurrentID);
    void QuitSetLoactionPage(int nCurrentID);

private:
    bool m_bPopNotify;
};

#define g_E911Controller (CE911LocationController::GetInstance())
#endif

#endif //_E911_LOCATION_CONTROLLER_H