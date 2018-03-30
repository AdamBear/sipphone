#include "ringplayer.h"

#include <ETLLib.hpp>
#include "commonunits/modcommonunits.h"
#include "voice/include/modvoice.h"
#include "mainwnd/mainwnd.h"

namespace
{
#define RING_MIN_VOLUME 0
#define RING_MAX_VOLUME 15
}

qRingPlayer::qRingPlayer(QObject * parent)
    : QObject(parent)
    , m_bIsRinging(false)
    , m_strRingPath("")
{
    connect(&m_timerDelayPlayRing, SIGNAL(timeout()), this, SLOT(OnPlayRing()));
}

qRingPlayer::~qRingPlayer()
{
    StopRingPlay();
}

// 开始准备播放铃声()
void qRingPlayer::StartRingPlay(const yl::string & strRingPath, int nMinSecDelay)
{
    StopRingPlay();

    m_strRingPath = strRingPath;
    m_timerDelayPlayRing.start(nMinSecDelay);
}

void qRingPlayer::StopRingPlay()
{
    if (m_bIsRinging)
    {
        m_bIsRinging = false;
        voice_StopRingInMenu();
        _MainWND.SetVolumeBarVisible(false);
    }

    if (m_timerDelayPlayRing.isActive())
    {
        m_timerDelayPlayRing.stop();
    }
}

bool qRingPlayer::IsRinging() const
{
    return m_bIsRinging;
}

bool qRingPlayer::IsBeginPlay() const
{
    return m_timerDelayPlayRing.isActive();
}

bool qRingPlayer::IsPlayRing(const yl::string & strRingPath)
{
    return strRingPath == m_strRingPath;
}

bool qRingPlayer::AdjustVolume(bool bIncrease)
{
    int nVolume = voice_GetVolume(VT_AUTO);

    if (bIncrease)
    {
        nVolume += 1;
    }
    else
    {
        nVolume -= 1;
    }

    if (nVolume >= RING_MIN_VOLUME && nVolume <= RING_MAX_VOLUME)
    {
        voice_SetVolume(nVolume, VT_AUTO, true);
    }

    return true;
}

void qRingPlayer::OnPlayRing()
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
