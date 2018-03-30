#include "dlgusbidlerecord.h"
#include "qtcommon/qmisc.h"
#include "baseui/t4xpicpath.h"
#include "commonapi/stringhelperapi.h"
#include "uikernel/languagehelper.h"
#include "record/include/modrecord.h"
#include "record/include/recordmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "imagemanager/modimagemanager.h"

namespace
{
#define ACTION_START_RECORD     "StartRecord"
#define ACTION_STOP_RECORD      "StopRecord"
#define ACTION_PAUSE_RECORD     "PauseRecord"
#define ACTION_RESUME_RECORD    "ResumeRecord"
#define ACTION_NEW_CALL         "NewCall"

#define IDLE_RECORD_TIME_LEFT       20
#define IDLE_RECORD_TIME_TOP        60
#define IDLE_RECORD_TIME_WIDTH      480
#define IDLE_RECORD_TIME_HEIGHT     200

#define IDLE_RECORD_ICON_LEFT       110
#define IDLE_RECORD_ICON_TOP        132
#define IDLE_RECORD_ICON_WIDTH      60
#define IDLE_RECORD_ICON_HEIGHT     60
}

LRESULT OnIdleRecordMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnIdleRecordMsg [%d] [%d] [%d]", objMessage.message, objMessage.wParam,
                   objMessage.wParam);

    CDlgUSBIdleRecord * pUSBIdleRecord = qobject_cast<CDlgUSBIdleRecord *>(UIManager_GetPrivateDlg(
            DLG_CDlgUSBIdleRecord));
    if (NULL != pUSBIdleRecord || pUSBIdleRecord->IsTopPage())
    {
        pUSBIdleRecord->OnReceiveMsg(objMessage);
    }

    return 0;
}

CDlgUSBIdleRecord::CDlgUSBIdleRecord(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitPageData();
}

CDlgUSBIdleRecord::~CDlgUSBIdleRecord()
{
}

void CDlgUSBIdleRecord::Init()
{
    etl_RegisterMsgHandle(RECORD_STORAGE_VOLUME_WARNING, RECORD_STORAGE_RECORD_RESULT
                          , OnIdleRecordMsg);
}

void CDlgUSBIdleRecord::Uninit()
{
    etl_UnregisterMsgHandle(RECORD_STORAGE_VOLUME_WARNING, RECORD_STORAGE_RECORD_RESULT,
                            OnIdleRecordMsg);
}

bool CDlgUSBIdleRecord::IsStatusChanged()
{
    return false;
}

bool CDlgUSBIdleRecord::SavePageData()
{
    return true;
}

void CDlgUSBIdleRecord::OnReceiveMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("CDlgUSBIdleRecord::OnReceiveMsg[%d] [%d] [%d]", objMessage.message,
                   objMessage.wParam, objMessage.lParam);

    switch (objMessage.message)
    {
    case RECORD_STORAGE_VOLUME_WARNING:
        {
            //提示上来的是 以秒为单位的 所以 要除60
            int iTime = objMessage.wParam / 60;
            yl::string strWarning = commonAPI_FormatString("%d", iTime);
            strWarning += " ";
            strWarning += fromQString(LANG_TRANSLATE(TRID_MINUTES_LEFT_FOR_CALL_RECORDING));
            strWarning += ".";
            MessageBox_ShowCommonMsgBox(NULL, toQString(strWarning), MESSAGEBOX_Cancel);
        }
        break;
    case RECORD_STORAGE_SPACE_NOTENOUGH:
        {
            //http://bugfree.yealink.com/Bug.php?BugID=133002 如果开启该功能 尽量把弹框改成 cancel型
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SPACE_FULL_SAVE_FILE), MESSAGEBOX_NOTE, 3000);
        }
        break;
    case RECORD_STORAGE_RECORD_RESULT:
        {
            if (objMessage.wParam == 0
                    && objMessage.lParam == 1)
            {
                StartUSBRecord();
            }
            else if (objMessage.wParam == 1
                     && objMessage.lParam != 0)
            {
                StopUSBRecord();
            }
            else if (objMessage.wParam == 3
                     && objMessage.lParam == 1)
            {
                PauseUSBRecord();
            }
            else if (objMessage.wParam == 4
                     && objMessage.lParam == 1)
            {
                StartUSBRecord();
            }

            emit RefreshSoftkey();
        }
        break;
    default:
        break;
    }
}

//界面初始化
void CDlgUSBIdleRecord::InitPageData()
{
    QFont fontText;
    fontText.setPointSize(55);
    fontText.setBold(true);

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    m_RecordTime.setPalette(pe);

    m_RecordIcon.setParent(this);
    m_RecordIcon.setGeometry(IDLE_RECORD_ICON_LEFT, IDLE_RECORD_ICON_TOP,
                             IDLE_RECORD_ICON_WIDTH, IDLE_RECORD_ICON_HEIGHT);
    m_RecordIcon.setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

    m_RecordTime.setParent(this);
    m_RecordTime.setFont(fontText);
    m_RecordTime.setGeometry(IDLE_RECORD_TIME_LEFT, IDLE_RECORD_TIME_TOP,
                             IDLE_RECORD_TIME_WIDTH, IDLE_RECORD_TIME_HEIGHT);
    m_RecordTime.setAlignment(Qt::AlignVCenter | Qt::AlignCenter);
    m_RecordTime.SetTime(0, 0, 0);
    QString strNote = "";
    m_pGeneralFrame->SetNote(strNote);

    SetRejectReturnToIdle();
}

bool CDlgUSBIdleRecord::LoadPageData()
{
    if (modRecord_IsRecordIdle(RECORD_AUDIO))
    {
        StopUSBRecord();
    }
    else if (modRecord_IsRecording(RECORD_AUDIO))
    {
        StartUSBRecord();
    }
    else if (modRecord_IsRecordPause(RECORD_AUDIO))
    {
        PauseUSBRecord();
    }

    UINT uHour;
    UINT uMinute;
    UINT uSecond;
    modRecord_GetRecordTime(uHour, uMinute, uSecond, RECORD_AUDIO);
    m_RecordTime.SetTime(uHour, uMinute, uSecond);
    SETTINGUI_INFO("CDlgUSBIdleRecord::LoadPageData() uHour[%d] uMinute[%d] uSecond[%d]", uHour,
                   uMinute, uSecond);
    return true;
}

bool CDlgUSBIdleRecord::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    if (modRecord_IsRecording(RECORD_AUDIO))
    {
        objSoftData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_PAUSE_RECORD);
        objSoftData[0].m_strSoftkeyAction = ACTION_PAUSE_RECORD;
        objSoftData[0].m_strIconPath = PIC_SOFTKEY_RECORD_PAUSERECORD;

        objSoftData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_STOP_RECORD);
        objSoftData[1].m_strSoftkeyAction = ACTION_STOP_RECORD;
        objSoftData[1].m_strIconPath = PIC_SOFTKEY_RECORD_STOPRECORD;

        objSoftData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_NEW_CALL);
        objSoftData[2].m_strSoftkeyAction = ACTION_NEW_CALL;
        objSoftData[2].m_strIconPath = PIC_SOFTKEY_NEWCALL;
    }
    else if (modRecord_IsRecordPause(RECORD_AUDIO))
    {
        objSoftData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_RESUME_RECORD);
        objSoftData[0].m_strSoftkeyAction = ACTION_RESUME_RECORD;
        objSoftData[0].m_strIconPath = PIC_SOFTKEY_RECORD_RESUMERECORD;

        objSoftData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_STOP_RECORD);
        objSoftData[1].m_strSoftkeyAction = ACTION_STOP_RECORD;
        objSoftData[1].m_strIconPath = PIC_SOFTKEY_RECORD_STOPRECORD;

        objSoftData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_NEW_CALL);
        objSoftData[2].m_strSoftkeyAction = ACTION_NEW_CALL;
        objSoftData[2].m_strIconPath = PIC_SOFTKEY_NEWCALL;
    }
    else
    {
        if (modRecord_IsRecordEnable(RECORD_AUDIO))
        {
            objSoftData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_START_RECORD);
            objSoftData[0].m_strSoftkeyAction = ACTION_START_RECORD;
            objSoftData[0].m_strIconPath = PIC_SOFTKEY_RECORD_STARTRECORD;
        }
    }

    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

bool CDlgUSBIdleRecord::PorcessHomeBtnClick()
{
    emit BtnBackClick();
    UIManager_ReturnToIdle(RETURN_IDLE_TYPE_HOME_PRESS);
    return true;
}

bool CDlgUSBIdleRecord::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("CDlgUSBIdleRecord::OnAction[%s]", strAction.toUtf8().data());
    bool bHandled = true;
    if (strAction == ACTION_START_RECORD)
    {
        if (!modRecord_IsStorageConnect()
                || modRecord_IsRecording(RECORD_AUDIO))
        {
            return true;
        }

        return modRecord_RecordStart(RECORD_AUDIO, "Idle_REC", RECORD_SCENE_IDLE);
    }
    else if (strAction == ACTION_STOP_RECORD)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO)
                && !modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return true;
        }

        return modRecord_RecordStop(RECORD_AUDIO);
    }
    else if (strAction == ACTION_PAUSE_RECORD)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO))
        {
            return true;
        }

        return modRecord_RecordPause(RECORD_AUDIO);
    }
    else if (strAction == ACTION_RESUME_RECORD)
    {
        if (!modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return true;
        }

        return modRecord_RecordResume(RECORD_AUDIO);
    }
    else if (strAction == ACTION_NEW_CALL)
    {
        talklogic_EnterDialUI("");
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

void CDlgUSBIdleRecord::StartUSBRecord()
{
    if (m_RecordTime.IsTimerStart())
    {
        m_RecordTime.PauseTimer();
    }
    m_RecordTime.StartTimer();
    m_RecordIcon.setPixmap(THEME_GET_BMP(PIC_IDLE_USB_RECORD_STARTRECORD));
}

void CDlgUSBIdleRecord::PauseUSBRecord()
{
    m_RecordTime.PauseTimer();
    m_RecordIcon.setPixmap(THEME_GET_BMP(PIC_IDLE_USB_RECORD_PAUSERECORD));
}

void CDlgUSBIdleRecord::StopUSBRecord()
{
    m_RecordTime.StopTimer();
    m_RecordIcon.setPixmap(THEME_GET_BMP(PIC_IDLE_USB_RECORD_STOPRECORD));
}
