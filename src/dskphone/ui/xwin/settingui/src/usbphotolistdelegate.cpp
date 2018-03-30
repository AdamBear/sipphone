#include "usbphotolistdelegate.h"

#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
#include "translateiddefine.h"
#include "commonapi/stringhelperapi.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "common/common_data_define.h"

#include "languagehelper/modlangtranslate.h"
#include "messagebox/modmessagebox.h"
#include "pageiddefine.h"
#include "setuidatadefine.h"
#include "settinguimanager.h"
#include "uicallbackfun.h"
#include "pageiddefine.h"

#include "record/include/recordmessagedefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"

namespace
{
typedef YLList<RecordFileInfo> LIST_RECORD_FILE_INFO;
enum
{
    MSGBOX_ID_CLEAR_PHOTO_FILE = 151,
    MSGBOX_ID_DELETE_PHOTO_FILE,
    MSG_ID_READING_FILE
};
#define DELAY_LOAD_TIME         500
}

CUSBPhotoListDelegate::CUSBPhotoListDelegate()
{
    etl_RegisterMsgHandle(RECORD_STORAGE_LOAD_FILE_LIST, RECORD_STORAGE_LOAD_FILE_LIST,
                          &CUSBPhotoListDelegate::OnUSBPhotoMsg);
}

CUSBPhotoListDelegate::~CUSBPhotoListDelegate()
{
    etl_UnregisterMsgHandle(RECORD_STORAGE_LOAD_FILE_LIST, RECORD_STORAGE_LOAD_FILE_LIST,
                            &CUSBPhotoListDelegate::OnUSBPhotoMsg);
}

CSettingUIDelegateBase* CUSBPhotoListDelegate::CreateUSBPhotoListDelegate()
{
    CUSBPhotoListDelegate* pAudioListDelegate = new CUSBPhotoListDelegate;
    return pAudioListDelegate;
}

LRESULT CUSBPhotoListDelegate::OnUSBPhotoMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageIDUsbPhotoList != pDelegate->GetPageID()
            || RECORD_STORAGE_LOAD_FILE_LIST != msg.message || 1 != msg.lParam
            || RECORD_PICTURE != msg.wParam)
    {
        return FALSE;
    }

    CUSBPhotoListDelegate* pPhotoDelegate = static_cast<CUSBPhotoListDelegate*>(pDelegate);
    if (NULL == pPhotoDelegate)
    {
        return FALSE;
    }

    MessageBox_RemoveAllMessageBox(MessageBox_Destory);
    pPhotoDelegate->LoadPhotoData();
    g_SetUICallBackFun.RefreshUI(false);
    return TRUE;
}

CSettingUIPageDataBase* CUSBPhotoListDelegate::LoadPageData(bool bReLoad/* = false*/)
{
    CSettingUIPageDataBase* pPageData = CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData || !bReLoad)
    {
        return pPageData;
    }

    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_USB_READING_FILE),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                           MSG_ID_READING_FILE, NULL, this);

    m_timerDelayLoad.SingleShot(DELAY_LOAD_TIME,
                                TIMER_METHOD(this, CUSBPhotoListDelegate::OnDelayLoadList));


    return pPageData;
}

void CUSBPhotoListDelegate::AddPhotoFile(int nFileIndex, const RecordFileInfo& infoFile)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    yl::string strSize = modRecord_TransSizeToString(infoFile.lSize);
    yl::string strDisplayName = infoFile.strName;
    AddBaseItemData(m_pPageData, strDisplayName, strSize, "", false, false);
}

void CUSBPhotoListDelegate::LoadPhotoData()
{
    LIST_RECORD_FILE_INFO listFile;
    if (modRecord_GetResourceList(listFile, RECORD_PICTURE))
    {
        LIST_RECORD_FILE_INFO::iterator iter = listFile.begin();

        for (int i = 0; iter != listFile.end(); ++iter, ++i)
        {
            AddPhotoFile(i, *iter);
        }
    }
}

void CUSBPhotoListDelegate::OnDelayLoadList(xThreadTimer* timer, LPVOID pExtraData)
{
    if (!modRecord_IsLoadingFile(RECORD_PICTURE))
    {
        LoadPhotoData();
        MessageBox_RemoveAllMessageBox(MessageBox_Destory);
        g_SetUICallBackFun.RefreshUI(false);
    }
}

void CUSBPhotoListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    int nSize = vecSoftKey.size();

    if (nSize < MIN_SETTING_SOFTKEY_NUM || GetTotalDataCount() <= 0)
    {
        return;
    }

    vecSoftKey[2].m_iType = ST_DELETE;
    vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE);

    vecSoftKey[3].m_iType = ST_VIEW;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_PREVIEW);
}

bool CUSBPhotoListDelegate::OnAction(int eActionType, int nDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType && GetTotalDataCount() > 0)
    {
        PageOpenData stOpenPageData;
        stOpenPageData.strPageAction = kszPageIDUsbPhotoBrowser;
        stOpenPageData.pExtraData = (void*)(&nDataIndex);
        g_SetUICallBackFun.EnterPage(stOpenPageData);
        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, nDataIndex);
}

bool CUSBPhotoListDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (!bLongPress)
    {
        switch (iType)
        {
        case ST_DELETEALL:
            {
                g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_ALL_PHOTOS),
                                                       SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                                       MSGBOX_ID_CLEAR_PHOTO_FILE, NULL, this);
                return true;
            }
            break;
        case ST_DELETE:
            {
                g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_SELECTED_PHOTO),
                                                       SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                                       MSGBOX_ID_DELETE_PHOTO_FILE, NULL, this);
                return true;
            }
            break;
        case ST_VIEW:
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

void CUSBPhotoListDelegate::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    if (NULL == pMessageBox || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    int nIndex = m_pPageData->GetPageDataIndex();

    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_CLEAR_PHOTO_FILE:
        {
            if (modRecord_DeleteAll(RECORD_AUDIO))
            {
                SetFocusIndex(0);
            }

            g_SetUICallBackFun.RefreshUI(true, m_strPageID);
        }
        break;
    case MSGBOX_ID_DELETE_PHOTO_FILE:
        {
            int nIndex = GetFocusIndex();
            RecordFileInfo fileInfo;

            if (-1 != nIndex && modRecord_GetResourceByIndex(fileInfo, nIndex, RECORD_PICTURE))
            {
                yl::string strFilePath = fileInfo.strPath + fileInfo.strName;

                bool bResult = modRecord_DeleteResoucrce(RECORD_PICTURE, strFilePath.c_str());

                if (bResult && NULL != m_pPageData)
                {
                    int nFoucsIndex = GetFocusIndex();
                    if (nFoucsIndex == m_pPageData->GetTotalItemNumber() - 1 && nFoucsIndex > 0)
                    {
                        SetFocusIndex(nFoucsIndex - 1);
                    }
                }

                g_SetUICallBackFun.RefreshUI(true, m_strPageID);
            }
        }
        break;
    case MSG_ID_READING_FILE:
        {
            if (MessageBox_Cancel == pMessageBox->GetResult())
            {
                g_SetUICallBackFun.FallBackToPrevousPage();
            }
        }
        break;
    default:
        break;
    }
}
#endif
