#ifndef SETTING_DISPLAYCONTROLLER_H_
#define SETTING_DISPLAYCONTROLLER_H_

#include <settinguilogic/include/settingdefine.h>
#if IF_SUPPORT_WALLPAPER
#include <ylstring.h>
#include "ETLLib.hpp"
#include "settinguilogic/include/common.h"
#include "resource/modresource.h"
#include "commondefine.h"

namespace NS_SETTING
{

typedef YLSmartPtr<BaseResItem>         SMART_RES_ITEM_PTR;
typedef YLList<SMART_RES_ITEM_PTR>      LISTSMART_RES_ITEM_PTR;

// Class CDisplayController.
class CDisplayController
{
public:
    // Get singleton instance.
    static CDisplayController* GetInstance();
    // Release singleton instance.
    static void ReleaseInstance();

    // 处理相关消息
    LRESULT ProcessMessage(msgObject& refMsg);

public:
    // Max back light time interval.
    static const int m_knMaxBackLightTimeInterval = 30 * 60;  // 30 minutes.
    // Max screen saver interval.
    static const int m_knMaxScreenSaverInterval = 30 * 60;  // 30 minutes.

    // Get active level.
    int GetActiveLevel() const;
    // Set active level.
    // | nLevel | is the active level to set.
    void SetActiveLevel(int nLevel);

    // Get inactive level.
    int GetInactiveLevel() const;
    // Set inactive level.
    // | nInactiveLevel | is the inactive level to set.
    void SetInactiveLevel(int nLevel);

    // Get back light time.
    // Return back light time (ms) if successfully, otherwise return -1.
    int GetBackLightTime() const;
    // Set back light time.
    // | nTime | is the time (ms) to set.
    void SetBackLightTime(int nBackLigthTime);

    // Get default wallpaper.
    // Return default wallpaper if successfully, otherwise return empty.
    const yl::string GetDefaultWallpaper() const;
    // Get current wallpaper.
    // Return current wallpaper full path.
    const yl::string GetCurrentWallpaper() const;
    // Set current wallpaper.
    // | strWallPaperPath | is the wallpaper full path.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentWallpaper(const yl::string& strWallPaperPath);

    const yl::string GetDefaultExpWallpaper() const;
    const yl::string GetCurrentExpWallpaper() const;
    bool SetCurrentExpWallpaper(const yl::string& strWallPaperPath);

    // Get current theme.
    // Return current theme full path.
    const yl::string GetCurrentTheme() const;
    // Set current theme.
    // | strThemePath | is the theme path to set.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentTheme(const yl::string& strThemePath);

    // Set screen saver type.
    // | nType | is the screen type to set, it must be ScreenSaverType_E.
    void SetScreenSaverType(int nType);
    // Set screen saver interval.
    // | nInterval | is the screen saver interval to set.
    void SetScreenSaverInterval(int nInterval);
    // Get screen saver timeout.
    // Return timeout value if timeout is set, return 0 indicates timeout
    // is off.
    int GetScreenTimeout() const;
    // Set screen saver timeout.
    // | nTimeout | is the timeout to set.
    void SetScreenTimeout(int nTimeout);

    // Get general data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetGeneralData(ST_DisplayGeneralData* pData) const;
    // Set back light time.
    // | refData | is the data to set.
    void SetGeneralData(const ST_DisplayGeneralData& refData);

    // Get wallpaper list data.
    // | pWallpaperListData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData) const;
    // Remove wallpaper.
    // | strResourceName | is the wallpaper resource name.
    // Return RRT_OK if removed successfully, otherwise return
    // error enum.
    RemoveResutType_E RemoveWallpaper(const yl::string& strWallPaperPath);
    // Can Remove Wallpaper.
    // | strResourceName | is the wallpaper resource name.
    // Return bool if can removed
    bool CanRemoveWallpaper(const yl::string& strWallPaperPath);
    //return 0 : success other :fail
    int RemoveMD5File(const yl::string& strFilePath);

    bool GetExpWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData) const;

    RemoveResutType_E RemoveExpWallpaper(const yl::string& strWallPaperPath);
    bool CanRemoveExpWallpaper(const yl::string& strWallPaperPath);

    bool GetScreenSaverListData(LIST_WALLPAPER_DATA& listWallpaperData) const;

    // Get theme list data.
    // | pThemeListData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetThemeListData(LIST_STRING* pThemeListData) const;
    // Get default theme.
    // Return default theme if successfully, otherwise return empty.
    const yl::string GetDefaultTheme() const;

    // Get screen saver data.
    // | pScreenSaverData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetScreenSaverData(ST_ScreenSaverData* pScreenSaverData) const;
    // Set screen saver data.
    // | refData | is the data to set.
    void SetScreenSaverData(const ST_ScreenSaverData& refData);

    // Get photo list data.
    // | pPhotoListData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetPhotoListData(LIST_STRING* pPhotoListData) const;
    // Remove photo.
    // | strPhotoPath | is the photo full path.
    // Return RRT_OK if removed successfully, otherwise return
    // error enum.
    RemoveResutType_E RemovePhoto(const yl::string& strPhotoPath);

    // Whether or not the wallpaper can be removed.
    // | strWallpaperPath | is the wallpaper full path.
    // Return true if wallpaper can be removed, otherwise return false.
    bool MyIsWallpaperRemovable(const yl::string& strWallpaperPath) const;

private:
    // Ctor and Dtor.
    CDisplayController();
    ~CDisplayController();

private:
    static CDisplayController* s_pController; // The singleton instance.
};

}  // namespace NS_SETTING

#define g_pDisplayController (NS_SETTING::CDisplayController::GetInstance())
#endif

#endif  // SETTING_DISPLAYCONTROLLER_H_
