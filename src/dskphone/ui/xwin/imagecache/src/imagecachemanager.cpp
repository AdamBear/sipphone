#include "imagecachemanager.h"
#include "interfacedefine.h"
#include "commonapi/systemhelperapi.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "imagefileinfo.h"
#include "imagecache.h"
#include "imagelrucache.h"
#include "imageclassifier.h"
#include "dsklog/log.h"
#include "imagetaskmanager.h"

CImageCacheManager::CImageCacheManager()
    : m_pDefaultLoadCache(NULL)
    , m_pDefaultFindCache(NULL)
    , m_mapPathCacheType(true)
{
}

CImageCacheManager::~CImageCacheManager()
{
    ClearCache();
    ClearClassifier();

    m_mapPathCacheType.clear();
}

CImageCacheManager& CImageCacheManager::GetImageCacheManager()
{
    static CImageCacheManager s_ImageCacheManager;

    return s_ImageCacheManager;
}

void CImageCacheManager::ClearCache()
{
    MAP_IMAGE_CACHE::iterator iter = m_mapCache.begin();

    for (; iter != m_mapCache.end(); ++iter)
    {
        CImageCacheBase* pCache = iter->second;

        if (NULL != pCache)
        {
            pCache->UnInit();
            delete pCache;
        }
    }

    m_mapCache.clear();

    m_pDefaultLoadCache = NULL;
    m_pDefaultFindCache = NULL;
}

void CImageCacheManager::ClearClassifier()
{
    MAP_IMAGE_CLASSIFIER::iterator iter = m_mapClassifier.begin();

    for (; iter != m_mapClassifier.end(); ++iter)
    {
        CImageClassifier* pClassifier = iter->second;

        if (NULL != pClassifier)
        {
            delete pClassifier;
        }
    }

    m_mapClassifier.clear();
}

CImageCacheBase* CImageCacheManager::GetCache(int nCacheType)
{
    MAP_IMAGE_CACHE::iterator iter = m_mapCache.find(nCacheType);

    if (iter != m_mapCache.end())
    {
        return iter->second;
    }

    return NULL;
}

CImageCacheBase* CImageCacheManager::GetCacheByClassifyType(int nClassifyType)
{
    CImageClassifier* pClassifier = GetClassifier(nClassifyType);

    if (NULL != pClassifier)
    {
        return GetCache(pClassifier->GetCacheType());
    }

    return NULL;
}

CImageCacheBase* CImageCacheManager::GetLoadCacheByClassifyType(int nClassifyType)
{
    CImageCacheBase* pCache = GetCacheByClassifyType(nClassifyType);

    if (NULL != pCache)
    {
        return pCache;
    }

    return m_pDefaultLoadCache;
}

CImageCacheBase* CImageCacheManager::GetFindCacheByClassifyType(int nClassifyType)
{
    CImageCacheBase* pCache = GetCacheByClassifyType(nClassifyType);

    if (NULL != pCache)
    {
        return pCache;
    }

    return m_pDefaultFindCache;
}

CImageClassifier* CImageCacheManager::GetClassifier(int nClassifierType)
{
    MAP_IMAGE_CLASSIFIER::iterator iter = m_mapClassifier.find(nClassifierType);

    if (iter != m_mapClassifier.end())
    {
        return iter->second;
    }

    return NULL;
}

void CImageCacheManager::Init()
{
    g_ImageTaskMgr.Init();

    CImageCacheBase* pNoCache = new CImageCacheBase;

    if (NULL != pNoCache && !RegisterCache(pNoCache))
    {
        delete pNoCache;
    }

    CImageCache* pCache = new CImageCache;

    if (NULL != pCache && !RegisterCache(pCache))
    {
        delete pCache;
    }

#if IS_SUPPORT_LRU_CACHE
    CImageLRUCache* pLRUCache = new CImageLRUCache;

    if (NULL != pLRUCache && !RegisterCache(pLRUCache))
    {
        delete pLRUCache;
    }
#endif

    CImageClassifier* pThemeClassifier = new CImageClassifier;

    if (NULL != pThemeClassifier)
    {
        pThemeClassifier->SetImageClassifyType(IMG_CLASSIFY_THEME);
        pThemeClassifier->SetCacheType(IMG_CACHE_TYPE_MANUAL);
        pThemeClassifier->SetAction(CImageClassifier::IMG_ACTION_CONVERT_DEFAULT_FORMAT);

        if (!RegisterClassifier(pThemeClassifier))
        {
            delete pThemeClassifier;
        }
    }

    CImageClassifier* pStockClassifier = new CImageClassifier;

    if (NULL != pStockClassifier)
    {
        pStockClassifier->SetImageClassifyType(IMG_CLASSIFY_STOCK);
        pStockClassifier->SetCacheType(IMG_CACHE_TYPE_MANUAL);
        pThemeClassifier->SetAction(CImageClassifier::IMG_ACTION_CONVERT_DEFAULT_FORMAT);

        if (!RegisterClassifier(pStockClassifier))
        {
            delete pStockClassifier;
        }
    }

    CImageClassifier* pNoCacheClassifier = new CImageClassifier;

    if (NULL != pNoCacheClassifier)
    {
        pNoCacheClassifier->SetImageClassifyType(IMG_CLASSIFY_NOCACHE);
        pNoCacheClassifier->SetCacheType(IMG_CACHE_TYPE_NONE);

        if (!RegisterClassifier(pNoCacheClassifier))
        {
            delete pNoCacheClassifier;
        }
    }

    CImageClassifier* pCustomClassifier = new CImageClassifier;

    if (NULL != pCustomClassifier)
    {
        pCustomClassifier->SetImageClassifyType(IMG_CLASSIFY_CUSTOM);
        pCustomClassifier->SetCacheType(
#if IS_SUPPORT_LRU_CACHE
            IMG_CACHE_TYPE_LRU
#else
            IMG_CACHE_TYPE_MANUAL
#endif
        );

        if (!RegisterClassifier(pCustomClassifier))
        {
            delete pCustomClassifier;
        }
    }

#if IS_SUPPORT_LRU_CACHE
    SetDefaultLoadCache(IMG_CACHE_TYPE_LRU);
#else
    SetDefaultLoadCache(IMG_CACHE_TYPE_MANUAL);
#endif

    SetDefaultFindCache(IMG_CACHE_TYPE_MANUAL);
}

void CImageCacheManager::UnInit()
{
    g_ImageTaskMgr.UnInit();
    ClearCache();
    ClearClassifier();

    m_mapPathCacheType.clear();
}

bool CImageCacheManager::RegisterCache(CImageCacheBase* pImageCache)
{
    if (NULL == pImageCache)
    {
        return false;
    }

    int nCacheType = pImageCache->GetType();
    MAP_IMAGE_CACHE::iterator iter = m_mapCache.find(nCacheType);

    chASSERTx(iter == m_mapCache.end(), "repeat same type cache:%0x", nCacheType);

    if (iter != m_mapCache.end())
    {
        return false;
    }

    pImageCache->Init();
    m_mapCache[nCacheType] = pImageCache;

    return true;
}

bool CImageCacheManager::UnRegisterCache(int nCacheType)
{
    MAP_IMAGE_CACHE::iterator iter = m_mapCache.find(nCacheType);

    if (iter == m_mapCache.end())
    {
        return false;
    }

    CImageCacheBase* pImageCache = iter->second;

    m_mapCache.erase(iter);

    if (NULL == pImageCache)
    {
        return true;
    }

    if (pImageCache == m_pDefaultLoadCache)
    {
        m_pDefaultLoadCache = NULL;
    }

    if (pImageCache == m_pDefaultFindCache)
    {
        m_pDefaultFindCache = NULL;
    }

    pImageCache->UnInit();
    delete pImageCache;

    return true;
}

bool CImageCacheManager::RegisterClassifier(CImageClassifier* pImageClassifier)
{
    if (NULL == pImageClassifier)
    {
        return false;
    }

    int nClassifyType = pImageClassifier->GetImageClassifyType();
    MAP_IMAGE_CLASSIFIER::iterator iter = m_mapClassifier.find(nClassifyType);

    chASSERTx(iter == m_mapClassifier.end(), "repeat same type classifier:%0x", nClassifyType);

    if (iter != m_mapClassifier.end())
    {
        return false;
    }

    m_mapClassifier[nClassifyType] = pImageClassifier;

    return true;
}

bool CImageCacheManager::UnRegisterClassifier(int nClassifyType)
{
    MAP_IMAGE_CLASSIFIER::iterator iter = m_mapClassifier.find(nClassifyType);

    if (iter == m_mapClassifier.end())
    {
        return false;
    }

    CImageClassifier* pClassifier = iter->second;

    m_mapClassifier.erase(iter);

    if (NULL != pClassifier)
    {
        delete pClassifier;
    }

    return true;
}

void CImageCacheManager::SetDefaultLoadCache(int nCacheType)
{
    m_pDefaultLoadCache = GetCache(nCacheType);
}

void CImageCacheManager::SetDefaultFindCache(int nCacheType)
{
    m_pDefaultFindCache = GetCache(nCacheType);
}

bool CImageCacheManager::LoadImage(const yl::string& strPath, int nClassifyType, bool bReload)
{
    CImageCacheBase* pCache = GetLoadCacheByClassifyType(nClassifyType);

    if (NULL == pCache)
    {
        return false;
    }

    return pCache->LoadImage(strPath, nClassifyType, bReload);
}

bool CImageCacheManager::LoadImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic, bool bReload)
{
    CImageCacheBase* pCache = GetLoadCacheByClassifyType(nClassifyType);

    if (NULL == pCache)
    {
        return false;
    }

    return pCache->LoadImage(strFilePath, nClassifyType, pic, bReload);
}

bool CImageCacheManager::AddImage(const yl::string& strFilePath, int nClassifyType, const char* pFlow,
                                  int nFlowSize, bool bReload)
{
    CImageCacheBase* pCache = GetLoadCacheByClassifyType(nClassifyType);

    if (NULL == pCache)
    {
        return false;
    }

    return pCache->AddImage(strFilePath, nClassifyType, pFlow, nFlowSize, bReload);
}

bool CImageCacheManager::RemoveImage(const yl::string& strFilePath, int nClassifyType)
{
    if (IMG_CLASSIFY_NONE == nClassifyType)
    {
        yl::string strPath;

        if (GetPathByFile(strFilePath, strPath))
        {
            MAP_IMAGE_PATH_CACHE_TYPE::iterator iter = m_mapPathCacheType.find(strPath);

            for (; iter != m_mapPathCacheType.end(); iter++)
            {
                if (iter->first != strPath)
                {
                    break;
                }

                CImageCacheBase* pCache = GetCache(iter->second);

                if (NULL != pCache && pCache->RemoveImage(strFilePath, IMG_CLASSIFY_NONE))
                {
                    return true;
                }
            }
        }

        if (NULL != m_pDefaultLoadCache && m_pDefaultLoadCache->RemoveImage(strFilePath, IMG_CLASSIFY_NONE))
        {
            return true;
        }

        return false;
    }

    CImageCacheBase* pCache = GetLoadCacheByClassifyType(nClassifyType);

    if (NULL == pCache)
    {
        return false;
    }

    return pCache->RemoveImage(strFilePath, nClassifyType);
}

void CImageCacheManager::ClearImage(int nClassifyType)
{
    CImageCacheBase* pCache = GetLoadCacheByClassifyType(nClassifyType);

    if (NULL != pCache)
    {
        pCache->ClearImage(nClassifyType);
    }
}

bool CImageCacheManager::GetImage(const yl::string& strFilePath, xPixmap& pic)
{
    CImageInfoBase* pInfo = FindImageInfo(strFilePath);

    if (NULL != pInfo)
    {
        pic = pInfo->GetPixmap();

        return true;
    }

    if (NULL != m_pDefaultLoadCache)
    {
        return m_pDefaultLoadCache->GetImage(strFilePath, IMG_CLASSIFY_NONE, pic);
    }

    return false;
}

CImageInfoBase* CImageCacheManager::FindImageInfo(const yl::string& strFilePath)
{
    yl::string strPath;

    if (GetPathByFile(strFilePath, strPath))
    {
        MAP_IMAGE_PATH_CACHE_TYPE::iterator iter = m_mapPathCacheType.find(strPath);

        for (; iter != m_mapPathCacheType.end(); iter++)
        {
            if (iter->first != strPath)
            {
                break;
            }

            CImageCacheBase* pCache = GetCache(iter->second);
            CImageInfoBase* pInfo = NULL;

            if (NULL != pCache)
            {
                pInfo = pCache->FindImage(strFilePath);
            }

            if (NULL != pInfo)
            {
                return pInfo;
            }
        }
    }

    if (NULL != m_pDefaultFindCache)
    {
        return m_pDefaultFindCache->FindImage(strFilePath);
    }

    return NULL;
}

bool CImageCacheManager::GetImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic)
{
    CImageCacheBase* pCache = GetFindCacheByClassifyType(nClassifyType);

    if (NULL != pCache && pCache->GetImage(strFilePath, nClassifyType, pic))
    {
        return true;
    }

    if (NULL != m_pDefaultLoadCache)
    {
        return m_pDefaultLoadCache->GetImage(strFilePath, nClassifyType, pic);
    }

    return false;
}

IMAGE_SIZE_TYPE CImageCacheManager::CheckImageSize(const yl::string& strFilePath)
{
    CImageFileInfo infoFile;

    return CheckImageSize(infoFile.getImageDimension(strFilePath));
}

IMAGE_SIZE_TYPE CImageCacheManager::CheckImageSize(const chSize& size)
{
    if (size.cx <= 0 || size.cy <= 0)
    {
        return IMG_SIZE_INVALID;
    }

    if ((size.cx * size.cy) > MAX_PICTURE_RESOLUTION)
    {
        return IMG_SIZE_OVER_LIMIT;
    }

    return IMG_SIZE_REGULAR;
}

bool CImageCacheManager::GetPathByFile(const yl::string& strFilePath, yl::string& strPath)
{
    yl::string::size_type pos = strFilePath.find_last_of("/");

    if (yl::string::npos == pos)
    {
        return false;
    }

    strPath = strFilePath.substr(0, pos + 1);

    return true;
}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
void CImageCacheManager::ClearScreenGraphic(int nClassifyType)
{
    CImageCacheBase* pCache = GetLoadCacheByClassifyType(nClassifyType);

    if (NULL != pCache)
    {
        pCache->ClearScreenGraphic(nClassifyType);
    }
}

void CImageCacheManager::ClearScreenGraphic(const yl::string& strFilePath)
{
    xPixmap pmTmp;

    if (GetImage(strFilePath, pmTmp))
    {
        pmTmp.DetachScreenGraphic();
    }
}
#endif

#if IF_IMAGE_MEMORY_DUMP
void CImageCacheManager::Dump(int nPageSize)
{
    UIKERNEL_INFO("Begin Dump Cache Images, memory page size:%d", nPageSize);

    MAP_IMAGE_CACHE::iterator iter = m_mapCache.begin();
    int nTotalSize = 0;
    int nTotalMemSize = 0;

    for (; iter != m_mapCache.end(); ++iter)
    {
        CImageCacheBase* pCache = iter->second;

        if (NULL != pCache)
        {
            pCache->Dump(nPageSize, nTotalSize, nTotalMemSize);
        }
    }

    UIKERNEL_INFO("End Dump Cache Images, TotalSize:%d TotalMemSize:%d", nTotalSize, nTotalMemSize);
}
#endif

void CImageCacheManager::OnCacheImage(int nCacheType, const yl::string& strFilePath)
{
    if (NULL != m_pDefaultFindCache && nCacheType == m_pDefaultFindCache->GetType())
    {
        return;
    }

    yl::string strPath;

    if (!GetPathByFile(strFilePath, strPath))
    {
        return;
    }

    MAP_IMAGE_PATH_CACHE_TYPE::iterator iter = m_mapPathCacheType.find(strPath);

    while (iter != m_mapPathCacheType.end())
    {
        if (iter->first != strPath)
        {
            break;
        }

        if (iter->second == nCacheType)
        {
            return;
        }

        iter++;
    }

    m_mapPathCacheType.insert(strPath, nCacheType);
}
