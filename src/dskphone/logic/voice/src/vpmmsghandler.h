#ifndef __VPM_MSG_HANDLER_H__
#define __VPM_MSG_HANDLER_H__

#include "singletonclassdefine.h"

class CVPMMSGHandler
{
    DECLARE_SINGLETON_CLASS(CVPMMSGHandler)

public:
    // 打开通道
    void OpenChannel(REAL_CHANNEL eChannel);
    // 播放信号音
    void PlayTone(int iTone, int iStrategy = 1, int iDuration = 0);
    // 停止信号音
    void StopTone();
    // 播放铃声
    void PlayRing(const yl::string & strRing, int loop = -1);
    // 停止铃声
    void StopRing();
    // 播放按键音
    void PlayLocalDTMF(int iKey);
    // 发送远端DTMF
    void SendRemoteDTMF(int iCallID, int iKey, DTMF_DIR eDtmfDir);
    // 开启通话
    void StartTalk(int iCallID, void * pTalkParam);
    // 停止通话
    void StopTalk(int iCallID);
    // 重置通话
    void ResetTalk(int iCallID, void * pTalkParam);
    // 设置远端静音
    void SetFarMute(int iCallID, void * pTalkParam, bool bFarMute);
    // 建立会议
    void StartConf(int iCallSum, int * pCallIDList, bool isVideoConf);
    // 停止会议
    void StopConf(int iCallSum, int * pCallIDList, bool isVideoConf);
    // 设置是否静音
    void SetMute(int iCallID, bool bMute);
    // hold通话  http://192.168.1.99/Bug.php?BugID=12664
    void SetCallHold(int iCallID, bool bHold = true);
    // 重置VPM状态
    //void ResetVPM();
    // 物理上是否在播放铃声
    bool IsRealPlayRing()
    {
        return m_bRealPlayRing;
    }
    // 设置物理上播放铃声
    void SetRealPlayRing(bool bPlay)
    {
        m_bRealPlayRing = bPlay;
    }
    // 获取当前物理通道
    REAL_CHANNEL GetRealChannel()
    {
        return m_eRealChannel;
    }
    // 设置物理通道
    void SetRealChannel(REAL_CHANNEL eRealChannel)
    {
        m_eRealChannel = eRealChannel;
    }
    // 设置音量
    void SetVolume(int iVolume);
#ifdef IF_BT_SUPPORT_PHONE
    //开启蓝牙通话
    void StartBtTalk(int iCallID);
    //重置蓝牙通话
    void ResetBtTalk(int iCallID);
    // hold bt 通话
    void SetBtCallHold(int iCallID, bool bHold = true);
    //停止蓝牙通话
    void StopBtTalk(int iCallID);
    // mute bt 通话
    void SetBtFarMute(int iCallID, bool bFarMute);
#endif // IF_BT_SUPPORT_PHONE

    void PlayA2dp(bool bPlay);

private:
    int GetIpvpMode(REAL_CHANNEL eChannel);

    REAL_CHANNEL CorrectRealChannel(REAL_CHANNEL eChannel);
private:
    // 记录当前真是通道
    REAL_CHANNEL   m_eRealChannel;
    // 记录物理上是否在播放铃声
    bool           m_bRealPlayRing;
    // 记录当前开启的VPM通道
    int            m_iPvpMode;
};

// 获取全局唯一的对象
#define _VPMMSGHandler GET_SINGLETON_INSTANCE(CVPMMSGHandler)

#endif // __VPM_MSG_HANDLER_H__
