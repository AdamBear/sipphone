#include "pngformathandle.h"
#include "dsklog/log.h"

#if IF_SUPPORT_PNG
#include "xGraphic.h"
#include "xgraphicproess.h"

extern "C" {
#include <png/png.h>
}

namespace
{
#define PNG_IDENTITY_NUM 4
}

CPNGFormatHandle::CPNGFormatHandle()
{

}

CPNGFormatHandle::~CPNGFormatHandle()
{

}

CImageFormatHandleBase* CPNGFormatHandle::CreatePNGFormatHandle()
{
    CPNGFormatHandle* pHandle = new CPNGFormatHandle;
    return pHandle;
}

bool CPNGFormatHandle::LoadFile(FILE* pFile, xPixmap& pixmap, int& nErrorCode)
{
    if (NULL == pFile)
    {
        return false;
    }

    rewind(pFile);

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (NULL == png_ptr)
    {
        XWINDOW_INFO("CPNGFormatHandle, create read struct error file");
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (NULL == info_ptr)
    {
        XWINDOW_INFO("CPNGFormatHandle, create info struct error file");
        png_destroy_read_struct(&png_ptr, 0, 0);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        return false;
    }

    png_init_io(png_ptr, pFile);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

    /*获取宽度，高度，位深，颜色类型*/
    int channels = png_get_channels(png_ptr, info_ptr); /*获取通道数*/
    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr); /* 获取位深 */
    int color_type = png_get_color_type(png_ptr, info_ptr); /*颜色类型*/
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
#if IF_IMAGE_FORMAT_DEBUG
    XWINDOW_INFO("CPNGFormatHandle::LoadFile, channel[%d], width[%d], height[%d], bit_depth[%d], color[%d]\n",
                 channels, width, height, bit_depth, color_type);
#endif
    int nImageFormat = 0;
    if (4 == channels || PNG_COLOR_TYPE_RGB_ALPHA == color_type)
    {
        nImageFormat = GRAPHIC_FORMAT_BGRA32;
    }
    else if (3 == channels || PNG_COLOR_TYPE_RGB == color_type)
    {
        nImageFormat = GRAPHIC_FORMAT_BGR24;
    }

    bool bResult = AddInfoToPixmap(pixmap, row_pointers, nImageFormat, width, height);

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    return bResult;
}

bool CPNGFormatHandle::AddInfoToPixmap(xPixmap& pixmap, unsigned char** pData, int nImageFormat, int nWidth, int nHeight)
{
    if ((GRAPHIC_FORMAT_BGR24 != nImageFormat && GRAPHIC_FORMAT_BGRA32 != nImageFormat)
            || nWidth <= 0 || nHeight <= 0)
    {
        return false;
    }

    if (!pixmap.Create(nWidth, nHeight, GRAPHIC_FORMAT_BGRA32))
    {
        return false;
    }

    xGraphicPtr pGraphic = pixmap.GetGraphic();
    if (pGraphic.IsEmpty())
    {
        return false;
    }

    BYTE* pPixel = pGraphic->pixels();

    if (NULL == pPixel)
    {
        return false;
    }

    int nBytesPerPixel = (GRAPHIC_FORMAT_BGR24 == nImageFormat) ? 3 : 4;
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    bool bFullFullTransparent = true;
    bool bOpacity = true;
#endif

    for (int i = 0; i < nHeight; i++)
    {
        for (int j = 0; j < nWidth; j++)
        {
            int nIndex = i * nWidth + j;
            xPixel* pixDst = (xPixel*)pPixel;

            pixDst[nIndex].r = pData[i][nBytesPerPixel * j + 0];
            pixDst[nIndex].g = pData[i][nBytesPerPixel * j + 1];
            pixDst[nIndex].b = pData[i][nBytesPerPixel * j + 2];

            BYTE byAlpha = pixDst[nIndex].a = (4 == nBytesPerPixel) ? pData[i][nBytesPerPixel * j + 3] : 0xff;

            pixDst[nIndex].a = byAlpha;

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
            if (0xff != byAlpha)
            {
                bOpacity = false;

                if (0x00 != byAlpha)
                {
                    bFullFullTransparent = false;
                }
            }
#endif
        }
    }

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    pGraphic->setAlphaFlag(bOpacity, bFullFullTransparent);
#endif

    return true;
}

bool CPNGFormatHandle::SaveFile(const yl::string& strFileName, xGraphic* pGraphic, int& nErrorCode)
{
    if (NULL == pGraphic)
    {
        return false;
    }

    FILE* pFile = fopen(strFileName.c_str(), "wb");
    if (NULL == pFile)
    {
        XWINDOW_ERR("Can not open file[%s]", strFileName.c_str());
        return false;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (NULL == png_ptr)
    {
        XWINDOW_INFO("CPNGFormatHandle, create read struct error file[%s]", strFileName.c_str());
        fclose(pFile);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (NULL == info_ptr)
    {
        fclose(pFile);
        png_destroy_write_struct(&png_ptr, 0);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(pFile);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    png_init_io(png_ptr, pFile);

    int nWidth = pGraphic->width();
    int nHeight = pGraphic->height();
    int bit_depth = 8;
    png_set_IHDR(png_ptr, info_ptr, nWidth, nHeight, bit_depth, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_packing(png_ptr);//设置打包信息
    png_write_info(png_ptr, info_ptr); //写入文件头
    png_bytep pData = (png_bytep)pGraphic->pixels();
    png_bytep* pRowPointers = new png_bytep[nHeight];

    if (nHeight > PNG_UINT_32_MAX / sizeof(png_bytep))
    {
        png_error(png_ptr, "Image is too tall to process in memory");
    }

    for (int k = 0; k < nHeight; k++)
    {
        pRowPointers[k] = pData + k * nWidth * 4;
    }

    png_write_image(png_ptr, pRowPointers);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(pFile);

    SAFE_DELETE_ARRAY(pRowPointers)

    return true;
}

bool CPNGFormatHandle::IsFileHandleCorrect(FILE* pFile)
{
    if (NULL == pFile || 0 != fseek(pFile, 0, SEEK_SET))
    {
        return false;
    }

    char buf[PNG_IDENTITY_NUM];

    if (fread(buf, 1, PNG_IDENTITY_NUM, pFile) != PNG_IDENTITY_NUM)
    {
        return false;
    }

    bool bResult = !png_sig_cmp((png_bytep)buf, 0, PNG_IDENTITY_NUM);

    return bResult;
}

int CPNGFormatHandle::GetSaveFormatBitDepth()
{
    return GRAPHIC_FORMAT_RGBA32;
}

bool CPNGFormatHandle::GetImageSize(FILE* pFile, chSize& szImage)
{
    if (NULL == pFile)
    {
        return false;
    }

    unsigned char arrData[24] = { 0 };

    //读取宽高数据，png的宽在头的第16字节，高在第20字节，各占四字节
    if (fread(arrData, 1, sizeof(arrData), pFile) != sizeof(arrData))
    {
        return false;
    }

    szImage.cx = MAKEUI(arrData[16], arrData[17], arrData[18], arrData[19]);
    szImage.cy = MAKEUI(arrData[20], arrData[21], arrData[22], arrData[23]);

    return true;
}

#endif
