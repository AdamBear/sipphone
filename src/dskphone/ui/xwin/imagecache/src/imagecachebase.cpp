#include "imagecachebase.h"
#ifdef _WIN32
#include "commonapi/stringhelperapi.h"
#endif
#include "commonapi/systemhelperapi.h"
#include "imagecachemanager.h"
#include "imagefileinfo.h"


namespace
{
#define FILE_EXTENSION_NAMES            "png,jpg,jpeg,bmp,gif"
}

CImageCacheBase::CImageCacheBase(int nType/* = IMG_CACHE_TYPE_NONE*/)
    : m_nType(nType)
{

}

CImageCacheBase::~CImageCacheBase()
{

}

bool CImageCacheBase::IsPathValid(const yl::string& strPath)
{
    if (strPath.empty() || "" == strPath || !pathFileExist(strPath.c_str()))
    {
        return false;
    }

    return true;
}

bool CImageCacheBase::IsFolder(const yl::string& strPath)
{
    if (S_ISDIR(pathStat(strPath.c_str())))
    {
        return true;
    }

    return false;
}

CImageClassifier* CImageCacheBase::GetClassifier(int nClassifyType)
{
    return g_ImageCacheMgr.GetClassifier(nClassifyType);
}

CImageInfoBase* CImageCacheBase::AllocImageInfo(int nClassifyType)
{
    CImageClassifier* pClassifier = g_ImageCacheMgr.GetClassifier(nClassifyType);
    CImageInfoBase* pImageInfo = NULL;

    if (NULL != pClassifier)
    {
        pImageInfo = pClassifier->AllocImageInfo();
    }
    else
    {
        pImageInfo = new CImageInfoBase;
    }

    return pImageInfo;
}

void CImageCacheBase::FreeImageInfo(CImageInfoBase* pImageInfo)
{
    if (NULL != pImageInfo)
    {
        delete pImageInfo;
    }
}

void CImageCacheBase::OnLoadImageInfo(CImageInfoBase* pImageInfo)
{
    CImageClassifier* pClassifier = NULL;

    if (NULL != pImageInfo)
    {
        pClassifier = g_ImageCacheMgr.GetClassifier(pImageInfo->GetClassifyType());
    }

    if (NULL != pClassifier)
    {
        pClassifier->OnLoadImageInfo(pImageInfo);
    }
}

void CImageCacheBase::OnFreeImageInfo(CImageInfoBase* pImageInfo)
{
    CImageClassifier* pClassifier = NULL;

    if (NULL != pImageInfo)
    {
        pClassifier = g_ImageCacheMgr.GetClassifier(pImageInfo->GetClassifyType());
    }

    if (NULL != pClassifier)
    {
        pClassifier->OnFreeImageInfo(pImageInfo);
    }
}

bool CImageCacheBase::IsImageBefitting(const yl::string& strFilePath)
{
    CImageFileInfo infoFile;
    chSize sizeImage;

    sizeImage = infoFile.getImageDimension(strFilePath);

    if (IMG_SIZE_REGULAR != g_ImageCacheMgr.CheckImageSize(sizeImage))
    {
        return false;
    }

    if (!IsCacheImage())
    {
        return true;
    }

    if ((sizeImage.cx * sizeImage.cy) > MAX_CACHE_RESOLUTION)
    {
        return false;
    }

    return true;
}

CImageInfoBase* CImageCacheBase::LoadImageInfo(const yl::string& strFilePath, int nClassifyType, bool& bReload)
{
    if (!IsImageBefitting(strFilePath))
    {
        return NULL;
    }

    CImageInfoBase* pImageInfo = FindImage(strFilePath);

    if (NULL != pImageInfo)
    {
        if (pImageInfo->GetClassifyType() != nClassifyType)
        {
            return NULL;
        }

        if (!bReload)
        {
            return pImageInfo;
        }
    }
    else
    {
        pImageInfo = AllocImageInfo(nClassifyType);
        bReload = false;
    }

    if (NULL != pImageInfo && !pImageInfo->LoadPixmap(strFilePath))
    {
        if (!bReload)
        {
            FreeImageInfo(pImageInfo);
        }

        return NULL;
    }

    return pImageInfo;
}

void CImageCacheBase::Init()
{

}

void CImageCacheBase::UnInit()
{

}

void CImageCacheBase::ClearImage(int nClassifyType)
{

}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
void CImageCacheBase::ClearScreenGraphic(int nClassifyType)
{

}
#endif

#if IF_IMAGE_MEMORY_DUMP
void CImageCacheBase::Dump(int nPageSize, int& nTotalSize, int& nTotalMemSize)
{

}
#endif

bool CImageCacheBase::RemoveImage(const yl::string& strFilePath, int nClassifyType)
{
    return false;
}

bool CImageCacheBase::LoadImage(const yl::string& strPath, int nClassifyType, bool bReload)
{
    if (!IsPathValid(strPath))
    {
        return false;
    }

    if (!IsFolder(strPath))
    {
#if _WIN32
        yl::string strRealPath = commonAPI_replaceSubStringByTag(strPath, "\\", "/");

        strRealPath = commonAPI_replaceSubStringByTag(strPath, "//", "/");

        CImageInfoBase* pImageInfo = LoadImageInfo(strRealPath, nClassifyType, bReload);
#else
        CImageInfoBase* pImageInfo = LoadImageInfo(strPath, nClassifyType, bReload);
#endif

        if (NULL == pImageInfo)
        {
            return false;
        }

        // 真正重载不需要做其他处理
        if (bReload)
        {
            return true;
        }

        OnLoadImage(pImageInfo);

        return true;
    }

    yl::string strImageFilePath = strPath;
    YLList<yl::string> listFilePath;

    commonAPI_getFilesInDirectoryByExt(&listFilePath, strImageFilePath, FILE_EXTENSION_NAMES, true, false, true);

    YLList<yl::string>::iterator it = listFilePath.begin();

    for (; it != listFilePath.end(); ++it)
    {
        bool bImageReload = bReload;

#if _WIN32
        yl::string strRealPath = commonAPI_replaceSubStringByTag((*it), "\\", "/");

        strRealPath = commonAPI_replaceSubStringByTag(strRealPath, "//", "/");

        CImageInfoBase* pImageInfo = LoadImageInfo(strRealPath, nClassifyType, bImageReload);
#else
        CImageInfoBase* pImageInfo = LoadImageInfo((*it), nClassifyType, bImageReload);
#endif

        // 真正重载不需要做其他处理
        if (NULL == pImageInfo || bImageReload)
        {
            continue;
        }

        OnLoadImage(pImageInfo);
    }

    return true;
}

bool CImageCacheBase::LoadImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic, bool bReload)
{
    if (!IsPathValid(strFilePath))
    {
        return false;
    }

    if (IsFolder(strFilePath))
    {
        return false;
    }

    CImageInfoBase* pImageInfo = LoadImageInfo(strFilePath, nClassifyType, bReload);

    if (NULL == pImageInfo)
    {
        return false;
    }

    pic = pImageInfo->GetPixmap();

    // 真正重载不需要做其他处理
    if (!bReload)
    {
        OnLoadImage(pImageInfo);
    }

    return true;
}

bool CImageCacheBase::AddImage(const yl::string& strFilePath, int nClassifyType, const char* pFlow,
                               int nFlowSize, bool bReload)
{
    if (NULL == pFlow || nFlowSize <= 0)
    {
        return false;
    }

    if (strFilePath.empty() || "" == strFilePath || IsFolder(strFilePath))
    {
        return false;
    }

    CImageInfoBase* pImageInfo = FindImage(strFilePath);

    if (NULL != pImageInfo)
    {
        if (pImageInfo->GetClassifyType() != nClassifyType)
        {
            return false;
        }

        if (!bReload)
        {
            return true;
        }
    }
    else
    {
        pImageInfo = AllocImageInfo(nClassifyType);

        bReload = false;
    }

    if (NULL == pImageInfo)
    {
        return false;
    }

    pImageInfo->SetFilePath(strFilePath);

    if (!pImageInfo->LoadPixmap(pFlow, nFlowSize))
    {
        if (!bReload)
        {
            FreeImageInfo(pImageInfo);
        }

        return false;
    }

    // 真正重载不需要做其他处理
    if (!bReload)
    {
        OnLoadImage(pImageInfo);
    }

    return true;
}

bool CImageCacheBase::GetImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic)
{
    if (!IsPathValid(strFilePath) || IsFolder(strFilePath))
    {
        return false;
    }

    if (!IsImageBefitting(strFilePath))
    {
        return false;
    }

    return pic.load(strFilePath);
}

CImageInfoBase* CImageCacheBase::FindImage(const yl::string& strFilePath)
{
    return NULL;
}

void CImageCacheBase::OnLoadImage(CImageInfoBase* pImageInfo)
{
    // 默认无缓存，删除掉对应的指针
    FreeImageInfo(pImageInfo);
}

void CImageCacheBase::OnCacheImage(CImageInfoBase* pImageInfo)
{
    if (NULL == pImageInfo)
    {
        return;
    }

    g_ImageCacheMgr.OnCacheImage(m_nType, pImageInfo->GetFilePath());
}

bool CImageCacheBase::IsCacheImage() const
{
    return false;
}
