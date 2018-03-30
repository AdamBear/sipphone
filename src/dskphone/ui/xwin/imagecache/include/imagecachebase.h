#ifndef __IMAGE_CACHE_BASE_H__
#define __IMAGE_CACHE_BASE_H__

#include "imagecachedefine.h"
#include "imageinfo.h"
#include "imageclassifier.h"


class CImageCacheBase
{
public:
    explicit CImageCacheBase(int nType = IMG_CACHE_TYPE_NONE);
    virtual ~CImageCacheBase();

    static bool IsPathValid(const yl::string& strPath);
    static bool IsFolder(const yl::string& strPath);

    static CImageInfoBase* AllocImageInfo(int nClassifyType);
    static void FreeImageInfo(CImageInfoBase* pImageInfo);

    static void OnLoadImageInfo(CImageInfoBase* pImageInfo);

    static void OnFreeImageInfo(CImageInfoBase* pImageInfo);

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

    bool LoadImage(const yl::string& strPath, int nClassifyType, bool bReload);
    bool LoadImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic, bool bReload);

    bool AddImage(const yl::string& strFilePath, int nClassifyType, const char* pFlow, int nFlowSize, bool bReload);

    int GetType() const
    {
        return m_nType;
    }

protected:
    virtual void OnLoadImage(CImageInfoBase* pImageInfo);
    virtual void OnCacheImage(CImageInfoBase* pImageInfo);
    virtual bool IsCacheImage() const;

    CImageClassifier* GetClassifier(int nClassifyType);

    bool IsImageBefitting(const yl::string& strFilePath);

    CImageInfoBase* LoadImageInfo(const yl::string& strFilePath, int nClassifyType, bool& bReload);

protected:
    int m_nType;
};

#endif
