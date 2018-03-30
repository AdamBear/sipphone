#ifndef __IMAGE_LRU_CACHE_H__
#define __IMAGE_LRU_CACHE_H__

#include "ylsmartptr.h"
#include "lrucache.hpp"
#include "imagecachedefine.h"
#include "imagecachebase.h"
#include "imageinfo.h"


#if IS_SUPPORT_LRU_CACHE
// 缓存算法类
typedef YLSmartPtr<CImageInfoBase> CImageInfoBasePtr;
typedef lru::Node<yl::string, CImageInfoBasePtr> LRU_NODE_IMAGE_INFO;
typedef YLHashMap<yl::string, LRU_NODE_IMAGE_INFO*> MAP_IMAGE_INFO_NODE;

typedef lru::Cache<yl::string, CImageInfoBasePtr> LRU_CACHE_IMAGE_INFO;

class CImageLRUCache : public CImageCacheBase
{
public:
    explicit CImageLRUCache(int nType = IMG_CACHE_TYPE_LRU);
    virtual ~CImageLRUCache();

    static CImageLRUCache* GetImageLRUCache(CImageCacheBase* pCache);

public:
    virtual void Init();
    virtual void UnInit();

    virtual void ClearImage(int nClassifyType);
#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    virtual void ClearScreenGraphic(int nClassifyType);
#endif

#if IF_IMAGE_MEMORY_DUMP
    virtual void Dump(int nPageSize, int& nTotalSize, int& nTotalMemSize);
#endif

    virtual bool RemoveImage(const yl::string& strFilePath, int nClassifyType);

    virtual bool GetImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic);
    virtual CImageInfoBase* FindImage(const yl::string& strFilePath);

protected:
    virtual void OnLoadImage(CImageInfoBase* pImageInfo);
    virtual bool IsCacheImage() const;

protected:
    LRU_CACHE_IMAGE_INFO m_ImageCache;
};
#endif

#endif
