#ifndef _SETTINGUI_XSI_DELEGATE_BASE_H_
#define _SETTINGUI_XSI_DELEGATE_BASE_H_

#include "settinguiaccountbasedelegate.h"
#include "messagebox/messageboxcallback.h"

enum XSI_MSGBOX_ID
{
    XSI_MSGBOX_ID_NONE = 1009,
    XSI_MSGBOX_ID_DELETE,
    XSI_MSGBOX_ID_DELETE_ALL,
    XSI_MSGBOX_ID_FAIL,
    XSI_MSGBOX_ID_DELETEING,
    XSI_MSGBOX_ID_LOADING,
    XSI_MSGBOX_ID_SAVE,
    XSI_MSGBOX_ID_NETWORK_UNAVAIL,
    XSI_MSGBOX_ID_NO_ACCOUNT,
    XSI_MSGBOX_ID_ADD,
    XSI_MSGBOX_ID_PWD_ERROR,
    XSI_MSGBOX_ID_OPERATE,
    XSI_MSGBOX_ID_LOAD_DETAIL,
    XSI_MSGBOX_ID_INCOMPLETE_INFO,
};

class CSettinguiXSIDelegatebase : public CSettingUIAccountDelegateBase, public CMsgBoxCallBackAction
{
public:
    CSettinguiXSIDelegatebase();
    virtual ~CSettinguiXSIDelegatebase();

    static CSettingUIDelegateBase * CreateXSIDelegatebase();
public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual void OnMessageReqFinish(msgObject & msg);
    virtual void OnMessageReqFail(msgObject & msg);
protected:
    virtual void OnCancleClick();
    virtual bool IsHasAccountRegister();
    bool IsEnableToEnterNextPage(const yl::string & strPageID, int nAccountID);
    void ShowMessageBox(const yl::string & strTitle, const yl::string & strNote, int nID, int nTime,
                        int nType);
protected:
    XSI_MSGBOX_ID m_eMsgboxID;
};

#endif //_SETTINGUI_XSI_DELEGATE_BASE_H_
