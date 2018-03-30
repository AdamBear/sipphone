#include "wallpaperbasedelegate.h"

#if IF_SUPPORT_WALLPAPER

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguimanager.h"
#include "uicallbackfun.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/modbaseui.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"
#include "imagecache/include/modimagecache.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/displaycontroller.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"


namespace
{
enum
{
    MSG_ID_DELETE_WAPPER = 1000,
    MSG_ID_AFTER_DELETE_NOTE,
};
#define WALLPAPER_NOTE_MSGBOX_SHOW_TIME     1500
}

CWallpaperBaseDelegate::CWallpaperBaseDelegate(BrowseImageType_E eBrowseType)
    : m_eBrowseType(eBrowseType)
    , m_strCurImage("")
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CWallpaperBaseDelegate::OnWallpaperMessage);
}

CWallpaperBaseDelegate::~CWallpaperBaseDelegate()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CWallpaperBaseDelegate::OnWallpaperMessage);
}

CSettingUIDelegateBase* CWallpaperBaseDelegate::CreatWallpaperBaseDelegate()
{
    CWallpaperBaseDelegate* pDelegate = new CWallpaperBaseDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CWallpaperBaseDelegate::OnWallpaperMessage(msgObject& msg)
{
    if (NULL == g_pSettingUIManager || CONFIG_MSG_BCAST_CHANGED != msg.message
            || (ST_WALLPAPER != msg.wParam && ST_EXP_WALLPAPER != msg.wParam))
    {
        return FALSE;
    }

    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate)
    {
        return FALSE;
    }

    yl::string strPageID = pDelegate->GetPageID();
    if (kszPageIDWallPaper != strPageID &&
            kszPageIDExpWallPaper != strPageID)
    {
        return FALSE;
    }

    g_SetUICallBackFun.RefreshUI(true);
    return TRUE;
}

void CWallpaperBaseDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (NULL == g_pDisplayController)
    {
        return;
    }

    yl::string strPaperPath = GetValue(WALLPAPER_SELECT);
    if (g_pDisplayController->CanRemoveWallpaper(strPaperPath))
    {
        vecSoftKey[1].m_iType = ST_DELETE;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_DELETE);
    }
    else
    {
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
    }
}

bool CWallpaperBaseDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_DELETE == iType)
    {
        DeleteWallpapper();
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CWallpaperBaseDelegate::InitExtraData(void* pData)
{
    if (NULL != pData)
    {
        ST_DisPlayWallPaperData stWallpaperData = *((ST_DisPlayWallPaperData*)pData);
        m_eBrowseType = stWallpaperData.eBrowseType;
        m_strCurImage = stWallpaperData.strCurPaper;
    }
}

void CWallpaperBaseDelegate::OnComboBoxValueChange(int nIndex)
{
    m_strCurImage = GetValue(WALLPAPER_SELECT);
    g_SetUICallBackFun.RefreshUI(false, GetPageID());
}

bool CWallpaperBaseDelegate::GetWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData)
{
    return false;
}

yl::string CWallpaperBaseDelegate::GetCurWallPaper()
{
    return NS_SET::kszEmptyString;
}

RemoveResutType_E CWallpaperBaseDelegate::RemoveWallpaper(const yl::string& strWallPaperPath)
{
    return RRT_FILE_CANNOT_REMOVED;
}

yl::string CWallpaperBaseDelegate::GetWallpaperTitle()
{
    return NS_SET::kszEmptyString;
}

bool CWallpaperBaseDelegate::GetWallpaperData(ST_DisPlayWallPaperData& stWallData)
{
    stWallData.eBrowseType = m_eBrowseType;
    stWallData.strCurPaper = m_strCurImage;
    return true;
}

bool CWallpaperBaseDelegate::IsEqualWallpaper(const yl::string& strWallpaper)
{
    if (NULL == g_pDisplayController)
    {
        return false;
    }

    return strWallpaper == g_pDisplayController->GetCurrentWallpaper();
}

bool CWallpaperBaseDelegate::IsEqualExpWallpaper(const yl::string& strEXPWallpaper)
{
    if (NULL == g_pDisplayController)
    {
        return false;
    }
    return strEXPWallpaper == g_pDisplayController->GetCurrentExpWallpaper();
}

void CWallpaperBaseDelegate::DeleteWallpapper()
{
    yl::string strCurPic = GetValue(WALLPAPER_SELECT);

    if (IsEqualWallpaper(strCurPic) && IsEqualExpWallpaper(strCurPic))
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_MAKE_EXP_AND_WALLPAPER_DEFAULT),
                                               SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                               MSG_ID_DELETE_WAPPER);
    }
    else if (IsEqualWallpaper(strCurPic))
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_MAKE_WALLPAPER_DEFAULT),
                                               SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                               MSG_ID_DELETE_WAPPER);

    }
    else if (IsEqualExpWallpaper(strCurPic))
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_MAKE_EXP_WALLPAPER_DEFAULT),
                                               SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                               MSG_ID_DELETE_WAPPER);
    }
    else
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_SELECTED_PHOTO),
                                               SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                               MSG_ID_DELETE_WAPPER);
    }
}

void CWallpaperBaseDelegate::AddComboboxItemValue(CSettingUIComboBoxItemData* pComboBoxItem, const yl::string& strDisplayName, const yl::string& strValue)
{
    if (NULL == pComboBoxItem)
    {
        return;
    }

    CSettingUIComboBoxOption* pOption = new CSettingUIComboBoxOption;
    if (NULL == pOption)
    {
        return;
    }

    pOption->m_strDisplayName = strDisplayName;
    pOption->m_strItemVale = strValue;
    if (!pComboBoxItem->AddOptionItem(pOption))
    {
        delete pOption;
    }
}

bool CWallpaperBaseDelegate::ProcessMsgCallBack(int nResult, int nID,
        void* pExtraData /*= NULL*/)
{
    if (NULL == g_pDisplayController)
    {
        return false;
    }

    switch (nID)
    {
    case MSG_ID_DELETE_WAPPER:
        {
            if (MessageBox_OK == nResult)
            {
                RemoveResutType_E eResult = g_pDisplayController->RemoveWallpaper(m_strCurImage);
                SETTINGUI_INFO("CWallpaperBaseDelegate : Delete result code = [%d]", eResult);
                if (RRT_OK != eResult)
                {
                    SETTINGUI_WARN("CWallpaperBaseDelegate : remove wallpapper error");
                }
                else
                {
                    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_DELETE_SUCCESSED),
                                                           SETTING_MESSAGEBOX_NOTE, "", "", WALLPAPER_NOTE_MSGBOX_SHOW_TIME,
                                                           MSG_ID_AFTER_DELETE_NOTE);
                }
                return true;
            }
        }
        break;
    case MSG_ID_AFTER_DELETE_NOTE:
        {
            g_SetUICallBackFun.RefreshUI(true);
        }
        break;
    default:
        break;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase* CWallpaperBaseDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        pPageData->SetPageTitle(_LANG(GetWallpaperTitle()));

        LIST_WALLPAPER_DATA ListWallData;
        bool bIsSuccess = GetWallpaperListData(ListWallData);
        m_strCurImage = GetCurWallPaper();
        if (m_strCurImage.empty())
        {
            SETTINGUI_WARN("current wallpaper is empty!");
        }

        CSettingUIItemDataBasePtr pItemInfo = pPageData->GetItemByItemId(WALLPAPER_SELECT);
        CSettingUIComboBoxItemDataPtr pComboboxItem = CSettingUIComboBoxItemData::GetComboBoxDataItem(pItemInfo);
        if (NULL != pComboboxItem && bIsSuccess)
        {
            pComboboxItem->SetItemID(WALLPAPER_SELECT);
            pComboboxItem->SetDisplayName(_LANG(TRID_WALLPAPER));
            pComboboxItem->SetShowIndex(true);

            LIST_WALLPAPER_DATA::iterator iter = ListWallData.begin();
            for (; iter != ListWallData.end(); ++iter)
            {
                const ST_WallPaperData& refData = *iter;
                AddComboboxItemValue(pComboboxItem, refData.m_strPaperDisplayName, refData.m_strPaperPath);
            }

            pComboboxItem->SetValue(m_strCurImage);
        }
    }

    return pPageData;
}

#endif

