#ifndef __DIR_MSG_BOX_MGR_H__
#define __DIR_MSG_BOX_MGR_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "messagebox/messageboxcallback.h"
#include "directorycommon.h"
#include "baseui/include/baseuicommon.h"


class CDirMsgBoxMgr : public CMsgBoxCallBackAction
{
    DECLARE_SINGLETON_CLASS(CDirMsgBoxMgr)

public:
    // 显示提示框
    static void ShowMsgBox(const char * pszMsg, const char * pszExtra = NULL,
                           int nAction = DIR_ACTION_NONE);
    // 弹出选择提示框(确定返回true，其他返回false)
    static bool PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction,
                              int nId, const char * pszExtra = NULL);
#if IF_FEATURE_FAVORITE_DIR
    static bool PopSureEditMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId);
#endif
    // 显示等待窗口
    static void ShowWaiting(const char * pszTitle, const char * pszMsg, int nAction);
    // 关闭等待窗口
    static void EndWaiting();

    static void ShowOptionMsgBox(VEC_OPTION_ITEM_DATA & vecOption, int nID = -1,
                                 int nSelectIndex = -1, bool bShowCheckIcon = false);

protected:
    static void SetMsgBoxActionId(int nAction, int nId);
    static int PopMsgBoxActionId(int nAction);

public:
    void Init();
    void UnInit();

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    typedef YLHashMap<int, int> MAP_MSG_BOX_ACTION_ID;
    static MAP_MSG_BOX_ACTION_ID s_mapActionId;

    static bool s_bShowWaiting;
    static int s_nWaitingAction;
};

#define g_DirMsgBoxMgr GET_SINGLETON_INSTANCE(CDirMsgBoxMgr)

#endif
