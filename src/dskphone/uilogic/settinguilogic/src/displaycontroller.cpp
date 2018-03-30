#include "displaycontroller.h"

#if IF_SUPPORT_WALLPAPER
#include "ETLLib/ETLLib.hpp"
#include "commonunits/modcommonunits.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "etlmsghandler/modetlmsghandler.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/include/settinguicommondata.h"
#include "settinguilogic/include/displaymessagedefine.h"

namespace NS_SETTING
{
#define AUTOP_MD5_FILE  ".MD5"
// Default wallpaper name.
static const char* const kszDefaultWallpaper = "wallpaper.jpg";
// Default theme.
static const char* const kszDefaultTheme = "Theme1";
//////////////////////////////////////////////////////////////////////////
// Class CDisplayController.
CDisplayController* CDisplayController::s_pController = NULL;

CDisplayController* CDisplayController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (NULL == s_pController)
    {
        s_pController = new CDisplayController();
    }
    return s_pController;
}

void CDisplayController::ReleaseInstance()
{
    // Release the singleton instance.
    if (NULL != s_pController)
    {
        delete s_pController;
        s_pController = NULL;
    }
}

LRESULT CDisplayController::ProcessMessage(msgObject& refMsg)
{
    if (CONFIG_MSG_BCAST_CHANGED != refMsg.message)
    {
        return FALSE;
    }

    if (ST_WALLPAPER == refMsg.wParam)
    {
        // 获取当前壁纸
        yl::string strCurWP = configParser_GetConfigString(kszCurrentWallPaper);
        commonUnits_Hide2Real(strCurWP, PHFILE_TYPE_WALLPAPER_FILE);

        yl::string strCurExpWP = configParser_GetConfigString(kszCurrentExpWallPaper);
        commonUnits_Hide2Real(strCurExpWP, PHFILE_TYPE_EXP_WALLPAPER_FILE);

        if (0 == refMsg.lParam)
        {
            // Post message.
            etl_NotifyAppEx(false, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                            0, 0, (int)strCurWP.length() + 1, strCurWP.c_str());
        }
        else if (1 == refMsg.lParam)
        {
            // 添加或删除图片的时候需要判断当前使用壁纸是否被删除
            LISTSMART_RES_ITEM_PTR lsWallpaper;
            if (0 == res_getResItemsByResType(lsWallpaper, WALLPAPER_TYPE))
            {
                LISTSMART_RES_ITEM_PTR::iterator iter = lsWallpaper.begin();
                for (/* */; iter != lsWallpaper.end(); iter++)
                {
                    SMART_RES_ITEM_PTR& pItem = *iter;
                    if (pItem.IsEmpty())
                    {
                        continue;
                    }

                    if (pItem->resPath == strCurWP)
                    {
                        break;
                    }
                }

                // 当前壁纸被删除则设置默认图片为当前壁纸
                if (iter == lsWallpaper.end())
                {
                    SetCurrentWallpaper(GetDefaultWallpaper());
                }
            }

            // 添加或删除图片的时候需要判断当前使用壁纸是否被删除
            LISTSMART_RES_ITEM_PTR lsExpWallpaper;
            if (0 == res_getResItemsByResType(lsExpWallpaper, EXP_WALLPAPER_TYPE))
            {
                LISTSMART_RES_ITEM_PTR::iterator iter = lsExpWallpaper.begin();
                for (/* */; iter != lsExpWallpaper.end(); iter++)
                {
                    SMART_RES_ITEM_PTR& pItem = *iter;
                    if (pItem.IsEmpty())
                    {
                        continue;
                    }

                    if (pItem->resPath == strCurExpWP)
                    {
                        break;
                    }
                }

                // 当前壁纸被删除则设置默认图片为当前壁纸
                if (iter == lsExpWallpaper.end())
                {
                    SetCurrentExpWallpaper(GetDefaultExpWallpaper());
                }
            }
        }
    }

    return FALSE;
}

bool CDisplayController::GetGeneralData(ST_DisplayGeneralData* pData) const
{
    // Call model's method to get general data.
    if (NULL == pData)
    {
        return false;
    }

    pData->m_nActiveLevel = GetActiveLevel();
    pData->m_nInactiveLevel = GetInactiveLevel();
    pData->m_nBackLightTime = GetBackLightTime();
    return true;
}

void CDisplayController::SetGeneralData(const ST_DisplayGeneralData& refData)
{
    // Call model's method to set general data.

    SetActiveLevel(refData.m_nActiveLevel);
    SetInactiveLevel(refData.m_nInactiveLevel);
    SetBackLightTime(refData.m_nBackLightTime);

    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_BACK_LIGHT, 0);
}

bool CDisplayController::GetWallpaperListData(
    LIST_WALLPAPER_DATA& listWallpaperData) const
{

    LISTSMART_RES_ITEM_PTR lsWallpaper;
    if (0 == res_getResItemsByResType(lsWallpaper, WALLPAPER_TYPE, true))
    {
        ST_WallPaperData paperData;
        LISTSMART_RES_ITEM_PTR::iterator iter = lsWallpaper.begin();
        for (/* */; iter != lsWallpaper.end(); iter++)
        {
            SMART_RES_ITEM_PTR& pItem = *iter;
            if (pItem.IsEmpty())
            {
                continue;
            }

            paperData.m_strPaperDisplayName = pItem->resItemDispLabel;
            paperData.m_strResName          = pItem->resItemName;
            paperData.m_strPaperPath        = pItem->resPath;
            listWallpaperData.push_back(paperData);
        }
        return true;
    }

    return false;
}

const yl::string CDisplayController::GetDefaultWallpaper() const
{
    // 从墙纸管理器重获取默认墙纸.
    SMART_RES_ITEM_PTR pItem = res_getDefaultItem(WALLPAPER_TYPE);
    if (pItem.IsEmpty())
    {
        return NS_SET::kszEmptyString;
    }
    return pItem->resPath;
}

int CDisplayController::RemoveMD5File(const yl::string& strFilePath)
{
    //http://10.2.1.199/Bug.php?BugID=99608 如果是AUTOP上传则要同时删除MD5文件
    yl::string strWallPaperMD5Path = strFilePath + AUTOP_MD5_FILE;
    int nMD5Result = -1;
    if (0 == access(strWallPaperMD5Path.c_str(), F_OK))
    {
        nMD5Result = remove(strWallPaperMD5Path.c_str());
    }
    else
    {
        nMD5Result = 0;
    }

    return nMD5Result;
}

RemoveResutType_E CDisplayController::RemoveWallpaper(
    const yl::string& strWallPaperPath)
{
    // Remove wallpaper.
    if (!CanRemoveWallpaper(strWallPaperPath))
    {
        return RRT_FILE_CANNOT_REMOVED;
    }

    yl::string strCurrentWallPaperPath = GetCurrentWallpaper();
    yl::string strCurrentExpWallPaperPath = GetCurrentExpWallpaper();

    int iResult = res_remove(strWallPaperPath.c_str(), WALLPAPER_TYPE);

    if (0 == RemoveMD5File(strWallPaperPath) && 0 == iResult)
    {
        iResult = 0;
    }
    else
    {
        iResult = -1;
    }

    switch (iResult)
    {
    case 0:
        {
            //成功删除当前壁纸之后设置成默认壁纸
            if (strCurrentWallPaperPath == strWallPaperPath && RRT_OK == iResult)
            {
                yl::string strDefaultWallpaperPath = GetDefaultWallpaper();
                SetCurrentWallpaper(strDefaultWallpaperPath);
            }

            //如果删除的图片也是当前的EXP壁纸则设置EXP壁纸为默认壁纸
            if (strCurrentExpWallPaperPath == strWallPaperPath && RRT_OK == iResult)
            {
                yl::string strDefaultExpWallpaperPath = GetDefaultExpWallpaper();
                SetCurrentExpWallpaper(strDefaultExpWallpaperPath);
            }

            return RRT_OK;
        }
        break;
    default:
        {
            return RRT_FAIL_TO_REMOVE;
        }
        break;
    }

    return RRT_OTHER_ERROR;
}

bool CDisplayController::CanRemoveWallpaper(const yl::string& strWallPaperPath)
{
    // 只有导入类型的图片可以删除
    yl::string strItemType = res_getResItemType(strWallPaperPath.c_str(), WALLPAPER_TYPE);
    return (IMPORT_TYPE == strItemType);
}

bool CDisplayController::GetThemeListData(LIST_STRING* pThemeListData) const
{
    if (NULL == pThemeListData)
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

void CDisplayController::SetScreenSaverData(const ST_ScreenSaverData& refData)
{
    // Set screen saver data.
    SetScreenSaverType(refData.m_eType);
    SetScreenSaverInterval(refData.m_nInterval);
    SetScreenTimeout(refData.m_nTimeout);
}

bool CDisplayController::GetPhotoListData(LIST_STRING* pPhotoListData) const
{
    if (NULL == pPhotoListData)
    {
        return false;
    }

    // 从PhotoManager中获取各个图片.
    return false;
//#warning "TODO:CommonUnits_GetPhotoList"
//     return CommonUnits_GetPhotoList(pPhotoListData);
}

RemoveResutType_E CDisplayController::RemovePhoto(const yl::string& strPhotoPath)
{
    // 从PhotoManager中删除指定的图片.
    return RRT_OTHER_ERROR;
//#warning "TODO:CommonUnits_RemovePhoto"
//     return static_cast<RemoveResutType_E>(CommonUnits_RemovePhoto(strPhotoPath));
}

CDisplayController::CDisplayController()
{
}

CDisplayController::~CDisplayController()
{
}

bool CDisplayController::MyIsWallpaperRemovable(
    const yl::string& strWallpaperPath) const
{
    if (strWallpaperPath.empty() || strWallpaperPath == GetDefaultWallpaper())
    {
        return false;
    }
    return true;
}
////////////////////////////////////EXP壁纸///////////////////////////////////
bool CDisplayController::GetExpWallpaperListData(
    LIST_WALLPAPER_DATA& listWallpaperData) const
{
    LISTSMART_RES_ITEM_PTR lsWallpaper;
    if (0 == res_getResItemsByResType(lsWallpaper, EXP_WALLPAPER_TYPE, true))
    {
        ST_WallPaperData paperData;
        LISTSMART_RES_ITEM_PTR::iterator iter = lsWallpaper.begin();
        for (/* */; iter != lsWallpaper.end(); iter++)
        {
            SMART_RES_ITEM_PTR& pItem = *iter;
            if (pItem.IsEmpty())
            {
                continue;
            }

            paperData.m_strPaperDisplayName = pItem->resItemDispLabel;
            paperData.m_strResName          = pItem->resItemName;
            paperData.m_strPaperPath        = pItem->resPath;
            listWallpaperData.push_back(paperData);
        }
        return true;
    }

    return false;
}

////////////////////////////////////屏保///////////////////////////////////
bool CDisplayController::GetScreenSaverListData(
    LIST_WALLPAPER_DATA& listWallpaperData) const
{

    LISTSMART_RES_ITEM_PTR lsWallpaper;
    if (0 == res_getResItemsByResType(lsWallpaper, SCREENSAVER_TYPE, true))
    {
        ST_WallPaperData paperData;
        LISTSMART_RES_ITEM_PTR::iterator iter = lsWallpaper.begin();
        for (/* */; iter != lsWallpaper.end(); iter++)
        {
            SMART_RES_ITEM_PTR& pItem = *iter;
            if (pItem.IsEmpty())
            {
                continue;
            }

            paperData.m_strPaperDisplayName = pItem->resItemDispLabel;
            paperData.m_strResName          = pItem->resItemName;
            paperData.m_strPaperPath        = pItem->resPath;
            listWallpaperData.push_back(paperData);
        }
        return true;
    }

    return false;
}

const yl::string CDisplayController::GetDefaultExpWallpaper() const
{
    // 从墙纸管理器重获取默认扩展台墙纸.
    SMART_RES_ITEM_PTR pItem = res_getDefaultItem(EXP_WALLPAPER_TYPE);
    if (pItem.IsEmpty())
    {
        return NS_SET::kszEmptyString;
    }
    return pItem->resPath;
}

RemoveResutType_E CDisplayController::RemoveExpWallpaper(
    const yl::string& strWallPaperPath)
{
    // Remove wallpaper.
    if (!CanRemoveExpWallpaper(strWallPaperPath))
    {
        return RRT_FILE_CANNOT_REMOVED;
    }

    yl::string strCurrentExpWallPaperPath = GetCurrentExpWallpaper();
    yl::string strCurrentWallPaperPath = GetCurrentWallpaper();

    int iResult = res_remove(strWallPaperPath.c_str(), EXP_WALLPAPER_TYPE);

    if (0 == RemoveMD5File(strWallPaperPath) && 0 == iResult)
    {
        iResult = 0;
    }
    else
    {
        iResult = -1;
    }

    switch (iResult)
    {
    case 0:
        {
            //如果删除的图片也是当前的EXP壁纸则设置EXP壁纸为默认壁纸
            if (strCurrentExpWallPaperPath == strWallPaperPath && RRT_OK == iResult)
            {
                yl::string strDefaultExpWallpaperPath = GetDefaultExpWallpaper();
                SetCurrentExpWallpaper(strDefaultExpWallpaperPath);
            }

            //成功删除当前壁纸之后设置成默认壁纸
            if (strCurrentWallPaperPath == strWallPaperPath && RRT_OK == iResult)
            {
                yl::string strDefaultWallpaperPath = GetDefaultWallpaper();
                SetCurrentWallpaper(strDefaultWallpaperPath);
            }

            return RRT_OK;
        }
    default:
        {
            return RRT_FAIL_TO_REMOVE;
        }
    }

    return RRT_OTHER_ERROR;
}

bool CDisplayController::CanRemoveExpWallpaper(const yl::string& strWallPaperPath)
{
    // 只有导入类型的图片可以删除
    yl::string strItemType = res_getResItemType(strWallPaperPath.c_str(), EXP_WALLPAPER_TYPE);
    return (IMPORT_TYPE == strItemType);
}

// Class CDisplayController.
int CDisplayController::GetActiveLevel() const
{
    // Read active level.
    return configParser_GetConfigInt(kszActiveLevel);
}

void CDisplayController::SetActiveLevel(int nLevel)
{
    // Set active level.
    configParser_SetConfigInt(kszActiveLevel, nLevel, CONFIG_LEVEL_PHONE);
}

int CDisplayController::GetInactiveLevel() const
{
    // Read inactive level.
    return configParser_GetConfigInt(kszInactiveLevel);
}

void CDisplayController::SetInactiveLevel(int nLevel)
{
    // Set inactive level.
    configParser_SetConfigInt(kszInactiveLevel, nLevel, CONFIG_LEVEL_PHONE);
}

int CDisplayController::GetBackLightTime() const
{
    return configParser_GetConfigInt(kszBackLightTime);
}

void CDisplayController::SetBackLightTime(int nBackLigthTime)
{
    if (nBackLigthTime < 0)
    {
        nBackLigthTime = 0;
    }
    else if (nBackLigthTime > m_knMaxBackLightTimeInterval)
    {
        nBackLigthTime = m_knMaxBackLightTimeInterval;
    }

    // Save value to config file.
    configParser_SetConfigInt(kszBackLightTime, nBackLigthTime, CONFIG_LEVEL_PHONE);

    // Post message.
    etl_NotifyApp(false, DISPLAY_BACKLIGHT_TIME_CHANGED,
                  0, nBackLigthTime);
}

const yl::string CDisplayController::GetCurrentWallpaper() const
{
    // Read current wallpaper.
    yl::string strPath = configParser_GetConfigString(kszCurrentWallPaper);
    return commonUnits_Hide2Real(strPath, PHFILE_TYPE_WALLPAPER_FILE);
}

bool CDisplayController::SetCurrentWallpaper(const yl::string& strWallPaperPath)
{
    // Set current wallpaper.
    yl::string strPath(strWallPaperPath);
    commonUnits_Real2Hide(strPath, PHFILE_TYPE_WALLPAPER_FILE);
    if (configParser_SetConfigString(kszCurrentWallPaper, strPath.c_str(), CONFIG_LEVEL_PHONE))
    {
        // Post message.
        return etl_NotifyAppEx(false, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                               0, 0, (int)strWallPaperPath.length() + 1, strWallPaperPath.c_str());
    }
    return false;
}

const yl::string CDisplayController::GetCurrentExpWallpaper() const
{
    // Read current wallpaper.
    yl::string strPath = configParser_GetConfigString(kszCurrentExpWallPaper);
    return commonUnits_Hide2Real(strPath, PHFILE_TYPE_EXP_WALLPAPER_FILE);
}

bool CDisplayController::SetCurrentExpWallpaper(const yl::string& strWallPaperPath)
{
    // Set current wallpaper.
    yl::string strPath(strWallPaperPath);
    commonUnits_Real2Hide(strPath, PHFILE_TYPE_EXP_WALLPAPER_FILE);
    if (configParser_SetConfigString(kszCurrentExpWallPaper, strPath.c_str(), CONFIG_LEVEL_PHONE))
    {
        // Post message.
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_EXP_WALLPAPER, 0);
        return true;
    }
    return false;
}

const yl::string CDisplayController::GetCurrentTheme() const
{
    return configParser_GetConfigString(kszCurrentTheme);
}

bool CDisplayController::SetCurrentTheme(const yl::string& strThemePath)
{
    // Set current theme.
    if (configParser_SetConfigString(kszCurrentTheme, strThemePath.c_str(), CONFIG_LEVEL_PHONE))
    {
        // Post message.
        return etl_NotifyAppEx(false, DISPLAY_CURRENT_THEME_CHANGED,
                               0, 0, (int)strThemePath.length() + 1, strThemePath.c_str());
    }
    return false;
}

bool CDisplayController::GetScreenSaverData(ST_ScreenSaverData* pData) const
{
    if (NULL == pData)
    {
        return false;
    }

    // Read screen type.
    pData->m_eType =
        static_cast<ScreenSaverType_E>(configParser_GetConfigInt(kszScreenType));
    // Read screen interval.
    pData->m_nInterval = configParser_GetConfigInt(kszScreenInterval);
    // Read screen timeout.
    pData->m_nTimeout = configParser_GetConfigInt(kszScreenTimeout);
    return true;
}

void CDisplayController::SetScreenSaverType(int nType)
{
    // Save screen type to config file.
    if (nType > SCREEN_SAVER_TYPE_NONE && nType < SCREEN_SAVER_MAX)
    {
        configParser_SetConfigInt(kszScreenType, nType, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false, DISPLAY_SCREEN_SAVER_TYPE_CHANGED,
                      0, nType);
    }
}

void CDisplayController::SetScreenSaverInterval(int nInterval)
{
    // Set screen interval.
    if (nInterval < 0)
    {
        nInterval = 0;
    }
    else if (nInterval > m_knMaxScreenSaverInterval)
    {
        nInterval = m_knMaxScreenSaverInterval;
    }

    // Save value to config file.
    configParser_SetConfigInt(kszScreenInterval, nInterval, CONFIG_LEVEL_PHONE);

    // Post message.
    etl_NotifyApp(false, DISPLAY_SCREEN_SAVER_INTERVAL_CHANGED,
                  0, nInterval);
}

int CDisplayController::GetScreenTimeout() const
{
    // Get timeout time
    return configParser_GetConfigInt(kszScreenTimeout);
}

void CDisplayController::SetScreenTimeout(int nTimeout)
{
    // Set screen saver timeout.
    configParser_SetConfigInt(kszScreenTimeout, nTimeout, CONFIG_LEVEL_PHONE);
}

}  // namespace NS_SETTING
#endif
