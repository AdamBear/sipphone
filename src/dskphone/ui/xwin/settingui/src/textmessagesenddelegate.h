#ifndef _TEXT_MESSAGE_SEND_DELEGATE_H_
#define _TEXT_MESSAGE_SEND_DELEGATE_H_

#include <settingui/include/settinguidefine.h>
#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguidirectoryassociatebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif

#if IF_SUPPORT_SELECT_CONTACT_INFO
class CTextMessageSendDelegate : public CDirectoryAssociateBaseDelegate
#else
class CTextMessageSendDelegate : public CSettinguiDataDelegateBase
#endif
{
public:
    CTextMessageSendDelegate();
    ~CTextMessageSendDelegate();

    static CSettingUIDelegateBase * CreateTextMessageSendDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
#if IF_SUPPORT_SELECT_CONTACT_INFO
public:
    static void UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber);
protected:
    void UpdateSendData(const yl::string& strContactNumber);
    virtual bool IsShowDirectorySoftkey();
#endif
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    virtual bool OnBackAction();
protected:

    void LoadTextMessageSendItemData(CSettingUIPageDataBase * pPageData);
};

#endif //_TEXT_MESSAGE_SEND_DELEGATE_H_
