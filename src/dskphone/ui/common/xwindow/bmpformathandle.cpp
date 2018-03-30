#include "bmpformathandle.h"
#include "dsklog/log.h"

#if IF_SUPPORT_BMP

#include "xGraphic.h"

namespace
{
#define BPP1 (1)
#define BPP4 (4)
#define BPP8 (8)
#define BPP16 (16)
#define BPP24 (24)
#define BPP32 (32)
#define FILE_HEADER_SZIE 14
}

CBMPFormatHandle::CBMPFormatHandle()
{

}

CBMPFormatHandle::~CBMPFormatHandle()
{

}

CImageFormatHandleBase* CBMPFormatHandle::CreateBMPFormatHandle()
{
    CBMPFormatHandle* pHandle = new CBMPFormatHandle;
    return pHandle;
}

bool CBMPFormatHandle::LoadFile(FILE* pFile, xPixmap& pixmap, int& nErrorCode)
{
    if (NULL == pFile)
    {
        return false;
    }

    rewind(pFile);

    BM_FILE_HEADER fileHeader;

    if (fread(&fileHeader, 1, sizeof(fileHeader), pFile) != sizeof(fileHeader))
    {
        return false;
    }

    BM_INFORMATION_HEADER infoHeader;

    if (fread(&infoHeader, 1, sizeof(infoHeader), pFile) != sizeof(infoHeader))
    {
        return false;
    }

    if (0 == infoHeader.width || 0 == infoHeader.height)
    {
        return false;
    }

    if (!pixmap.Create(infoHeader.width, infoHeader.height, GRAPHIC_FORMAT_BGRA32))
    {
        return false;
    }

    bool bResult = false;
    if (BPP1 == infoHeader.bits_per_pixel
            || BPP4 == infoHeader.bits_per_pixel
            || BPP8 == infoHeader.bits_per_pixel)
    {
        //读取调色板
        return ReadPaletteBmpData(pFile, pixmap, nErrorCode, fileHeader.bm_data_offset, infoHeader);
    }
    else if (BPP16 == infoHeader.bits_per_pixel)
    {
        return Read16BmpData(pFile, pixmap, nErrorCode, fileHeader.bm_data_offset, infoHeader);
    }
    else if (BPP24 == infoHeader.bits_per_pixel || BPP32 == infoHeader.bits_per_pixel)
    {
        return Read24Or32BmpData(pFile, pixmap, nErrorCode, fileHeader.bm_data_offset, infoHeader);
    }

    return false;
}

bool CBMPFormatHandle::ReadPaletteBmpData(FILE* pFile, xPixmap& pixmap,
        int& nErrorCode, int nDataOffset,
        const BM_INFORMATION_HEADER& infoHeader)
{
    xGraphicPtr pGraphic = pixmap.GetGraphic();

    if (NULL == pFile
            || pGraphic.IsEmpty()
            || infoHeader.width != pGraphic->width()
            || infoHeader.height != pGraphic->height())
    {
        return false;
    }

    xPixel* pPixDst = (xPixel*)(pGraphic->pixels());

    if (NULL == pPixDst)
    {
        return false;
    }

    int nLineByteCnt = ((31 + (infoHeader.width * infoHeader.bits_per_pixel)) >> 5) << 2;
    int nPalSize = (nDataOffset - infoHeader.bm_header_size - FILE_HEADER_SZIE) >> 2;
    if (nPalSize < 0)
    {
        return false;
    }

    BMP_RGB_QUAD* pPalette = new BMP_RGB_QUAD[nPalSize];
    int nTotalSize = sizeof(BMP_RGB_QUAD) * nPalSize;

    if (fread(pPalette, 1, nTotalSize, pFile) != nTotalSize)
    {
        SAFE_DELETE_ARRAY(pPalette)
        return false;
    }

    if (0 != fseek(pFile, nDataOffset, SEEK_SET))
    {
        SAFE_DELETE_ARRAY(pPalette)
        return false;
    }

    // 1个字节表示的位数
    int nPixelPerByte = 1;

    if (BPP1 == infoHeader.bits_per_pixel)
    {
        nPixelPerByte = 8;
    }
    else if (BPP4 == infoHeader.bits_per_pixel)
    {
        nPixelPerByte = 2;
    }

    BYTE* pLineByte = new BYTE[nLineByteCnt];
    BYTE* pPixel = new BYTE[nPixelPerByte];

    for (int i = 0; i < infoHeader.height; i++)
    {
        if (nLineByteCnt != fread(pLineByte, 1, nLineByteCnt, pFile))
        {
            SAFE_DELETE_ARRAY(pPalette)
            SAFE_DELETE_ARRAY(pLineByte)
            SAFE_DELETE_ARRAY(pPixel)

            return false;
        }

        //BMP图片扫描顺序是从下往上
        int nRealLine = infoHeader.height - i - 1;
        int nPixelIndex = 0;
        for (int j = 0; j < nLineByteCnt; ++j)
        {
            if (8 == nPixelPerByte)
            {
                pPixel[0] = pLineByte[j] >> 7 & 0x1;
                pPixel[1] = pLineByte[j] >> 6 & 0x1;
                pPixel[2] = pLineByte[j] >> 5 & 0x1;
                pPixel[3] = pLineByte[j] >> 4 & 0x1;
                pPixel[4] = pLineByte[j] >> 3 & 0x1;
                pPixel[5] = pLineByte[j] >> 2 & 0x1;
                pPixel[6] = pLineByte[j] >> 1 & 0x1;
                pPixel[7] = pLineByte[j]  & 0x1;
            }
            else if (2 == nPixelPerByte)
            {
                pPixel[0] = pLineByte[j] >> 4 & 0xf;
                pPixel[1] = pLineByte[j] & 0xf;
            }
            else
            {
                pPixel[0] = pLineByte[j];
            }

            for (int k = 0; k < nPixelPerByte; ++k)
            {
                int nIndex = nRealLine * infoHeader.width + nPixelIndex;
                int nPalIndex = pLineByte[k];

                pPixDst[nIndex].r = pPalette[nPalIndex].rgbRed;
                pPixDst[nIndex].g = pPalette[nPalIndex].rgbGreen;
                pPixDst[nIndex].b = pPalette[nPalIndex].rgbBlue;
                pPixDst[nIndex].a = 0xff;
                nPixelIndex++;

                if (nPixelIndex >= infoHeader.width)
                {
                    break;
                }
            }

            if (nPixelIndex >= infoHeader.width)
            {
                break;
            }
        }
    }

    SAFE_DELETE_ARRAY(pPalette)
    SAFE_DELETE_ARRAY(pLineByte)
    SAFE_DELETE_ARRAY(pPixel)

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    pGraphic->setAlphaFlag(xGraphic::GRAPHIC_FLAG_OPACITY);
#endif

    return true;
}

bool CBMPFormatHandle::Read24Or32BmpData(FILE* pFile, xPixmap& pixmap,
        int& nErrorCode, int nDataOffset,
        const BM_INFORMATION_HEADER& infoHeader)
{
    xGraphicPtr pGraphic = pixmap.GetGraphic();
    if (NULL == pFile
            || pGraphic.IsEmpty()
            || infoHeader.width != pGraphic->width()
            || infoHeader.height != pGraphic->height())
    {
        return false;
    }

    xPixel* pPixDst = (xPixel*)(pGraphic->pixels());
    if (NULL == pPixDst)
    {
        return false;
    }

    int nLineByteCnt = ((31 + (infoHeader.width * infoHeader.bits_per_pixel)) >> 5) << 2;
    bool bLoad24Bmp = (BPP24 == infoHeader.bits_per_pixel) ? true : false;
    int nBytePerPixel = bLoad24Bmp ? 3 : 4;

#if IF_IMAGE_FORMAT_DEBUG
    XWINDOW_INFO("CBMPFormatHandle::Read24Or32BmpData, bLoad24Bmp[%d], Width[%d], Height[%d], iLineByteCnt[%d]"
                 , bLoad24Bmp, infoHeader.width, infoHeader.height, iLineByteCnt);
#endif

    BYTE* pLineByte = new BYTE[nLineByteCnt];

    for (int i = 0; i < infoHeader.height; i++)
    {
        if (nLineByteCnt != fread(pLineByte, 1, nLineByteCnt, pFile))
        {
            SAFE_DELETE_ARRAY(pLineByte)

            return false;
        }

        int nRealLine = infoHeader.height - i - 1;
        int nPixelIndex = 0;
        int nPixelBegin = 0;
        for (int j = 0; j < nLineByteCnt; ++j)
        {
            int nIndex = nPixelBegin % nBytePerPixel;

            if (0 == nIndex)
            {
                pPixDst[nRealLine * infoHeader.width + nPixelIndex].b = pLineByte[j];
            }
            else if (1 == nIndex)
            {
                pPixDst[nRealLine * infoHeader.width + nPixelIndex].g = pLineByte[j];
            }
            else if (2 == nIndex)
            {
                pPixDst[nRealLine * infoHeader.width + nPixelIndex].r = pLineByte[j];

                if (bLoad24Bmp)
                {
                    pPixDst[nRealLine * infoHeader.width + nPixelIndex].a = 0xff;
                    nPixelIndex++;
                }
            }
            else if (3 == nIndex)
            {
                pPixDst[nRealLine * infoHeader.width + nPixelIndex].a  = pLineByte[j];
                nPixelIndex++;
            }

            nPixelBegin++;

            if (nPixelIndex >= infoHeader.width)
            {
                break;
            }
        }
    }

    SAFE_DELETE_ARRAY(pLineByte)

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    pGraphic->setAlphaFlag(xGraphic::GRAPHIC_FLAG_OPACITY);
#endif

    return true;
}

bool CBMPFormatHandle::Read16BmpData(FILE* pFile, xPixmap& pixmap,
                                     int& nErrorCode, int nDataOffset,
                                     const BM_INFORMATION_HEADER& infoHeader)
{
    xGraphicPtr pGraphic = pixmap.GetGraphic();
    if (NULL == pFile
            || pGraphic.IsEmpty()
            || infoHeader.width != pGraphic->width()
            || infoHeader.height != pGraphic->height()
            || (BI_COMPRESSION_RGB != infoHeader.compression && BI_COMPRESSION_BITFIELDS != infoHeader.compression))
    {
        return false;
    }

    xPixel* pPixDst = (xPixel*)(pGraphic->pixels());
    if (NULL == pPixDst)
    {
        return false;
    }

    int nLineByteCnt = ((31 + (infoHeader.width * infoHeader.bits_per_pixel)) >> 5) << 2;

#if IF_IMAGE_FORMAT_DEBUG
    XWINDOW_INFO("CBMPFormatHandle::Read16BmpData, Width[%d], Height[%d], iLineByteCnt[%d]", infoHeader.width, infoHeader.height, iLineByteCnt);
#endif

    int nMaskRed = 0;
    if (BI_COMPRESSION_BITFIELDS == infoHeader.compression)
    {
        if (4 != fread(&nMaskRed, 1, 4, pFile))
        {
            return false;
        }
    }

    if (0 != fseek(pFile, nDataOffset, SEEK_SET))
    {
        return false;
    }

    BYTE* pLineByte = new BYTE[nLineByteCnt];
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    bool bFullFullTransparent = true;
    bool bOpacity = true;
#endif

    for (int i = 0; i < infoHeader.height; ++i)
    {
        if (nLineByteCnt != fread(pLineByte, 1, nLineByteCnt, pFile))
        {
            SAFE_DELETE_ARRAY(pLineByte)
            return false;
        }

        int nRealLine = infoHeader.height - i - 1;

        int nPixelIndex = 0;
        int nPixelBegin = 0;
        for (int j = 0; j + 1 < nLineByteCnt; j += 2)
        {
            char cByteLow = pLineByte[j];//低
            char cByteHigh = pLineByte[j + 1];//高
            int nValue = cByteHigh << 8 | cByteLow;
            int nIndex = nRealLine * infoHeader.width + nPixelIndex;
            xPixel& pixDst = pPixDst[nIndex];

            if (BI_COMPRESSION_RGB == infoHeader.compression)
            {
                pixDst.b = (nValue << 3) & 0xFF;
                pixDst.g = (nValue >> 5 << 3) & 0xFF;
                pixDst.r = (nValue >> 10 << 3) & 0xFF;

                bool bTransparency = (1 == ((nValue >> 15) & 0x1));

                if (bTransparency)
                {
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
                    bOpacity = false;
#endif
                    pixDst.a = 0x00;
                }
                else
                {
                    pixDst.a = 0xFF;
                }
            }
            else if (0xF800 == nMaskRed)
            {
                //565
                pixDst.b = (nValue << 3) & 0xFF;
                pixDst.g = ((nValue & 0x07E0) >> 5 << 2) & 0xFF;
                pixDst.r = ((nValue & 0xF800) >> 11 << 3) & 0xFF;
                pixDst.a = 0xFF;
            }
            else if (0x7C00 == nMaskRed)
            {
                //555
                pixDst.b = (nValue << 3) & 0xFF;
                pixDst.g = ((nValue & 0x03E0) >> 5 << 3) & 0xFF;
                pixDst.r = ((nValue & 0x7C00) >> 10 << 3) & 0xFF;

                bool bTransparency = (1 == (((nValue & 0x8000) >> 15) & 0x1));

                if (bTransparency)
                {
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
                    bOpacity = false;
#endif
                    pixDst.a = 0x00;
                }
                else
                {
                    pixDst.a  = 0xFF;
                }
            }
            else if (0x0F00 == nMaskRed)
            {
                //444
                pixDst.b = ((nValue & 0x000F) << 4) & 0xFF;
                pixDst.g = (nValue & 0x00F0);
                pixDst.r = ((nValue & 0x0F00) >> 8 << 4) & 0xFF;
                pixDst.a = ((nValue & 0x8000) >> 12 << 4) & 0xFF;

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
                if (0xFF != pixDst.a)
                {
                    bOpacity = false;

                    if (0x00 != pixDst.a)
                    {
                        bFullFullTransparent = false;
                    }
                }
#endif
            }
            else
            {
                SAFE_DELETE_ARRAY(pLineByte)

                return false;
            }

#if IF_IMAGE_FORMAT_DEBUG
            XWINDOW_INFO("CBMPFormatHandle::Read16BmpData::RGC555, nByteIndex[%d], nPixelIndex[%d], nLine[%d], Low[%0x], high[%0x], Value[%0x], Blue:[%0x], Green[%0x], Red[%0x], X[%0x]"
                         , j, nPixelIndex, i, cByteLow, cByteHigh, nValue, pixDst[nIndex].b, pixDst[nIndex].g
                         , pixDst[nIndex].r, pixDst[nIndex].a);
#endif

            nPixelIndex++;
            if (nPixelIndex >= infoHeader.width)
            {
                break;
            }
        }
    }

    SAFE_DELETE_ARRAY(pLineByte)

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    pGraphic->setAlphaFlag(bOpacity, bFullFullTransparent);
#endif

    return true;
}

bool CBMPFormatHandle::SaveFile(const yl::string& strFileName, xGraphic* pGraphic, int& nErrorCode)
{
    if (NULL == pGraphic || pGraphic->GetFormat() != GetSaveFormatBitDepth())
    {
        return false;
    }

    FILE* pFile = fopen(strFileName.c_str(), "wb");
    if (NULL == pFile)
    {
        XWINDOW_ERR("Can Not Open File[%s]", strFileName.c_str());
        return false;
    }

    int nWidth = pGraphic->width();
    int nHeight = pGraphic->height();
    int iLineByteCnt = ((31 + (nWidth * 24)) >> 5) << 2;

    char identifier[2] = {0x42, 0x4d};

    BM_FILE_HEADER fileHeader;
    fileHeader.file_size = iLineByteCnt * nHeight + 0x36;
    fileHeader.bm_data_offset = 0x36;
    fileHeader.reserved = 0;

    BM_INFORMATION_HEADER infoHeader;
    infoHeader.bm_header_size = 0x28;
    infoHeader.width = nWidth;
    infoHeader.height = nHeight;
    infoHeader.planes = 1;
    infoHeader.bits_per_pixel = 0x18;
    infoHeader.compression = BI_COMPRESSION_RGB;
    infoHeader.bitmap_data_size = iLineByteCnt * nHeight;
    infoHeader.hresolution = 0x0;
    infoHeader.vresolution = 0x0;
    infoHeader.colors = 0x0;
    infoHeader.important_colors = 0x0;

    if (fwrite(identifier, 1, 2, pFile) != 2
            || fwrite(&fileHeader, 1, 12, pFile) != (size_t)12)
    {
        XWINDOW_ERR("Write BmpFileHeader error");
        fclose(pFile);
        return false;
    }

    if (fwrite(&infoHeader, 1, 40, pFile) != (size_t)40)
    {
        XWINDOW_ERR("Write BmpInfoHeader Error");
        fclose(pFile);
        return false;
    }

    unsigned char* pData = (unsigned char*)pGraphic->pixels();
    if (NULL == pData)
    {
        fclose(pFile);
        return false;
    }

#if IF_IMAGE_FORMAT_DEBUG
    for (int i = 0; i < nHeight * nWidth * 3; i += 3)
    {
        XWINDOW_INFO("Check Data, [%0x][%0x][%0x]", pData[i + 0], pData[i + 1], pData[i + 2]);
    }
#endif

    for (int i = nHeight - 1; i >= 0; --i)
    {
        for (int j = 0; j < nWidth; ++j)
        {
            char szByte[3];
            int nOffset = i * nWidth * 3 + j * 3;
            szByte[0] = pData[nOffset + 0];
            szByte[1] = pData[nOffset + 1];
            szByte[2] = pData[nOffset + 2];
            if (fwrite(szByte, 1, 3, pFile) != 3)
            {
                XWINDOW_INFO("write bmpfileheader error\n");
                fclose(pFile);
                return false;
            }
        }

        char cFill = 0x0;
        for (int k = nWidth * 3; k < iLineByteCnt; ++k)
        {
            if (fwrite(&cFill, 1, 1, pFile) != 1)
            {
                XWINDOW_ERR("Write BmpData Error");
                fclose(pFile);
                return false;
            }
        }

    }

    fclose(pFile);
    return true;
}

bool CBMPFormatHandle::IsFileHandleCorrect(FILE* pFile)
{
    if (NULL == pFile || 0 != fseek(pFile, 0, SEEK_SET))
    {
        return false;
    }

    char szFileFlag[3];

    if (2 != fread(szFileFlag, 1, 2, pFile))
    {
        return false;
    }

    szFileFlag[2] = '\0';

    if (0 != strcmp(szFileFlag, "BM"))
    {
        return false;
    }

    return true;
}

int CBMPFormatHandle::GetSaveFormatBitDepth()
{
    return GRAPHIC_FORMAT_BGRA32;
}

bool CBMPFormatHandle::GetImageSize(FILE* pFile, chSize& szImage)
{
    if (NULL == pFile)
    {
        return false;
    }

    unsigned char arrData[26] = { 0 };

    //读取宽高数据，bmp的宽在头的第18字节，高在第22字节，各占四字节
    if (fread(arrData, 1, sizeof(arrData), pFile) != sizeof(arrData))
    {
        return false;
    }

    szImage.cx = MAKEUI(arrData[21], arrData[20], arrData[19], arrData[18]);
    szImage.cy = MAKEUI(arrData[25], arrData[24], arrData[23], arrData[22]);

    return true;
}

#endif //#if IF_SUPPORT_BMP
