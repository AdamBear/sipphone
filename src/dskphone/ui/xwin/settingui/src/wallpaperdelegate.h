#ifndef _WALLPAPER_DELEGATE_H_
#define _WALLPAPER_DELEGATE_H_

#include "settinguilogic/include/settingdefine.h"
#if IF_SUPPORT_WALLPAPER
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "wallpaperbasedelegate.h"


class CWallpaperDelegate : public CWallpaperBaseDelegate
{
public:
    CWallpaperDelegate(BrowseImageType_E eBrowseType = BROWSE_TYPE_WALL_PAPER);
    virtual ~CWallpaperDelegate();
    static CSettingUIDelegateBase* CreatWallpaperDelegate();

public:
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool GetWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData);
    virtual yl::string GetWallpaperTitle();
    virtual yl::string GetCurWallPaper();
    virtual RemoveResutType_E RemoveWallpaper(const yl::string& strWallPaperPath);

};
#endif

#endif //_WALLPAPER_DELEGATE_H_
