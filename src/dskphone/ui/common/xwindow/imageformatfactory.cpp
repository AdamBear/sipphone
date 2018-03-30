#include "ETLLib/ETLLib.hpp"
#include "imageformatfactory.h"
#include "xwindow-kernel/include/xkernelcommon.h"
#include "imageformatdefine.h"
#include "dsklog/log.h"

#if !IF_ONLY_USE_GRADATION
CImageFormatFactory::CImageFormatFactory()
{
}

CImageFormatFactory::~CImageFormatFactory()
{
    m_mapCreateImageFormat.clear();
}

CImageFormatFactory& CImageFormatFactory::GetInstance()
{
#if IF_MULTI_WORKSTATION_ENBALE
    MULTI_KERNEL_INSTANCE(CImageFormatFactory)
#else
    SINGLE_KERNEL_INSTANCE(CImageFormatFactory)
#endif
}

void CImageFormatFactory::RegisterImageFormatFactory(const yl::string& strFileSuffixName, FUN_CREATE_IMAGE_FORMAT_HANDLE pFunCreateImageFormat)
{
    if (NULL == pFunCreateImageFormat)
    {
        return;
    }

    chASSERTx(m_mapCreateOverLengthEffect.find(strFileSuffixName) == m_mapCreateOverLengthEffect.end(), "repeat register same image format type:%s", strFileSuffixName);

    m_mapCreateImageFormat[strFileSuffixName] = (*pFunCreateImageFormat)();
}

CImageFormatHandleBase* CImageFormatFactory::GetImageHandleByFileName(const yl::string& strFileName)
{
    yl::string strSuffixName = GetSuffixByFileName(strFileName);

    if (strSuffixName.empty())
    {
        return NULL;
    }

    MAP_CREATE_IMAGE_FORMAT_HANDLE_FUN::iterator it = m_mapCreateImageFormat.find(strSuffixName);

    if (it != m_mapCreateImageFormat.end())
    {
        return it->second;
    }

    return NULL;
}

CImageFormatHandleBase* CImageFormatFactory::GetImageHandleByFileHead(FILE* pFile,
        CImageFormatHandleBase* pExcludeHandle)
{
    MAP_CREATE_IMAGE_FORMAT_HANDLE_FUN::iterator iter = m_mapCreateImageFormat.begin();
    CImageFormatHandleBase* pFitHandle = NULL;

    for (; iter != m_mapCreateImageFormat.end(); ++iter)
    {
        CImageFormatHandleBase* pHandle = iter->second;

        if (NULL != pHandle && pExcludeHandle != pHandle
                && pHandle->IsFileHandleCorrect(pFile))
        {
            return pHandle;
        }
    }

    return NULL;
}

CImageFormatHandleBase* CImageFormatFactory::GetFitSaveImageFormatHandle(const yl::string& strFileName,
        const yl::string& strFormat /* = */)
{
    CImageFormatHandleBase* pHandle = NULL;

    if (!strFormat.empty())
    {
        pHandle = GetImageHandleByFileName(strFormat);

        if (NULL == pHandle)
        {
            XWINDOW_INFO("Not support this format, format[%s]", strFormat.c_str());
        }

        return pHandle;
    }

    pHandle = GetImageHandleByFileName(strFileName);

    if (NULL == pHandle)
    {
        pHandle = GetImageHandleByFileName(kszDefaultFormat);
    }

    return pHandle;
}

yl::string CImageFormatFactory::GetSuffixByFileName(const yl::string& strFileName)
{
    //查找最后一个.字符
    yl::string strResult = "";
    yl::string::size_type nSuffixBegin = strFileName.find_last_of('.');
    if (yl::string::npos != nSuffixBegin)
    {
        strResult = strFileName.substr(nSuffixBegin, strFileName.length() - nSuffixBegin);
    }

    return strResult;
}

bool CImageFormatFactory::GetImageSize(const yl::string&strFileName, chSize& szImage)
{
    FILE* pFile = fopen(strFileName.c_str(), "rb");

    if (NULL == pFile)
    {
        return false;
    }

    CImageFormatHandleBase* pHandle = GetImageHandleByFileName(strFileName);
    bool bResult = false;

    if (NULL == pHandle || !pHandle->IsFileHandleCorrect(pFile))
    {
        pHandle = GetImageHandleByFileHead(pFile, pHandle);
    }

    if (NULL != pHandle)
    {
        rewind(pFile);

        bResult = pHandle->GetImageSize(pFile, szImage);
    }

    fclose(pFile);

    return bResult;
}
#endif
