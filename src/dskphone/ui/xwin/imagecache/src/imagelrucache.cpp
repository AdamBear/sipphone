#include "imagelrucache.h"
#include "xwindow/xwindowcommon.h"
#include "dsklog/log.h"


#if IS_SUPPORT_LRU_CACHE
namespace
{
#define CACHE_MAX_SIZE    24
#define CACHE_ELASTICITY  8
}

CImageLRUCache::CImageLRUCache(int nType/* = IMG_CACHE_TYPE_LRU*/)
    : CImageCacheBase(nType)
    , m_ImageCache(CACHE_MAX_SIZE, CACHE_ELASTICITY)
{

}

CImageLRUCache::~CImageLRUCache()
{
    m_ImageCache.clear();
}

CImageLRUCache* CImageLRUCache::GetImageLRUCache(CImageCacheBase* pCache)
{
    if (NULL == pCache || IMG_CACHE_TYPE_LRU != (IMG_CACHE_TYPE_LRU & pCache->GetType()))
    {
        return NULL;
    }

    return static_cast<CImageLRUCache*>(pCache);
}

void CImageLRUCache::Init()
{
    m_ImageCache.clear();
}

void CImageLRUCache::UnInit()
{
    m_ImageCache.clear();
}

void CImageLRUCache::ClearImage(int nClassifyType)
{
    LIST_STRING listPath;

    MAP_IMAGE_INFO_NODE& mapImgNode = m_ImageCache.getCache();
    MAP_IMAGE_INFO_NODE::iterator iter = mapImgNode.begin();

    for (; iter != mapImgNode.end(); ++iter)
    {
        LRU_NODE_IMAGE_INFO* pNode = iter->second;

        if (NULL == pNode)
        {
            continue;
        }

        CImageInfoBasePtr pInfo = pNode->value;

        if (pInfo.IsEmpty())
        {
            continue;
        }

        if (pInfo->GetClassifyType() == nClassifyType)
        {
            listPath.push_back(pNode->key);
        }
    }

    LIST_STRING::iterator iterPath = listPath.begin();

    for (; iterPath != listPath.end(); ++iterPath)
    {
        m_ImageCache.remove(*iterPath);
    }
}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
void CImageLRUCache::ClearScreenGraphic(int nClassifyType)
{
    MAP_IMAGE_INFO_NODE& mapImgNode = m_ImageCache.getCache();
    MAP_IMAGE_INFO_NODE::iterator iter = mapImgNode.begin();

    for (; iter != mapImgNode.end(); ++iter)
    {
        LRU_NODE_IMAGE_INFO* pNode = iter->second;

        if (NULL == pNode)
        {
            continue;
        }

        CImageInfoBasePtr pInfo = pNode->value;

        if (pInfo.IsEmpty())
        {
            continue;
        }

        if (pInfo->GetClassifyType() == nClassifyType)
        {
            pInfo->DetachScreenGraphic();
        }
    }
}
#endif

#if IF_IMAGE_MEMORY_DUMP
void CImageLRUCache::Dump(int nPageSize, int& nTotalSize, int& nTotalMemSize)
{
    UIKERNEL_INFO("Begin dump memory in cache, cachetype:IMG_CACHE_TYPE_LRU");

    MAP_IMAGE_INFO_NODE& mapImgNode = m_ImageCache.getCache();
    MAP_IMAGE_INFO_NODE::iterator iter = mapImgNode.begin();
    int nCacheTotalSize = 0;
    int nCacheTotalMemSize = 0;

    for (; iter != mapImgNode.end(); ++iter)
    {
        LRU_NODE_IMAGE_INFO* pNode = iter->second;

        if (NULL == pNode)
        {
            continue;
        }

        CImageInfoBasePtr pInfo = pNode->value;

        if (pInfo.IsEmpty())
        {
            continue;
        }

        int nSize = 0;
        int nMemSize = 0;

        pInfo->Dump(nPageSize, nSize, nMemSize);

        nCacheTotalSize += nSize;
        nCacheTotalMemSize += nMemSize;
    }

    UIKERNEL_INFO("End dump memory in cache, Total Image Size:%d, Total Memory Size:%d", nCacheTotalSize, nCacheTotalMemSize);

    nTotalSize += nCacheTotalSize;
    nTotalMemSize += nCacheTotalMemSize;
}
#endif

bool CImageLRUCache::RemoveImage(const yl::string& strFilePath, int nClassifyType)
{
    CImageInfoBasePtr pInfo;

    if (!m_ImageCache.tryGet(strFilePath, pInfo) || pInfo.IsEmpty())
    {
        return false;
    }

    if (IMG_CLASSIFY_NONE != nClassifyType && pInfo->GetClassifyType() != nClassifyType)
    {
        return false;
    }

    m_ImageCache.remove(strFilePath);
    return true;
}

bool CImageLRUCache::GetImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic)
{
    CImageInfoBase* pImageInfo = FindImage(strFilePath);

    if (NULL != pImageInfo)
    {
#if IF_IMAGE_FILE_DEBUG
        UIKERNEL_INFO("CImageLRUCache::GetImage%s return:%p", strFilePath.c_str(), &(pImageInfo->GetPixmap()));
#endif

        pic = pImageInfo->GetPixmap();

        return true;
    }

    return LoadImage(strFilePath, nClassifyType, pic, false);
}

CImageInfoBase* CImageLRUCache::FindImage(const yl::string& strFilePath)
{
    CImageInfoBasePtr pInfo;

    if (!m_ImageCache.tryGet(strFilePath, pInfo) || pInfo.IsEmpty())
    {
        return NULL;
    }

    return pInfo.Get();
}

void CImageLRUCache::OnLoadImage(CImageInfoBase* pImageInfo)
{
    if (NULL == pImageInfo)
    {
        return;
    }

    yl::string strFilePath = pImageInfo->GetFilePath();
    CImageInfoBasePtr pDetail;

    if (m_ImageCache.tryGet(strFilePath, pDetail) && !pDetail.IsEmpty())
    {
        if (pImageInfo != pDetail.Get())
        {
#if IF_IMAGE_FILE_DEBUG
            UIKERNEL_INFO("CImageLRUCache::OnLoadImage FreeImageInfo:%p", pImageInfo);
#endif

            FreeImageInfo(pImageInfo);
        }

        return;
    }

    pDetail = CImageInfoBasePtr(pImageInfo);

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("CImageLRUCache::OnLoadImage insert:%s %p", strFilePath.c_str(), pImageInfo);
#endif

    m_ImageCache.insert(strFilePath, pDetail);

    OnCacheImage(pImageInfo);
}

bool CImageLRUCache::IsCacheImage() const
{
    return true;
}

#endif
