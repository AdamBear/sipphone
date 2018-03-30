#ifndef _SETTINGUI_VIEW_VOICE_MAIL_DELEGATE_H_
#define _SETTINGUI_VIEW_VOICE_MAIL_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettinguiViewVoiceMailDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiViewVoiceMailDelegate();
    ~CSettinguiViewVoiceMailDelegate();

    static CSettingUIDelegateBase * CreateViewVoiceMailDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
protected:
    int GetRealIndex(int nIndex);
    void LoadMailAmountItem(CSettingUIPageDataBase * pPageData);
    void OnConnectPress(int nDataIndex);
    void OnEnterPress(int nDataIndex);
};

#endif //_SETTINGUI_VIEW_VOICE_MAIL_DELEGATE_H_
