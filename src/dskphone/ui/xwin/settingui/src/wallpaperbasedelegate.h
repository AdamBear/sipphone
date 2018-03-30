#ifndef _WALLPAPERBASE_DELEGATE_H_
#define _WALLPAPERBASE_DELEGATE_H_

#include "settinguilogic/include/settingdefine.h"
#if IF_SUPPORT_WALLPAPER
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"

class CSettingUIComboBoxItemData;

class CWallpaperBaseDelegate : public CSettinguiDataDelegateBase
{
public:
    CWallpaperBaseDelegate(BrowseImageType_E eBrowseType = BROWSE_TYPE_NONE);
    virtual ~CWallpaperBaseDelegate();
    static CSettingUIDelegateBase* CreatWallpaperBaseDelegate();
    static LRESULT OnWallpaperMessage(msgObject& msg);

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
    // 打开界面附加操作
    virtual void InitExtraData(void* pData);
    virtual void OnComboBoxValueChange(int nIndex);

    // 自定义虚接口
    virtual bool GetWallpaperListData(LIST_WALLPAPER_DATA& listWallpaperData);
    virtual yl::string GetCurWallPaper();
    virtual RemoveResutType_E RemoveWallpaper(const yl::string& strWallPaperPath);
    virtual yl::string GetWallpaperTitle();

    bool GetWallpaperData(ST_DisPlayWallPaperData& stWallData);
protected:
    bool IsEqualWallpaper(const yl::string& strWallpaper);
    bool IsEqualExpWallpaper(const yl::string& strEXPWallpaper);
    void DeleteWallpapper();
    void AddComboboxItemValue(CSettingUIComboBoxItemData* pComboBoxItem,
                              const yl::string& strDisplayName, const yl::string& strValue);

protected:
    BrowseImageType_E   m_eBrowseType;
    yl::string          m_strCurImage;   //界面初始化要显示的图片全路径名称
};
#endif

#endif //_WALLPAPERBASE_DELEGATE_H_
