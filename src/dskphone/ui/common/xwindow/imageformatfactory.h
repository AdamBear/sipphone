#ifndef _IMAGE_FORMAT_FACTORY_H__
#define _IMAGE_FORMAT_FACTORY_H__

#include "imageformathandlebase.h"
#include <ylhashmap.h>

#if !IF_ONLY_USE_GRADATION
typedef CImageFormatHandleBase* (*FUN_CREATE_IMAGE_FORMAT_HANDLE)();
typedef YLHashMap<yl::string, CImageFormatHandleBase*> MAP_CREATE_IMAGE_FORMAT_HANDLE_FUN;

class CImageFormatFactory
{
public:
    CImageFormatFactory();
    ~CImageFormatFactory();

    static CImageFormatFactory& GetInstance();

public:
    void RegisterImageFormatFactory(const yl::string& strFileSuffixName, FUN_CREATE_IMAGE_FORMAT_HANDLE pFunCreateImageFormat);
    bool GetImageSize(const yl::string&strFileName, chSize& szImage);

    CImageFormatHandleBase* GetImageHandleByFileName(const yl::string& strFileName);
    CImageFormatHandleBase* GetImageHandleByFileHead(FILE* pFile, CImageFormatHandleBase* pExcludeHandle);

    CImageFormatHandleBase* GetFitSaveImageFormatHandle(const yl::string& strFileName, const yl::string& strFormat = "");

public:
    yl::string GetSuffixByFileName(const yl::string& strFileName);

protected:
    MAP_CREATE_IMAGE_FORMAT_HANDLE_FUN m_mapCreateImageFormat;
};

#define g_ImageFormatFactory CImageFormatFactory::GetInstance()
#endif

#endif //_IMAGE_FORMAT_FACTORY_H__
