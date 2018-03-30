#include "imagecache.h"
#include "dsklog/log.h"


CImageCache::CImageCache(int nType/* = IMG_CACHE_TYPE_MANUAL*/)
    : CImageCacheBase(nType)
{

}

CImageCache::~CImageCache()
{
    ClearImageInfo();
}

CImageCache* CImageCache::GetImageCache(CImageCacheBase* pCache)
{
    if (NULL == pCache || IMG_CACHE_TYPE_MANUAL != (IMG_CACHE_TYPE_MANUAL & pCache->GetType()))
    {
        return NULL;
    }

    return static_cast<CImageCache*>(pCache);
}

void CImageCache::ClearImageInfo()
{
    MAP_IMAGE_INFO::iterator iter = m_mapImage.begin();

    for (; iter != m_mapImage.end(); ++iter)
    {
        FreeImageInfo(iter->second);
    }

    m_mapImage.clear();
}

void CImageCache::Init()
{
    ClearImageInfo();
}

void CImageCache::UnInit()
{
    ClearImageInfo();
}

void CImageCache::ClearImage(int nClassifyType)
{
    MAP_IMAGE_INFO::iterator iter = m_mapImage.begin();

    while (iter != m_mapImage.end())
    {
        CImageInfoBase* pImageInfo = iter->second;

        if (NULL != pImageInfo && pImageInfo->GetClassifyType() == nClassifyType)
        {
            iter = m_mapImage.erase(iter);

            FreeImageInfo(pImageInfo);

            continue;
        }

        ++iter;
    }
}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
void CImageCache::ClearScreenGraphic(int nClassifyType)
{
    MAP_IMAGE_INFO::iterator iter = m_mapImage.begin();

    for (; iter != m_mapImage.end(); ++iter)
    {
        CImageInfoBase* pImageInfo = iter->second;

        if (NULL != pImageInfo && pImageInfo->GetClassifyType() == nClassifyType)
        {
            pImageInfo->DetachScreenGraphic();
        }
    }
}
#endif

#if IF_IMAGE_MEMORY_DUMP
void CImageCache::Dump(int nPageSize, int& nTotalSize, int& nTotalMemSize)
{
    UIKERNEL_INFO("Begin dump memory in cache, cachetype:IMG_CACHE_TYPE_MANUAL");

    MAP_IMAGE_INFO::iterator iter = m_mapImage.begin();
    int nCacheTotalSize = 0;
    int nCacheTotalMemSize = 0;

    for (; iter != m_mapImage.end(); ++iter)
    {
        CImageInfoBase* pInfo = iter->second;

        if (NULL != pInfo)
        {
            int nSize = 0;
            int nMemSize = 0;

            pInfo->Dump(nPageSize, nSize, nMemSize);

            nCacheTotalSize += nSize;
            nCacheTotalMemSize += nMemSize;
        }
    }

    UIKERNEL_INFO("End dump memory in cache, Total Image Size:%d, Total Memory Size:%d", nCacheTotalSize, nCacheTotalMemSize);

    nTotalSize += nCacheTotalSize;
    nTotalMemSize += nCacheTotalMemSize;
}
#endif

bool CImageCache::RemoveImage(const yl::string& strFilePath, int nClassifyType)
{
    MAP_IMAGE_INFO::iterator iter = m_mapImage.find(strFilePath);

    if (iter == m_mapImage.end())
    {
        return false;
    }

    CImageInfoBase* pImageInfo = iter->second;

    if (NULL == pImageInfo)
    {
        m_mapImage.erase(iter);

        return false;
    }

    if (IMG_CLASSIFY_NONE == nClassifyType || pImageInfo->GetClassifyType() == nClassifyType)
    {
        m_mapImage.erase(iter);

        FreeImageInfo(pImageInfo);

        return true;
    }

    return false;
}

bool CImageCache::GetImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic)
{
    CImageInfoBase* pImageInfo = FindImage(strFilePath);

    if (NULL != pImageInfo)
    {
        pic = pImageInfo->GetPixmap();

        return true;
    }

    return false;
}

CImageInfoBase* CImageCache::FindImage(const yl::string& strFilePath)
{
    MAP_IMAGE_INFO::iterator iter = m_mapImage.find(strFilePath);

    if (iter != m_mapImage.end())
    {
        return iter->second;
    }

    return NULL;
}

void CImageCache::OnLoadImage(CImageInfoBase* pImageInfo)
{
    if (NULL == pImageInfo)
    {
        return;
    }

    const yl::string& strFilePath = pImageInfo->GetFilePath();
    MAP_IMAGE_INFO::iterator iter = m_mapImage.find(strFilePath);

    if (iter != m_mapImage.end())
    {
        FreeImageInfo(pImageInfo);
        return;
    }

    m_mapImage[strFilePath] = pImageInfo;

    OnCacheImage(pImageInfo);
}

bool CImageCache::IsCacheImage() const
{
    return true;
}
