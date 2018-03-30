#include "usbaudiorecorddelegate.h"

#include "translateiddefine.h"
#include "common/common_data_define.h"
#include "languagehelper/modlangtranslate.h"
#ifdef IF_USB_SUPPORT
#include "record/include/modrecord.h"
#endif
#include "talk/talklogic/include/modtalklogic.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "dlgusbaudiorecord.h"
#include "usbrecordcontroler.h"
#include "pageiddefine.h"


#if (defined IF_USB_SUPPORT) && (defined IF_USB_RECORD)
CUSBAudioRecordDelegate::CUSBAudioRecordDelegate()
{
    g_UsbRecordControler.Init(kszPageIDUsbIdleRecord);
}

CUSBAudioRecordDelegate::~CUSBAudioRecordDelegate()
{
    g_UsbRecordControler.UnInit(kszPageIDUsbIdleRecord);
}

CSettingUIDelegateBase * CUSBAudioRecordDelegate::CreateUSBAudioRecordDelegate()
{
    CUSBAudioRecordDelegate * pDelegate = new CUSBAudioRecordDelegate;

    return pDelegate;
}

bool CUSBAudioRecordDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_BACK == iType)
    {
        return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
    }

    if (bLongPress)
    {
        return false;
    }

    if (ST_START_RECORD == iType)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO) && !modRecord_IsRecordPause(RECORD_AUDIO))
        {
            modRecord_RecordStart(RECORD_AUDIO, "idleREC", RECORD_SCENE_IDLE);
        }
    }
    else if (ST_STOP_RECORD == iType)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO) && !modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return false;
        }

        modRecord_RecordStop(RECORD_AUDIO);
    }
    else if (ST_PAUSE_RECORD == iType)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO))
        {
            return false;
        }

        return modRecord_RecordPause(RECORD_AUDIO);
    }
    else if (ST_RESUME_RECORD == iType)
    {
        if (!modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return false;
        }

        return modRecord_RecordResume(RECORD_AUDIO);
    }
    else if (ST_CONFERENCE == iType)
    {
        talklogic_EnterDialUI("");
    }
    else
    {
        return false;
    }

    return true;
}

void CUSBAudioRecordDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (!g_UsbRecordControler.IsIdleRecordEnabled() || vecSoftKey.size() < 4)
    {
        return;
    }

    if (modRecord_IsRecordIdle(RECORD_AUDIO))
    {
        vecSoftKey[1].m_iType = ST_START_RECORD;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_START_RECORD);
    }
    else if (modRecord_IsRecording(RECORD_AUDIO))
    {
        vecSoftKey[1].m_iType = ST_STOP_RECORD;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_STOP_RECORD);

        vecSoftKey[2].m_iType = ST_PAUSE_RECORD;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_PAUSE_RECORD);

        vecSoftKey[3].m_iType = ST_CONFERENCE;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_CONFERENCE);
    }
    else if (modRecord_IsRecordPause(RECORD_AUDIO))
    {
        vecSoftKey[1].m_iType = ST_STOP_RECORD;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_STOP_RECORD);

        vecSoftKey[2].m_iType = ST_RESUME_RECORD;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_RESUME_RECORD);

        vecSoftKey[3].m_iType = ST_CONFERENCE;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_CONFERENCE);
    }
}

#endif
