#ifndef __RING_PLAYER_H__
#define __RING_PLAYER_H__

#include "ETLLib/ETLLib.hpp"
#include <ylstring.h>
#include "xwindow-kernel/include/xthreadtimer.h"


class CRingPlayer : public xTimerHandler
{
public:
    CRingPlayer();
    ~CRingPlayer();

public:
    // 开始准备播放铃声()
    void StartRingPlay(const yl::string & strRingPath, int nMinSecDelay);
    // 停止铃声播放
    void StopRingPlay();

    bool IsRinging() const;
    bool IsBeginPlay();

public:
    // 播放铃声
    void OnPlayRing(xThreadTimer* timer, LPVOID pExtraData);

private:
    //记录铃声是否在播放
    bool m_bIsRinging;

    // 铃声路径
    yl::string m_strRingPath;

    // 延迟播放铃声，解决快速切换时等2,3秒才播放铃声问题。
    xThreadTimer m_timerDelayPlayRing;
};

#endif // __RING_PLAYER_H__
