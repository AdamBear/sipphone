#ifndef SETTING_DISPLAYCONTROLLER_H_
#define SETTING_DISPLAYCONTROLLER_H_

#include <ylstring.h>
#include "ETLLib.hpp"
#include "setting/include/common.h"

#include "commondefine.h"

namespace NS_SETTING
{

class CDisplayModel;

// Class CDisplayController.
class CDisplayController
{
public:
    // Get singleton instance.
    static CDisplayController * GetInstance();
    // Release singleton instance.
    static void ReleaseInstance();

    // 处理相关消息
    LRESULT ProcessMessage(msgObject & refMsg);

    // Get page menu list by click action.
    // | pData | [out] is the data retrieved.
    // | strClickAction | is the click action, it must begin with OpenPage.
    // Such as "OpenPage("account_list.xml")".
    // Returns true if successfully, otherwise return false.
    bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                      const yl::string & strClickAction) const;

    // Get title by click action.
    // | strClickAction | is the click action.
    // Return the title if Retrieved successfully, otherwise return empty.
    const yl::string GetTitleByClickAction(
        const yl::string & strClickAction) const;

    // Get general data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetGeneralData(DisplayGeneralData * pData) const;
    // Set back light time.
    // | refData | is the data to set.
    void SetGeneralData(const DisplayGeneralData & refData);

    // Get wallpaper list data.
    // | pWallpaperListData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetWallpaperListData(WallPaperListData * pWallpaperListData) const;
    // Get default wallpaper.
    // Return default wallpaper if successfully, otherwise return empty.
    const yl::string GetDefaultWallpaper() const;
    // Get current wallpaper.
    // Return current wallpaper resource name.
    const yl::string GetCurrentWallpaper() const;
    // Set current wallpaper.
    // | strWallPaperPath | is the wallpaper full path.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentWallpaper(const yl::string & strWallPaperPath);
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    const yl::string GetDsskeyWallpaper() const;
    bool SetDsskeyWallpaper(const yl::string & strWallPaperPath);
#endif
    // Remove wallpaper.
    // | strResourceName | is the wallpaper resource name.
    // Return RRT_OK if removed successfully, otherwise return
    // error enum.
    RemoveResutType RemoveWallpaper(const yl::string & strWallPaperPath);
    // Can Remove Wallpaper.
    // | strResourceName | is the wallpaper resource name.
    // Return bool if can removed
    bool CanRemoveWallpaper(const yl::string & strWallPaperPath);
    bool GetScreenSaverListData(WallPaperListData * pWallpaperListData) const;

    // Get theme list data.
    // | pThemeListData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetThemeListData(ThemeListData * pThemeListData) const;
    // Get default theme.
    // Return default theme if successfully, otherwise return empty.
    const yl::string GetDefaultTheme() const;
    // Get current theme.
    // Return current theme full path.
    const yl::string GetCurrentTheme() const;
    // Set current theme.
    // | strThemePath | is the theme path to set.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentTheme(const yl::string & strThemePath);

    // Get screen saver data.
    // | pScreenSaverData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetScreenSaverData(ScreenSaverData * pScreenSaverData) const;
    // Set screen saver data.
    // | refData | is the data to set.
    void SetScreenSaverData(const ScreenSaverData & refData);

    // Get photo list data.
    // | pPhotoListData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetPhotoListData(PhotoListData * pPhotoListData) const;
    // Remove photo.
    // | strPhotoPath | is the photo full path.
    // Return RRT_OK if removed successfully, otherwise return
    // error enum.
    RemoveResutType RemovePhoto(const yl::string & strPhotoPath);

    // Whether or not the wallpaper can be removed.
    // | strWallpaperPath | is the wallpaper full path.
    // Return true if wallpaper can be removed, otherwise return false.
    bool MyIsWallpaperRemovable(const yl::string & strWallpaperPath) const;

private:
    // Ctor and Dtor.
    CDisplayController();
    ~CDisplayController();

    // Initialize.
    // Return true if initialized successfully, otherwise return false.
    bool MyInit();
    // Finalize.
    // Return true if finalized successfully, otherwise return false.
    bool MyFinal();

private:
    CDisplayModel * m_pModel; // The model instance.
    static CDisplayController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CDisplayController);

    bool JudgeConfigFormat(const yl::string & strConfig, yl::string & strResult);
};

}  // namespace NS_SETTING

#define g_pDisplayController (NS_SETTING::CDisplayController::GetInstance())

#endif  // SETTING_DISPLAYCONTROLLER_H_
