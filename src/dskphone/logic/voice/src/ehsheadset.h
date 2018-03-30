#ifndef __EHS_HEADSET_H__
#define __EHS_HEADSET_H__

class CEHSHeadset
{
public:
    CEHSHeadset(int iIndex);
    ~CEHSHeadset();

public:
    // 开启无线耳麦
    void Open(bool bForceToClose = false);
    // 播放铃声
    void PlayRing();
    // 停止铃声
    void StopRing(bool bForceStopEHS = true);
    // 定时器处理
    bool OnTimer(unsigned int uTimeID);
    // 按键消息处理
    void OnKeyPressed(bool bOnlyChangeChannel = false);
    // 获取耳麦在EXP模块中的Index
    int GetIndex();
    // 调整EHS的状态，如果耳麦打开则打开EHS，用于和通话状态同步
    void SwitchEHSStatus();
    // 关闭无线耳麦
    void Close(bool bForceToClose = false);
#if IF_BUG_27190
    //调用接口播放话机的铃声
    void PlayPhoneRing();
#endif
private:
    // 初始化无线耳麦的状态
    void InitStatus();
    // 给无线耳麦发送响铃消息
    void SendRingMsgToDev();


private:
    bool    m_bStatus;       // 当前EHS是打开还是关闭
    bool    m_bRing;         // 当前是否在响铃
    int     m_iRingInterval; // 响铃时间间隔,默认3秒,同时变量地址当成响铃计时器ID
    int     m_iDelayStopRing;// 响铃时间间隔,默认3秒,同时变量地址当成响铃计时器ID
    int     m_iIndex;        // 无线耳麦在扩展台链中的位置
#if IF_BUG_27190
    bool    m_bPhoneRing;
#endif
};

#endif // __EHS_HEADSET_H__
