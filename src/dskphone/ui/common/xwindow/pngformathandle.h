#ifndef __PNG_FORMAT_HANDLE_H__
#define __PNG_FORMAT_HANDLE_H__

#include "imageformatdefine.h"

#if IF_SUPPORT_PNG

#include "imageformathandlebase.h"
#include <setjmp.h>

class CPNGFormatHandle : public CImageFormatHandleBase
{
public:
    explicit CPNGFormatHandle();
    ~CPNGFormatHandle();
    static CImageFormatHandleBase* CreatePNGFormatHandle();
public:
    virtual bool LoadFile(FILE* pFile, xPixmap& pixmap, int& nErrorCode);
    virtual bool SaveFile(const yl::string& strFileName, xGraphic* pGraphic, int& nErrorCode);
    virtual bool IsFileHandleCorrect(FILE* pFile);
    virtual int GetSaveFormatBitDepth();
    virtual bool GetImageSize(FILE* pFile, chSize& szImage);
protected:
    bool AddInfoToPixmap(xPixmap& pixmap, unsigned char** pData, int nBytesPerPixel, int nWidth, int nHeight);
};

#endif // #if IF_SUPPORT_PNG
#endif //__PNG_FORMAT_HANDLE_H__
