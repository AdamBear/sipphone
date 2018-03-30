#ifndef _SETTINGUI_VOICE_MAIL_PLAY_DELEGATE_H_
#define _SETTINGUI_VOICE_MAIL_PLAY_DELEGATE_H_

#if IF_FEATURE_BROADSOFT_VOICEMAIL

#include "settinguixsidelegatebase.h"
#include "baseui/include/baseuicommon.h"
#include "broadsoft/voicemail/include/voicemailcommon.h"
#include "ylvector.h"
#include "xwindow-kernel/include/xmessagehandler.h"

class CSettinguiVoiceMailPlayDelegate : public CSettinguiXSIDelegatebase, public xMessageHandler
{
public:
    CSettinguiVoiceMailPlayDelegate();
    ~CSettinguiVoiceMailPlayDelegate();

    static CSettingUIDelegateBase* CreateVoiceMailPlayDelegate();
public:
    static LRESULT OnMessage(msgObject& msg);
    virtual void OnMessageReqFinish(msgObject& msg);
    virtual void OnMessageReqFail(msgObject& msg);
    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);
    virtual void InitExtraData(void* pData);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    virtual void OnTimeOut();
protected:
    // 更新Title
    bool UpdateTitle();
    bool ProcessCallBack();
    bool ProcessPlay();
    bool ProcessPause();
    bool ProcessResume();
    bool ProcessStop();

    int GetTotalTime();

    void ShowDownloadMsgBox(bool bSuccessed);
    void ShowOperateMsgBox(bool bSuccessed);
    void EnterPlay(int nID);
    void AddSoftkey(CArraySoftKeyBarData& vecSoftKey, int nType, const yl::string& strDisp);

protected:
    sVoiceMailData m_sVoiceMailData;
    int m_nCurrentIndex;
    int m_nInitIndex;
};

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
#endif //_SETTINGUI_VOICE_MAIL_PLAY_DELEGATE_H_
