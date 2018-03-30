#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTA2DP_H__
#define __BTA2DP_H__

#include "wireless/bluetoothNew/include/btdefines.h"

class CBtA2dp
{
    DECLARE_SINGLETON_CLASS(CBtA2dp)

public:
    // 处理回调事件
    LRESULT OnA2dpEvent(msgObject& msg);
    // 是否a2dp消息
    bool IsA2dpMsg(unsigned long ulMsg);
    // 初始化
    bool InitA2dp(unsigned long ulHandle);
    // 注销
    bool ExitA2dp();
    // 播放操作
    bool PlayOperation(int iOperate);
    // 开始播放
    bool StartPlay(bool bResume = false);
    // 停止播放
    bool StopPlay(bool bPause = false);

    // 设置a2dp连接状态
    void SetA2dpConnect(bool bConnect)
    {
        m_bConnected = bConnect;
    }
    // a2dp当前是否连接
    bool IsA2dpConnected()
    {
        return m_bConnected;
    }

    // 是否正在播放
    bool IsPlaying();
    // 是否暂停播放
    bool IsPlayPaused();
    // 是否停止播放
    bool IsPlayStoped();

private:
    bool                m_bConnected;   // 当前是否连接
    unsigned long       m_ulHandle;     // 当前使用a2dp的设备handle
};

#define _BTA2DP GET_SINGLETON_INSTANCE(CBtA2dp)

#endif // __BTA2DP_H__
#endif // #ifdef IF_SUPPORT_BLUETOOTH
