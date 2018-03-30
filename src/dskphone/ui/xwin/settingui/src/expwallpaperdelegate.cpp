#include "expwallpaperdelegate.h"

#if IF_SUPPORT_EXPWALLPAPER

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguimanager.h"
#include "uicallbackfun.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/displaycontroller.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"

CExpWallpaperDelegate::CExpWallpaperDelegate(BrowseImageType_E eBrowseType)
    : CWallpaperBaseDelegate(eBrowseType)
{
}

CExpWallpaperDelegate::~CExpWallpaperDelegate()
{
}

CSettingUIDelegateBase* CExpWallpaperDelegate::CreatExpWallpaperDelegate()
{
    CExpWallpaperDelegate* pDelegate = new CExpWallpaperDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}


bool CExpWallpaperDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == pPageData || !pPageData->IsEnable() || NULL == g_pDisplayController)
    {
        return false;
    }

    m_strCurImage = GetValue(WALLPAPER_SELECT);
    g_pDisplayController->SetCurrentWallpaper(m_strCurImage);
    return CSettingUIDelegateBase::SavePageData(pPageData, bBackToPrevious, bPopHint);
}

bool CExpWallpaperDelegate::GetWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData)
{
    if (NULL == g_pDisplayController)
    {
        return false;
    }
    return g_pDisplayController->GetExpWallpaperListData(listWallpaperData);
}

yl::string CExpWallpaperDelegate::GetWallpaperTitle()
{
    return TRID_EXP_WALLPAPER;
}

yl::string CExpWallpaperDelegate::GetCurWallPaper()
{
    if (NULL == g_pDisplayController)
    {
        return NS_SET::kszEmptyString;
    }
    return g_pDisplayController->GetCurrentExpWallpaper();
}

RemoveResutType_E CExpWallpaperDelegate::RemoveWallpaper(const yl::string& strWallPaperPath)
{
    if (NULL == g_pDisplayController)
    {
        return RRT_OTHER_ERROR;
    }
    return g_pDisplayController->RemoveExpWallpaper(strWallPaperPath);
}

#endif

