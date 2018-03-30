#include "fileplayercontroller.h"
#include "dsklog/log.h"

CFilePlayerController::CFilePlayerController(QObject * pParent)
    : QObject(pParent)
    , m_pFilePlayer(NULL)
    , m_bStartTime(true)
    , m_bSwifted(false)
    , m_bSwiftToNext(true)
    , m_nSwiftStartTime(FILE_PLAYER_DEFAULT_SWIFT_STARTTIME)
    , m_nSwiftInterval(FILE_PLAYER_DEFAULT_SWIFT_INTERVAL)
    , m_eLastFilePlayStatus(FILEPLAYER_STATE_ERROR)
{
    InitData();
}

CFilePlayerController::~CFilePlayerController()
{

}

FILEPLAYER_STATE CFilePlayerController::GetCurrentPlayState()
{
    if (NULL == m_pFilePlayer)
    {
        return FILEPLAYER_STATE_ERROR;
    }

    //当last等于error的时候，说明不处于快进/快退中，才可以获取player中的状态
    if (FILEPLAYER_STATE_ERROR == m_eLastFilePlayStatus)
    {
        return m_pFilePlayer->GetCurrentPlayState();
    }
    //否则以return状态作为当前状态
    else
    {
        return m_eLastFilePlayStatus;
    }

    return FILEPLAYER_STATE_ERROR;
}

FilePlayerData CFilePlayerController::GetPlayerData()
{
    if (NULL != m_pFilePlayer)
    {
        return m_pFilePlayer->GetPlayerData();
    }

    static FilePlayerData tmpFilePlayerData;
    return tmpFilePlayerData;
}

void CFilePlayerController::InitData()
{
    m_SwiftTimer.setParent(this);
    StopSwiftTime();
    connect(&m_SwiftTimer, SIGNAL(timeout()), this, SLOT(OnSwiftTimerOut()));
}

void CFilePlayerController::SetFilePlayer(CFilePlayer * pFilePlayer)
{
    if (NULL == pFilePlayer)
    {
        return;
    }

    m_pFilePlayer = pFilePlayer;
    m_pFilePlayer->setParent(this);
    connect(m_pFilePlayer, SIGNAL(PlayStateChange()), this, SLOT(OnPlayStateChange()));
}

void CFilePlayerController::LoadFilePlayData(const FilePlayerData & structFilePlayerData)
{
    if (NULL == m_pFilePlayer)
    {
        return;
    }

    m_pFilePlayer->LoadFilePlayData(structFilePlayerData);
}

bool CFilePlayerController::SwiftAction()
{
    SETTINGUI_INFO("CFilePlayerController::SwiftAction");

    if (NULL == m_pFilePlayer)
    {
        return false;
    }

    if (m_bSwiftToNext)
    {
        return m_pFilePlayer->SwiftNext();
    }
    else
    {
        return m_pFilePlayer->SwiftPre();
    }

    return false;
}

bool CFilePlayerController::StartSwiftTime(bool bStartTime)
{
    SETTINGUI_INFO("CFilePlayerController::StartSwiftTime, %d", bStartTime);

    if (NULL == m_pFilePlayer)
    {
        return false;
    }

    m_bStartTime = bStartTime;

    //只有一开始的启动计时需要去暂停
    if (bStartTime)
    {
        //快进/快退前，先暂停
        //记录快进/快退操作前，文件的播放状态
        m_eLastFilePlayStatus = m_pFilePlayer->GetCurrentPlayState();

        //如果是play状态，就去暂停
        if (FILEPLAYER_STATE_PLAY == m_eLastFilePlayStatus)
        {
            if (!m_pFilePlayer->Pause(false))
            {
                return false;
            }
        }
        else if (FILEPLAYER_STATE_STOP == m_eLastFilePlayStatus)
        {
            m_eLastFilePlayStatus = FILEPLAYER_STATE_PLAY;
        }
        else
        {
            //
        }
    }

    StopSwiftTime();
    if (m_bStartTime)
    {
        m_SwiftTimer.start(m_nSwiftStartTime);
    }
    else
    {
        m_SwiftTimer.start(m_nSwiftInterval);
    }

    return true;
}

void CFilePlayerController::StopSwiftTime()
{
    SETTINGUI_INFO("CFilePlayerController::StopSwiftTime");

    if (m_SwiftTimer.isActive())
    {
        m_SwiftTimer.stop();
    }
}

bool CFilePlayerController::SwiftOver()
{
    SETTINGUI_INFO("CFilePlayerController::SwiftOver");

    if (NULL == m_pFilePlayer)
    {
        return false;
    }

    StopSwiftTime();

    if (m_bSwifted)
    {
        m_bSwifted = false;
    }
    else
    {
        //如果一次都没有快进/快退过，说明按下后立刻抬起了，所以进行一次快进/快退
        SwiftAction();
    }

    const FilePlayerData & tmpPlayData = m_pFilePlayer->GetPlayerData();
    SETTINGUI_INFO("CFilePlayerController::SwiftOver, current time = %d",
                   tmpPlayData.m_lCurrentSeconds);

    //快进/快退完成后，如果原来是暂停
    if (FILEPLAYER_STATE_PAUSE == m_eLastFilePlayStatus)
    {
        m_eLastFilePlayStatus = FILEPLAYER_STATE_ERROR;

        if (m_pFilePlayer->SeekBySecond(tmpPlayData.m_lCurrentSeconds))
        {
            m_pFilePlayer->Pause();
        }
        else
        {
            return false;
        }
    }
    else
    {
        m_eLastFilePlayStatus = FILEPLAYER_STATE_ERROR;

        m_pFilePlayer->Resume();
    }

    return true;
}

void CFilePlayerController::OnSwiftTimerOut()
{
    SETTINGUI_INFO("CFilePlayerController::OnSwiftTimerOut");

    if (m_bStartTime)
    {
        //如果是启动计时结束，则停止计时器，然后重新进行间隔计时
        StopSwiftTime();

        //记录已经至少快进/快退过一次
        m_bSwifted = true;

        //进行第一次快进/快退操作
        if (SwiftAction())
        {
            StartSwiftTime(false);
        }
    }
    else
    {
        //如果是间隔计时时间到，则进行一次快进/快退操作
        if (SwiftAction())
        {
            //
        }
        else
        {
            StopSwiftTime();
        }
    }
}

bool CFilePlayerController::StartMove()
{
    if (NULL == m_pFilePlayer)
    {
        return false;
    }

    //拖动前，先暂停
    //记录快进/快退操作前，文件的播放状态
    m_eLastFilePlayStatus = m_pFilePlayer->GetCurrentPlayState();

    //如果是play状态，就去暂停
    if (FILEPLAYER_STATE_PLAY == m_eLastFilePlayStatus)
    {
        if (m_pFilePlayer->Pause())
        {
            return true;
        }
    }

    return false;
}

bool CFilePlayerController::InMoving(int nPercent)
{
    if (NULL == m_pFilePlayer)
    {
        return false;
    }

    return m_pFilePlayer->InMoving(nPercent);
}

bool CFilePlayerController::MoveOver(long lTargetTime)
{
    if (NULL == m_pFilePlayer)
    {
        return false;
    }

    m_pFilePlayer->SeekByPercent(lTargetTime);
    //拖动完成后，如果原来是暂停
    if (FILEPLAYER_STATE_PAUSE == m_eLastFilePlayStatus)
    {
        m_eLastFilePlayStatus = FILEPLAYER_STATE_ERROR;

        if (!m_pFilePlayer->Pause())
        {
            return false;
        }
    }
    else
    {
        m_eLastFilePlayStatus = FILEPLAYER_STATE_ERROR;

        if (!m_pFilePlayer->Resume())
        {
            return false;
        }
    }

    return true;
}

bool CFilePlayerController::OnPlayerAction(QString strAction, QVariant var)
{
    SETTINGUI_INFO("CFilePlayerController::OnPlayerAction, %s", strAction.toUtf8().data());

    if (NULL == m_pFilePlayer)
    {
        return false;
    }

    bool bResult = false;

    if (FILEPLAYER_ACTION_PLAY == strAction)
    {
        bResult = m_pFilePlayer->Play(true);
    }
    else if (FILEPLAYER_ACTION_PAUSE == strAction)
    {
        bResult = m_pFilePlayer->Pause();
    }
    else if (FILEPLAYER_ACTION_RESUME == strAction)
    {
        bResult = m_pFilePlayer->Resume();
    }
    else if (FILEPLAYER_ACTION_STOP == strAction
             || FILEPLAYER_ACTION_STOPONLY == strAction)
    {
        bResult = m_pFilePlayer->Stop();
    }
    /////////////////////////////////
    else if (FILEPLAYER_ACTION_SEEK_SECOND == strAction)
    {
        bResult = m_pFilePlayer->SeekBySecond(var.value<long>());
    }
    /////////////////////////////////
    else if (FILEPLAYER_ACTION_SEEK_PERCENT_START == strAction)
    {
        bResult = StartMove();
    }
    else if (FILEPLAYER_ACTION_SEEK_PERCENT_MOVE == strAction)
    {
        bResult = InMoving(var.value<long>());
    }
    else if (FILEPLAYER_ACTION_SEEK_PERCENT_OVER == strAction)
    {
        bResult = MoveOver(var.value<long>());
    }
    /////////////////////////////////
    else if (FILEPLAYER_ACTION_SWIFT_NEXT_START == strAction)
    {
        m_bSwiftToNext = true;
        m_bSwifted = false;
        bResult = StartSwiftTime(true);
    }
    else if (FILEPLAYER_ACTION_SWIFT_NEXT_OVER == strAction)
    {
        bResult = SwiftOver();
    }
    else if (FILEPLAYER_ACTION_SWIFT_PRE_START == strAction)
    {
        m_bSwiftToNext = false;
        m_bSwifted = false;
        bResult = StartSwiftTime(true);
    }
    else if (FILEPLAYER_ACTION_SWIFT_PRE_OVER == strAction)
    {
        bResult = SwiftOver();
    }
    else
    {
        //其他不需要controller关心的操作应返回true
        bResult = true;
    }

    return bResult;
}

void CFilePlayerController::OnPlayStateChange()
{
    //
}

