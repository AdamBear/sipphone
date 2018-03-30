#include "imageclassifier.h"
#include "imagecachebase.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
CImageClassifier::CImageClassifier(int nType/* = IMAGE_CLASSIFIER_TYPE_BASE*/)
    : m_nType(nType)
    , m_nImageClassifyType(IMG_CLASSIFY_NONE)
    , m_nCacheType(IMG_CACHE_TYPE_NONE)
    , m_nAction(IMG_ACTION_NONE)
    , m_bAsyncLoad(false)
{

}

CImageClassifier::~CImageClassifier()
{

}

void CImageClassifier::SetImageClassifyType(int nClassifyType)
{
    m_nImageClassifyType = nClassifyType;
}

void CImageClassifier::SetCacheType(int nCacheType)
{
    m_nCacheType = nCacheType;
}

void CImageClassifier::SetAction(int nAction)
{
    m_nAction = nAction;
}

void CImageClassifier::SetAsyncLoad(bool bAsync)
{
    m_bAsyncLoad = bAsync;
}

CImageInfoBase* CImageClassifier::AllocImageInfo()
{
    CImageInfoBase* pImageInfo = new CImageInfoBase;

    if (NULL != pImageInfo)
    {
        pImageInfo->SetClassifyType(m_nImageClassifyType);
    }

    return pImageInfo;
}

void CImageClassifier::OnLoadImageInfo(CImageInfoBase* pImageInfo)
{
    if (NULL == pImageInfo)
    {
        return;
    }

    if (IMG_ACTION_CONVERT_DEFAULT_FORMAT == (IMG_ACTION_CONVERT_DEFAULT_FORMAT & m_nAction))
    {
        pImageInfo->GetPixmap().convertDefaultFormat();
    }
}

void CImageClassifier::OnFreeImageInfo(CImageInfoBase* pImageInfo)
{
    if (NULL == pImageInfo)
    {
        return;
    }

    if (IMG_ACTION_DELETE_FILE_WHEN_FREE == (IMG_ACTION_DELETE_FILE_WHEN_FREE & m_nAction))
    {
        const yl::string& strFilePath = pImageInfo->GetFilePath();

        if (CImageCacheBase::IsPathValid(strFilePath))
        {
            remove(strFilePath.c_str());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CScaleImageClassifier::CScaleImageClassifier(int nType/* = IMAGE_CLASSIFIER_TYPE_SCALE*/)
    : CImageClassifier(nType)
{

}

CScaleImageClassifier::~CScaleImageClassifier()
{

}

int CScaleImageClassifier::GetScaleImageClassifierType()
{
    return IMAGE_CLASSIFIER_TYPE_SCALE;
}

CScaleImageClassifier* CScaleImageClassifier::GetScalImageClassifier(CImageClassifier* pClassifier)
{
    if (NULL == pClassifier || IMAGE_CLASSIFIER_TYPE_SCALE != (IMAGE_CLASSIFIER_TYPE_SCALE & pClassifier->GetType()))
    {
        return NULL;
    }

    return static_cast<CScaleImageClassifier*>(pClassifier);
}

void CScaleImageClassifier::SetImageSize(int nWidth, int nHeight)
{
    m_sizeImage.cx = nWidth;
    m_sizeImage.cy = nHeight;
}

void CScaleImageClassifier::SetImageSize(const chSize& size)
{
    m_sizeImage = size;
}

CImageInfoBase* CScaleImageClassifier::AllocImageInfo()
{
    CScaleImageInfo* pImageInfo = new CScaleImageInfo;

    if (NULL != pImageInfo)
    {
        pImageInfo->SetClassifyType(m_nImageClassifyType);
    }

    return pImageInfo;
}

chSize CScaleImageClassifier::GetScaleSize(const xPixmap& pm)
{
    return m_sizeImage;
}

void CScaleImageClassifier::SetUpdateFile(bool bUpdate)
{
    m_bUpdate = bUpdate;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CProportionImageClassifier::CProportionImageClassifier(int nType/* = IMAGE_CLASSIFIER_TYPE_SCALE_BY_PROPORTION*/)
    : CScaleImageClassifier(nType)
{

}

CProportionImageClassifier::~CProportionImageClassifier()
{

}

int CProportionImageClassifier::GetProportionImageClassifierType()
{
    return IMAGE_CLASSIFIER_TYPE_SCALE_BY_PROPORTION;
}

CProportionImageClassifier* CProportionImageClassifier::GetPropertionImageClassifier(CImageClassifier* pClassifier)
{
    if (NULL == pClassifier || IMAGE_CLASSIFIER_TYPE_SCALE_BY_PROPORTION != (IMAGE_CLASSIFIER_TYPE_SCALE_BY_PROPORTION & pClassifier->GetType()))
    {
        return NULL;
    }

    return static_cast<CProportionImageClassifier*>(pClassifier);
}

chSize CProportionImageClassifier::GetScaleSize(const xPixmap& pm)
{
    int nWidth = pm.width();
    int nHeight = pm.height();

    if (nWidth <= m_sizeImage.cx && nHeight <= m_sizeImage.cy)
    {
        return chSize(nWidth, nHeight);
    }

    float fScaleRate = 1.0;

    if (nWidth > m_sizeImage.cx && nHeight <= m_sizeImage.cy)
    {
        if (nWidth > 0)
        {
            fScaleRate = ((float)m_sizeImage.cx) / nWidth;
        }

        nWidth = m_sizeImage.cx;
        nHeight = nHeight * fScaleRate;

        return chSize(nWidth, nHeight);
    }

    if (nWidth <= m_sizeImage.cx && nHeight > m_sizeImage.cy)
    {
        if (nHeight > 0)
        {
            fScaleRate = ((float)m_sizeImage.cy) / nHeight;
        }

        nWidth = nWidth * fScaleRate;
        nHeight = m_sizeImage.cy;

        return chSize(nWidth, nHeight);
    }

    float fScaleRateWidth = 1.0;
    float fScaleRateHeight = 1.0;

    if (nWidth > 0)
    {
        fScaleRateWidth = ((float)m_sizeImage.cx) / nWidth;
    }

    if (nHeight > 0)
    {
        fScaleRateHeight = ((float)m_sizeImage.cy) / nHeight;
    }

    fScaleRate = (fScaleRateWidth < fScaleRateHeight ? fScaleRateWidth : fScaleRateHeight);

    nWidth = nWidth * fScaleRate;
    nHeight = nHeight * fScaleRate;

    return chSize(nWidth, nHeight);
}
