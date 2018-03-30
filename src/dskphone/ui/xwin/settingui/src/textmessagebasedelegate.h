#ifndef _TEXT_MESSAGE_BASE_DELEGATE_H_
#define _TEXT_MESSAGE_BASE_DELEGATE_H_

#include "basesettinguidelegate.h"
#include "baseui/include/baseuicommon.h"
#include "messagebox/messageboxcallback.h"
#include "settinguilogic/include/common.h"

class CTextMessageBaseDelegate : public CSettingUIDelegateBase, public CMsgBoxCallBackAction
{
public:
    explicit CTextMessageBaseDelegate(int nGType = TEXT_MSG_GROUP_TYPE_NONE);
    virtual ~CTextMessageBaseDelegate();
    static CSettingUIDelegateBase * CreatTextMessageBaseDelegate();
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
public:
    void LoadTextMessageBoxItem(CSettingUIPageDataBase * pPageData, int nGroupType);
    bool GetOption(VEC_OPTION_ITEM_DATA & vecOption);
public:
    //回到Idle前的处理函数
    virtual void OnReturnToIdle();
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
protected:
    int     m_nGType;
};

#endif //_TEXT_MESSAGE_BASE_DELEGATE_H_
