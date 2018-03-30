#include "usbaudiolistdelegate.h"
#include "translateiddefine.h"
#include "commonapi/stringhelperapi.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/modmessagebox.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "pageiddefine.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "usbrecordcontroler.h"
#include "pageiddefine.h"
#include "common/common_data_define.h"


#ifdef IF_USB_SUPPORT
#ifndef _WIN32
#warning YLList<RecordFileInfo>
#endif
typedef YLList<RecordFileInfo> LIST_RECORD_FILE_INFO;
#define MSGBOX_ID_CLEAR_AUDIO_FILE 1
#define MSGBOX_ID_DELETE_AUDIO_FILE 2

CUSBAudioListDelegate::CUSBAudioListDelegate()
{
    g_UsbRecordControler.Init(kszPageIDUsbAudioList);
}

CUSBAudioListDelegate::~CUSBAudioListDelegate()
{
    g_UsbRecordControler.UnInit(kszPageIDUsbAudioList);
}

CSettingUIDelegateBase * CUSBAudioListDelegate::CreateUSBAudioListDelegate()
{
    CUSBAudioListDelegate * pAudioListDelegate = new CUSBAudioListDelegate;

    return pAudioListDelegate;
}

CSettingUIPageDataBase * CUSBAudioListDelegate::LoadPageData(bool bReLoad/* = false*/)
{
    CSettingUIPageDataBase * pPageData = CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData || !bReLoad)
    {
        return pPageData;
    }

    LIST_RECORD_FILE_INFO listFile;

    if (modRecord_GetResourceList(listFile, RECORD_AUDIO))
    {
        LIST_RECORD_FILE_INFO::iterator iter = listFile.begin();

        for (int i = 0; iter != listFile.end(); ++iter, ++i)
        {
            AddAudioRecordFile(i, *iter);
        }
    }

    return pPageData;
}

void CUSBAudioListDelegate::AddAudioRecordFile(int nFileIndex, const RecordFileInfo & infoFile)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    //卡住问题临时解决同T49交互
    //UINT uHour = infoFile.lTime / 3600;
    //UINT uMinute = (infoFile.lTime / 60) % 60;
    //UINT uSecond = infoFile.lTime % 60;

    yl::string strSize = modRecord_TransSizeToString(infoFile.lSize);
    //yl::string strValue = commonAPI_FormatString("%.2d:%.2d:%.2d(%s)", uHour, uMinute, uSecond, strSize.c_str());

    yl::string strDisplayName = g_UsbRecordControler.GetAudioDisplayName(infoFile.strName);
    AddBaseItemData(m_pPageData, strDisplayName, strSize, "", false, false);

}

void CUSBAudioListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    int nSize = vecSoftKey.size();

    if (nSize < 4 || GetTotalDataCount() <= 0)
    {
        return;
    }

    CSoftKeyBarData & skClear = vecSoftKey[1];
    CSoftKeyBarData & skDelete = vecSoftKey[2];
    CSoftKeyBarData & skPlay = vecSoftKey[3];

    skClear.m_iType = ST_CLEAR;
    skClear.m_strSoftkeyTitle = _LANG(TRID_CLEAR);

    skDelete.m_iType = ST_DELETE;
    skDelete.m_strSoftkeyTitle = _LANG(TRID_DELETE);

    skPlay.m_iType = ST_PLAY;
    skPlay.m_strSoftkeyTitle = _LANG(TRID_PLAY);
}

bool CUSBAudioListDelegate::OnAction(int eActionType, int nDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        if (modRecord_IsRecordIdle(RECORD_AUDIO))
        {
            g_UsbRecordControler.SetAudioIndex(nDataIndex);
            OnEnterPage(kszPageIDUsbAudioPlayer, nDataIndex);
        }
        else
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE),
                                                   _LANG(TRID_USB_RECORDING_NOT_ALLOW_PLAY_RECORD),
                                                   SETTING_MESSAGEBOX_NOTE, "", "", SHORT_TIME);
        }

        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, nDataIndex);
}

bool CUSBAudioListDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (!bLongPress)
    {
        switch (iType)
        {
        case ST_CLEAR:
            {
                g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_RECORD_DELETE_ALL),
                                                       SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                                       MSGBOX_ID_CLEAR_AUDIO_FILE, NULL, this);
                return true;
            }
            break;
        case ST_DELETE:
            {
                g_UsbRecordControler.SetAudioIndex(nDataIndex);

                g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_RECORD_DELETE),
                                                       SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                                       MSGBOX_ID_DELETE_AUDIO_FILE, NULL, this);
                return true;
            }
            break;
        case ST_PLAY:
            {
                OnAction(SET_ACTION_TYPE_CLICK, nDataIndex);
                return true;
            }
            break;
        default:
            break;
        }
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CUSBAudioListDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    int nIndex = m_pPageData->GetPageDataIndex();

    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_CLEAR_AUDIO_FILE:
        {
            if (modRecord_DeleteAll(RECORD_AUDIO))
            {
                SetFocusIndex(0);
            }

            g_SetUICallBackFun.RefreshUI(true, kszPageIDUsbAudioList);
        }
        break;
    case MSGBOX_ID_DELETE_AUDIO_FILE:
        {
            int nIndex = g_UsbRecordControler.GetAudioIndex();
            RecordFileInfo fileInfo;

            if (-1 != nIndex && modRecord_GetResourceByIndex(fileInfo, nIndex, RECORD_AUDIO))
            {
                yl::string strFilePath = fileInfo.strPath + fileInfo.strName;

                bool bResult = modRecord_DeleteResoucrce(RECORD_AUDIO, strFilePath.c_str());

                if (bResult && NULL != m_pPageData)
                {
                    int nFoucsIndex = GetFocusIndex();
                    if (nFoucsIndex == m_pPageData->GetTotalItemNumber() - 1 && nFoucsIndex > 0)
                    {
                        SetFocusIndex(nFoucsIndex - 1);
                    }
                }

                g_SetUICallBackFun.RefreshUI(true, kszPageIDUsbAudioList);
            }
        }
        break;
    default:
        break;
    }
}

#endif
