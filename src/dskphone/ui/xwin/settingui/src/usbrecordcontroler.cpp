#include "usbrecordcontroler.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "record/include/recordmessagedefine.h"
#include "record/include/modrecord.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/include/settinguicommondata.h"
#include "pageiddefine.h"
#include "dlgusbaudiorecord.h"
#include "dlgusbaudioplayer.h"
#include "settinguidialogmanager.h"
#include "settinguimanager.h"
#include "uicallbackfun.h"
#include "talkui/include/modtalkui.h"
#include "translateiddefine.h"

#ifdef IF_USB_SUPPORT
namespace
{
#define USB_MSGBOX_TIME_OUT 2000
}

CUSBRecordControler * CUSBRecordControler::s_pUsbRecordControler = NULL;


CUSBRecordControler::CUSBRecordControler()
    : m_bConnect(false)
    , m_nAudioIndex(-1)
    , m_lPlayTime(0)
{

}

CUSBRecordControler::~CUSBRecordControler()
{
}

CUSBRecordControler & CUSBRecordControler::GetInstance()
{
    if (NULL == s_pUsbRecordControler)
    {
        s_pUsbRecordControler = new CUSBRecordControler;
    }

    return *s_pUsbRecordControler;
}

void CUSBRecordControler::ReleaseControler()
{
    if (NULL != s_pUsbRecordControler)
    {
        CUSBRecordControler * pControler = s_pUsbRecordControler;

        s_pUsbRecordControler = NULL;

        delete pControler;
    }
}

bool CUSBRecordControler::IsBindPage(const yl::string & strPageID)
{
    LIST_STRING::iterator iter = m_listPageId.begin();

    for (; iter != m_listPageId.end(); ++iter)
    {
        const yl::string & strTemp = *iter;

        if (strPageID == strTemp)
        {
            return true;
        }
    }

    return false;
}

void CUSBRecordControler::Init(const yl::string & strPageID)
{
    if (IsBindPage(strPageID))
    {
        return;
    }

    if (m_listPageId.size() <= 0)
    {
        m_nAudioIndex = -1;
        m_bConnect = modRecord_IsStorageConnect();

        etl_RegisterMsgHandle(RECORD_MESSAGE_BEGIN, RECORD_MESSAGE_END, &CUSBRecordControler::OnMessage);
    }

    m_listPageId.push_back(strPageID);
}

void CUSBRecordControler::UnInit(const yl::string & strPageID)
{
    LIST_STRING::iterator iter = m_listPageId.begin();

    for (; iter != m_listPageId.end(); ++iter)
    {
        const yl::string & strTemp = *iter;

        if (strPageID == strTemp)
        {
            m_listPageId.erase(iter);
            break;
        }
    }

    if (m_listPageId.size() <= 0)
    {
        //来电导致其他界面关闭，在离开界面时需要停止播放，否则录音不会停止
        if (kszPageIDUsbAudioPlayer == strPageID && IsPlayerRunning())
        {
            ProcessAudio(AUDIO_PLAYER_ACTION_STOP);
        }

        m_nAudioIndex = -1;

        etl_UnregisterMsgHandle(RECORD_MESSAGE_BEGIN, RECORD_MESSAGE_END, &CUSBRecordControler::OnMessage);

        ReleaseControler();
    }
}

bool CUSBRecordControler::IsUsbConnect()
{
    return m_bConnect;
}

bool CUSBRecordControler::IsIdleRecordEnabled()
{
    return modRecord_IsRecordEnable(RECORD_AUDIO)
           && 1 == configParser_GetConfigInt(kszUSBIdleRecordEnable);
}

bool CUSBRecordControler::IsPlayerRunning()
{
    return modRecord_IsPlaying(RECORD_AUDIO) || modRecord_IsPlayPause(RECORD_AUDIO);
}

UINT CUSBRecordControler::GetRecordingTime()
{
    UINT unHour = 0;
    UINT unMin = 0;
    UINT unSec = 0;

    modRecord_GetRecordTime(unHour, unMin, unSec, RECORD_AUDIO);

    return (unHour * 3600 + unMin * 60 + unSec);
}

long CUSBRecordControler::GetPlayTime()
{
    if (m_lPlayTime > 0)
    {
        return m_lPlayTime;
    }

    RecordFileInfo infoFile;

    if (modRecord_GetResourceByIndex(infoFile, m_nAudioIndex, RECORD_AUDIO))
    {
        yl::string strFilePath = infoFile.strPath + infoFile.strName;

        //卡住问题临时解决同T49交互
        modRecord_GetResourceDetail(infoFile, strFilePath.c_str(), RECORD_AUDIO);

        m_lPlayTime = infoFile.lTime;
    }
    else
    {
        SETTINGUI_ERR("modRecord_GetResourceByIndex(%d) fail.", m_nAudioIndex);
    }

    return m_lPlayTime;
}

yl::string CUSBRecordControler::GetAudioDisplayName(const yl::string & strName)
{
    size_t pos = strName.find(".wav");
    if (yl::string::npos == pos)
    {
        pos = strName.find(".aac");
    }

    if (yl::string::npos != pos)
    {
        return strName.substr(0, pos);
    }
    else
    {
        return strName;
    }
}

bool CUSBRecordControler::GetPlayAudioName(yl::string & strName)
{
    RecordFileInfo infoFile;

    if (modRecord_GetResourceByIndex(infoFile, m_nAudioIndex, RECORD_AUDIO))
    {
        strName = GetAudioDisplayName(infoFile.strName);

        return true;
    }
    else
    {
        SETTINGUI_ERR("modRecord_GetResourceByIndex(%d) fail.", m_nAudioIndex);
    }

    return false;
}

void CUSBRecordControler::SetAudioIndex(int nIndex)
{
    m_nAudioIndex = nIndex;
    m_lPlayTime = 0;
}

int CUSBRecordControler::GetAudioIndex() const
{
    return m_nAudioIndex;
}

bool CUSBRecordControler::ProcessAudio(int nAction, long lData/* = 0*/)
{
    RecordFileInfo infoFile;

    if (!modRecord_GetResourceByIndex(infoFile, m_nAudioIndex, RECORD_AUDIO))
    {
        SETTINGUI_ERR("ProcessAudio fail! Action:%d Data:%d Index:%d", nAction, lData, m_nAudioIndex);
        return false;
    }

    yl::string strFilePath = infoFile.strPath + infoFile.strName;

    switch (nAction)
    {
    case AUDIO_PLAYER_ACTION_PLAY:
        {
            modRecord_PlayStart(RECORD_AUDIO, strFilePath.c_str());
            return true;
        }
        break;
    case AUDIO_PLAYER_ACTION_PAUSE:
        {
            modRecord_PlayPause(RECORD_AUDIO, strFilePath.c_str(), lData);
            return true;
        }
        break;
    case AUDIO_PLAYER_ACTION_RESUME:
        {
            // modRecord_PlayResume接口会在通话暂停后播放无声，
            // 按逻辑层要求，改成modRecord_PlayStart
            // 逻辑层会记录下停止时的时长，
            modRecord_PlayStart(RECORD_AUDIO, strFilePath.c_str());
            return true;
        }
        break;
    case AUDIO_PLAYER_ACTION_PLAY_LEFT:
    case AUDIO_PLAYER_ACTION_PLAY_RIGHT:
        {
            modRecord_PlaySeek(RECORD_AUDIO, strFilePath.c_str(), lData);
            return true;
        }
        break;
    case AUDIO_PLAYER_ACTION_STOP:
        {
            modRecord_PlayStop(RECORD_AUDIO, strFilePath.c_str());

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

LRESULT CUSBRecordControler::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case RECORD_STORAGE_STATE_CHANGE:
        {
            g_UsbRecordControler.OnStorageStateChange(msg);

            return TRUE;
        }
        break;
    case RECORD_STORAGE_VOLUME_WARNING:
        {
            g_UsbRecordControler.OnStorageVolumeWarning(msg);

            return TRUE;
        }
        break;
    case RECORD_PLAYRECORD_END:
        {
            g_UsbRecordControler.OnAudioPlayEnd(msg);

            return TRUE;
        }
        break;
    case RECORD_STORAGE_RECORD_RESULT:
        {
            g_UsbRecordControler.OnRecordResult(msg);

            return TRUE;
        }
        break;
    default:
        break;
    }

    return FALSE;
}

void CUSBRecordControler::OnStorageStateChange(msgObject & msg)
{
    if (0 == msg.wParam)
    {
        // 断开
        g_UsbRecordControler.m_bConnect = false;

        if (!talkui_IsInTalk()
                && (g_pSettingUIManager->IsPageDelegateCreate(kszPageIDUsbRecord)
                    || g_pSettingUIManager->IsPageDelegateCreate(kszPageIDUsbAudioPlayer)
                    || g_pSettingUIManager->IsPageDelegateCreate(kszPageIDUsbIdleRecord)))
        {
            g_SetUICallBackFun.BackToIdle();
        }
    }
    else if (1 == msg.wParam)
    {
        // 连上
        g_UsbRecordControler.m_bConnect = true;

        if (g_pSettingUIManager->GetPageDelegate(kszPageIDUsbRecord) ==
                g_pSettingUIManager->GetTopDelegate())
        {
            g_SetUICallBackFun.RefreshUI(true, kszPageIDUsbRecord);
        }
    }
}

void CUSBRecordControler::OnStorageVolumeWarning(msgObject & msg)
{
    //提示上来的是 以秒为单位的 所以 要除60
    int nTime = msg.wParam / 60;
    yl::string strWarning = commonAPI_FormatString("%d", nTime);
    strWarning += " ";
    strWarning += _LANG(TRID_MINUTES_LEFT_FOR_CALL_RECORDING);

    g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbIdleRecord, _LANG(TRID_WARNING), strWarning,
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1, 0);
}

void CUSBRecordControler::OnAudioPlayEnd(msgObject & msg)
{
    CDlgUSBAudioPlayer * pDlgAudioPlayer = CDlgUSBAudioPlayer::GetDlgUSBAudioPlayer(
            g_pSettingUIDialogManager->GetTopActiveDialog());
    CSettingUIDelegateBase * pUsbPlayerDelegate = g_pSettingUIManager->GetPageDelegate(
                kszPageIDUsbAudioPlayer);

    if (NULL == pDlgAudioPlayer || NULL == pUsbPlayerDelegate
            || pUsbPlayerDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return;
    }

    pDlgAudioPlayer->OnStop();
}

void CUSBRecordControler::OnRecordResult(msgObject & msg)
{
    CDlgUSBAudioRecord * pDlgAudioRecord = CDlgUSBAudioRecord::GetDlgUSBAudioRecord(
            g_pSettingUIDialogManager->GetTopActiveDialog());
    CSettingUIDelegateBase * pUsbRecordDelegate = g_pSettingUIManager->GetPageDelegate(
                kszPageIDUsbIdleRecord);

    if (NULL == pDlgAudioRecord || NULL == pUsbRecordDelegate
            || pUsbRecordDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return;
    }

    // waparm：0=开始录制,1=停止录制,2=截图,3=暂停录制,4=恢复录制  lparam：成功与否(0/1)
    if (0 == msg.wParam && 1 == msg.lParam)
    {
        // Start Usb Record
        pDlgAudioRecord->RefreshRecordStatus(RECORD_STATUS_START);
    }
    else if (1 == msg.wParam && 0 != msg.lParam)
    {
        // Stop Usb Record
        pDlgAudioRecord->RefreshRecordStatus(RECORD_STATUS_STOP);
    }
    else if (3 == msg.wParam && 1 == msg.lParam)
    {
        // Pause Usb Record
        pDlgAudioRecord->RefreshRecordStatus(RECORD_STATUS_PAUSE);
    }
    else if (4 == msg.wParam && 1 == msg.lParam)
    {
        // Resume Usb Record
        pDlgAudioRecord->RefreshRecordStatus(RECORD_STATUS_RESMUE);
    }
}

#endif
