#include "ringplayer.h"

#include <ETLLib.hpp>
#include "commonunits/modcommonunits.h"
#include "voice/include/modvoice.h"


CRingPlayer::CRingPlayer()
    : m_bIsRinging(false)
    , m_strRingPath("")
{
}

CRingPlayer::~CRingPlayer()
{
    StopRingPlay();
}

// 开始准备播放铃声()
void CRingPlayer::StartRingPlay(const yl::string & strRingPath, int nMinSecDelay)
{
    StopRingPlay();

    m_strRingPath = strRingPath;
    m_timerDelayPlayRing.SingleShot(nMinSecDelay, TIMER_METHOD(this, CRingPlayer::OnPlayRing));
}

void CRingPlayer::StopRingPlay()
{
    if (m_bIsRinging)
    {
        m_bIsRinging = false;
        voice_StopRingInMenu();
    }

    if (m_timerDelayPlayRing.IsTimerRuning())
    {
        m_timerDelayPlayRing.KillTimer();
    }
}

bool CRingPlayer::IsRinging() const
{
    return m_bIsRinging;
}

bool CRingPlayer::IsBeginPlay()
{
    return m_timerDelayPlayRing.IsTimerRuning();
}

void CRingPlayer::OnPlayRing(xThreadTimer* timer, LPVOID pExtraData)
{
    StopRingPlay();

    //准备播放其他铃声
    m_bIsRinging = true;

    if (m_strRingPath.empty())
    {
        //播放默认铃声
        voice_PlayDefaultRing();
    }
    else
    {
        //播放选择的铃声
        voice_PlayRingFromFilePath(m_strRingPath.c_str());
    }
}
