#include "fileplayer.h"

CFilePlayer::CFilePlayer(QObject * pParent)
    : QObject(pParent)
    , m_eCurrentFilePlayStatus(FILEPLAYER_STATE_ERROR)
{
    InitData();
}

CFilePlayer::~CFilePlayer()
{
}

void CFilePlayer::InitData()
{
    m_CounterTimer.setParent(this);
    StopCountTime();
    connect(&m_CounterTimer, SIGNAL(timeout()), this, SLOT(OnCountTimerOut()));
}

bool CFilePlayer::LoadFilePlayData(const FilePlayerData & structFilePlayerData)
{
    //类型判断
    m_FilePlayerData.m_eFilePlayerType = structFilePlayerData.m_eFilePlayerType;
    if (FILEPLAYERTYPE_INDEX == structFilePlayerData.m_eFilePlayerType)
    {
        m_FilePlayerData.m_nFileIndex = structFilePlayerData.m_nFileIndex;
        m_FilePlayerData.m_strFileName = FILE_PLAYER_DEFAULT_NAME;
    }
    else if (FILEPLAYERTYPE_NAME == structFilePlayerData.m_eFilePlayerType)
    {
        m_FilePlayerData.m_nFileIndex = FILE_PLAYER_DEFAULT_INDEX;
        m_FilePlayerData.m_strFileName = structFilePlayerData.m_strFileName;
    }
    else
    {
        m_FilePlayerData.m_nFileIndex = FILE_PLAYER_DEFAULT_INDEX;
        m_FilePlayerData.m_strFileName = FILE_PLAYER_DEFAULT_NAME;
    }

    //数据合法性判断
    m_FilePlayerData.m_lTotalSeconds = structFilePlayerData.m_lTotalSeconds;
    if (m_FilePlayerData.m_lTotalSeconds < FILE_PLAYER_ZERO_SECOND)
    {
        m_FilePlayerData.m_lTotalSeconds = FILE_PLAYER_ZERO_SECOND;
    }

    m_FilePlayerData.m_lCurrentSeconds = structFilePlayerData.m_lCurrentSeconds;
    if (m_FilePlayerData.m_lCurrentSeconds < FILE_PLAYER_ZERO_SECOND
            || m_FilePlayerData.m_lCurrentSeconds > m_FilePlayerData.m_lTotalSeconds)
    {
        m_FilePlayerData.m_lCurrentSeconds = FILE_PLAYER_ZERO_SECOND;
    }

    if (FILE_PLAYER_ZERO_SECOND == m_FilePlayerData.m_lCurrentSeconds)
    {
        m_eCurrentFilePlayStatus = FILEPLAYER_STATE_STOP;
    }
    else
    {
        m_eCurrentFilePlayStatus = FILEPLAYER_STATE_PAUSE;
    }

    m_FilePlayerData.m_nNextTime = structFilePlayerData.m_nNextTime;
    if (m_FilePlayerData.m_nNextTime <= FILE_PLAYER_ZERO_SECOND)
    {
        m_FilePlayerData.m_nNextTime = FILE_PLAYER_DEFAULT_SWIFT_NEXT_TIME;
    }

    m_FilePlayerData.m_nPreTime = structFilePlayerData.m_nPreTime;
    if (m_FilePlayerData.m_nPreTime <= FILE_PLAYER_ZERO_SECOND)
    {
        m_FilePlayerData.m_nPreTime = FILE_PLAYER_DEFAULT_SWIFT_PRE_TIME;
    }

    return true;
}

void CFilePlayer::StartCountTime()
{
    StopCountTime();
    m_CounterTimer.start(FILE_PLAYER_ONE_SECOND);
}

void CFilePlayer::StopCountTime()
{
    if (m_CounterTimer.isActive())
    {
        m_CounterTimer.stop();
    }
}

void CFilePlayer::OnCountTimerOut()
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return;
    }

    //如果还没有到最大值，则可以继续增加
    if (m_FilePlayerData.m_lTotalSeconds > m_FilePlayerData.m_lCurrentSeconds)
    {
        ++m_FilePlayerData.m_lCurrentSeconds;
        emit PlayStateChange();
    }
    else
        //如果已经到了最大值，则应停止计时器
    {
        LogicStop();
        m_eCurrentFilePlayStatus = FILEPLAYER_STATE_STOP;

        StopCountTime();
        emit PlayStateChange();
        emit PlayFinish();

        //如果与黑白屏统一，即播放完成后停止在结束的位置
//      ResetPlayData();
    }
}

bool CFilePlayer::JudgeTimeState()
{
    return !(m_FilePlayerData.m_lTotalSeconds <= FILE_PLAYER_ZERO_SECOND
             || m_FilePlayerData.m_lCurrentSeconds < FILE_PLAYER_ZERO_SECOND
             || m_FilePlayerData.m_lCurrentSeconds > m_FilePlayerData.m_lTotalSeconds);

    return true;
}

void CFilePlayer::ResetPlayData()
{
    m_FilePlayerData.m_lCurrentSeconds = FILE_PLAYER_ZERO_SECOND;
    StopCountTime();
    emit PlayStateChange();
}

bool CFilePlayer::Play(bool bRestart/* = false*/)
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return false;
    }

    if (bRestart)
    {
        m_FilePlayerData.m_lCurrentSeconds = FILE_PLAYER_ZERO_SECOND;
        LogicStop();
    }

    if (FILE_PLAYER_ZERO_SECOND == m_FilePlayerData.m_lCurrentSeconds)
    {
        if (LogicPlay())
        {
            m_eCurrentFilePlayStatus = FILEPLAYER_STATE_PLAY;
            ResetPlayData();
            StartCountTime();
            return true;
        }
    }
    else
    {
        if (LogicPlay())
        {
            LogicSeek(m_FilePlayerData.m_lCurrentSeconds);
            m_eCurrentFilePlayStatus = FILEPLAYER_STATE_PLAY;
            StartCountTime();
            emit PlayStateChange();
            return true;
        }
    }

    return false;
}

bool CFilePlayer::Pause(bool bRealAction/* = true*/)
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return false;
    }

    if (bRealAction)
    {
        if (LogicPause())
        {
            m_eCurrentFilePlayStatus = FILEPLAYER_STATE_PAUSE;
            StopCountTime();
            emit PlayStateChange();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        StopCountTime();
        emit PlayStateChange();
        return true;
    }

    return false;

}

bool CFilePlayer::Resume()
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return false;
    }

    LogicResume();
    if (LogicSeek(m_FilePlayerData.m_lCurrentSeconds))
    {
        m_eCurrentFilePlayStatus = FILEPLAYER_STATE_PLAY;
        emit PlayStateChange();
        StartCountTime();
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool CFilePlayer::Stop()
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return false;
    }

    if (LogicStop())
    {
        m_eCurrentFilePlayStatus = FILEPLAYER_STATE_STOP;
        ResetPlayData();
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool CFilePlayer::SeekBySecond(long lSeekTimePoint)
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return false;
    }

    if (lSeekTimePoint < FILE_PLAYER_ZERO_SECOND
            || lSeekTimePoint > m_FilePlayerData.m_lTotalSeconds)
    {
        ResetPlayData();
        return false;
    }

    if (LogicSeek(lSeekTimePoint))
    {
        emit PlayStateChange();
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool CFilePlayer::SeekByPercent(int nPercent)
{
    if (!JudgeTimeState()
            || nPercent < FILE_PLAYER_EMPTY_PERCENT
            || nPercent > FILE_PLAYER_FULL_PERCENT)
    {
        ResetPlayData();
        return false;
    }

    long lSeekTarget = (long)((float)nPercent / FILE_PLAYER_FULL_PERCENT *
                              m_FilePlayerData.m_lTotalSeconds);
    if (lSeekTarget < FILE_PLAYER_ZERO_SECOND
            || lSeekTarget > m_FilePlayerData.m_lTotalSeconds)
    {
        ResetPlayData();
        return false;
    }

    if (LogicSeek(lSeekTarget))
    {
        m_FilePlayerData.m_lCurrentSeconds = lSeekTarget;
        emit PlayStateChange();
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool CFilePlayer::SwiftNext()
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return false;
    }

    //如果当前时间小于m_lTotalSeconds - m_nNextTime则可以加m_nNextTime秒
    if (m_FilePlayerData.m_lCurrentSeconds < m_FilePlayerData.m_lTotalSeconds -
            m_FilePlayerData.m_nNextTime)
    {
        long lTargetSeconds = m_FilePlayerData.m_lCurrentSeconds + m_FilePlayerData.m_nNextTime;
        m_FilePlayerData.m_lCurrentSeconds = lTargetSeconds;
        emit PlayStateChange();
        return true;
    }
    //如果当前时间大于m_lTotalSeconds - m_nNextTime则直接移动到最后
    else
    {
        m_FilePlayerData.m_lCurrentSeconds = m_FilePlayerData.m_lTotalSeconds;
        emit PlayStateChange();
        return true;
    }

    return false;
}

bool CFilePlayer::SwiftPre()
{
    if (!JudgeTimeState())
    {
        ResetPlayData();
        return false;
    }

    //如果当前时间大于m_nPreTime则可以减m_nPreTime秒
    if (m_FilePlayerData.m_lCurrentSeconds > m_FilePlayerData.m_nPreTime)
    {
        long lTargetSeconds = m_FilePlayerData.m_lCurrentSeconds - m_FilePlayerData.m_nPreTime;
        m_FilePlayerData.m_lCurrentSeconds = lTargetSeconds;
        emit PlayStateChange();
        return true;
    }
    //如果当前时间小于m_nPreTime则直接从头开始
    else
    {
        m_FilePlayerData.m_lCurrentSeconds = FILE_PLAYER_ZERO_SECOND;
        emit PlayStateChange();
        return true;
    }

    return false;
}

bool CFilePlayer::InMoving(int nPercent)
{
    long lSeekTarget = (m_FilePlayerData.m_lTotalSeconds * nPercent / FILE_PLAYER_FULL_PERCENT);

    //如果和上一次的时间一样，则不需要重新设置
    if (m_FilePlayerData.m_lCurrentSeconds == lSeekTarget)
    {
        return true;
    }

    m_FilePlayerData.m_lCurrentSeconds = lSeekTarget;
    emit PlayStateChange();
    return true;
}
