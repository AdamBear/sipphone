#ifndef __IMAGE_CACHE_H__
#define __IMAGE_CACHE_H__

#include "ylhashmap.h"
#include "imagecachedefine.h"
#include "imagecachebase.h"
#include "imageinfo.h"


// 图片资源映射表类型定义
typedef YLHashMap<yl::string, CImageInfoBase*>      MAP_IMAGE_INFO;
typedef MAP_IMAGE_INFO::iterator                        MAP_IMAGE_INFO_ITER;

class CImageCache : public CImageCacheBase
{
public:
    explicit CImageCache(int nType = IMG_CACHE_TYPE_MANUAL);
    virtual ~CImageCache();

    static CImageCache* GetImageCache(CImageCacheBase* pCache);

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

    void ClearImageInfo();

protected:
    MAP_IMAGE_INFO m_mapImage;
};

#endif
