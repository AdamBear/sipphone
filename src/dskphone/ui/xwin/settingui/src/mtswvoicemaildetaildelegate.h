#ifndef _MTSW_VOICEMAIL_DETAIL_DELEGATE_H_
#define _MTSW_VOICEMAIL_DETAIL_DELEGATE_H_

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"

class CMtswVoiceMailDetailDelegate : public CSettinguiDataDelegateBase
{
public:
    CMtswVoiceMailDetailDelegate();
    virtual ~CMtswVoiceMailDetailDelegate();
    static CSettingUIDelegateBase* CreateMtswVoiceMailDetailDelegate();
    static LRESULT OnMTSWVoiceMailDetailMsg(msgObject& msg);

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);

public:
    //获取VoiceMail 详情列表
    void GetVoiceMailDetail();

private:
    bool SwitchMessageDetail(bool bNext);
    // 发送处理
    void ProcessCallBack();
    // Play键处理
    void ProcessPlayKey();
    // 进入播放界面
    void EnterPlay();

};
#endif

#endif //_MTSW_VOICEMAIL_DETAIL_DELEGATE_H_
