#ifndef SETTING_DISPLAYMODEL_H_
#define SETTING_DISPLAYMODEL_H_

#include <list>
#include <ylstring.h>

struct ScreenSaverData;

namespace NS_SETTING
{
// Class CDisplayModel
class CDisplayModel
{
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

    // Get current wallpaper.
    // Return current wallpaper full path.
    const yl::string GetCurrentWallpaper() const;
    // Set current wallpaper.
    // | strWallPaperPath | is the wallpaper full path.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentWallpaper(const yl::string & strWallPaperPath);

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    const yl::string GetDsskeyWallpaper() const;
    bool SetDsskeyWallpaper(const yl::string & strWallPaperPath);
#endif

    // Get current theme.
    // Return current theme full path.
    const yl::string GetCurrentTheme() const;
    // Set current theme.
    // | strThemePath | is the theme path to set.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentTheme(const yl::string & strThemePath);

    // Get screen saver data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetScreenData(ScreenSaverData * pData) const;
    // Set screen saver type.
    // | nType | is the screen type to set, it must be ScreenSaverType.
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

private:
    // Ctor and Dtor.
    CDisplayModel();
    ~CDisplayModel();

    // Initialize.
    // Return true if initialized successfully, otherwise return false.
    bool MyInit();
    // Finalize.
    // Return true if finalized successfully, otherwise return false.
    bool MyFinal();

    // Load wallpaper list.
    // Return true if loaded successfully, otherwise return false.
    bool MyLoadWallpaperList();

    // Load theme list.
    // Return true if loaded successfully, otherwise return false.
    bool MyLoadThemeList();

private:
    std::list<yl::string> m_listVideo;  // Video list.

    friend class CDisplayController;
};

}  // namespace NS_SETTING

#endif  // SETTING_DISPLAYMODEL_H_
