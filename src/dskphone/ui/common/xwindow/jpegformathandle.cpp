#include "jpegformathandle.h"
#include "dsklog/log.h"

#if IF_SUPPORT_JPEG
#include "xGraphic.h"
#include "xgraphicproess.h"

extern "C" {
#include <jpeg/jpeglib.h>
}

struct my_error_mgr
{
    struct jpeg_error_mgr pub;  /* "public" fields */
    jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

CJPEGFormatHandle::CJPEGFormatHandle()
{

}

CJPEGFormatHandle::~CJPEGFormatHandle()
{

}

CImageFormatHandleBase* CJPEGFormatHandle::CreateJPEGFormatHandle()
{
    CJPEGFormatHandle* pHandle = new CJPEGFormatHandle;
    return pHandle;
}

void my_error_exit(j_common_ptr cinfo)
{
    if (NULL == cinfo)
    {
        return ;
    }

    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    if (NULL == myerr)
    {
        return;
    }

    (*cinfo->err->output_message)(cinfo);

    longjmp(myerr->setjmp_buffer, 1);
}

bool CJPEGFormatHandle::LoadFile(FILE* pFile, xPixmap& pixmap, int& nErrorCode)
{
    if (NULL == pFile)
    {
        return false;
    }

    rewind(pFile);

    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPARRAY buffer;
    int row_stride = 0;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, pFile);

    (void) jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_RGB;  // 设置输出格式
    cinfo.output_components = 3;

    (void) jpeg_start_decompress(&cinfo);

    //创建Graphic区域
    if (!pixmap.Create(cinfo.output_width, cinfo.output_height, GRAPHIC_FORMAT_RGB24))
    {
        (void)jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        return false;
    }

    xGraphicPtr pGraphic = pixmap.GetGraphic();

    if (pGraphic.IsEmpty())
    {
        (void)jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    BYTE* pPixel = pGraphic->pixels();

    if (NULL == pPixel)
    {
        (void)jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        return false;
    }

    int nLineDataLen = xGraphicOperate::getDataLength(GRAPHIC_FORMAT_RGB24, cinfo.output_width);

#if IF_IMAGE_FORMAT_DEBUG
    XWINDOW_INFO("CJPEGFormatHandle::ReadFileInfo, Width[%d], Height[%d], Bpp[%d]", cinfo.output_width, cinfo.output_height, cinfo.output_components);
#endif

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    int iLine = 0;
    /* 4.Process data由左上角从上到下行行扫描 */
    while (cinfo.output_scanline < cinfo.output_height)
    {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);

        memcpy(pPixel, buffer[0], nLineDataLen);

        pPixel += nLineDataLen;
        ++iLine;
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    pGraphic->setAlphaFlag(xGraphic::GRAPHIC_FLAG_OPACITY);
#endif

    pixmap.convertFormat(GRAPHIC_FORMAT_BGRA32);

    return true;
}

bool CJPEGFormatHandle::SaveFile(const yl::string& strFileName, xGraphic* pGraphic, int& nErrorCode)
{
    if (NULL == pGraphic || pGraphic->GetFormat() != GetSaveFormatBitDepth())
    {
        return false;
    }

    FILE * outfile = fopen(strFileName.c_str(), "wb");
    if (NULL == outfile)
    {
        XWINDOW_INFO("Can Not Open File[%s]", strFileName.c_str());
        return false;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    int nWidth = pGraphic->width();
    int nHeight = pGraphic->height();

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = nWidth;
    cinfo.image_height = nHeight;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = nWidth * 3;
    unsigned char* pData = (unsigned char*)pGraphic->pixels();

#if IF_IMAGE_FORMAT_DEBUG
    for (int i = 0; i < nHeight * nWidth * 3; i += 3)
    {
        XWINDOW_INFO("Check Data, [%0x][%0x][%0x]", pData[i + 0], pData[i + 1], pData[i + 2]);
    }
#endif

    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = (pData + cinfo.next_scanline * row_stride);
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    fclose(outfile);

    jpeg_destroy_compress(&cinfo);

    return true;
}

bool CJPEGFormatHandle::IsFileHandleCorrect(FILE* pFile)
{
    if (NULL == pFile || 0 != fseek(pFile, 0, SEEK_SET))
    {
        return false;
    }

    BYTE szFileFlag[2];

    if (2 != fread(szFileFlag, 1, 2, pFile))
    {
        return false;
    }

    if (szFileFlag[0] != 0xff || szFileFlag[1] != 0xd8)
    {
        return false;
    }

    return true;
}

int CJPEGFormatHandle::GetSaveFormatBitDepth()
{
    return GRAPHIC_FORMAT_RGB24;
}

bool CJPEGFormatHandle::GetImageSize(FILE* pFile, chSize& szImage)
{
    if (NULL == pFile)
    {
        return false;
    }

    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPARRAY buffer;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);

        return false;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, pFile);

    (void)jpeg_read_header(&cinfo, TRUE);

    szImage.cx = cinfo.image_width;
    szImage.cy = cinfo.image_height;

    jpeg_destroy_decompress(&cinfo);

    return true;
}

#endif //#if IF_SUPPORT_JPEG
