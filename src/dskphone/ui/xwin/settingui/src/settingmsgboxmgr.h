#ifndef __SETTING_MSG_BOX_MGR_H__
#define __SETTING_MSG_BOX_MGR_H__

#include <singletonclassdefine.h>
#include "messagebox/messageboxcallback.h"
#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"
#include "xwindow-kernel/include/xmessagehandler.h"

#include <settingui/include/settinguidefine.h>


class CAsyncOperateAction
{
public:
    CAsyncOperateAction();
    virtual ~CAsyncOperateAction();

public:
    virtual void AsyncOperateCallBack() = 0;
};

typedef YLList<CAsyncOperateAction*>  LIST_ASYNC_OPERATE_ACTION;

class CSettingMsgBoxMgr : public xMessageHandler, public CMsgBoxCallBackAction
{
    DECLARE_SINGLETON_CLASS(CSettingMsgBoxMgr)

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    void ShowReboot(int nTimeDelay);
    void ShowResetFactory(int nResetType);
    void ShowUploadConfig();
    void ShowUpdateFirware();
    void ShowRebootMsgBox(int nTimeDelay, const yl::string & strNote);


    void AddAsyncOperate(CAsyncOperateAction* pAsyncOperateAction = NULL);
    void RemoveAsyncOperate(CAsyncOperateAction* pAsyncOperateAction = NULL);

#if IF_UNLOCK_CHECK_POPBOX
public:
    void ShowUnlockCheckMsgBox(bool bEnterBySetting = false);
    void ShowUnlockPwdMsgBox();
protected:
    void OnUnlockCheckCallback(CMessageBoxBase* pMessageBox);
#endif

protected:
    void OnRebooting();
    void OnResetting();
    void OnUploading();
    void OnUpdateing();
    yl::string GetResetStringByType(int nResetType);
    BOOL OnAsyncOperate(msgObject& objMessage);

protected:
    bool m_bWaitSystemOperate;
    int m_nSystemOperateData;
    bool m_bReseted;
    LIST_ASYNC_OPERATE_ACTION       m_lsAysncOperateAction;
#if IF_UNLOCK_CHECK_POPBOX
    bool m_bEnterBySetting;
#endif
};

#define g_SettingMsgBoxMgr GET_SINGLETON_INSTANCE(CSettingMsgBoxMgr)

#endif
