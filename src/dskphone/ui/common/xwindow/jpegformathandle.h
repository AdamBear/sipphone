#ifndef __JPEG_FORMAT_HANDLE_H__
#define __JPEG_FORMAT_HANDLE_H__

#include "imageformatdefine.h"

#if IF_SUPPORT_JPEG

#include "imageformathandlebase.h"
#include <setjmp.h>

class CJPEGFormatHandle : public CImageFormatHandleBase
{
public:
    explicit CJPEGFormatHandle();
    ~CJPEGFormatHandle();
    static CImageFormatHandleBase* CreateJPEGFormatHandle();
public:
    virtual bool LoadFile(FILE* pFile, xPixmap& pixmap, int& nErrorCode);
    virtual bool SaveFile(const yl::string& strFileName, xGraphic* pGraphic, int& nErrorCode);
    virtual bool IsFileHandleCorrect(FILE* pFile);
    virtual int GetSaveFormatBitDepth();
    virtual bool GetImageSize(FILE* pFile, chSize& szImage);
};

#endif //IF_SUPPORT_JPEG
#endif  //__JPEG_FORMAT_HANDLE_H__
