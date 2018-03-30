#include "displaymodel.h"

#include "ETLLib.hpp"
#include "configparser/modconfigparser.h"
// #include "CommonAPI/CommonAPI.h"
#include "commonunits/modcommonunits.h"
#include "setting/include/common.h"
#include <configiddefine.h>
#include "path_define.h"
#include "setting/include/displaymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace NS_SETTING
{

// Default wallpaper name.
static const char * const kszDefaultWallpaper = "wallpaper.jpg";
// Default theme.
static const char * const kszDefaultTheme = "Theme1";

//////////////////////////////////////////////////////////////////////////
// Class CDisplayModel.
int CDisplayModel::GetActiveLevel() const
{
    // Read active level.
    return configParser_GetConfigInt(kszActiveLevel);
}

void CDisplayModel::SetActiveLevel(int nLevel)
{
    // Set active level.
    configParser_SetConfigInt(kszActiveLevel, nLevel, CONFIG_LEVEL_PHONE);
}

int CDisplayModel::GetInactiveLevel() const
{
    // Read inactive level.
    return configParser_GetConfigInt(kszInactiveLevel);
}

void CDisplayModel::SetInactiveLevel(int nLevel)
{
    // Set inactive level.
    configParser_SetConfigInt(kszInactiveLevel, nLevel, CONFIG_LEVEL_PHONE);
}

int CDisplayModel::GetBackLightTime() const
{
    return configParser_GetConfigInt(kszBackLightTime);
}

void CDisplayModel::SetBackLightTime(int nBackLigthTime)
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

const yl::string CDisplayModel::GetCurrentWallpaper() const
{
    // Read current wallpaper.
    yl::string strPath = configParser_GetConfigString(kszCurrentWallPaper);
    return commonUnits_Hide2Real(strPath, PHFILE_TYPE_WALLPAPER_FILE);
}

bool CDisplayModel::SetCurrentWallpaper(const yl::string & strWallPaperPath)
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

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
const yl::string CDisplayModel::GetDsskeyWallpaper() const
{
    yl::string strPath = configParser_GetConfigString(kszDsskeyWallPaper);
    if (strcasecmp(strPath.c_str(), "auto") == 0)
    {
        return strPath;
    }
    return commonUnits_Hide2Real(strPath, PHFILE_TYPE_WALLPAPER_FILE);
}

bool CDisplayModel::SetDsskeyWallpaper(const yl::string & strWallPaperPath)
{
    yl::string strPath(strWallPaperPath);
    if (strcasecmp(strPath.c_str(), "auto") != 0)
    {
        commonUnits_Real2Hide(strPath, PHFILE_TYPE_WALLPAPER_FILE);
    }

    if (configParser_SetConfigString(kszDsskeyWallPaper, strPath.c_str(), CONFIG_LEVEL_PHONE))
    {
        // Post message.
        return etl_NotifyAppEx(false, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                               0, 0, (int)strWallPaperPath.length() + 1, strWallPaperPath.c_str());
    }
    return false;
}
#endif

const yl::string CDisplayModel::GetCurrentTheme() const
{
    return configParser_GetConfigString(kszCurrentTheme);
}

bool CDisplayModel::SetCurrentTheme(const yl::string & strThemePath)
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

bool CDisplayModel::GetScreenData(ScreenSaverData * pData) const
{
    if (pData == NULL)
    {
        return false;
    }

    // Read screen type.
    pData->m_eType =
        static_cast<ScreenSaverType>(configParser_GetConfigInt(kszScreenType));
    // Read screen interval.
    pData->m_nInterval = configParser_GetConfigInt(kszScreenInterval);
    // Read screen timeout.
    pData->m_nTimeout = configParser_GetConfigInt(kszScreenTimeout);
    return true;
}

void CDisplayModel::SetScreenSaverType(int nType)
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

void CDisplayModel::SetScreenSaverInterval(int nInterval)
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

int CDisplayModel::GetScreenTimeout() const
{
    // Get timeout time
    return configParser_GetConfigInt(kszScreenTimeout);
}

void CDisplayModel::SetScreenTimeout(int nTimeout)
{
    // Set screen saver timeout.
    configParser_SetConfigInt(kszScreenTimeout, nTimeout, CONFIG_LEVEL_PHONE);
}

CDisplayModel::CDisplayModel()
{
    MyInit();
}

CDisplayModel::~CDisplayModel()
{
    MyFinal();
}

bool CDisplayModel::MyInit()
{
    return true;
}

bool CDisplayModel::MyFinal()
{
    return true;
}

}  // namespace NS_SETTING
