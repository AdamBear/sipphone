#ifndef _SETTINGUI_VOICE_MAIL_LIST_DELEGATE_H_
#define _SETTINGUI_VOICE_MAIL_LIST_DELEGATE_H_

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "settinguixsidelegatebase.h"
#include "baseui/include/baseuicommon.h"

class CSettinguiVoiceMailListDelegate : public CSettinguiXSIDelegatebase, public xTimerHandler
{
public:
    CSettinguiVoiceMailListDelegate();
    ~CSettinguiVoiceMailListDelegate();

    static CSettingUIDelegateBase * CreateVoiceMailListDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    virtual void OnMessageReqFinish(msgObject & msg);
    virtual void OnMessageReqFail(msgObject & msg);
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void OnCancleClick();
    virtual bool IsResetFocusWhileReload()
    {
        return true;
    }
public:
    void UpdateUIData();
    void OnStarRequest(xThreadTimer* timer, LPVOID pExtraData);
    void OnOptionClick();
    void OnOptionBoxCallBack(int nAction);
    void ProcessCallBack(int nID);
    bool EnterDetail(int nID);
    bool EnterPlay(int iIndex);
    bool AddToContacts(int nID);
protected:
    int GetFocusItemID();
    void ShowOperateMsgBox(bool bSuccessed);
    void ShowDownloadMsgBox(bool bSuccessed);
    void AddOption(VEC_OPTION_ITEM_DATA & vecOption, int nAction, const yl::string & strDisp);
    void LoadVoiceListItemData(CSettingUIPageDataBase * pPageData);
    void AddVoiceMailItem(CSettingUIPageDataBase * pPageData, const yl::string & strItemID,
                          const yl::string & strDisp, const yl::string & strValue, bool bRead);
protected:
    xThreadTimer      m_tmStarRequest;
};

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
#endif //_SETTINGUI_VOICE_MAIL_LIST_DELEGATE_H_
