#ifndef __DATA_LOADER_H__
#define __DATA_LOADER_H__

#include "settinguicommondata.h"
#include "ylstl/ylhashmap.h"
#include "ylstl/ylstring.h"

class CSettingUIPageDataBase;
class CSettingUIItemDataBase;

typedef YLHashMap<yl::string, PageLoadFun> LoaderMap;
class CDataLoader
{
public:
    static CDataLoader * GetInstance();

    void Init();
    void RegisterLoadFun(const yl::string & strPageName, PageLoadFun pFun);
    bool LoadData(CSettingUIPageDataBase * pPageInfo, int iLineID = -1);
    bool LoadItemData(CSettingUIItemDataBase * pItemData, int iLineID = -1);
    //获取生成函数指针
    PageLoadFun GetLoadFun(const yl::string & strPageName);
    yl::string LoadDoubleParamconfig(const yl::string & strConfigId, const int iLineID);
    yl::string LoadSingleParamconfig(const yl::string & strConfigId, const int iLineID,
                                     const yl::string & strItemID);

private:
    CDataLoader();
    bool NormalLoad(CSettingUIPageDataBase * pPageData, int iLineID = -1);

private:
    static CDataLoader * m_pInstance;
    LoaderMap m_loaderMap;
};

#define g_DataLoader (CDataLoader::GetInstance())

#endif  //__DATA_LOADER_H__

