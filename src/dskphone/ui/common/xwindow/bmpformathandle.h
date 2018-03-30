#ifndef __BMP_FORMAT_HANDLE_H__
#define __BMP_FORMAT_HANDLE_H__

#include "imageformatdefine.h"
#if IF_SUPPORT_BMP

#include "imageformathandlebase.h"

typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned int    U32;

//调色板信息
typedef struct
{
    U8 rgbBlue;
    U8 rgbGreen;
    U8 rgbRed;
    U8 rgbReserved;
} BMP_RGB_QUAD;

//文件头
typedef struct
{
    U32 file_size;
    U32 reserved;
    U32 bm_data_offset;
} BM_FILE_HEADER;

//文件信息头部
typedef struct
{
    U32 bm_header_size;//结构体长度
    U32 width;//宽
    U32 height;//高
    U16 planes;//平面数
    U16 bits_per_pixel;//采用的颜色位数/位深度
    U32 compression; //压缩方式
    U32 bitmap_data_size; //实际位图占用的字节数
    U32 hresolution; // X分辨率
    U32 vresolution;// Y分辨率
    U32 colors;//使用颜色数
    U32 important_colors;//重要颜色数
} BM_INFORMATION_HEADER;

//压缩方式
enum BI_COMPRESSION
{
    BI_COMPRESSION_RGB = 0,
    BI_COMPRESSION_RLE8,
    BI_COMPRESSION_RLE4,
    BI_COMPRESSION_BITFIELDS,
};

class CBMPFormatHandle : public CImageFormatHandleBase
{
public:
    explicit CBMPFormatHandle();
    virtual ~CBMPFormatHandle();
    static CImageFormatHandleBase* CreateBMPFormatHandle();

public:
    virtual bool LoadFile(FILE* pFile, xPixmap& pixmap, int& nErrorCode);
    virtual bool SaveFile(const yl::string& strFileName, xGraphic* pGraphic, int& nErrorCode);
    virtual bool IsFileHandleCorrect(FILE* pFile);
    virtual int GetSaveFormatBitDepth();
    virtual bool GetImageSize(FILE* pFile, chSize& szImage);

protected:
    bool ReadPaletteBmpData(FILE* pFile, xPixmap& pixmap,
                            int& nErrorCode, int nDataOffset,
                            const BM_INFORMATION_HEADER& infoHeader);
    bool Read24Or32BmpData(FILE* pFile, xPixmap& pixmap,
                           int& nErrorCode, int nDataOffset,
                           const BM_INFORMATION_HEADER& infoHeader);
    bool Read16BmpData(FILE* pFile, xPixmap& pixmap,
                       int& nErrorCode, int nDataOffset,
                       const BM_INFORMATION_HEADER& infoHeader);
};

#endif //IF_SUPPORT_BMP
#endif //__BMP_FORMAT_HANDLE_H__
