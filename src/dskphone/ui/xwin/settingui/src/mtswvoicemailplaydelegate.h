#ifndef _MTSW_VOICE_MAIL_PLAY_DELEGATE_H_
#define _MTSW_VOICE_MAIL_PLAY_DELEGATE_H_

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "settinguilogic/include/common.h"
#include "voicemailplaybasedelegate.h"
#include "xwindow-kernel/include/xthreadtimer.h"

class CMtswVoiceMailPlayDelegate : public CVoiceMailPlayBaseDelegate, public xTimerHandler
{
public:
    CMtswVoiceMailPlayDelegate();
    virtual ~CMtswVoiceMailPlayDelegate();
    static CSettingUIDelegateBase* CreateMtswVoiceMailPlayDelegate();
    static LRESULT OnMTSWVoiceMailPlayMsg(msgObject& msg);

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
    // 打开界面附加操作
    virtual void InitExtraData(void* pData);
    // 处理按键事件
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    virtual bool IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix);
    virtual void OnTimeOut();

    // 开始播放语音
    void StartVoiceMail();
    // Play键处理
    void ProcessPlay();
    // Pause键处理
    void ProcessPause();
    // Stop键处理
    void ProcessStop();
    // Next键处理
    void ProcessNext();

    void OnDelayExit(xThreadTimer* timer, LPVOID pExtraData);

private:
    // 发送处理
    void ProcessCallBack();
    // 切换另一条消息详情
    bool SwitchMessage(bool bNext);

public:
    //获取MLHG列表
    void LoadVoiceMailPlayInfo();

protected:
    xThreadTimer        m_timerExit; // 延迟退出

};
#endif

#endif //_MTSW_VOICE_MAIL_PLAY_DELEGATE_H_
