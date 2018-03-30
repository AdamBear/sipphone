#include "dlgusbaudiorecord.h"

#include "translateiddefine.h"
#include <xwindow/xlistviewdefine.h>
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/t2xpicpath.h"
#include "baseui/include/timerpieceview.h"
#include "basesettinguidelegate.h"
#include "messagebox/modmessagebox.h"
#include "record/include/modrecord.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "usbrecordcontroler.h"


#if (defined IF_USB_SUPPORT) && (defined IF_USB_RECORD)
namespace
{
const char * DLG_USB_AUDIO_RECORD_LAYOUT_FILE = "setting/dlgusbaudiorecord.xml";
const char * DLG_USB_AUDIO_RECORD_ID_STATUS = "imgStatus";
const char * DLG_USB_AUDIO_RECORD_ID_TIMER = "timerView";
}


CDlgUSBAudioRecord::CDlgUSBAudioRecord()
    : CSettingDialogBase(this, eWT_PageMenu)
    , m_pIconStatus(NULL)
    , m_pTimerView(NULL)
    , m_bSelfExit(false)
    , m_bReShow(false)
{
    m_nDialogType = SETTTING_DIALOG_TYPE_AUDIO_RECORD;
}

CDlgUSBAudioRecord::~CDlgUSBAudioRecord()
{

}

CBaseDialog * CDlgUSBAudioRecord::CreateInstance()
{
    CDlgUSBAudioRecord * pDlgUSBAudioRecord = new CDlgUSBAudioRecord;

    if (NULL != pDlgUSBAudioRecord)
    {
        pDlgUSBAudioRecord->loadContent(DLG_USB_AUDIO_RECORD_LAYOUT_FILE);
    }

    return pDlgUSBAudioRecord;
}

CDlgUSBAudioRecord * CDlgUSBAudioRecord::GetDlgUSBAudioRecord(CSettingDialogBase * pDlg)
{
    if (NULL == pDlg || SETTTING_DIALOG_TYPE_AUDIO_RECORD != pDlg->GetDialogType())
    {
        return NULL;
    }

    return static_cast<CDlgUSBAudioRecord *>(pDlg);
}

void CDlgUSBAudioRecord::loadChildrenElements(xml_node & node)
{
    xLinearLayout::loadChildrenElements(node);

    m_pIconStatus = static_cast<xImageView *>(getViewById(DLG_USB_AUDIO_RECORD_ID_STATUS));
    m_pTimerView = static_cast<xTimerPieceView *>(getViewById(DLG_USB_AUDIO_RECORD_ID_TIMER));

    BindTitle(getViewById("boxTitle"));
}

void CDlgUSBAudioRecord::OnFocusChanged(bool bFocused)
{
    if (bFocused)
    {
        if (m_bReShow)
        {
            RefreshRecordInfo();
        }

        m_bReShow = false;
    }
    else
    {
        if (NULL != m_pTimerView)
        {
            m_pTimerView->StopTimer();
        }

        m_bReShow = true;
    }
}

bool CDlgUSBAudioRecord::IsRejectReturnToIdle()
{
    if (!g_UsbRecordControler.IsUsbConnect()
            || modRecord_IsRecordIdle(RECORD_AUDIO)
            || m_bSelfExit)
    {
        m_bSelfExit = false;

        return false;
    }

    return true;
}

bool CDlgUSBAudioRecord::ProcessKeyEvent(int nKeyCode, bool bPress)
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

void CDlgUSBAudioRecord::RefreshUI(bool bReLoadData/* = false*/)
{
    CSettingDialogBase::RefreshUI(bReLoadData);

    if (bReLoadData)
    {
        RefreshRecordInfo();
    }
}

void CDlgUSBAudioRecord::RefreshRecordInfo()
{
    if (modRecord_IsRecording())
    {
        RefreshRecordStatus(CUSBRecordControler::RECORD_STATUS_START);
    }
    else if (modRecord_IsRecordPause())
    {
        RefreshRecordStatus(CUSBRecordControler::RECORD_STATUS_PAUSE);
    }
    else
    {
        RefreshRecordStatus(CUSBRecordControler::RECORD_STATUS_NONE);
    }
}

void CDlgUSBAudioRecord::RefreshRecordStatus(int nStatus)
{
    if (NULL == m_pIconStatus || NULL == m_pTimerView)
    {
        return;
    }

    switch (nStatus)
    {
    case CUSBRecordControler::RECORD_STATUS_START:
        {
            m_pIconStatus->setPixmap(PIC_USB_RECORD_START);

            UINT unTime = g_UsbRecordControler.GetRecordingTime();

            m_pTimerView->SetStartDuration(unTime);
            m_pTimerView->StartTimer();
        }
        break;
    case CUSBRecordControler::RECORD_STATUS_PAUSE:
        {
            m_pIconStatus->setPixmap(PIC_USB_RECORD_PAUSE);

            if (m_pTimerView->IsTimerStart())
            {
                m_pTimerView->PauseTimer();
            }
            else
            {
                UINT unTime = g_UsbRecordControler.GetRecordingTime();

                m_pTimerView->SetStartDuration(unTime);
                m_pTimerView->UpdateTimerText();
            }
        }
        break;
    case CUSBRecordControler::RECORD_STATUS_RESMUE:
        {
            m_pIconStatus->setPixmap(PIC_USB_RECORD_START);

            if (!m_pTimerView->IsTimerStart())
            {
                m_pTimerView->ResumeTimer();
            }
        }
        break;
    case CUSBRecordControler::RECORD_STATUS_STOP:
        {
            m_pIconStatus->setPixmap(PIC_USB_RECORD_STOP);
            m_pTimerView->ResetTimer();

            yl::string strNote = TRID_RECORD_FILE_SAVED;
            if (modRecord_IsStorageFull(RECORD_AUDIO))
            {
                strNote = TRID_SPACE_FULL_SAVE_FILE;
            }

            PopCommonMessageBox(_LANG(TRID_NOTE), _LANG(strNote), SETTING_MESSAGEBOX_NOTE, "", "", SHORT_TIME);
        }
        break;
    default:
        {
            m_pIconStatus->setPixmap(PIC_USB_RECORD_STOP);
            m_pTimerView->SetText(xTimerPieceView::FormatMS(0));
        }
        break;
    }

    RefreshSoftkey();
}
#endif
