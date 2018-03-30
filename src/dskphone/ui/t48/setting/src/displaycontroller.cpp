#include "displaycontroller.h"

#include "ETLLib/ETLLib.hpp"
#include "commonunits/modcommonunits.h"
#include "setting/include/common.h"

#include "basemethod.h"
#include "displaymodel.h"
#include "resource/modresource.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "etlmsghandler/modetlmsghandler.h"
#include "setting/include/displaymessagedefine.h"

namespace NS_SETTING
{
#define PREFIX_RESOURCE                         "Resource:"
#define RESOURCE_ZERO                               '0'
#define RESOURCE_CONFIG_ZERO_POS        9
#define RESOURCE_CONFIG_NOCHANGE_LENGTH     14


//////////////////////////////////////////////////////////////////////////
// Class CDisplayController.
CDisplayController * CDisplayController::m_pController = NULL;

CDisplayController * CDisplayController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CDisplayController();
        // Do initialization.
        if (m_pController != NULL)
        {
            m_pController->MyInit();
        }
    }
    return m_pController;
}

void CDisplayController::ReleaseInstance()
{
    // Release the singleton instance.
    if (m_pController != NULL)
    {
        // Do finalization.
        m_pController->MyFinal();

        delete m_pController;
        m_pController = NULL;
    }
}

bool CDisplayController::JudgeConfigFormat(const yl::string & strConfig, yl::string & strResult)
{
    int nSize = strConfig.size();
    if (nSize <= RESOURCE_CONFIG_ZERO_POS || nSize > RESOURCE_CONFIG_NOCHANGE_LENGTH)
    {
        return false;
    }

    if (strConfig.substr(0, RESOURCE_CONFIG_ZERO_POS) == PREFIX_RESOURCE
            && strConfig[RESOURCE_CONFIG_ZERO_POS] != RESOURCE_ZERO)
    {
        strResult = PREFIX_RESOURCE;
        strResult += RESOURCE_ZERO;
        strResult += strConfig.substr(RESOURCE_CONFIG_ZERO_POS);

        return true;
    }
    else
    {
        return false;
    }
}

LRESULT CDisplayController::ProcessMessage(msgObject & refMsg)
{
    if (refMsg.message != CONFIG_MSG_BCAST_CHANGED)
    {
        return FALSE;
    }

    if (ST_WALLPAPER == refMsg.wParam)
    {
        // 获取当前壁纸
        yl::string strCurWP = configParser_GetConfigString(kszCurrentWallPaper);

        yl::string strResult = "";
        bool bResult = JudgeConfigFormat(strCurWP, strResult);
        if (bResult)
        {
            strCurWP = strResult;
            configParser_SetConfigString(kszCurrentWallPaper, strResult.c_str());
        }

        commonUnits_Hide2Real(strCurWP, PHFILE_TYPE_WALLPAPER_FILE);

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
        // 获取当前Dsskey壁纸
        yl::string strCurDsskeyWP = configParser_GetConfigString(kszDsskeyWallPaper);

        if (strcasecmp(strCurDsskeyWP.c_str(), "auto") != 0)
        {
            yl::string strDsskeyResult = "";
            bool bDsskeyResult = JudgeConfigFormat(strCurDsskeyWP, strDsskeyResult);
            if (bDsskeyResult)
            {
                strCurDsskeyWP = strDsskeyResult;
                configParser_SetConfigString(kszDsskeyWallPaper, strDsskeyResult.c_str());
            }

            commonUnits_Hide2Real(strCurDsskeyWP, PHFILE_TYPE_WALLPAPER_FILE);
        }
#endif

        if (refMsg.lParam == 0)
        {
            // Post message.
            etl_NotifyAppEx(false, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                            0, 0, (int)strCurWP.length() + 1, strCurWP.c_str());
        }
        else if (refMsg.lParam == 1)
        {
            // 添加或删除图片的时候需要判断当前使用壁纸是否被删除
            YLList<YLSmartPtr<BaseResItem> > lsWallpaper;
            if (0 == res_getResItemsByResType(lsWallpaper, WALLPAPER_TYPE, true))
            {
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
                bool bCurWPExist = false;
                bool bCurDsskeyWPExist = false;
#endif
                YLList<YLSmartPtr<BaseResItem> >::iterator iter = lsWallpaper.begin();
                for (/* */; iter != lsWallpaper.end(); iter++)
                {
                    YLSmartPtr<BaseResItem> & pItem = *iter;
                    if (pItem.IsEmpty())
                    {
                        continue;
                    }

                    if (pItem->resPath == strCurWP)
                    {
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
                        bCurWPExist = true;
#else
                        break;
#endif
                    }
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
                    if (pItem->resPath == strCurDsskeyWP)
                    {
                        bCurDsskeyWPExist = true;
                    }
#endif
                }

                // 当前壁纸被删除则设置默认图片为当前壁纸
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
                if (!bCurWPExist)
#else
                if (iter == lsWallpaper.end())
#endif
                {
                    SetCurrentWallpaper(GetDefaultWallpaper());
                }

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
                if (!bCurDsskeyWPExist)
                {
                    SetDsskeyWallpaper("Auto");
                }
#endif
            }
        }
        else
        {
            // none
        }
    }

    return FALSE;
}

bool CDisplayController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    if (pData == NULL || strClickAction.empty())
    {
        return false;
    }

    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CDisplayController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CDisplayController::GetGeneralData(DisplayGeneralData * pData) const
{
    // Call model's method to get general data.
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    pData->m_nActiveLevel = m_pModel->GetActiveLevel();
    pData->m_nInactiveLevel = m_pModel->GetInactiveLevel();
    pData->m_nBackLightTime = m_pModel->GetBackLightTime();
    return true;
}

void CDisplayController::SetGeneralData(const DisplayGeneralData & refData)
{
    // Call model's method to set general data.
    if (m_pModel != NULL)
    {
        m_pModel->SetActiveLevel(refData.m_nActiveLevel);
        m_pModel->SetInactiveLevel(refData.m_nInactiveLevel);
        m_pModel->SetBackLightTime(refData.m_nBackLightTime);
    }

    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_BACK_LIGHT, 0);
}

bool CDisplayController::GetWallpaperListData(
    WallPaperListData * pWallpaperListData) const
{
    if (pWallpaperListData == NULL)
    {
        return false;
    }

    YLList<YLSmartPtr<BaseResItem> > lsWallpaper;
    if (0 == res_getResItemsByResType(lsWallpaper, WALLPAPER_TYPE, true))
    {
        WallPaperData paperData;
        YLList<YLSmartPtr<BaseResItem> >::iterator iter = lsWallpaper.begin();
        for (/* */; iter != lsWallpaper.end(); iter++)
        {
            YLSmartPtr<BaseResItem> & pItem = *iter;
            if (pItem.IsEmpty())
            {
                continue;
            }

            paperData.m_strPaperDisplayName = pItem->resItemDispLabel;
            paperData.m_strResName          = pItem->resItemName;
            paperData.m_strPaperPath        = pItem->resPath;
            pWallpaperListData->m_listPaperData.push_back(paperData);
        }
        return true;
    }

    return false;
}

const yl::string CDisplayController::GetDefaultWallpaper() const
{
    // 从墙纸管理器重获取默认墙纸.
    return res_getDefaultItem(WALLPAPER_TYPE)->resPath;
}

const yl::string CDisplayController::GetCurrentWallpaper() const
{
    // Call model's method to get current wallpaper.
    return m_pModel ? m_pModel->GetCurrentWallpaper() : "";
}

bool CDisplayController::SetCurrentWallpaper(const yl::string & strWallPaperPath)
{
    // Call model's method to set current wallpaper.
    return m_pModel ? m_pModel->SetCurrentWallpaper(strWallPaperPath) : false;
}

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
const yl::string CDisplayController::GetDsskeyWallpaper() const
{
    return m_pModel ? m_pModel->GetDsskeyWallpaper() : "";
}

bool CDisplayController::SetDsskeyWallpaper(const yl::string & strWallPaperPath)
{
    // Call model's method to set current wallpaper.
    return m_pModel ? m_pModel->SetDsskeyWallpaper(strWallPaperPath) : false;
}
#endif

RemoveResutType CDisplayController::RemoveWallpaper(
    const yl::string & strWallPaperPath)
{
    // Remove wallpaper.
    if (!CanRemoveWallpaper(strWallPaperPath))
    {
        return RRT_FILE_CANNOT_REMOVED;
    }

    yl::string strCurrentWallPaperPath = GetCurrentWallpaper();

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    yl::string strDsskeyWallPaperPath = GetDsskeyWallpaper();
#endif

    int iResult = res_remove(strWallPaperPath.c_str(), WALLPAPER_TYPE);

    switch (iResult)
    {
    case 0:
        {
            //成功删除当前壁纸之后设置成默认壁纸
            if (strCurrentWallPaperPath == strWallPaperPath && iResult == RRT_OK)
            {
                yl::string strDefaultWallpaperPath = GetDefaultWallpaper();
                SetCurrentWallpaper(strDefaultWallpaperPath);

                etl_NotifyAppEx(false, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                                0, 0, (int)strDefaultWallpaperPath.length() + 1, strDefaultWallpaperPath.c_str());
            }

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
            if (strDsskeyWallPaperPath == strWallPaperPath && iResult == RRT_OK)
            {
                yl::string strDefaultWallpaperPath = "Auto";
                SetDsskeyWallpaper(strDefaultWallpaperPath);

                etl_NotifyAppEx(false, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                                0, 0, (int)strDefaultWallpaperPath.length() + 1, strDefaultWallpaperPath.c_str());
            }
#endif

            return RRT_OK;
        }
    default:
        {
            return RRT_FAIL_TO_REMOVE;
        }
    }

    return RRT_OTHER_ERROR;
}

bool CDisplayController::CanRemoveWallpaper(const yl::string & strWallPaperPath)
{
    // 只有导入类型的图片可以删除
    yl::string strItemType = res_getResItemType(strWallPaperPath.c_str(), WALLPAPER_TYPE);
    return (strItemType == IMPORT_TYPE);
}

bool CDisplayController::GetThemeListData(ThemeListData * pThemeListData) const
{
    if (pThemeListData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // 从主题管理类中获取主题列表.
    return false;

//#warning "TODO:CommonUnits_GetThemeList"
//     return CommonUnits_GetThemeList(pThemeListData);

}

const yl::string CDisplayController::GetDefaultTheme() const
{
    // Get default theme.
    return "";

//#warning "TODO:CommonUnits_GetDefThemePath"
//     return CommonUnits_GetDefThemePath();

}

const yl::string CDisplayController::GetCurrentTheme() const
{
    // Get current theme from model.
    return m_pModel ? m_pModel->GetCurrentTheme() : "";
}

bool CDisplayController::SetCurrentTheme(const yl::string & strThemePath)
{
    // Set current theme to model.
    return m_pModel ? m_pModel->SetCurrentTheme(strThemePath) : false;
}

bool CDisplayController::GetScreenSaverData(ScreenSaverData * pScreenSaverData) const
{
    if (pScreenSaverData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Get screen saver data from model's method.
    return m_pModel->GetScreenData(pScreenSaverData);
}

void CDisplayController::SetScreenSaverData(const ScreenSaverData & refData)
{
    // Set screen saver data.
    if (m_pModel != NULL)
    {
        m_pModel->SetScreenSaverType(refData.m_eType);
        m_pModel->SetScreenSaverInterval(refData.m_nInterval);
        m_pModel->SetScreenTimeout(refData.m_nTimeout);
    }
}

bool CDisplayController::GetPhotoListData(PhotoListData * pPhotoListData) const
{
    if (pPhotoListData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // 从PhotoManager中获取各个图片.
    return false;

//#warning "TODO:CommonUnits_GetPhotoList"
//     return CommonUnits_GetPhotoList(pPhotoListData);
}

RemoveResutType CDisplayController::RemovePhoto(const yl::string & strPhotoPath)
{
    // 从PhotoManager中删除指定的图片.
    return RRT_OTHER_ERROR;

//#warning "TODO:CommonUnits_RemovePhoto"
//     return static_cast<RemoveResutType>(CommonUnits_RemovePhoto(strPhotoPath));

}

CDisplayController::CDisplayController()
    : m_pModel(NULL)
{
}

CDisplayController::~CDisplayController()
{
}

bool CDisplayController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CDisplayModel();
    }
    return m_pModel != NULL;
}

bool CDisplayController::MyFinal()
{
    // Release the model instance.
    if (m_pModel != NULL)
    {
        delete m_pModel;
        m_pModel = NULL;
    }
    return m_pModel == NULL;
}

bool CDisplayController::MyIsWallpaperRemovable(
    const yl::string & strWallpaperPath) const
{
    if (strWallpaperPath.empty() || strWallpaperPath == GetDefaultWallpaper())
    {
        return false;
    }
    return true;
}

////////////////////////////////////屏保///////////////////////////////////
bool CDisplayController::GetScreenSaverListData(
    WallPaperListData * pWallpaperListData) const
{
    if (pWallpaperListData == NULL)
    {
        return false;
    }

    YLList<YLSmartPtr<BaseResItem> > lsWallpaper;
    if (0 == res_getResItemsByResType(lsWallpaper, SCREENSAVER_TYPE, true))
    {
        WallPaperData paperData;
        YLList<YLSmartPtr<BaseResItem> >::iterator iter = lsWallpaper.begin();
        for (/* */; iter != lsWallpaper.end(); iter++)
        {
            YLSmartPtr<BaseResItem> & pItem = *iter;
            if (pItem.IsEmpty())
            {
                continue;
            }

            paperData.m_strPaperDisplayName = pItem->resItemDispLabel;
            paperData.m_strResName          = pItem->resItemName;
            paperData.m_strPaperPath        = pItem->resPath;
            pWallpaperListData->m_listPaperData.push_back(paperData);
        }
        return true;
    }

    return false;
}
}  // namespace NS_SETTING
