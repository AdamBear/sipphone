#ifndef __VOICE_SESSION_H__
#define __VOICE_SESSION_H__

#include <ylstring.h>
#include <ylsmartptr.h>
#include "service_ipvp.h"
#include "voicecommon.h"

class CBaseChannelMode;

class CVoiceSession
{
public:
    // 群听类型
    enum GROUP_TYPE
    {
        GROUP_HANDSET,
        GROUP_HEADSET,
        GROUP_NONE
    };

    CVoiceSession(CHANNEL_MODE eChannel);
    virtual ~CVoiceSession();

    // 激活Session
    virtual void Activate();
    // 获得焦点，恢复通道
    void Resume();
    // 失去焦点，暂停播放
    void Pause();

    void PrintVoiceStatus() const;

    // 手动打开通道
    void SwitchChannel(CHANNEL_MODE eMode = CHANNEL_AUTO, bool bOpenEHS = false,
                       bool bForeOpenGroup = false);

    CHANNEL_MODE GetChannelAccess(const CHANNEL_MODE eMode, REAL_CHANNEL eChannel);

    // 打开物理通道
    void OpenRealChannel(REAL_CHANNEL eChannel);

    // 获取当前通道类型
    CHANNEL_MODE GetCurrentChannel() const;
    // 设置当前通道
    void SetCurrentChannel(CHANNEL_MODE eChannel, bool bNotify = true);

    GROUP_TYPE GetGroupType() const;
    void SetGroupType(GROUP_TYPE eGroupType);

    // 是否在播放信号音
    bool IsPlayingTone() const;
    // 是否在播放铃声
    bool IsPlayingRing() const;
    yl::string GetLastRingFile() const;

    // 播放信号音
    void PlayTone(int iToneType, int iAutoStopTime = 0, CHANNEL_MODE eChannel = CHANNEL_AUTO,
                  int iStrategy = 1);
    // 停止信号音
    void StopTone(int iToneType = -1);

    // 播放铃声
    void PlayRing(const char * lpszRingFile, const char * lpszDefaultRingFlieName);
    // 播放铃声（通过绝对路径）
    void PlayRingFromFilePath(const char * lpszRingPath);
    // 播放默认铃声
    void PlayDefaultRing();
    void PlayRingStart(yl::string strRingPath, bool bLoop = false, bool bNeedPlay = true);
    // 停止播放铃声
    void StopRing(bool bInMenu = false, bool bForceStopEHS = true);

    // 开启通话
    void StartTalk(int iCallID, ipvp_speech_t * pTalkParam);
    // 停止通话
    void StopTalk(int iCallID);
    // 重置通话参数
    void ResetTalk(int iCallID, void * pTalkParam);

#ifdef IF_BT_SUPPORT_PHONE
    // 开启蓝牙通话
    void StartBtTalk();
    // 停止蓝牙通话
    void StopBtTalk();
    // 设置蓝牙远端静音
    void SetBtFarMute(int iCallID, bool bFarMute);
#endif // IF_BT_SUPPORT_PHONE

    // 设置静音
    void SetMute(int iCallID, bool bMute);
    // 设置远端静音
    void SetFarMute(int iCallID, ipvp_speech_t * pTalkParam, bool bFarMute);

    // 开始会议
    void StartConf(int iCallSum, int * pCallIDList, bool isVideoConf);
    // 停止会议
    void StopConf(int iCallSum, int * pCallIDList, bool isVideoConf);

    // hold通话  http://192.168.1.99/Bug.php?BugID=12664
    void SetCallHold(int iCallID, bool bHold = true);

    // 通话中发送DTMF音
    void SendRemoteDTMF(int iCallID, int iKey, DTMF_DIR eDtmfDir);

private:
    bool         m_bPaused;         // 是否挂起
    CHANNEL_MODE m_eCurrentChannel; // 当前通道
    GROUP_TYPE   m_eGroupType;      // 群听类型
    bool         m_bPlayTone;       // 是否在播放tone
    bool         m_bPlayRing;       // 是否在播放铃声
    int          m_iPlayToneType;   // 当前播放的Tone类型
    yl::string   m_strLastRingFile; // 记录最近播放的铃声文件
    CBaseChannelMode * m_pChannelMode;

    // 根据当前语音设备状态判断逻辑通道
    CHANNEL_MODE AmendChannel(CHANNEL_MODE eChannel) const;
    // 根据逻辑通道获取物理通道
    REAL_CHANNEL GetVPMChannel(CHANNEL_MODE eChannel);
};

typedef YLSmartPtr<CVoiceSession> VoiceSessionPtr;
typedef YLWeakPtr<CVoiceSession> VoiceSessionWeakPtr;

#endif // __VOICE_SESSION_H__
