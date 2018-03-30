#ifndef __CHANNEL_FOR_RESUME_H__
#define __CHANNEL_FOR_RESUME_H__

#ifndef _DECT
// 用于播放tone和ring之后恢复之前的通道
class CChannelForResume
{
    DECLARE_SINGLETON_CLASS(CChannelForResume)
public:
    // 设置播放铃声前的通道
    void SetChannelBeforeRing(CHANNEL_MODE eChannel)
    {
        m_eChannelBeforeRing = eChannel;
    }
    // 设置播放Tone前的通道
    void SetChannelBeforeTone(CHANNEL_MODE eChannel)
    {
        m_eChannelBeforeTone = eChannel;
    }
    // 停止铃声后恢复通道
    void ResumeAfterRing();
    // 停止Tone后恢复通道
    void ResumeAfterTone();
    // 通道键更改通道对恢复通道的影响
    void ChannelKeyPress();

private:
    // 保存变成铃声通道前的通道，停止铃声之后恢复
    CHANNEL_MODE m_eChannelBeforeRing;
    // 保存播放Tone之前的通道，停止Tone之后恢复
    CHANNEL_MODE m_eChannelBeforeTone;
};

// 获取全局唯一的对象
#define _ChannelResume GET_SINGLETON_INSTANCE(CChannelForResume)

#endif // _DECT
#endif // __CHANNEL_FOR_RESUME_H__
