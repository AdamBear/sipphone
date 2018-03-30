#include "imageinfo.h"
#include <ETLLib/ETLLib.hpp>
#include "imageclassifier.h"
#include "imagecachebase.h"
#include "imagefileinfo.h"
#include "imagecachemanager.h"
#include "dsklog/log.h"
#include "imagetaskmanager.h"

//////////////////////////////////CImageBaseInfo////////////////////////////////////////
CImageInfoBase::CImageInfoBase(int nType/* = IMAGE_INFO_TYPE_BASE*/, int nClassifyType/* = IMG_CLASSIFY_NONE*/)
    : m_nType(nType)
    , m_nClassifyType(nClassifyType)
{
#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("new CImageInfoBase:%p m_pixmap:%p", this, &m_pixmap);
#endif
}

CImageInfoBase::~CImageInfoBase()
{
    CImageCacheBase::OnFreeImageInfo(this);

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("delete CImageInfoBase:%p", this);
#endif
}

void CImageInfoBase::SetClassifyType(int nClassifyType)
{
    m_nClassifyType = nClassifyType;
}

void CImageInfoBase::SetFilePath(const yl::string& strFilePath)
{
#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("CImageInfoBase:%p SetFilePath:%s", this, strFilePath.c_str());
#endif
    m_strFilePath = strFilePath;
}

bool CImageInfoBase::LoadPixmap(const char* pFlow, int nFlowSize)
{
    if (m_pixmap.load(pFlow, nFlowSize))
    {
        if (!m_strFilePath.empty() && "" != m_strFilePath)
        {
            chSize sizeImage(m_pixmap.width(), m_pixmap.height());

            m_pixmap.save(m_strFilePath, sizeImage);
        }

        CImageCacheBase::OnLoadImageInfo(this);

        return true;
    }

    return false;
}

bool CImageInfoBase::LoadPixmap(const yl::string& strFile)
{
    if (!pathFileExist(strFile.c_str()) || !CImageFileInfo::IsImage(strFile.c_str()))
    {
        return false;
    }

    m_strFilePath = strFile;

    CImageClassifier* pClassifier = GetClassifier();
    if (NULL == pClassifier)
    {
        return false;
    }

    bool bASync = pClassifier->IsAsyncLoad();
    bool bRet = false;
    if (bASync)
    {
        chSize sizeImg = chSize(0, 0);
        bRet = g_ImageTaskMgr.AddImageTask(m_strFilePath, m_nClassifyType, sizeImg);
    }
    else
    {
        bRet = m_pixmap.load(strFile);

        if (bRet)
        {
            CImageCacheBase::OnLoadImageInfo(this);
        }
    }

    return bRet;
}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
void CImageInfoBase::DetachScreenGraphic()
{
    m_pixmap.DetachScreenGraphic();
}
#endif

#if IF_IMAGE_MEMORY_DUMP
void CImageInfoBase::Dump(int nPageSize, int& nSize, int& nMemSize)
{
    nSize = m_pixmap.width() * m_pixmap.height() * 4;
    nMemSize = nSize;

    if (nPageSize > 0)
    {
        nMemSize = nMemSize - (nMemSize % nPageSize) + nPageSize;
    }

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    UIKERNEL_INFO("Image:%s Size:%d MemSize:%d ScreenGraphic:%d", m_strFilePath.c_str(),
                  nSize, nMemSize, (NULL != m_pixmap.GetScreenGraphic() ? 1 : 0));
#else
    UIKERNEL_INFO("Image:%s Size:%d MemSize:%d", m_strFilePath.c_str(), nSize, nMemSize);
#endif
}

#endif

CImageClassifier* CImageInfoBase::GetClassifier()
{
    return g_ImageCacheMgr.GetClassifier(m_nClassifyType);
}

//////////////////////////////////CScaleImageInfo////////////////////////////////////////
CScaleImageInfo::CScaleImageInfo(int nType/* = IMAGE_INFO_TYPE_SCALE*/)
    : CImageInfoBase(nType)
{

}

CScaleImageInfo::~CScaleImageInfo()
{

}

bool CScaleImageInfo::LoadPixmap(const char* pFlow, int nFlowSize)
{
    xPixmap pmTmp;

    if (!pmTmp.load(pFlow, nFlowSize))
    {
        return false;
    }

    CScaleImageClassifier* pScaleClassifier = CScaleImageClassifier::GetScalImageClassifier(GetClassifier());
    chSize sizeImage;

    if (NULL != pScaleClassifier)
    {
        sizeImage = pScaleClassifier->GetScaleSize(pmTmp);
    }

    if (sizeImage.cx <= 0 || sizeImage.cy <= 0)
    {
        sizeImage.cx = pmTmp.width();
        sizeImage.cy = pmTmp.height();
    }

    if (!m_strFilePath.empty() || "" != m_strFilePath)
    {
        pmTmp.save(m_strFilePath, sizeImage);
    }

    bool bResult = ScaleFromPixmap(pmTmp, sizeImage);

    if (bResult)
    {
        CImageCacheBase::OnLoadImageInfo(this);
    }

    return bResult;
}

bool CScaleImageInfo::LoadPixmap(const yl::string& strFilePath)
{
    if (!pathFileExist(strFilePath.c_str()) || !CImageFileInfo::IsImage(strFilePath.c_str()))
    {
        return false;
    }

    m_strFilePath = strFilePath;

    xPixmap pmTmp;

    bool bASync = false;
    CScaleImageClassifier* pScaleClassifier = CScaleImageClassifier::GetScalImageClassifier(GetClassifier());
    chSize sizeImg;

    if (NULL != pScaleClassifier)
    {
        sizeImg = pScaleClassifier->GetScaleSize(pmTmp);
        bASync = pScaleClassifier->IsAsyncLoad();
    }

    bool bRet = false;
    if (bASync)
    {
        bRet = g_ImageTaskMgr.AddImageTask(m_strFilePath, m_nClassifyType, sizeImg);
    }
    else
    {
        pmTmp.load(strFilePath);
        bRet = ScaleFromPixmap(pmTmp, sizeImg);

        if (bRet)
        {
            CImageCacheBase::OnLoadImageInfo(this);
        }
    }

    if (!bASync && bRet && NULL != pScaleClassifier && pScaleClassifier->IsUpdateFile()
            && pmTmp.width() != sizeImg.cx && pmTmp.height() != sizeImg.cy)
    {
        if (0 == remove(strFilePath.c_str()))
        {
            m_pixmap.save(strFilePath, sizeImg);
        }
    }

    return bRet;
}

bool CScaleImageInfo::ScaleFromPixmap(xPixmap& src, const chSize& size)
{
    chSize szPixmap(src.width(), src.height());

    if (szPixmap == size || size.cx <= 0 || size.cy <= 0)
    {
        m_pixmap = src;

        return true;
    }

    if (!m_pixmap.Create(size.cx, size.cy))
    {
        return false;
    }

    return m_pixmap.ScaleCopy(src);
}
