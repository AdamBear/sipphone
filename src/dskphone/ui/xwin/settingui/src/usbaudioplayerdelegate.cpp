#include "usbaudioplayerdelegate.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "common/common_data_define.h"
#ifdef IF_USB_SUPPORT
#include "record/include/modrecord.h"
#endif
#include "settinguilogic/src/settinguibasepagedata.h"

#include "settinguidialogmanager.h"
#include "dlgusbaudioplayer.h"
#include "usbrecordcontroler.h"
#include "pageiddefine.h"


#ifdef IF_USB_SUPPORT

CUSBAudioPlayerDelegate::CUSBAudioPlayerDelegate()
{
    g_UsbRecordControler.Init(kszPageIDUsbAudioPlayer);
}

CUSBAudioPlayerDelegate::~CUSBAudioPlayerDelegate()
{
    g_UsbRecordControler.UnInit(kszPageIDUsbAudioPlayer);
}

CSettingUIDelegateBase * CUSBAudioPlayerDelegate::CreateUSBAudioPlayerDelegate()
{
    CUSBAudioPlayerDelegate * pPlayerDelegate = new CUSBAudioPlayerDelegate;

    return pPlayerDelegate;
}

CSettingUIPageDataBase * CUSBAudioPlayerDelegate::LoadPageData(bool bReLoad/* = false*/)
{
    CSettingUIPageDataBase * pPageData = CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL != pPageData && bReLoad)
    {
        yl::string strAudioName;

        g_UsbRecordControler.GetPlayAudioName(strAudioName);

        pPageData->SetPageTitle(strAudioName);
    }

    return pPageData;
}

void CUSBAudioPlayerDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (vecSoftKey.size() < 4 || !g_UsbRecordControler.IsPlayerRunning())
    {
        return;
    }

    if (modRecord_IsPlaying(RECORD_AUDIO))
    {
        vecSoftKey[3].m_iType = ST_PAUSE;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_PAUSE);
    }
    else
    {
        vecSoftKey[3].m_iType = ST_PLAY;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_PLAY);
    }
}

bool CUSBAudioPlayerDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_BACK == iType)
    {
        return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
    }

    CSettingDialogBase * pActiveDialog = g_pSettingUIDialogManager->GetTopActiveDialog();
    CDlgUSBAudioPlayer * pDlgAudioPlayer = CDlgUSBAudioPlayer::GetDlgUSBAudioPlayer(pActiveDialog);

    if (NULL == pDlgAudioPlayer)
    {
        return false;
    }

    switch (iType)
    {
    case ST_PAUSE:
        {
            if (bLongPress)
            {
                return false;
            }

            pDlgAudioPlayer->Pause();
            return true;
        }
        break;
    case ST_PLAY:
        {
            if (bLongPress)
            {
                return false;
            }

            pDlgAudioPlayer->Resume();
            return true;
        }
        break;
    case ST_PLAYLEFT:
        {
            pDlgAudioPlayer->PlaySeek(false, bLongPress);

            return true;
        }
        break;
    case ST_PLAYRIGHT:
        {
            pDlgAudioPlayer->PlaySeek(true, bLongPress);

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

#endif
