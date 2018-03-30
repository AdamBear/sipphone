#ifndef _EXPWALLPAPER_DELEGATE_H_
#define _EXPWALLPAPER_DELEGATE_H_

#include "settinguilogic/include/settingdefine.h"
#if IF_SUPPORT_EXPWALLPAPER
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "wallpaperbasedelegate.h"

class CExpWallpaperDelegate : public CWallpaperBaseDelegate
{
public:
    CExpWallpaperDelegate(BrowseImageType_E eBrowseType = BROWSE_TYPE_WALL_PAPER);
    virtual ~CExpWallpaperDelegate();
    static CSettingUIDelegateBase* CreatExpWallpaperDelegate();

public:
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool GetWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData);
    virtual yl::string GetWallpaperTitle();
    virtual yl::string GetCurWallPaper();
    virtual RemoveResutType_E RemoveWallpaper(const yl::string& strWallPaperPath);

};
#endif

#endif //_EXPWALLPAPER_DELEGATE_H_
