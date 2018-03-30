#include "dlgusbaudioplayer.h"

#include "keymap.h"
#include "xwindow/xWindow.h"
#include "xwindow/xSoftkeyBar.h"
#include "xwindow/xWindowManager.h"
#include "common/common_data_define.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/t2xpicpath.h"
#include "baseui/include/t2xwidget.h"
#include "baseui/include/timerpieceview.h"
#include "record/include/modrecord.h"
#include "record/include/recordmessagedefine.h"
#include "voice/include/voicecommon.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "uimanager/modvolumebar.h"
#include "uimanager/moduimanager.h"

#include "pageiddefine.h"
#include "uicallbackfun.h"
#include "usbrecordcontroler.h"
#include "talk/talklogic/include/modtalklogic.h"

#ifdef IF_USB_SUPPORT
namespace
{
#define DLG_USB_AUDIO_PLAYER_LAYOUT_FILE "setting/dlgusbaudioplayer.xml"
#define DLG_AUDIO_PLAYER_SOFTKEY_INDEX_PLAY_LEFT 1
#define DLG_AUDIO_PLAYER_SOFTKEY_INDEX_PLAY_RIGHT 2
#define DLG_AUDIO_PLAYER_LONG_PRESS_TIME 1000
#define DLG_AUDIO_PLAYER_DELAY_PLAY_TIME 50
#define DLG_AUDIO_PLAYER_SEEK_TIME 200
#define DLG_AUDIO_PLAYER_DELAY_EXIT_TIME 3000
#define DLG_AUDIO_PLAYER_SEEK_STEP 8
#define VOLUME_MIN_VALUE 1
#define VOLUME_MAX_VALUE 15

const char * DLG_AUDIO_PLAYER_ID_STATUS = "txtStatus";
const char * DLG_AUDIO_PLAYER_ID_TIME_PIECE = "timerPiece";
const char * DLG_AUDIO_PLAYER_ID_TOTAL_TIME = "txtTotal";
const char * DLG_AUDIO_PLAYER_ID_PROGRESS = "barProgress";
#define USB_AUDIO_PLAYER_DEBUG 0
}

CDlgUSBAudioPlayer::CDlgUSBAudioPlayer()
    : CSettingDialogBase(this, eWT_PageMenu)
    , m_nAction(TIMER_ACTION_TYPE_NONE)
    , m_bSelfExit(false)
    , m_pTimerPiece(NULL)
    , m_pTextStatus(NULL)
    , m_pTextTotalTime(NULL)
    , m_pProgressBar(NULL)
    , m_bPressed(false)
{
    m_nDialogType = SETTTING_DIALOG_SUB_TYPE_USB_PLAYER;
}

CDlgUSBAudioPlayer::~CDlgUSBAudioPlayer()
{

}

CBaseDialog * CDlgUSBAudioPlayer::CreateInstance()
{
    CDlgUSBAudioPlayer * pDlgAudioPlayer = new CDlgUSBAudioPlayer;

    if (NULL != pDlgAudioPlayer)
    {
        pDlgAudioPlayer->loadContent(DLG_USB_AUDIO_PLAYER_LAYOUT_FILE);
    }

    return pDlgAudioPlayer;
}

CDlgUSBAudioPlayer * CDlgUSBAudioPlayer::GetDlgUSBAudioPlayer(CSettingDialogBase * pDlg)
{
    if (NULL == pDlg || SETTTING_DIALOG_SUB_TYPE_USB_PLAYER != pDlg->GetDialogType())
    {
        return NULL;
    }

    return static_cast<CDlgUSBAudioPlayer *>(pDlg);
}

void CDlgUSBAudioPlayer::Uninit()
{
    m_ActionTimer.KillTimer();
    m_nAction = TIMER_ACTION_TYPE_NONE;

    if (g_UsbRecordControler.IsPlayerRunning())
    {
        g_UsbRecordControler.ProcessAudio(CUSBRecordControler::AUDIO_PLAYER_ACTION_STOP);
    }

    if (NULL != m_pTimerPiece)
    {
        SLOT_DISCONNECT(m_pTimerPiece, signalTextChanged, this, CDlgUSBAudioPlayer::OnTimerChanged);
    }
}

void CDlgUSBAudioPlayer::OnFocusChanged(bool bFocused)
{
    if (bFocused)
    {
        g_WorkStation.SetKeyLongPressTime(DLG_AUDIO_PLAYER_LONG_PRESS_TIME);

        if (modRecord_IsPlaying(RECORD_AUDIO) && (!modRecord_IsPlayPause(RECORD_AUDIO)))
        {
            CheckPlayEnd();
        }
    }
    else
    {
        Volumebar_SetVolumeBarVisible(false);

        g_WorkStation.SetKeyLongPressTime();

        ResetDelayAction();

        if (modRecord_IsPlaying(RECORD_AUDIO) && (!modRecord_IsPlayPause(RECORD_AUDIO)))
        {
            Pause();
        }

        m_bPressed = false;
    }
}

void CDlgUSBAudioPlayer::loadChildrenElements(xml_node & node)
{
    xLinearLayout::loadChildrenElements(node);

    m_pTimerPiece = static_cast<xTimerPieceView *>(getViewById(DLG_AUDIO_PLAYER_ID_TIME_PIECE));
    m_pTextStatus = static_cast<xTextView *>(getViewById(DLG_AUDIO_PLAYER_ID_STATUS));
    m_pTextTotalTime = static_cast<xTextView *>(getViewById(DLG_AUDIO_PLAYER_ID_TOTAL_TIME));
    m_pProgressBar = static_cast<xProgressBar *>(getViewById(DLG_AUDIO_PLAYER_ID_PROGRESS));

    if (NULL != m_pTimerPiece)
    {
        SLOT_CONNECT(m_pTimerPiece, signalTextChanged, this, CDlgUSBAudioPlayer::OnTimerChanged);
    }

    BindTitle(getViewById("boxTitle"));
}

bool CDlgUSBAudioPlayer::IsRejectReturnToIdle()
{
    if (m_bSelfExit
            || !g_UsbRecordControler.IsUsbConnect()
            || !g_UsbRecordControler.IsPlayerRunning())
    {
        return false;
    }

    return true;
}

bool CDlgUSBAudioPlayer::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    if (PHONE_KEY_HANDSET_ON_HOOK == nKeyCode)
    {
        m_bSelfExit = true;
    }
    else if (PHONE_KEY_HANDFREE == nKeyCode)
    {
        UIManager_ReturnToIdle();

        yl::string  strDialNum = "";
        talklogic_EnterDialUI(strDialNum);

        return true;
    }
    return false;
}

bool CDlgUSBAudioPlayer::onKeyPressedEvent(int key)
{
    m_bPressed = true;

    // 结束延时退出前，只要按下回退的Softkey，都会回退，结束退出延时
    if (PHONE_KEY_FN2 == key && TIMER_ACTION_TYPE_EXIST == m_nAction)
    {
        ResetDelayAction();
    }

    if (!g_UsbRecordControler.IsPlayerRunning())
    {
        return false;
    }

    if (PHONE_KEY_OK == key)
    {
        if (modRecord_IsPlaying(RECORD_AUDIO))
        {
            OnSoftkeyAction(ST_PAUSE, false);
        }
        else
        {
            OnSoftkeyAction(ST_PLAY, false);
        }

        return true;
    }

    return false;
}

bool CDlgUSBAudioPlayer::onKeyReleasedEvent(int key)
{
    if (!m_bPressed)
    {
        return false;
    }

    if (PHONE_KEY_FN2 == key)
    {
        if (TIMER_ACTION_TYPE_PLAY_LEFT == m_nAction)
        {
            ResetDelayAction();
        }
        else if (NULL != m_pSettingUIDelegate)
        {
            m_pSettingUIDelegate->OnSoftkeyAction(ST_PLAYLEFT, false, INVALID_ITEM_INDEX);
        }
    }
    else if (PHONE_KEY_FN3 == key)
    {
        if (TIMER_ACTION_TYPE_PLAY_RIGHT == m_nAction)
        {
            ResetDelayAction();
        }
        else if (NULL != m_pSettingUIDelegate)
        {
            m_pSettingUIDelegate->OnSoftkeyAction(ST_PLAYRIGHT, false, INVALID_ITEM_INDEX);
        }
    }

    return false;
}

bool CDlgUSBAudioPlayer::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
        yl::string & strPix)
{
    if (g_UsbRecordControler.IsPlayerRunning())
    {
        iVolumeType = VT_AUTO;
        iMin = VOLUME_MIN_VALUE;
        iMax = VOLUME_MAX_VALUE;

        return true;
    }

    return false;
}

void CDlgUSBAudioPlayer::RefreshUI(bool bReLoadData/* = false*/)
{
    CSettingDialogBase::RefreshUI(bReLoadData);

    if (bReLoadData)
    {
        RefreshAudioInfo();
        UpdateStatusText();

        StartDelayAction(TIMER_ACTION_TYPE_PLAY);
    }

    RefreshSoftkey();
}

void CDlgUSBAudioPlayer::RefreshSoftkey()
{
    CSettingDialogBase::RefreshSoftkey();

    if (!g_UsbRecordControler.IsPlayerRunning())
    {
        return;
    }

    CIconHotKey * pLeftKey = new CIconHotKey;
    CIconHotKey * pRightKey = new CIconHotKey;

    if (NULL == pLeftKey || NULL == pRightKey)
    {
        if (NULL != pLeftKey)
        {
            delete pLeftKey;
            pLeftKey = NULL;
        }

        if (NULL != pRightKey)
        {
            delete pRightKey;
            pRightKey = NULL;
        }

        return;
    }

    pLeftKey->SetType(ST_PLAYLEFT);
    pLeftKey->SetIndex(DLG_AUDIO_PLAYER_SOFTKEY_INDEX_PLAY_LEFT);
    pLeftKey->SetupLongPressSlot(this,
                                 static_cast<slotMethod>(&CDlgUSBAudioPlayer::OnIconSoftkeyLongPress));
    pLeftKey->SetIcon(PIC_LEFT_KEY);

    pRightKey->SetType(ST_PLAYRIGHT);
    pRightKey->SetIndex(DLG_AUDIO_PLAYER_SOFTKEY_INDEX_PLAY_RIGHT);
    pRightKey->SetupLongPressSlot(this,
                                  static_cast<slotMethod>(&CDlgUSBAudioPlayer::OnIconSoftkeyLongPress));
    pRightKey->SetIcon(PIC_RIGHT_KEY);

    if (!setupViewSoftKey(this, pLeftKey))
    {
        delete pLeftKey;
        pLeftKey = NULL;
    }

    if (!setupViewSoftKey(this, pRightKey))
    {
        delete pRightKey;
        pRightKey = NULL;
    }
}

void CDlgUSBAudioPlayer::RefreshAudioInfo()
{
    long lTime = g_UsbRecordControler.GetPlayTime();

    if (NULL != m_pTimerPiece)
    {
        m_pTimerPiece->SetMaxTick(lTime);
    }

    if (NULL != m_pTextTotalTime)
    {
        m_pTextTotalTime->SetText(xTimerPieceView::FormatHMS(lTime));
    }

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->SetRange(lTime);
        m_pProgressBar->SetPosition(0);
    }
}

void CDlgUSBAudioPlayer::UpdateStatusText()
{
    if (NULL == m_pTextStatus)
    {
        return;
    }

    yl::string strStatus;

    if (modRecord_IsPlaying(RECORD_AUDIO))
    {
        strStatus = _LANG(TRID_PLAYING);
    }
    else if (modRecord_IsPlayPause(RECORD_AUDIO))
    {
        strStatus = _LANG(TRID_PAUSE);
    }

    m_pTextStatus->SetText(strStatus);
}

void CDlgUSBAudioPlayer::ResetDelayAction()
{
#if USB_AUDIO_PLAYER_DEBUG
    SETTINGUI_INFO("ResetDelayAction");
#endif

    m_ActionTimer.KillTimer();
    m_nAction = TIMER_ACTION_TYPE_NONE;
}

void CDlgUSBAudioPlayer::OnIconSoftkeyLongPress(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    if (!m_bPressed)
    {
        return;
    }

    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->OnSoftkeyAction(wParam, true, INVALID_ITEM_INDEX);
    }
}

void CDlgUSBAudioPlayer::OnTimerChanged(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                        int nDataBytes, LPVOID pData)
{
    if (NULL != m_pTimerPiece)
    {
        long lDuration = m_pTimerPiece->GetDurationTime();

        if (NULL != m_pProgressBar)
        {
            m_pProgressBar->SetPosition(lDuration);
        }

#if USB_AUDIO_PLAYER_DEBUG
        SETTINGUI_INFO("OnTimerChanged:%d", lDuration);
#endif

        if (lDuration >= g_UsbRecordControler.GetPlayTime())
        {
            etl_NotifyApp(false, RECORD_PLAYRECORD_END, 0, 0);
        }
    }
}

bool CDlgUSBAudioPlayer::CheckPlayEnd()
{
    if (NULL == m_pTimerPiece)
    {
        return false;
    }

    long lDuration = m_pTimerPiece->GetDurationTime();

    if (lDuration >= g_UsbRecordControler.GetPlayTime())
    {
        etl_NotifyApp(false, RECORD_PLAYRECORD_END, 0, 0);

        return true;
    }

    return false;
}

void CDlgUSBAudioPlayer::OnDelayPlay(xThreadTimer* timer, LPVOID pExtraData)
{
    if (TIMER_ACTION_TYPE_PLAY != m_nAction)
    {
        return;
    }

    m_nAction = TIMER_ACTION_TYPE_NONE;

    g_UsbRecordControler.ProcessAudio(CUSBRecordControler::AUDIO_PLAYER_ACTION_PLAY);

    UpdateStatusText();
    RefreshSoftkey();

    if (NULL != m_pTimerPiece)
    {
        m_pTimerPiece->StartTimer();
    }
}

void CDlgUSBAudioPlayer::Pause()
{
    if (NULL == m_pTimerPiece || TIMER_ACTION_TYPE_NONE != m_nAction)
    {
        return;
    }

    m_pTimerPiece->PauseTimer();

    g_UsbRecordControler.ProcessAudio(CUSBRecordControler::AUDIO_PLAYER_ACTION_PAUSE,
                                      m_pTimerPiece->GetDurationTime());

    UpdateStatusText();
    RefreshSoftkey();
}

void CDlgUSBAudioPlayer::Resume()
{
    if (TIMER_ACTION_TYPE_NONE != m_nAction)
    {
        return;
    }

    g_UsbRecordControler.ProcessAudio(CUSBRecordControler::AUDIO_PLAYER_ACTION_RESUME);

    if (CheckPlayEnd())
    {
        return;
    }

    if (NULL != m_pTimerPiece)
    {
        m_pTimerPiece->ResumeTimer();
    }

    UpdateStatusText();
    RefreshSoftkey();
}

void CDlgUSBAudioPlayer::PlaySeek(bool bForward, bool bRepeate)
{
#if USB_AUDIO_PLAYER_DEBUG
    SETTINGUI_INFO("PlaySeek Froward:%d Repeate:%d Action:%d", (bForward ? 1 : 0), (bRepeate ? 1 : 0),
                   m_nAction);
#endif

    // 未播放前无效
    if (TIMER_ACTION_TYPE_PLAY == m_nAction)
    {
        return;
    }

    int nAction = TIMER_ACTION_TYPE_PLAY_LEFT;

    if (bForward)
    {
        nAction = TIMER_ACTION_TYPE_PLAY_RIGHT;
    }

    if (bRepeate)
    {
        m_ActionTimer.KillTimer();

        m_nAction = nAction;

        OnSeekTimer(&m_ActionTimer, NULL);

        m_ActionTimer.SetTimer(DLG_AUDIO_PLAYER_SEEK_TIME, TIMER_METHOD(this, CDlgUSBAudioPlayer::OnSeekTimer));
    }
    else
    {
        if (nAction == m_nAction)
        {
            ResetDelayAction();
        }
        else
        {
            if (bForward)
            {
                // 退出等待时，点击快进无效
                if (TIMER_ACTION_TYPE_EXIST == m_nAction)
                {
                    return;
                }
            }
            else
            {
                // 退出等待时，点击快退，关闭退出延时定时器
                if (TIMER_ACTION_TYPE_EXIST == m_nAction)
                {
                    m_ActionTimer.KillTimer();
                }
            }

            m_nAction = nAction;

            CDlgUSBAudioPlayer::OnSeekTimer(&m_ActionTimer, NULL);

            m_nAction = TIMER_ACTION_TYPE_NONE;
        }
    }
}

void CDlgUSBAudioPlayer::OnSeekTimer(xThreadTimer* timer, LPVOID pExtraData)
{
#if USB_AUDIO_PLAYER_DEBUG
    SETTINGUI_INFO("OnSeekTimer Action:%d", m_nAction);
#endif

    int nAction = CUSBRecordControler::AUDIO_PLAYER_ACTION_NONE;
    int nDeltaTime = 0;

    if (TIMER_ACTION_TYPE_PLAY_LEFT == m_nAction)
    {
        nAction = CUSBRecordControler::AUDIO_PLAYER_ACTION_PLAY_LEFT;
        nDeltaTime -= DLG_AUDIO_PLAYER_SEEK_STEP;
    }
    else if (TIMER_ACTION_TYPE_PLAY_RIGHT == m_nAction)
    {
        nAction = CUSBRecordControler::AUDIO_PLAYER_ACTION_PLAY_RIGHT;
        nDeltaTime = DLG_AUDIO_PLAYER_SEEK_STEP;
    }
    else
    {
        m_ActionTimer.KillTimer();
    }

    if (NULL != m_pTimerPiece && CUSBRecordControler::AUDIO_PLAYER_ACTION_NONE != nAction)
    {
        long lDuration = m_pTimerPiece->GetDurationTime();
        long lMaxTick = m_pTimerPiece->GetMaxTick();
        long lNewDuration = lDuration + nDeltaTime;

        if (lNewDuration < 0)
        {
            lNewDuration = 0;
        }
        else if (lNewDuration > lMaxTick)
        {
            lNewDuration = lMaxTick;
        }

#if USB_AUDIO_PLAYER_DEBUG
        SETTINGUI_INFO("Action:%d Duration:%d NewDuration:%d MaxDuration:%d DeltaTime:%d",
                       m_nAction, lDuration, lNewDuration, lMaxTick, nDeltaTime);
#endif

        if (nDeltaTime > 0 && lDuration >= lNewDuration)
        {
            ResetDelayAction();
            return;
        }

        bool bRestart = false;

        if (lDuration >= lMaxTick && lNewDuration < lDuration && (!modRecord_IsPlayPause(RECORD_AUDIO)))
        {
            g_UsbRecordControler.ProcessAudio(CUSBRecordControler::AUDIO_PLAYER_ACTION_STOP);
            g_UsbRecordControler.ProcessAudio(CUSBRecordControler::AUDIO_PLAYER_ACTION_PLAY);
            bRestart = true;
        }

        g_UsbRecordControler.ProcessAudio(nAction, lNewDuration);

        if (m_pTimerPiece->IsTimerStart())
        {
            bRestart = true;
            m_pTimerPiece->StopTimer();
        }

        m_pTimerPiece->SetStartDuration(lNewDuration);

        if (bRestart)
        {
            m_pTimerPiece->StartTimer();
        }
        else
        {
            m_pTimerPiece->UpdateTimerText();
        }
    }
}

void CDlgUSBAudioPlayer::OnStop()
{
#if USB_AUDIO_PLAYER_DEBUG
    SETTINGUI_INFO("OnStop");
#endif

    StartDelayAction(TIMER_ACTION_TYPE_EXIST);
}

void CDlgUSBAudioPlayer::OnDelayExit(xThreadTimer* timer, LPVOID pExtraData)
{
#if USB_AUDIO_PLAYER_DEBUG
    SETTINGUI_INFO("OnDelayExit:%d", m_nAction);
#endif

    if (TIMER_ACTION_TYPE_EXIST != m_nAction)
    {
        return;
    }

    m_nAction = TIMER_ACTION_TYPE_NONE;

    g_SetUICallBackFun.FallBackToPrevousPage(kszPageIDUsbAudioPlayer);
}

void CDlgUSBAudioPlayer::StartDelayAction(int nAction)
{
    m_ActionTimer.KillTimer();

    m_nAction = nAction;

    switch (nAction)
    {
    case TIMER_ACTION_TYPE_PLAY:
        {
            m_ActionTimer.SingleShot(DLG_AUDIO_PLAYER_DELAY_PLAY_TIME, TIMER_METHOD(this,
                                     CDlgUSBAudioPlayer::OnDelayPlay));
        }
        break;
    case TIMER_ACTION_TYPE_EXIST:
        {
            m_ActionTimer.SingleShot(DLG_AUDIO_PLAYER_DELAY_EXIT_TIME, TIMER_METHOD(this,
                                     CDlgUSBAudioPlayer::OnDelayExit));
        }
        break;
    case TIMER_ACTION_TYPE_PLAY_LEFT:
    case TIMER_ACTION_TYPE_PLAY_RIGHT:
        {
            m_ActionTimer.SetTimer(DLG_AUDIO_PLAYER_SEEK_TIME, TIMER_METHOD(this,
                                   CDlgUSBAudioPlayer::OnSeekTimer));
        }
        break;
    default:
        break;
    }
}
#endif
