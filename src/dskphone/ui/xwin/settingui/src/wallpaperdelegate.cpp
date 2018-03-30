#include "wallpaperdelegate.h"

#if IF_SUPPORT_WALLPAPER

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguimanager.h"
#include "uicallbackfun.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/displaycontroller.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"


CWallpaperDelegate::CWallpaperDelegate(BrowseImageType_E eBrowseType)
    : CWallpaperBaseDelegate(eBrowseType)
{
}

CWallpaperDelegate::~CWallpaperDelegate()
{
}

CSettingUIDelegateBase* CWallpaperDelegate::CreatWallpaperDelegate()
{
    CWallpaperDelegate* pDelegate = new CWallpaperDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}


bool CWallpaperDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == pPageData || !pPageData->IsEnable() || NULL == g_pDisplayController)
    {
        return false;
    }

    m_strCurImage = GetValue(WALLPAPER_SELECT);
    g_pDisplayController->SetCurrentWallpaper(m_strCurImage);
    return CSettingUIDelegateBase::SavePageData(pPageData, bBackToPrevious, bPopHint);
}

bool CWallpaperDelegate::GetWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData)
{
    if (NULL == g_pDisplayController)
    {
        return false;
    }
    return g_pDisplayController->GetWallpaperListData(listWallpaperData);
}

yl::string CWallpaperDelegate::GetWallpaperTitle()
{
    return TRID_WALLPAPER;
}

yl::string CWallpaperDelegate::GetCurWallPaper()
{
    if (NULL == g_pDisplayController)
    {
        return NS_SET::kszEmptyString;
    }
    return g_pDisplayController->GetCurrentWallpaper();
}

RemoveResutType_E CWallpaperDelegate::RemoveWallpaper(const yl::string& strWallPaperPath)
{
    if (NULL == g_pDisplayController)
    {
        return RRT_OTHER_ERROR;
    }
    return g_pDisplayController->RemoveWallpaper(strWallPaperPath);
}

#endif

