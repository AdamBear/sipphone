#ifndef _SETTINGUI_VOICE_MAIL_DETAIL_DELEGATE_H_
#define _SETTINGUI_VOICE_MAIL_DETAIL_DELEGATE_H_

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "settinguixsidelegatebase.h"
#include "baseui/include/baseuicommon.h"

class CSettinguiVoiceMailDetailDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiVoiceMailDetailDelegate();
    ~CSettinguiVoiceMailDetailDelegate();

    static CSettingUIDelegateBase * CreateVoiceMailDetailDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    virtual void OnMessageReqFinish(msgObject & msg);
    virtual void OnMessageReqFail(msgObject & msg);
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
protected:
    void EnterPlay();
    void ShowDownloadMsgBox(bool bSuccessed);
protected:
    int m_nVoiceMailIndex; //邮件ID
};

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
#endif //_SETTINGUI_VOICE_MAIL_DETAIL_DELEGATE_H_
