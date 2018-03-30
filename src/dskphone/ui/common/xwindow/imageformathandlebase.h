#ifndef __IMAGE_FORMAT_HANDLE_BASE_H__
#define __IMAGE_FORMAT_HANDLE_BASE_H__

#include "ylstringalgorithm.h"
#include "ylalgorithm.h"
#include "commonapi/stringhelperapi.h"
#include "xGraphic.h"
#define MAKEUI(a,b,c,d) ((UINT) ( ((UINT)(a)) << 24 | ((UINT)(b)) << 16 | ((UINT)(c)) << 8 | ((UINT)(d)) ))

#if !IF_ONLY_USE_GRADATION
class CImageFormatHandleBase
{
public:
    explicit CImageFormatHandleBase();
    virtual ~CImageFormatHandleBase();

public:
    virtual bool LoadFile(FILE* pFile, xPixmap& pixmap, int& nErrorCode)      = 0;
    virtual bool SaveFile(const yl::string& strFileName, xGraphic* pGraphic, int& nErrorCode)   = 0;
    virtual bool IsFileHandleCorrect(FILE* pFile)                                               = 0;
    virtual int GetSaveFormatBitDepth()                                                         = 0;
    virtual bool GetImageSize(FILE* pFile, chSize& szImage)                                     = 0;
};
#endif

#endif
