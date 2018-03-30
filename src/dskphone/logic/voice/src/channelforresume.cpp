#include "voicechannel_include.h"
#include "channelforresume.h"
#include "talk/talklogic/include/modtalklogic.h"

#ifndef _DECT
IMPLEMENT_GETINSTANCE(CChannelForResume)

CChannelForResume::CChannelForResume()
{
    m_eChannelBeforeRing = CHANNEL_AUTO;
    m_eChannelBeforeTone = CHANNEL_AUTO;
}

CChannelForResume::~CChannelForResume()
{

}

// 停止铃声后恢复通道
void CChannelForResume::ResumeAfterRing()
{
    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
    if (pSession.IsEmpty())
    {
        return;
    }
    if (m_eChannelBeforeRing == CHANNEL_GROUP)
    {
        // 之前是Group通道，则强制恢复
        pSession->SwitchChannel(CHANNEL_GROUP, false, true);
    }
    else if (m_eChannelBeforeRing == CHANNEL_AUTO
             || m_eChannelBeforeRing == CHANNEL_RING)
    {
        // auto和ring通道不处理
    }
    else
    {
        if (!(talklogic_SessionExist()
                && m_eChannelBeforeRing == CHANNEL_IDLE))
        {
            pSession->SwitchChannel(m_eChannelBeforeRing);
        }
    }

    m_eChannelBeforeRing = CHANNEL_AUTO;
}

// 停止Tone后恢复通道
void CChannelForResume::ResumeAfterTone()
{
    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
    if (pSession.IsEmpty())
    {
        return;
    }
    if (m_eChannelBeforeTone == CHANNEL_GROUP)
    {
        // 之前是Group通道，则强制恢复
        pSession->SwitchChannel(CHANNEL_GROUP, false, true);
    }
    else if (m_eChannelBeforeTone == CHANNEL_AUTO
             || m_eChannelBeforeTone == CHANNEL_RING)
    {
        // auto和ring通道不处理
    }
    else
    {
        if (!(talklogic_SessionExist()
                && m_eChannelBeforeTone == CHANNEL_IDLE))
        {
            pSession->SwitchChannel(m_eChannelBeforeTone);
        }
    }

    m_eChannelBeforeTone = CHANNEL_AUTO;
}

// 通道键更改通道对恢复通道的影响
void CChannelForResume::ChannelKeyPress()
{
    // 有通道按键则重置
    m_eChannelBeforeRing = CHANNEL_AUTO;
    m_eChannelBeforeTone = CHANNEL_AUTO;
}
#endif // _DECT

