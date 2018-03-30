#ifndef _DIR_IMAGE_MANAGER_H_
#define _DIR_IMAGE_MANAGER_H_

#include <ETLLib/ETLLib.hpp>
#include "ylstring.h"
#include "ylhashmap.h"
#include "imagecachedefine.h"
#include "imagecachebase.h"
#include "imageclassifier.h"
#include "imageinfo.h"


// 缓存器MAP
typedef YLHashMap<int, CImageCacheBase*> MAP_IMAGE_CACHE;
// 分类器MAP
typedef YLHashMap<int, CImageClassifier*> MAP_IMAGE_CLASSIFIER;
// 图片路径缓存器MAP
typedef YLHashMap<yl::string, int> MAP_IMAGE_PATH_CACHE_TYPE;


// 图片资源管理类
class CImageCacheManager
{
    friend class CImageCacheBase;
    friend class CImageInfoBase;

public:
    // 构造函数
    CImageCacheManager();
    // 析构函数
    virtual ~CImageCacheManager();

    static CImageCacheManager& GetImageCacheManager();

public:
    void Init();
    void UnInit();

    bool RegisterCache(CImageCacheBase* pImageCache);
    bool UnRegisterCache(int nCacheType);

    bool RegisterClassifier(CImageClassifier* pImageClassifier);
    bool UnRegisterClassifier(int nClassifyType);

    void SetDefaultLoadCache(int nCacheType);
    void SetDefaultFindCache(int nCacheType);

public:
    // 加载图片资源
    bool LoadImage(const yl::string& strPath, int nClassifyType, bool bReload);
    bool LoadImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic, bool bReload);
    bool AddImage(const yl::string& strFilePath, int nClassifyType, const char* pFlow, int nFlowSize, bool bReload);

    // 移除图片资源
    bool RemoveImage(const yl::string& strFilePath, int nClassifyType);

    // 根据类型释放缓存的图片资源
    void ClearImage(int nClassifyType);

    // 通过文件路径查找结构指针对象
    bool GetImage(const yl::string& strFilePath, xPixmap& pic);
    bool GetImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic);

    IMAGE_SIZE_TYPE CheckImageSize(const yl::string& strFilePath);

    CImageInfoBase* FindImageInfo(const yl::string& strFilePath);

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    void ClearScreenGraphic(int nClassifyType);
    void ClearScreenGraphic(const yl::string& strFilePath);
#endif

#if IF_IMAGE_MEMORY_DUMP
    void Dump(int nPageSize);
#endif

protected:
    void ClearCache();
    void ClearClassifier();

    CImageCacheBase* GetCache(int nCacheType);
    CImageCacheBase* GetCacheByClassifyType(int nClassifyType);
    CImageCacheBase* GetLoadCacheByClassifyType(int nClassifyType);
    CImageCacheBase* GetFindCacheByClassifyType(int nClassifyType);
    CImageClassifier* GetClassifier(int nClassifierType);

    IMAGE_SIZE_TYPE CheckImageSize(const chSize& size);

    bool GetPathByFile(const yl::string& strFilePath, yl::string& strPath);

    void OnCacheImage(int nCacheType, const yl::string& strFilePath);

protected:
    // 默认加载的缓存器
    CImageCacheBase* m_pDefaultLoadCache;
    // 默认查找的缓存器
    CImageCacheBase* m_pDefaultFindCache;

    // 缓存器
    MAP_IMAGE_CACHE m_mapCache;
    // 分类器
    MAP_IMAGE_CLASSIFIER m_mapClassifier;
    // 根据路径查找缓存器
    MAP_IMAGE_PATH_CACHE_TYPE m_mapPathCacheType;
};

#define g_ImageCacheMgr CImageCacheManager::GetImageCacheManager()

#endif // _DIR_IMAGE_MANAGER_H_
