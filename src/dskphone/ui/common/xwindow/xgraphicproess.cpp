#include "xgraphicproess.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphicOperate
xGraphicOperate::xGraphicOperate()
{

}

xGraphicOperate::~xGraphicOperate()
{

}

void xGraphicOperate::tranGraphic(const xGraphic& src, const chRect& rcSrc,
                                  xGraphic& dst, const chRect& rcDst
#if !IF_ONLY_USE_GRADATION
                                  , FUN_PIX_OPT pFunPixOpt
#endif
                                 )
{
#if IF_ONLY_USE_GRADATION
    const int& nSrcLineLength = src.m_nWidthPixels;
    const int& nDstLineLength = dst.m_nWidthPixels;
    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + rcSrc.left;
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * rcDst.top + rcDst.left;
    int nSrcSize = 1, nDstSize = 1;
#else
    int nSrcLineLength = getDataLength(src.m_nFormat, src.m_nWidthPixels);
    int nDstLineLength = getDataLength(dst.m_nFormat, dst.m_nWidthPixels);

    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + getDataLength(src.m_nFormat, rcSrc.left);
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * rcDst.top + getDataLength(dst.m_nFormat, rcDst.left);
    int nSrcSize = getDataLength(src.m_nFormat, 1);
    int nDstSize = getDataLength(dst.m_nFormat, 1);
#endif

    for (int nY = rcSrc.top; nY < rcSrc.bottom; ++nY)
    {
        BYTE* pLineSrc = pSrc;
        BYTE* pLineDst = pDst;

        for (int nX = rcSrc.left; nX < rcSrc.right; ++nX)
        {
#if IF_ONLY_USE_GRADATION
            *pLineDst = *pLineSrc;
#else
            (*pFunPixOpt)(pLineSrc, pLineDst);
#endif

            pLineSrc += nSrcSize;
            pLineDst += nDstSize;
        }

        pSrc += nSrcLineLength;
        pDst += nDstLineLength;
    }
}

void xGraphicOperate::fillColor(BYTE* pColor, xGraphic& dst, const chRect& rcDst
#if !IF_ONLY_USE_GRADATION
                                , FUN_PIX_OPT pFunPixOpt
#endif
                               )
{
#if IF_ONLY_USE_GRADATION
    const int& nDstLineLength = dst.m_nWidthPixels;
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * rcDst.top + rcDst.left;
    int nDstSize = 1;
#else
    int nDstLineLength = getDataLength(dst.m_nFormat, dst.m_nWidthPixels);
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * rcDst.top + getDataLength(dst.m_nFormat, rcDst.left);
    int nDstSize = getDataLength(dst.m_nFormat, 1);
#endif

    for (int nY = rcDst.top; nY < rcDst.bottom; ++nY)
    {
        BYTE* pLineDst = pDst;

        for (int nX = rcDst.left; nX < rcDst.right; ++nX)
        {
#if IF_ONLY_USE_GRADATION
            *pLineDst = *pColor;
#else
            (*pFunPixOpt)(pColor, pLineDst);
#endif

            pLineDst += nDstSize;
        }

        pDst += nDstLineLength;
    }
}


void xGraphicOperate::fillClipRect(xGraphic& dst, const chRect& rcDst, const xRegion& rgnClip, BYTE* pColor
#if !IF_ONLY_USE_GRADATION
                                   , FUN_PIX_OPT pFunPixOpt
#endif
                                  )
{
    const typeRectList& listRect = rgnClip.GetRectList();
#if IF_GRAPHIC_ROTATE
    int nDstWidth = dst.width();
    int nDstHeight = dst.height();
#endif

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        chRect rcTmp = rcDst & *it;

        if (rcTmp.IsEmpty())
        {
            continue;
        }

#if IF_GRAPHIC_ROTATE
        xGraphicConverter::RotateRightAngle(rcTmp, nDstWidth, nDstHeight);
#endif

#if IF_ONLY_USE_GRADATION
        xGraphicOperate::fillColor(pColor, dst, rcTmp);
#else
        xGraphicOperate::fillColor(pColor, dst, rcTmp, pFunPixOpt);
#endif
    }
}

void xGraphicOperate::drawClipGraphic(const xGraphic& src, const chRect& rcSrc,
                                      xGraphic& dst, const chRect& rcDst,
                                      const xRegion& rgnClip
#if !IF_ONLY_USE_GRADATION
                                      , FUN_PIX_OPT pFunPixOpt
#endif
                                     )
{
#if !IF_ONLY_USE_GRADATION && IF_GRAPHIC_ROTATE
    FUN_GRAPHIC_OPERATE pFunGraphicOpt = xGraphicConverter::GetFunGraphicOperate();
#endif

    const typeRectList& listRect = rgnClip.GetRectList();

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        chRect rcTmp = rcDst & *it;

        if (rcTmp.IsEmpty())
        {
            continue;
        }

        chRect rcRealSrc = rcSrc;

        rcRealSrc.OffsetRect(rcTmp.left - rcDst.left, rcTmp.top - rcDst.top);
        rcRealSrc.Resize(rcTmp.Width(), rcTmp.Height());

#if !IF_ONLY_USE_GRADATION && IF_GRAPHIC_ROTATE
        (*pFunGraphicOpt)(src, rcRealSrc, dst, rcTmp, pFunPixOpt);
#else
        xGraphicOperate::tranGraphic(src, rcRealSrc, dst, rcTmp);
#endif
    }
}

#if IF_USE_FREETYPE_LIB
void xGraphicOperate::fillPolygon(BYTE* pAlpha, const chPoint& ptOffset, int nLineLength,
                                  xGraphic& dst, const chRect& rcDst, const xColor& color)
{
    if (NULL == pAlpha)
    {
        return;
    }

#if !IF_ONLY_USE_GRADATION
    int nDstFormat = dst.GetFormat();
    FUN_PIX_OPT pFunTranPixOpt = xGraphicConverter::GetFunTranPixOpt(GRAPHIC_FORMAT_BGRA32, nDstFormat);
    FUN_PIX_OPT pFunBlendPixOpt = xGraphicConverter::GetFunBlendPixOpt(GRAPHIC_FORMAT_BGRA32, nDstFormat);

    if (NULL == pFunTranPixOpt || NULL == pFunBlendPixOpt)
    {
        return;
    }
#endif

    BYTE* pSrcAlpha = pAlpha + nLineLength * ptOffset.y + ptOffset.x;
#if IF_ONLY_USE_GRADATION
    BYTE* pDst = dst.m_pPixmap + dst.m_nWidthPixels * rcDst.top + rcDst.left;
#else
#if IF_GRAPHIC_ROTATE
    const int& nDstWidth = dst.m_nWidthPixels;
    const int& nDstHeight = dst.m_nHeightPixels;
#endif
    int nDstLineLength = getDataLength(dst.m_nFormat, nDstWidth);
#endif

    for (int nY = rcDst.top; nY < rcDst.bottom; ++nY)
    {
        BYTE* pLineSrcAlpha = pSrcAlpha;
#if IF_ONLY_USE_GRADATION
        BYTE* pLineDst = pDst;
#endif

        for (int nX = rcDst.left; nX < rcDst.right; ++nX)
        {
            if (0x00 != (*pLineSrcAlpha))
            {
#if IF_ONLY_USE_GRADATION
                if ((*pLineDst) >= 0x80)
                {
                    *pLineDst = color;
                }
#else
                chPoint ptDst(nX, nY);

#if IF_GRAPHIC_ROTATE
                xGraphicConverter::RotateRightAngle(ptDst, nDstWidth, nDstHeight);
#endif

                BYTE* pDst = dst.m_pPixmap + nDstLineLength * ptDst.y + getDataLength(dst.m_nFormat, ptDst.x);

                if (0xFF == *pLineSrcAlpha)
                {
                    (*pFunTranPixOpt)
                    ((BYTE*)&color, pDst);
                }
                else
                {
                    xColor clrPixel(*pLineSrcAlpha, color.r, color.g, color.b);

                    (*pFunBlendPixOpt)((BYTE*)&clrPixel, pDst);
                }
#endif
            }

            ++pLineSrcAlpha;
#if IF_ONLY_USE_GRADATION
            ++pLineDst;
#endif
        }

        pSrcAlpha += nLineLength;
#if IF_ONLY_USE_GRADATION
        pDst += nDstWidth;
#endif
    }
}
#endif

#if IF_ONLY_USE_GRADATION
/*==================================黑白屏======================================*/
void xGraphicOperate::tranGadation8B_2_1B(const xGraphic& src, xGraphic& dst)
{
    for (int nY = 0; nY < src.m_nHeightPixels; nY += 8)
    {
        for (int nX = 0; nX < src.m_nWidthPixels; ++nX)
        {
            BYTE byBit = 0;

            for (int i = 0; i < 8; ++i)
            {
                BYTE* pPixel = src.m_pPixmap + src.m_nWidthPixels * (nY + i) + nX;

                if ((*pPixel) < 128)
                {
                    byBit |= (0x01 << i);
                }
            }

            *(dst.m_pPixmap + dst.m_nWidthPixels * (nY >> 3) + nX) = byBit;
        }
    }
}

#if IF_INVERT_EFFECT
void xGraphicOperate::invertRect(xGraphic& dst, const chRect& rcDst)
{
    BYTE* pDst = dst.m_pPixmap + rcDst.top * dst.m_nWidthPixels + rcDst.left;

    for (int nY = rcDst.top; nY < rcDst.bottom; ++nY)
    {
        BYTE* pLineDst = pDst;

        for (int nX = rcDst.left; nX < rcDst.right; ++nX)
        {
            INVERT_GRADATION(*pLineDst)
            pLineDst += 1;
        }

        pDst += dst.m_nWidthPixels;
    }
}
#endif
/*------------------------------------------------------------------------------*/
#else
/*====================================彩屏======================================*/
int xGraphicOperate::getDataLength(int nFormat, int nWidth)
{
    switch (nFormat)
    {
#if !IF_ONLY_USE_GRADATION
    case GRAPHIC_FORMAT_BGR8:
        return nWidth;
        break;
    case GRAPHIC_FORMAT_BGR16_444:
    case GRAPHIC_FORMAT_BGR16_444_COLOR_KEY:
    case GRAPHIC_FORMAT_BGR16_555:
    case GRAPHIC_FORMAT_BGR16_555_COLOR_KEY:
    case GRAPHIC_FORMAT_BGR16_565:
    case GRAPHIC_FORMAT_BGR16_565_COLOR_KEY:
        return (nWidth << 1);
        break;
    case GRAPHIC_FORMAT_BGR24:
    case GRAPHIC_FORMAT_RGB24:
        return ((nWidth << 1) + nWidth);
        break;
    case GRAPHIC_FORMAT_BGRA32:
    case GRAPHIC_FORMAT_BGRA32_PRE_MULT:
    case GRAPHIC_FORMAT_RGBA32:
    case GRAPHIC_FORMAT_RGBA32_PRE_MULT:
        return (nWidth << 2);
        break;
#endif
#if IF_USE_GRADATION
    case GRAPHIC_FORMAT_GRADATION_8B:
        return nWidth;
        break;
    case GRAPHIC_FORMAT_GRADATION_1B:
        return ((nWidth + 7) >> 3);
        break;
#endif
    default:
        break;
    }

    return 0;
}

#if IF_PIXEL_ALPHA
// Alpha = srcAlpha + dstAlpha - srcAlpha * dstAlpha / 0xFF;
// R = (srcR * srcAlpha + dstR * dstAlpha - dstR * dstAlpha * srcAlpha / 0xFF) / Alpha
// G = ...; see also R
// B = ...; see also R
void xGraphicOperate::alphaBlend(const xPixel* src, xPixel* dst)
{
    dst->r = src->r * src->a / 255 + dst->r * (255 - src->a) / 255;
    dst->g = src->g * src->a / 255 + dst->g * (255 - src->a) / 255;
    dst->b = src->b * src->a / 255 + dst->b * (255 - src->a) / 255;
}
#endif

#if IF_XWINDOW_STATION_ALPHA
void xGraphicOperate::blendAlphaBGR16(xGraphic& dst, const chRect& rcDst, int nAlpha)
{
    u16int* pDst16 = ((u16int*)dst.m_pPixmap) + dst.m_nWidthPixels * rcDst.top;
    DWORD dwSia = nAlpha;

    for (int nY = rcDst.top; nY < rcDst.bottom; ++nY)
    {
        for (int nX = rcDst.left; nX < rcDst.right; ++nX)
        {
            DWORD dwPix = pDst16[nX];

            DWORD dwR = (dwPix & 0x0000f800);
            DWORD dwG = (dwPix & 0x000007e0);
            DWORD dwB = (dwPix & 0x0000001f);

            DWORD dwSiaR = dwR * dwSia;
            DWORD dwSiaG = dwG * dwSia;
            DWORD dwSiaB = dwB * dwSia;

            DWORD dwRR = (dwSiaR + (dwSiaR >> 8) + (0x80 << 8)) >> 8;
            DWORD dwRG = (dwSiaG + (dwSiaG >> 8) + (0x80 << 3)) >> 8;
            DWORD dwRB = (dwSiaB + (dwSiaB >> 8) + (0x80 >> 3)) >> 8;

            pDst16[nX] = (dwRR & 0xf800) | (dwRG & 0x07e0) | (dwRB);
        }

        pDst16 += dst.m_nWidthPixels;
    }
}
#endif

// rcSrc在src的有效范围内，函数内不再做有效性判断
xGraphicPtr xGraphicOperate::convertGraphic(const xGraphic& src, const chRect& rcSrc, int nDstFormat)
{
    FUN_PIX_OPT pFunPixOpt = xGraphicConverter::GetFunTranPixOpt(src.m_nFormat, nDstFormat);

    if (NULL == pFunPixOpt)
    {
        XWINDOW_ERR("undefine pixel format(%d-%d) convert", src.m_nFormat, nDstFormat);

        return xGraphicPtr(NULL);
    }

    xGraphic* pNewRawGraphic = new xGraphic;

    if (NULL == pNewRawGraphic)
    {
        return xGraphicPtr(NULL);
    }

    xGraphicPtr pNewGraphic(pNewRawGraphic);

    int nSrcWidth = rcSrc.Width();
    int nSrcHeight = rcSrc.Height();

    pNewRawGraphic->SetFormat(nDstFormat);
    pNewRawGraphic->createGraphic(nSrcWidth, nSrcHeight);

    chRect rcNewSrc(0, 0, nSrcWidth, nSrcHeight);
    chRect rcNewDst(0, 0, nSrcWidth, nSrcHeight);

    xGraphicOperate::tranGraphic(src, rcSrc, *pNewRawGraphic, rcNewDst, pFunPixOpt);

    return pNewGraphic;
}

#if IF_GRAPHIC_ROTATE
void xGraphicOperate::tranGraphic90(const xGraphic& src, const chRect& rcSrc,
                                    xGraphic& dst, const chRect& rcDst,
                                    FUN_PIX_OPT pFunPixOpt)
{
    const int& nTarHeight = dst.m_nHeightPixels;

#if IF_ONLY_USE_GRADATION
    const int& nSrcLineLength = src.m_nWidthPixels;
    const int& nDstLineLength = dst.m_nWidthPixels;

    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + rcSrc.left;
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * (nTarHeight - rcDst.left - 1) + rcDst.top;

    int nSrcOffset = nSrcLineLength * rcSrc.top + rcSrc.right - 1;
    int nSrcSize = 1, nDstSize = 1;
#else
    int nSrcLineLength = getDataLength(src.m_nFormat, src.m_nWidthPixels);
    int nDstLineLength = getDataLength(dst.m_nFormat, dst.m_nWidthPixels);

    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + getDataLength(src.m_nFormat, rcSrc.left);
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * (nTarHeight - rcDst.left - 1) + getDataLength(dst.m_nFormat, rcDst.top);

    int nSrcSize = getDataLength(src.m_nFormat, 1);
    int nDstSize = getDataLength(dst.m_nFormat, 1);
#endif

    for (int nY = rcSrc.top; nY < rcSrc.bottom; ++nY)
    {
        BYTE* pLineSrc = pSrc;
        BYTE* pLineDst = pDst;

        for (int nX = rcSrc.left; nX < rcSrc.right; ++nX)
        {
#if IF_ONLY_USE_GRADATION
            *pLineDst = *pSrc;
#else
            (*pFunPixOpt)(pLineSrc, pLineDst);
#endif

            pLineSrc += nSrcSize;
            pLineDst -= nDstLineLength;
        }

        pSrc += nSrcLineLength;
        pDst += nDstSize;
    }
}

void xGraphicOperate::tranGraphic180(const xGraphic& src, const chRect& rcSrc,
                                     xGraphic& dst, const chRect& rcDst,
                                     FUN_PIX_OPT pFunPixOpt)
{
    const int& nTarWidth = dst.m_nWidthPixels;
    const int& nTarHeight = dst.m_nHeightPixels;

#if IF_ONLY_USE_GRADATION
    const int& nSrcLineLength = src.m_nWidthPixels;
    const int& nDstLineLength = dst.m_nWidthPixels;

    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + rcSrc.left;
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * (nTarHeight - rcDst.top) + (nTarWidth - rcDst.left - 1);

    int nSrcSize = 1, nDstSize = 1;
#else
    int nSrcLineLength = getDataLength(src.m_nFormat, src.m_nWidthPixels);
    int nDstLineLength = getDataLength(dst.m_nFormat, nTarWidth);

    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + getDataLength(src.m_nFormat, rcSrc.left);
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * (nTarHeight - rcDst.top - 1) + getDataLength(dst.m_nFormat, nTarWidth - rcDst.left - 1);

    int nSrcSize = getDataLength(src.m_nFormat, 1);
    int nDstSize = getDataLength(dst.m_nFormat, 1);
#endif

    for (int nY = rcSrc.top; nY < rcSrc.bottom; ++nY)
    {
        BYTE* pLineSrc = pSrc;
        BYTE* pLineDst = pDst;

        for (int nX = rcSrc.left; nX < rcSrc.right; ++nX)
        {
#if IF_ONLY_USE_GRADATION
            *pLineDst = *pLineSrc;
#else
            (*pFunPixOpt)(pLineSrc, pLineDst);
#endif

            pLineSrc += nSrcSize;
            pLineDst -= nDstSize;
        }

        pSrc += nSrcLineLength;
        pDst -= nDstLineLength;
    }
}

void xGraphicOperate::tranGraphic270(const xGraphic& src, const chRect& rcSrc,
                                     xGraphic& dst, const chRect& rcDst,
                                     FUN_PIX_OPT pFunPixOpt)
{
    const int& nTarWidth = dst.m_nWidthPixels;

#if IF_ONLY_USE_GRADATION
    const int& nSrcLineLength = src.m_nWidthPixels;
    const int& nDstLineLength = dst.m_nWidthPixels;

    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + rcSrc.left;
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * rcDst.left + (nTarWidth - rcDst.top - 1);

    int nSrcOffset = nSrcLineLength * rcSrc.bottom + rcSrc.left;
    int nSrcSize = 1, nDstSize = 1;
#else
    int nSrcLineLength = getDataLength(src.m_nFormat, src.m_nWidthPixels);
    int nDstLineLength = getDataLength(dst.m_nFormat, dst.m_nWidthPixels);

    BYTE* pSrc = src.m_pPixmap + nSrcLineLength * rcSrc.top + getDataLength(src.m_nFormat, rcSrc.left);
    BYTE* pDst = dst.m_pPixmap + nDstLineLength * rcDst.left + getDataLength(dst.m_nFormat, nTarWidth - rcDst.top - 1);

    int nSrcSize = getDataLength(src.m_nFormat, 1);
    int nDstSize = getDataLength(dst.m_nFormat, 1);
#endif

    for (int nY = rcSrc.top; nY < rcSrc.bottom; ++nY)
    {
        BYTE* pLineSrc = pSrc;
        BYTE* pLineDst = pDst;

        for (int nX = rcSrc.left; nX < rcSrc.right; ++nX)
        {
#if IF_ONLY_USE_GRADATION
            *pLineDst = *pLineSrc;
#else
            (*pFunPixOpt)(pLineSrc, pLineDst);
#endif

            pLineSrc += nSrcSize;
            pLineDst += nDstLineLength;
        }

        pSrc += nSrcLineLength;
        pDst -= nDstSize;
    }
}
#endif

#if IF_GRAPHIC_ZOOM
void xGraphicOperate::tileDrawClipGraphic(const xGraphic& src, const chRect& rcSrc,
        xGraphic& dst, const chRect& rcDst,
        const xRegion& rgnClip)
{
    int nSrcWidth = rcSrc.Width();
    int nSrcHeight = rcSrc.Height();

    if (nSrcWidth <= 0 || nSrcHeight <= 0)
    {
        return;
    }

#if !IF_ONLY_USE_GRADATION
    FUN_GRAPHIC_OPERATE pFunGraphicOpt = xGraphicConverter::GetFunGraphicOperate();
    FUN_PIX_OPT pFunPixOpt = NULL;

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    if (src.hasAlphaPixel())
    {
        pFunPixOpt = xGraphicConverter::GetFunBlendPixOpt(src.GetFormat(), dst.GetFormat());
    }
    else
    {
#endif
        pFunPixOpt = xGraphicConverter::GetFunTranPixOpt(src.m_nFormat, dst.m_nFormat);
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    }
#endif

    if (NULL == pFunGraphicOpt || NULL == pFunPixOpt)
    {
        return;
    }
#endif

#if IF_DEBUG_PAINTER
    XWINDOW_INFO("Begin tileDrawClipGraphic src:%d %d %d %d dst:%d %d %d %d",
                 rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom,
                 rcDst.left, rcDst.top, rcDst.right, rcDst.bottom);
#endif

    const typeRectList& listRect = rgnClip.GetRectList();

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        chRect rcClip = rcDst & *it;

        if (rcClip.IsEmpty())
        {
            continue;
        }

        int nSrcX = (rcClip.left - rcDst.left) % nSrcWidth;
        int nSrcY = (rcClip.top - rcDst.top) % nSrcHeight;
        int nDstY = rcClip.top;
        int nCopyHeight = 0;

        for (; nDstY < rcClip.bottom; nDstY += nCopyHeight)
        {
            nCopyHeight = nSrcHeight - nSrcY;

            int nDstBottom = nDstY + nCopyHeight;

            if (nDstBottom > rcClip.bottom)
            {
                nDstBottom = rcClip.bottom;
                nCopyHeight = nDstBottom - nDstY;
            }

            int nDstX = rcClip.left;
            int nSrcCopyX = nSrcX;
            int nCopyWidth = 0;

            for (; nDstX < rcClip.right; nDstX += nCopyWidth)
            {
                nCopyWidth = nSrcWidth - nSrcCopyX;

                int nDstRight = nDstX + nCopyWidth;

                if (nDstRight > rcClip.right)
                {
                    nDstRight = rcClip.right;
                    nCopyWidth = nDstRight - nDstX;
                }

                int nSrcLeft = rcSrc.left + nSrcCopyX;
                int nSrcTop = rcSrc.top + nSrcY;
                chRect rcSrcCopy(nSrcLeft, nSrcTop, nSrcLeft + nCopyWidth, nSrcTop + nCopyHeight);
                chRect rcDstCopy(nDstX, nDstY, nDstRight, nDstBottom);

#if IF_DEBUG_PAINTER
                XWINDOW_INFO("draw graphic src:%d %d %d %d dst:%d %d %d %d",
                             rcSrcCopy.left, rcSrcCopy.top, rcSrcCopy.right, rcSrcCopy.bottom,
                             rcDstCopy.left, rcDstCopy.top, rcDstCopy.right, rcDstCopy.bottom);
#endif

#if IF_ONLY_USE_GRADATION
                xGraphicOperate::tranGraphic(src, rcSrcCopy, dst, rcDstCopy);
#else
                (*pFunGraphicOpt)(src, rcSrcCopy, dst, rcDstCopy, pFunPixOpt);
#endif

                nSrcCopyX = 0;
            }

            nSrcY = 0;
        }
    }

#if IF_DEBUG_PAINTER
    XWINDOW_INFO("End tileDrawClipGraphic");
#endif
}

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
// rcSrc在src的有效范围内，函数内不再做有效性判断
xGraphicPtr xGraphicOperate::zoomGraphic(const xGraphic& src, const chRect& rcSrc, int nDstWidth, int nDstHeight)
{
    const xGraphic* pSrcGraphic = &src;
    xGraphicPtr pNewGraphic(NULL);

    if (GRAPHIC_FORMAT_BGRA32 != src.m_nFormat)
    {
        XWINDOW_WARN("zoomGraphic unexpect format:%d", src.m_nFormat);

        pNewGraphic = convertGraphic(src, rcSrc, GRAPHIC_FORMAT_BGRA32);

        if (pNewGraphic.IsEmpty())
        {
            return xGraphicPtr(NULL);
        }

        pSrcGraphic = pNewGraphic.Get();
    }

#if IF_PIXEL_ALPHA
    return xGraphicScale::ScaleGraphic(*pSrcGraphic, rcSrc, nDstWidth, nDstHeight, true);
#else
    return xGraphicScale::ScaleGraphic(*pSrcGraphic, rcSrc, nDstWidth, nDstHeight, false);
#endif
}
#endif

void xGraphicOperate::zoomDrawClipGraphic(const xGraphic& src, const chRect& rcSrc,
        xGraphic& dst, const chRect& rcDst,
        const xRegion& rgnClip)
{
#if IF_DEBUG_GRAPHIC_SCALE
    XWINDOW_INFO("zoomDrawClipGraphic rcSrc:%d %d %d %d rcDst:%d %d %d %d",
                 rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom,
                 rcDst.left, rcDst.top, rcDst.right, rcDst.bottom);
#endif

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    xGraphicPtr pGraphic = zoomGraphic(src, rcSrc, rcDst.Width(), rcDst.Height());
#else
    xGraphicPtr pGraphic = xGraphicScale::ScaleGraphic(src, rcSrc, rcDst.Width(), rcDst.Height());
#endif

    if (pGraphic.IsEmpty())
    {
        return;
    }

    chRect rcPixmap(0, 0, pGraphic->width(), pGraphic->height());

    FUN_PIX_OPT pFunPixOpt = NULL;

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    if (pGraphic->hasAlphaPixel())
    {
        pFunPixOpt = xGraphicConverter::GetFunBlendPixOpt(pGraphic->GetFormat(), dst.GetFormat());
    }
    else
    {
#endif
        pFunPixOpt = xGraphicConverter::GetFunTranPixOpt(pGraphic->GetFormat(), dst.GetFormat());
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    }
#endif

    if (NULL == pFunPixOpt)
    {
        return;
    }

    xGraphicOperate::drawClipGraphic(*(pGraphic.Get()), rcPixmap, dst, rcDst, rgnClip, pFunPixOpt);
}
#endif

#if IF_GRAPHIC_ROTATE
void xGraphicOperate::rotateGraphic(xGraphic& src)
{
    xGraphic* pNewRawGraphic = new xGraphic;

    if (NULL == pNewRawGraphic)
    {
        return;
    }

    xGraphicPtr pNewGraphic(pNewRawGraphic);

    pNewRawGraphic->SetFormat(src.m_nFormat);

    int nDstWidth = src.m_nWidthPixels;
    int nDstHeight = src.m_nHeightPixels;

    xGraphicConverter::RotateRightAngle(nDstWidth, nDstHeight);

    if (!pNewRawGraphic->createGraphic(nDstWidth, nDstHeight))
    {
        return;
    }

    FUN_PIX_OPT pFunPixOpt = xGraphicConverter::GetFunTranPixOpt(src.m_nFormat, src.m_nFormat);

    if (NULL == pFunPixOpt)
    {
        return;
    }

    FUN_GRAPHIC_OPERATE pFunGraphicOpt = xGraphicConverter::GetFunGraphicOperate();
    chRect rcCopy(0, 0, src.m_nWidthPixels, src.m_nHeightPixels);

    (*pFunGraphicOpt)(src, rcCopy, *pNewRawGraphic, rcCopy, pFunPixOpt);

    int nDataLen = getDataLength(src.m_nFormat, src.m_nWidthPixels * src.m_nHeightPixels);

    memcpy(src.m_pPixmap, pNewRawGraphic->m_pPixmap, nDataLen);
    src.m_nWidthPixels = nDstWidth;
    src.m_nHeightPixels = nDstHeight;

    src.setRotateFlag(xGraphicConverter::s_nRotateAngle);
}
#endif

void xGraphicOperate::quickCopy(const xGraphic& src, xGraphic& dst, const chRect& rcSrc, int nDataLen)
{
    int nWidth = rcSrc.Width();
    int nHeight = rcSrc.Height();

    if (nWidth == dst.m_nWidthPixels && nHeight == dst.m_nHeightPixels)
    {
        memcpy(dst.m_pPixmap, src.m_pPixmap, nDataLen);

        return;
    }

    int nLineLength = getDataLength(src.m_nFormat, src.m_nWidthPixels);
    int nOffset = nLineLength * rcSrc.top + getDataLength(src.m_nFormat, rcSrc.left);
    int nCopyLen = getDataLength(src.m_nFormat, nWidth);
    BYTE* pSrc = src.m_pPixmap + nOffset;
    BYTE* pDst = dst.m_pPixmap + nOffset;

    for (int nY = rcSrc.top; nY < rcSrc.bottom; ++nY)
    {
        memcpy(pDst, pSrc, nCopyLen);

        pSrc += nLineLength;
        pDst += nLineLength;
    }
}
#endif

#if !IF_ONLY_USE_GRADATION
//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphicConverter
#if IF_GRAPHIC_ROTATE
int xGraphicConverter::s_nRotateAngle = 0;
#endif

xGraphicConverter::xGraphicConverter()
{
}

xGraphicConverter::~xGraphicConverter()
{
}

#if IF_GRAPHIC_ROTATE
void xGraphicConverter::SetRotateAngle(int nAngle)
{
    s_nRotateAngle = nAngle;
}

int xGraphicConverter::GetRotateAngle()
{
    return s_nRotateAngle;
}

void xGraphicConverter::RotateRightAngle(int& nWidth, int& nHeight)
{
    if (0 == s_nRotateAngle)
    {
        return;
    }

    switch (s_nRotateAngle)
    {
    case 90:
    case 270:
    case -90:
    case -270:
        {
            int nTmp = nWidth;

            nWidth = nHeight;
            nHeight = nTmp;
        }
        break;
    default:
        break;
    }
}

void xGraphicConverter::RotateRightAngle(chPoint& ptRotate, int nTarWidth, int nTarHeight)
{
    if (0 == s_nRotateAngle)
    {
        return;
    }

    int nX = ptRotate.x;
    int nY = ptRotate.y;

    switch (s_nRotateAngle)
    {
    case 90:
    case -270:
        {
            ptRotate.x = nY;
            ptRotate.y = nTarHeight - nX - 1;
        }
        break;
    case 180:
    case -180:
        {
            ptRotate.x = nTarWidth - nX - 1;
            ptRotate.y = nTarHeight - nY - 1;
        }
        break;
    case 270:
    case -90:
        {
            ptRotate.x = nTarWidth - nY - 1;
            ptRotate.y = nX;
        }
        break;
    default:
        break;
    }
}

void xGraphicConverter::RotateRightAngle(chRect& rcRotate, int nTarWidth, int nTarHeight)
{
    if (0 == s_nRotateAngle)
    {
        return;
    }

    int nWidth = rcRotate.Width();
    int nHeight = rcRotate.Height();

    switch (s_nRotateAngle)
    {
    case 90:
    case -270:
        {
            rcRotate.left = rcRotate.top;
            rcRotate.top = nTarHeight - rcRotate.right;
            rcRotate.right = rcRotate.left + nHeight;
            rcRotate.bottom = rcRotate.top + nWidth;
        }
        break;
    case 180:
    case -180:
        {
            rcRotate.left = nTarWidth - rcRotate.right;
            rcRotate.top = nTarHeight - rcRotate.bottom;
            rcRotate.right = rcRotate.left + nWidth;
            rcRotate.bottom = rcRotate.top + nHeight;
        }
        break;
    case 270:
    case -90:
        {
            rcRotate.top = rcRotate.left;
            rcRotate.left = nTarWidth - rcRotate.bottom;
            rcRotate.right = rcRotate.left + nHeight;
            rcRotate.bottom = rcRotate.top + nWidth;
        }
        break;
    default:
        break;
    }
}

FUN_GRAPHIC_OPERATE xGraphicConverter::GetFunGraphicOperate()
{
    switch (s_nRotateAngle)
    {
    case 90:
    case -270:
        {
            return &xGraphicOperate::tranGraphic90;
        }
        break;
    case 180:
    case -180:
        {
            return &xGraphicOperate::tranGraphic180;
        }
        break;
    case 270:
    case -90:
        {
            return &xGraphicOperate::tranGraphic270;
        }
        break;
    default:
        break;
    }

    return &xGraphicOperate::tranGraphic;
}
#endif

#if !IF_ONLY_USE_GRADATION
void xGraphicConverter::Copy_GBRA32_Or_RGBA32(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    xPixel* pDstP = (xPixel*)pDst;

    *pDstP = *pSrcP;
}

void xGraphicConverter::Tran_GBRA32_2_RGBA32(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    ST_RGBA32* pDstRGBA32 = (ST_RGBA32*)pDst;

    pDstRGBA32->r = pSrcP->r;
    pDstRGBA32->g = pSrcP->g;
    pDstRGBA32->b = pSrcP->b;
    pDstRGBA32->a = pSrcP->a;
}

void xGraphicConverter::Tran_GBRA32_2_PreMult(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    xPixel* pDstP = (xPixel*)pDst;

    pDstP->r = pSrcP->r * pSrcP->a / 255;
    pDstP->g = pSrcP->g * pSrcP->a / 255;
    pDstP->b = pSrcP->b * pSrcP->a / 255;
    pDstP->a = pSrcP->a;
}

#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
void xGraphicConverter::Tran_GBRA32_2_BGR24(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    ST_BGR24* pDstGBR24 = (ST_BGR24*)pDst;

    pDstGBR24->b = pSrcP->b;
    pDstGBR24->g = pSrcP->g;
    pDstGBR24->r = pSrcP->r;
}
#endif

void xGraphicConverter::Tran_GBRA32_2_RGB24(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    ST_RGB24* pDstRGB24 = (ST_RGB24*)pDst;

    pDstRGB24->r = pSrcP->r;
    pDstRGB24->g = pSrcP->g;
    pDstRGB24->b = pSrcP->b;
}

void xGraphicConverter::Tran_GBRA32_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    *pDst16 = GET_BGR565(pSrcP->r, pSrcP->g, pSrcP->b);
}

#if IF_GRAPHIC_SUPPORT_COLOR_KEY
void xGraphicConverter::Tran_GBRA32_2_BGR565_ColorKey(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    if (0x00 == pSrcP->a)
    {
        *pDst16 = xGraphic::s_u16Key;
    }
    else
    {
        *pDst16 = GET_BGR565(pSrcP->r, pSrcP->g, pSrcP->b);
    }
}
#endif

void xGraphicConverter::Blend_GBRA32_PreMult_2_GBRA32(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    xPixel* pDstP = (xPixel*)pDst;

    pDstP->a = pSrcP->a;

    if (0x00 == pSrcP->a || 0xff == pSrcP->a)
    {
        pDstP->r = pSrcP->r;
        pDstP->g = pSrcP->g;
        pDstP->b = pSrcP->b;
    }
    else
    {
        pDstP->r = pSrcP->r * 255 / pSrcP->a;
        pDstP->g = pSrcP->g * 255 / pSrcP->a;
        pDstP->b = pSrcP->b * 255 / pSrcP->a;
    }
}

void xGraphicConverter::Blend_GBRA32_PreMult_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    u16int* pDst16 = (u16int*)pDst;

    DWORD spix = *((DWORD*)pSrc);
    DWORD alpha = spix >> 24;

    if (0xff == alpha)
    {
        xPixel* pSrcP = (xPixel*)pSrc;

        *pDst16 = GET_BGR565(pSrcP->r, pSrcP->g, pSrcP->b);
    }
    else
    {
        DWORD sia = 255 - alpha;

        DWORD sr = (spix >> 8) & 0xf800;
        DWORD sg = (spix >> 5) & 0x07e0;
        DWORD sb = (spix >> 3) & 0x001f;

        DWORD dpix = *pDst16;
        DWORD dr = (dpix & 0x0000f800);
        DWORD dg = (dpix & 0x000007e0);
        DWORD db = (dpix & 0x0000001f);

        DWORD siar = dr * sia;
        DWORD siag = dg * sia;
        DWORD siab = db * sia;

        DWORD rr = sr + ((siar + (siar >> 8) + (0x80 << 8)) >> 8);
        DWORD rg = sg + ((siag + (siag >> 8) + (0x80 << 3)) >> 8);
        DWORD rb = sb + ((siab + (siab >> 8) + (0x80 >> 3)) >> 8);

        *pDst16 = (rr & 0xf800) | (rg & 0x07e0) | (rb);
    }
}

void xGraphicConverter::Blend_GBRA32_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    xPixel* pSrcP = (xPixel*)pSrc;
    u16int* pDst16 = (u16int*)pDst;
    u16int& u16Dst = *pDst16;
    xPixel pixDst(GET_BGR565_R(u16Dst), GET_BGR565_G(u16Dst), GET_BGR565_B(u16Dst));

    xGraphicOperate::alphaBlend(pSrcP, &pixDst);

    *pDst16 = GET_BGR565(pixDst.r, pixDst.g, pixDst.b);
}

#if IF_GRAPHIC_SUPPORT_RGBA_PAINT
void xGraphicConverter::Tran_RGBA32_2_PreMult(BYTE* pSrc, BYTE* pDst)
{
    ST_RGBA32* pSrcP = (ST_RGBA32*)pSrc;
    ST_RGBA32* pDstP = (ST_RGBA32*)pDst;

    pDstP->r = pSrcP->r * pSrcP->a / 255;
    pDstP->g = pSrcP->g * pSrcP->a / 255;
    pDstP->b = pSrcP->b * pSrcP->a / 255;
    pDstP->a = pSrcP->a;
}

#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
void xGraphicConverter::Tran_RGBA32_2_BGR24(BYTE* pSrc, BYTE* pDst)
{
    ST_RGBA32* pSrcP = (ST_RGBA32*)pSrc;
    ST_BGR24* pDstGBR24 = (ST_BGR24*)pDst;

    pDstGBR24->b = pSrcP->b;
    pDstGBR24->g = pSrcP->g;
    pDstGBR24->r = pSrcP->r;
}
#endif

void xGraphicConverter::Tran_RGBA32_2_RGB24(BYTE* pSrc, BYTE* pDst)
{
    ST_RGBA32* pSrcP = (ST_RGBA32*)pSrc;
    ST_RGB24* pDstRGB24 = (ST_RGB24*)pDst;

    pDstRGB24->r = pSrcP->r;
    pDstRGB24->g = pSrcP->g;
    pDstRGB24->b = pSrcP->b;
}

void xGraphicConverter::Tran_RGBA32_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    ST_RGBA32* pSrcP = (ST_RGBA32*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    *pDst16 = GET_BGR565(pSrcP->r, pSrcP->g, pSrcP->b);
}

#if IF_GRAPHIC_SUPPORT_COLOR_KEY
void xGraphicConverter::Tran_RGBA32_2_BGR565_ColorKey(BYTE* pSrc, BYTE* pDst)
{
    ST_RGBA32* pSrcP = (ST_RGBA32*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    if (0x00 == pSrcP->a)
    {
        *pDst16 = xGraphic::s_u16Key;
    }
    else
    {
        *pDst16 = GET_BGR565(pSrcP->r, pSrcP->g, pSrcP->b);
    }
}
#endif

void xGraphicConverter::Blend_RGBA32_PreMult_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    ST_RGBA32* pSrcP = (ST_RGBA32*)pSrc;
    u16int* pDst16 = (u16int*)pDst;
    u16int& u16Dst = *pDst16;
    xPixel pixDst(GET_BGR565_R(u16Dst), GET_BGR565_G(u16Dst), GET_BGR565_B(u16Dst));

    ALPHA_BLEND_PRE_MULT(pixDst, (*pSrcP))

    *pDst16 = GET_BGR565(pixDst.r, pixDst.g, pixDst.b);
}

void xGraphicConverter::Blend_RGBA32_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    ST_RGBA32* pSrcP = (ST_RGBA32*)pSrc;
    u16int* pDst16 = (u16int*)pDst;
    u16int& u16Dst = *pDst16;
    xPixel pixSrc(pSrcP->a, pSrcP->r, pSrcP->g, pSrcP->b);
    xPixel pixDst(GET_BGR565_R(u16Dst), GET_BGR565_G(u16Dst), GET_BGR565_B(u16Dst));

    xGraphicOperate::alphaBlend(&pixSrc, &pixDst);

    *pDst16 = GET_BGR565(pixDst.r, pixDst.g, pixDst.b);
}
#endif

#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
void xGraphicConverter::Tran_BGR24_2_GBRA32(BYTE* pSrc, BYTE* pDst)
{
    ST_BGR24* pSrcBGR24 = (ST_BGR24*)pSrc;
    xPixel* pDstP = (xPixel*)pDst;

    pDstP->a = 0xFF;
    pDstP->r = pSrcBGR24->r;
    pDstP->g = pSrcBGR24->g;
    pDstP->b = pSrcBGR24->b;
}

void xGraphicConverter::Tran_BGR24_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    ST_BGR24* pSrcBGR24 = (ST_BGR24*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    *pDst16 = GET_BGR565(pSrcBGR24->r, pSrcBGR24->g, pSrcBGR24->b);
}
#endif

void xGraphicConverter::Tran_RGB24_2_GBRA32(BYTE* pSrc, BYTE* pDst)
{
    ST_RGB24* pSrcRGB24 = (ST_RGB24*)pSrc;
    xPixel* pDstP = (xPixel*)pDst;

    pDstP->a = 0xFF;
    pDstP->r = pSrcRGB24->r;
    pDstP->g = pSrcRGB24->g;
    pDstP->b = pSrcRGB24->b;
}

void xGraphicConverter::Tran_RGB24_2_BGR565(BYTE* pSrc, BYTE* pDst)
{
    ST_RGB24* pSrcRGB24 = (ST_RGB24*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    *pDst16 = GET_BGR565(pSrcRGB24->r, pSrcRGB24->g, pSrcRGB24->b);
}

void xGraphicConverter::Copy_BGR565(BYTE* pSrc, BYTE* pDst)
{
    u16int* pSrc16 = (u16int*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    *pDst16 = *pSrc16;
}

#if IF_GRAPHIC_SUPPORT_COLOR_KEY
void xGraphicConverter::Tran_BGR565_ColorKey_2_BGRA32(BYTE* pSrc, BYTE* pDst)
{
    u16int* pSrc16 = (u16int*)pSrc;

    if ((*pSrc16) != xGraphic::s_u16Key)
    {
        u16int& u16Src = *pSrc16;
        xPixel* pDstP = (xPixel*)pDst;

        pDstP->a = 0xff;
        pDstP->r = GET_BGR565_R(u16Src);
        pDstP->g = GET_BGR565_G(u16Src);
        pDstP->b = GET_BGR565_B(u16Src);
    }
    else
    {
        DWORD* pDstDword = (DWORD*)pDst;

        (*pDstDword) = 0;
    }
}

void xGraphicConverter::Blend_BGR565_ColorKey(BYTE* pSrc, BYTE* pDst)
{
    u16int* pSrc16 = (u16int*)pSrc;
    u16int* pDst16 = (u16int*)pDst;

    if ((*pSrc16) != xGraphic::s_u16Key)
    {
        *pDst16 = *pSrc16;
    }
}
#endif

void xGraphicConverter::Tran_BGR565_2_GBRA32(BYTE* pSrc, BYTE* pDst)
{
    u16int* pSrc16 = (u16int*)pSrc;
    xPixel* pDstP = (xPixel*)pDst;

    pDstP->a = 0xff;
    pDstP->r = GET_BGR565_R(*pSrc16);
    pDstP->g = GET_BGR565_G(*pSrc16);
    pDstP->b = GET_BGR565_B(*pSrc16);
}

#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
void xGraphicConverter::Tran_BGR565_2_BGR24(BYTE* pSrc, BYTE* pDst)
{
    u16int* pSrc16 = (u16int*)pSrc;
    ST_BGR24* pDstBGR24 = (ST_BGR24*)pDst;

    pDstBGR24->r = GET_BGR565_R(*pSrc16);
    pDstBGR24->g = GET_BGR565_G(*pSrc16);
    pDstBGR24->b = GET_BGR565_B(*pSrc16);
}
#endif

void xGraphicConverter::Tran_BGR565_2_RGB24(BYTE* pSrc, BYTE* pDst)
{
    u16int* pSrc16 = (u16int*)pSrc;
    ST_RGB24* pDstRGB24 = (ST_RGB24*)pDst;

    pDstRGB24->r = GET_BGR565_R(*pSrc16);
    pDstRGB24->g = GET_BGR565_G(*pSrc16);
    pDstRGB24->b = GET_BGR565_B(*pSrc16);
}

FUN_PIX_OPT xGraphicConverter::GetFunTranPixOpt(int nSrcFormat, int nDstFormat)
{
    switch (nSrcFormat)
    {
    case GRAPHIC_FORMAT_BGRA32:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGRA32:
                return &xGraphicConverter::Copy_GBRA32_Or_RGBA32;
                break;
            case GRAPHIC_FORMAT_RGBA32:
                return &xGraphicConverter::Tran_GBRA32_2_RGBA32;
                break;
            case GRAPHIC_FORMAT_BGRA32_PRE_MULT:
                return &xGraphicConverter::Tran_GBRA32_2_PreMult;
                break;
#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
            case GRAPHIC_FORMAT_BGR24:
                return &xGraphicConverter::Tran_GBRA32_2_BGR24;
                break;
#endif
            case GRAPHIC_FORMAT_RGB24:
                return &xGraphicConverter::Tran_GBRA32_2_RGB24;
                break;
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Tran_GBRA32_2_BGR565;
                break;
#if IF_GRAPHIC_SUPPORT_COLOR_KEY
            case GRAPHIC_FORMAT_BGR16_565_COLOR_KEY:
                return &xGraphicConverter::Tran_GBRA32_2_BGR565_ColorKey;
                break;
#endif
            default:
                break;
            }
        }
        break;
    case GRAPHIC_FORMAT_BGRA32_PRE_MULT:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGRA32:
                return &xGraphicConverter::Blend_GBRA32_PreMult_2_GBRA32;
                break;
            default:
                break;
            }
        }
        break;
#if IF_GRAPHIC_SUPPORT_RGBA_PAINT
    case GRAPHIC_FORMAT_RGBA32:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_RGBA32:
                return &xGraphicConverter::Copy_GBRA32_Or_RGBA32;
                break;
            case GRAPHIC_FORMAT_BGRA32_PRE_MULT:
                return &xGraphicConverter::Tran_RGBA32_2_PreMult;
                break;
#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
            case GRAPHIC_FORMAT_BGR24:
                return &xGraphicConverter::Tran_RGBA32_2_BGR24;
                break;
#endif
            case GRAPHIC_FORMAT_RGB24:
                return &xGraphicConverter::Tran_RGBA32_2_RGB24;
                break;
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Tran_RGBA32_2_BGR565;
                break;
#if IF_GRAPHIC_SUPPORT_COLOR_KEY
            case GRAPHIC_FORMAT_BGR16_565_COLOR_KEY:
                return &xGraphicConverter::Tran_RGBA32_2_BGR565_ColorKey;
                break;
#endif
            default:
                break;
            }
        }
        break;
#endif
#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
    case GRAPHIC_FORMAT_BGR24:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGRA32:
                return &xGraphicConverter::Tran_BGR24_2_GBRA32;
                break;
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Tran_BGR24_2_BGR565;
                break;
            default:
                break;
            }
        }
        break;
#endif
    case GRAPHIC_FORMAT_RGB24:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGRA32:
                return &xGraphicConverter::Tran_RGB24_2_GBRA32;
                break;
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Tran_RGB24_2_BGR565;
                break;
            default:
                break;
            }
        }
        break;
    case GRAPHIC_FORMAT_BGR16_565:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGRA32:
                return &xGraphicConverter::Tran_BGR565_2_GBRA32;
                break;
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Copy_BGR565;
                break;
#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
            case GRAPHIC_FORMAT_BGR24:
                return &xGraphicConverter::Tran_BGR565_2_BGR24;
                break;
#endif
            case GRAPHIC_FORMAT_RGB24:
                return &xGraphicConverter::Tran_BGR565_2_RGB24;
                break;
            default:
                break;
            }
        }
        break;
#if IF_GRAPHIC_SUPPORT_COLOR_KEY
    case GRAPHIC_FORMAT_BGR16_565_COLOR_KEY:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Blend_BGR565_ColorKey;
                break;
            case GRAPHIC_FORMAT_BGRA32:
                return &xGraphicConverter::Tran_BGR565_ColorKey_2_BGRA32;
                break;
            default:
                break;
            }
        }
        break;
#endif
    default:
        break;
    }

    XWINDOW_ERR("undefine format transform %d->%d", nSrcFormat, nDstFormat);

    return NULL;
}

FUN_PIX_OPT xGraphicConverter::GetFunBlendPixOpt(int nSrcFormat, int nDstFormat)
{
    switch (nSrcFormat)
    {
    case GRAPHIC_FORMAT_BGRA32_PRE_MULT:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Blend_GBRA32_PreMult_2_BGR565;
                break;
            case GRAPHIC_FORMAT_BGRA32:
                return &xGraphicConverter::Blend_GBRA32_PreMult_2_GBRA32;
                break;
            default:
                break;
            }
        }
        break;
    case GRAPHIC_FORMAT_BGRA32:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Blend_GBRA32_2_BGR565;
                break;
            case GRAPHIC_FORMAT_BGRA32:
                return (FUN_PIX_OPT)&xGraphicOperate::alphaBlend;
                break;
            default:
                break;
            }
        }
        break;
#if IF_GRAPHIC_SUPPORT_RGBA_PAINT
    case GRAPHIC_FORMAT_RGBA32_PRE_MULT:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Blend_RGBA32_PreMult_2_BGR565;
                break;
            default:
                break;
            }
        }
        break;
    case GRAPHIC_FORMAT_RGBA32:
        {
            switch (nDstFormat)
            {
            case GRAPHIC_FORMAT_BGR16_565:
                return &xGraphicConverter::Blend_RGBA32_2_BGR565;
                break;
            default:
                break;
            }
        }
        break;
#endif
    default:
        break;
    }

    XWINDOW_ERR("undefine format blend %d->%d", nSrcFormat, nDstFormat);

    return NULL;
}
#endif

#if IF_GRAPHIC_ZOOM
//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphicScaleInfo
class xGraphicScaleInfo
{
    friend class xGraphicScale;

public:
    xGraphicScaleInfo();
    ~xGraphicScaleInfo();

    enum
    {
        GRAPHIC_SCALE_TYPE_NONE = 0,
        GRAPHIC_SCALE_TYPE_X = 1,
        GRAPHIC_SCALE_TYPE_Y = 2,
        GRAPHIC_SCALE_TYPE_XY = 3,
    };

    static xPixel** CalcYPoints(xPixel* src, int nStride, int nSrcLeft, int nSrcTop,
                                int nSrcHeight, int nDstHeight);
    static int* CalcXPoints(int nSrcWidth, int nDstWidth);
    static int* CalcApoints(int nSrc, int nDst, int up);

protected:
    void FreeScaleInfo();

protected:
    int* m_pXPoints;
    xPixel** m_ppYPoints;
    int* m_pXAPoints;
    int* m_pYAPoints;
    int m_nScaleType;
};

using namespace yl;

namespace
{
#ifndef WIN32
#ifndef __int64
typedef long long __int64;
#endif
#endif

#define INV_XAP                   (256 - xapoints[x])
#define XAP                       (xapoints[x])
#define INV_YAP                   (256 - yapoints[dyy + y])
#define YAP                       (yapoints[dyy + y])
}

xGraphicScaleInfo::xGraphicScaleInfo()
    : m_pXPoints(NULL)
    , m_ppYPoints(NULL)
    , m_pXAPoints(NULL)
    , m_pYAPoints(NULL)
    , m_nScaleType(GRAPHIC_SCALE_TYPE_NONE)
{

}

xGraphicScaleInfo::~xGraphicScaleInfo()
{

}

xPixel** xGraphicScaleInfo::CalcYPoints(xPixel* src, int nStride, int nSrcLeft, int nSrcTop,
                                        int nSrcHeight, int nDstHeight)
{
#if IF_DEBUG_GRAPHIC_SCALE
    XWINDOW_INFO("CalcYPoints xPixel:%p Stride:%d SrcLeft:%d SrcTop:%d SrcHeight:%d DstHeight:%d",
                 src, nStride, nSrcLeft, nSrcTop, nSrcHeight, nDstHeight);
#endif

    src = src + nSrcTop * nStride + nSrcLeft;

    xPixel** p = NULL;
    int i, j = 0, rv = 0;
    __int64 val, inc;

    if (nDstHeight < 0)
    {
        nDstHeight = -nDstHeight;
        rv = 1;
    }
    p = new xPixel*[nDstHeight + 1];

    int up = abs(nDstHeight) >= nSrcHeight;
    val = up ? 0x8000 * nSrcHeight / nDstHeight - 0x8000 : 0;
    inc = (((__int64)nSrcHeight) << 16) / nDstHeight;
    for (i = 0; i < nDstHeight; i++)
    {
        p[j] = src + max(0LL, val >> 16) * nStride;
        val += inc;
#if IF_DEBUG_GRAPHIC_SCALE
        XWINDOW_INFO("%d = %p", j, p[j]);
#endif
        j++;
    }
    if (rv)
    {
        for (i = nDstHeight / 2; --i >= 0;)
        {
            xPixel* tmp = p[i];
            p[i] = p[nDstHeight - i - 1];
            p[nDstHeight - i - 1] = tmp;
        }
    }
    return (p);
}

int* xGraphicScaleInfo::CalcXPoints(int nSrcWidth, int nDstWidth)
{
#if IF_DEBUG_GRAPHIC_SCALE
    XWINDOW_INFO("CalcXPoints SrcWidth:%d DstWidth:%d", nSrcWidth, nDstWidth);
#endif
    int *p, i, j = 0, rv = 0;
    __int64 val, inc;

    if (nDstWidth < 0)
    {
        nDstWidth = -nDstWidth;
        rv = 1;
    }
    p = new int[nDstWidth + 1];

    int up = abs(nDstWidth) >= nSrcWidth;
    val = up ? 0x8000 * nSrcWidth / nDstWidth - 0x8000 : 0;
    inc = (((__int64)nSrcWidth) << 16) / nDstWidth;
    for (i = 0; i < nDstWidth; i++)
    {
        p[j] = max(0LL, val >> 16);
        val += inc;
#if IF_DEBUG_GRAPHIC_SCALE
        XWINDOW_INFO("%d = %d", j, p[j]);
#endif
        j++;
    }

    if (rv)
    {
        for (i = nDstWidth / 2; --i >= 0;)
        {
            int tmp = p[i];
            p[i] = p[nDstWidth - i - 1];
            p[nDstWidth - i - 1] = tmp;
        }
    }
    return (p);
}

int* xGraphicScaleInfo::CalcApoints(int nSrc, int nDst, int up)
{
    int *p, i, j = 0, rv = 0;

    if (nDst < 0)
    {
        rv = 1;
        nDst = -nDst;
    }
    p = new int[nDst];

    /* scaling up */
    if (up)
    {
        __int64 val, inc;

        val = 0x8000 * nSrc / nDst - 0x8000;
        inc = (((__int64)nSrc) << 16) / nDst;
        for (i = 0; i < nDst; i++)
        {
            int pos = val >> 16;
            if (pos < 0)
            {
                p[j++] = 0;
            }
            else if (pos >= (nSrc - 1))
            {
                p[j++] = 0;
            }
            else
            {
                p[j++] = (val >> 8) - ((val >> 8) & 0xffffff00);
            }
            val += inc;
        }
    }
    /* scaling down */
    else
    {
        __int64 val, inc;
        int ap, Cp;
        val = 0;
        inc = (((__int64)nSrc) << 16) / nDst;
        Cp = ((nDst << 14) / nSrc) + 1;
        for (i = 0; i < nDst; i++)
        {
            ap = ((0x100 - ((val >> 8) & 0xff)) * Cp) >> 8;
            p[j] = ap | (Cp << 16);
            j++;
            val += inc;
        }
    }
    if (rv)
    {
        int tmp;
        for (i = nDst / 2; --i >= 0;)
        {
            tmp = p[i];
            p[i] = p[nDst - i - 1];
            p[nDst - i - 1] = tmp;
        }
    }
    return (p);
}

void xGraphicScaleInfo::FreeScaleInfo()
{
    if (NULL != m_pXPoints)
    {
        delete[] m_pXPoints;

        m_pXPoints = NULL;
    }

    if (NULL != m_ppYPoints)
    {
        delete[] m_ppYPoints;

        m_ppYPoints = NULL;
    }

    if (NULL != m_pXAPoints)
    {
        delete[] m_pXAPoints;

        m_pXAPoints = NULL;
    }

    if (NULL != m_pYAPoints)
    {
        delete[] m_pYAPoints;

        m_pYAPoints = NULL;
    }

    m_nScaleType = GRAPHIC_SCALE_TYPE_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphicScale
xGraphicScale::xGraphicScale()
{
}

xGraphicScale::~xGraphicScale()
{
}

xGraphicPtr xGraphicScale::ScaleGraphic(const xGraphic& src, const chRect& rcSrc,
                                        int nWidth, int nHeight
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
                                        , bool bAlpha
#endif
                                       )
{
    xPixel* pPixel = (xPixel*)src.m_pPixmap;

    if (NULL == pPixel)
    {
        return xGraphicPtr(NULL);
    }

    if (GRAPHIC_FORMAT_BGRA32 != src.m_nFormat)
    {
        XWINDOW_ERR("unsupport scale format:%d", src.m_nFormat);

        return xGraphicPtr(NULL);
    }

    xGraphic* pGraphic = new xGraphic;

    if (NULL == pGraphic)
    {
        return xGraphicPtr(NULL);
    }

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    int nFormat = GRAPHIC_FORMAT_BGR24;

    if (bAlpha)
    {
        nFormat = GRAPHIC_FORMAT_BGRA32;
    }

    pGraphic->SetFormat(nFormat);
#else
    pGraphic->SetFormat(GRAPHIC_FORMAT_BGRA32);
#endif

    if (!pGraphic->createGraphic(nWidth, nHeight))
    {
        delete pGraphic;

        return xGraphicPtr(NULL);
    }

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    if (ScaleGraphic(src, rcSrc, *pGraphic, bAlpha))
#else
    if (ScaleGraphic(src, rcSrc, *pGraphic))
#endif
    {
        return xGraphicPtr(pGraphic);
    }

    delete pGraphic;

    return xGraphicPtr(NULL);
}

bool xGraphicScale::ScaleGraphic(const xGraphic& src, const chRect& rcSrc, xGraphic& dst
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
                                 , bool bAlpha
#endif
                                )
{
    xPixel* pDstPixmap = (xPixel*)dst.m_pPixmap;
    int nWidth = dst.width();
    int nHeight = dst.height();

    if (NULL == pDstPixmap || nWidth <= 0 || nHeight <= 0)
    {
        return false;
    }

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    xGraphicScaleInfo* pScaleInfo = CalcScaleInfo(src, rcSrc, nWidth, nHeight, bAlpha);
#else
    xGraphicScaleInfo* pScaleInfo = CalcScaleInfo(src, rcSrc, nWidth, nHeight);
#endif

    if (NULL == pScaleInfo)
    {
        return false;
    }

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    if (bAlpha)
    {
        ScaleAARGBA(pScaleInfo, pDstPixmap, 0, 0, 0, 0, nWidth, nHeight, nWidth, src.width());
    }
    else
    {
        ScaleAARGB(pScaleInfo, pDstPixmap, 0, 0, 0, 0, nWidth, nHeight, nWidth, src.width());
    }
#else
    ScaleAARGBA(pScaleInfo, pDstPixmap, 0, 0, 0, 0, nWidth, nHeight, nWidth, src.width());
#endif

    FreeScaleInfo(pScaleInfo);

    return true;
}

xGraphicScaleInfo* xGraphicScale::FreeScaleInfo(xGraphicScaleInfo* pInfo)
{
    if (NULL != pInfo)
    {
        pInfo->FreeScaleInfo();

        delete pInfo;
    }

    return NULL;
}

xGraphicScaleInfo* xGraphicScale::CalcScaleInfo(const xGraphic& objGraphic, const chRect& rcSrc,
        int nDstWidth, int nDstHeight
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
        , bool bAlpha
#endif
                                               )
{
    if (rcSrc.IsEmpty())
    {
        return NULL;
    }

    xGraphicScaleInfo* pScaleInfo = NULL;
    int nSrcWidth = rcSrc.Width();
    int nSrcHeight = rcSrc.Height();

    pScaleInfo = new xGraphicScaleInfo;

    if (NULL == pScaleInfo)
    {
        return NULL;
    }

    if (abs(nDstWidth) >= nSrcWidth)
    {
        pScaleInfo->m_nScaleType |= xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_X;
    }

    if (abs(nDstHeight) >= nSrcHeight)
    {
        pScaleInfo->m_nScaleType |= xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_Y;
    }

    pScaleInfo->m_pXPoints = xGraphicScaleInfo::CalcXPoints(nSrcWidth, nDstWidth);

    if (NULL == pScaleInfo->m_pXPoints)
    {
        return FreeScaleInfo(pScaleInfo);
    }

    pScaleInfo->m_ppYPoints = xGraphicScaleInfo::CalcYPoints((xPixel*)objGraphic.m_pPixmap,
                              objGraphic.width(),
                              rcSrc.left,
                              rcSrc.top, nSrcHeight,
                              nDstHeight);

    if (NULL == pScaleInfo->m_ppYPoints)
    {
        return FreeScaleInfo(pScaleInfo);
    }

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    if (bAlpha)
    {
#endif
        bool bScaleX = (pScaleInfo->m_nScaleType & xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_X);

        pScaleInfo->m_pXAPoints = xGraphicScaleInfo::CalcApoints(nSrcWidth, nDstWidth, bScaleX);

        if (NULL == pScaleInfo->m_pXAPoints)
        {
            return FreeScaleInfo(pScaleInfo);
        }

        bool bScaleY = (pScaleInfo->m_nScaleType & xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_Y);

        pScaleInfo->m_pYAPoints = xGraphicScaleInfo::CalcApoints(nSrcHeight, nDstHeight, bScaleY);

        if (NULL == pScaleInfo->m_pYAPoints)
        {
            return FreeScaleInfo(pScaleInfo);
        }
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    }
#endif

    return pScaleInfo;
}

/* scale by area sampling */
void xGraphicScale::ScaleAARGBA(xGraphicScaleInfo* pScaleInfo, xPixel* dest, int dxx, int dyy,
                                int dx, int dy, int dw, int dh, int dow, int sow)
{
    if (NULL == pScaleInfo || NULL == dest)
    {
        return;
    }

#if IF_DEBUG_GRAPHIC_SCALE
    XWINDOW_INFO("ScaleAARGB ScaleInfo:%p dest:%p dxx:%d dyy:%d dx:%d dy:%d dw:%d dh:%d dow:%d sow:%d",
                 pScaleInfo, dest, dxx, dyy, dx, dy, dw, dh, dow, sow);
#endif
    xPixel* sptr = NULL;
    xPixel* dptr = NULL;
    int x, y, end;
    xPixel** ypoints = pScaleInfo->m_ppYPoints;
    int* xpoints = pScaleInfo->m_pXPoints;
    int* xapoints = pScaleInfo->m_pXAPoints;
    int* yapoints = pScaleInfo->m_pYAPoints;

    end = dxx + dw;
    /* scaling up both ways */
    if (xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_XY == pScaleInfo->m_nScaleType)
    {
        /* go through every scanline in the output buffer */
        for (y = 0; y < dh; y++)
        {
            /* calculate the source line we'll scan from */
            dptr = dest + dx + ((y + dy) * dow);
            sptr = ypoints[dyy + y];
            if (YAP > 0)
            {
                for (x = dxx; x < end; x++)
                {
                    int r, g, b, a;
                    int rr, gg, bb, aa;
                    xPixel* pix = NULL;

                    if (XAP > 0)
                    {
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = pix->r * INV_XAP;
                        g = pix->g * INV_XAP;
                        b = pix->b * INV_XAP;
                        a = pix->a * INV_XAP;
                        pix++;
                        r += pix->r * XAP;
                        g += pix->g * XAP;
                        b += pix->b * XAP;
                        a += pix->a * XAP;
                        pix += sow;
                        rr = pix->r * XAP;
                        gg = pix->g * XAP;
                        bb = pix->b * XAP;
                        aa = pix->a * XAP;
                        pix--;
                        rr += pix->r * INV_XAP;
                        gg += pix->g * INV_XAP;
                        bb += pix->b * INV_XAP;
                        aa += pix->a * INV_XAP;
                        r = ((rr * YAP) + (r * INV_YAP)) >> 16;
                        g = ((gg * YAP) + (g * INV_YAP)) >> 16;
                        b = ((bb * YAP) + (b * INV_YAP)) >> 16;
                        a = ((aa * YAP) + (a * INV_YAP)) >> 16;
                        *dptr = xPixel(a, r, g, b);
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b, dptr->a);
#endif
                        ++dptr;
                    }
                    else
                    {
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = pix->r * INV_YAP;
                        g = pix->g * INV_YAP;
                        b = pix->b * INV_YAP;
                        a = pix->a * INV_YAP;
                        pix += sow;
                        r += pix->r * YAP;
                        g += pix->g * YAP;
                        b += pix->b * YAP;
                        a += pix->a * YAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        a >>= 8;
                        *dptr = xPixel(a, r, g, b);
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b, dptr->a);
#endif
                        ++dptr;
                    }
                }
            }
            else
            {
                for (x = dxx; x < end; x++)
                {
                    int r, g, b, a;
                    xPixel* pix = NULL;;

                    if (XAP > 0)
                    {
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = pix->r * INV_XAP;
                        g = pix->g * INV_XAP;
                        b = pix->b * INV_XAP;
                        a = pix->a * INV_XAP;
                        pix++;
                        r += pix->r * XAP;
                        g += pix->g * XAP;
                        b += pix->b * XAP;
                        a += pix->a * XAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        a >>= 8;
                        *dptr = xPixel(a, r, g, b);
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b, dptr->a);
#endif
                        dptr++;
                    }
                    else
                    {
                        *dptr = sptr[xpoints[x]];
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b, dptr->a);
#endif
                        dptr++;
                    }
                }
            }
        }
    }
    /* if we're scaling down vertically */
    else if (xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_X == pScaleInfo->m_nScaleType)
    {
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cy, j;
        xPixel* pix;
        int r, g, b, a, rr, gg, bb, aa;
        int yap;

        /* go through every scanline in the output buffer */
        for (y = 0; y < dh; y++)
        {
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for (x = dxx; x < end; x++)
            {
                pix = ypoints[dyy + y] + xpoints[x];
                r = pix->r * yap;
                g = pix->g * yap;
                b = pix->b * yap;
                a = pix->a * yap;
                for (j = (1 << 14) - yap; j > Cy; j -= Cy)
                {
                    pix += sow;
                    r += pix->r * Cy;
                    g += pix->g * Cy;
                    b += pix->b * Cy;
                    a += pix->a * Cy;
                }
                if (j > 0)
                {
                    pix += sow;
                    r += pix->r * j;
                    g += pix->g * j;
                    b += pix->b * j;
                    a += pix->a * j;
                }
                if (XAP > 0)
                {
                    pix = ypoints[dyy + y] + xpoints[x] + 1;
                    rr = pix->r * yap;
                    gg = pix->g * yap;
                    bb = pix->b * yap;
                    aa = pix->a * yap;
                    for (j = (1 << 14) - yap; j > Cy; j -= Cy)
                    {
                        pix += sow;
                        rr += pix->r * Cy;
                        gg += pix->g * Cy;
                        bb += pix->b * Cy;
                        aa += pix->a * Cy;
                    }
                    if (j > 0)
                    {
                        pix += sow;
                        rr += pix->r * j;
                        gg += pix->g * j;
                        bb += pix->b * j;
                        aa += pix->a * j;
                    }
                    r = r * INV_XAP;
                    g = g * INV_XAP;
                    b = b * INV_XAP;
                    a = a * INV_XAP;
                    r = (r + ((rr * XAP))) >> 12;
                    g = (g + ((gg * XAP))) >> 12;
                    b = (b + ((bb * XAP))) >> 12;
                    a = (a + ((aa * XAP))) >> 12;
                }
                else
                {
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                    a >>= 4;
                }
                *dptr = xPixel(a >> 10, r >> 10, g >> 10, b >> 10);
#if IF_DEBUG_GRAPHIC_SCALE
                XWINDOW_INFO("%p = 0x%02x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b, dptr->a);
#endif
                dptr++;
            }
        }
    }
    /* if we're scaling down horizontally */
    else if (xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_Y == pScaleInfo->m_nScaleType)
    {
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cx, j;
        xPixel* pix;
        int r, g, b, a, rr, gg, bb, aa;
        int xap;

        /* go through every scanline in the output buffer */
        for (y = 0; y < dh; y++)
        {
            dptr = dest + dx + ((y + dy) * dow);
            for (x = dxx; x < end; x++)
            {
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                pix = ypoints[dyy + y] + xpoints[x];
                r = pix->r * xap;
                g = pix->g * xap;
                b = pix->b * xap;
                a = pix->a * xap;
                for (j = (1 << 14) - xap; j > Cx; j -= Cx)
                {
                    pix++;
                    r += pix->r * Cx;
                    g += pix->g * Cx;
                    b += pix->b * Cx;
                    a += pix->a * Cx;
                }
                if (j > 0)
                {
                    pix++;
                    r += pix->r * j;
                    g += pix->g * j;
                    b += pix->b * j;
                    a += pix->a * j;
                }
                if (YAP > 0)
                {
                    pix = ypoints[dyy + y] + xpoints[x] + sow;
                    rr = pix->r * xap;
                    gg = pix->g * xap;
                    bb = pix->b * xap;
                    aa = pix->a * xap;
                    for (j = (1 << 14) - xap; j > Cx; j -= Cx)
                    {
                        pix++;
                        rr += pix->r * Cx;
                        gg += pix->g * Cx;
                        bb += pix->b * Cx;
                        aa += pix->a * Cx;
                    }
                    if (j > 0)
                    {
                        pix++;
                        rr += pix->r * j;
                        gg += pix->g * j;
                        bb += pix->b * j;
                        aa += pix->a * j;
                    }
                    r = r * INV_YAP;
                    g = g * INV_YAP;
                    b = b * INV_YAP;
                    a = a * INV_YAP;
                    r = (r + ((rr * YAP))) >> 12;
                    g = (g + ((gg * YAP))) >> 12;
                    b = (b + ((bb * YAP))) >> 12;
                    a = (a + ((aa * YAP))) >> 12;
                }
                else
                {
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                    a >>= 4;
                }
                *dptr = xPixel(a >> 10, r >> 10, g >> 10, b >> 10);
#if IF_DEBUG_GRAPHIC_SCALE
                XWINDOW_INFO("%p = 0x%02x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b, dptr->a);
#endif
                dptr++;
            }
        }
    }
    /* if we're scaling down horizontally & vertically */
    else
    {
        /*\ 'Correct' version, with math units prepared for MMXification:
        |*|  The operation 'b = (b * c) >> 16' translates to pmulhw,
        |*|  so the operation 'b = (b * c) >> d' would translate to
        |*|  psllw (16 - d), %mmb; pmulh %mmc, %mmb
        \*/
        int Cx, Cy, i, j;
        xPixel* pix;
        int a, r, g, b, ax, rx, gx, bx;
        int xap, yap;

        for (y = 0; y < dh; y++)
        {
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for (x = dxx; x < end; x++)
            {
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                sptr = ypoints[dyy + y] + xpoints[x];
                pix = sptr;
                sptr += sow;
                rx = pix->r * xap;
                gx = pix->g * xap;
                bx = pix->b * xap;
                ax = pix->a * xap;

                pix++;
                for (i = (1 << 14) - xap; i > Cx; i -= Cx)
                {
                    rx += pix->r * Cx;
                    gx += pix->g * Cx;
                    bx += pix->b * Cx;
                    ax += pix->a * Cx;
                    pix++;
                }
                if (i > 0)
                {
                    rx += pix->r * i;
                    gx += pix->g * i;
                    bx += pix->b * i;
                    ax += pix->a * i;
                }

                r = (rx >> 5) * yap;
                g = (gx >> 5) * yap;
                b = (bx >> 5) * yap;
                a = (ax >> 5) * yap;

                for (j = (1 << 14) - yap; j > Cy; j -= Cy)
                {
                    pix = sptr;
                    sptr += sow;
                    rx = pix->r * xap;
                    gx = pix->g * xap;
                    bx = pix->b * xap;
                    ax = pix->a * xap;
                    pix++;
                    for (i = (1 << 14) - xap; i > Cx; i -= Cx)
                    {
                        rx += pix->r * Cx;
                        gx += pix->g * Cx;
                        bx += pix->b * Cx;
                        ax += pix->a * Cx;
                        pix++;
                    }
                    if (i > 0)
                    {
                        rx += pix->r * i;
                        gx += pix->g * i;
                        bx += pix->b * i;
                        ax += pix->a * i;
                    }

                    r += (rx >> 5) * Cy;
                    g += (gx >> 5) * Cy;
                    b += (bx >> 5) * Cy;
                    a += (ax >> 5) * Cy;
                }
                if (j > 0)
                {
                    pix = sptr;
                    sptr += sow;
                    rx = pix->r * xap;
                    gx = pix->g * xap;
                    bx = pix->b * xap;
                    ax = pix->a * xap;
                    pix++;
                    for (i = (1 << 14) - xap; i > Cx; i -= Cx)
                    {
                        rx += pix->r * Cx;
                        gx += pix->g * Cx;
                        bx += pix->b * Cx;
                        ax += pix->a * Cx;
                        pix++;
                    }
                    if (i > 0)
                    {
                        rx += pix->r * i;
                        gx += pix->g * i;
                        bx += pix->b * i;
                        ax += pix->a * i;
                    }

                    r += (rx >> 5) * j;
                    g += (gx >> 5) * j;
                    b += (bx >> 5) * j;
                    a += (ax >> 5) * j;
                }

                *dptr = xPixel(a >> 23, r >> 23, g >> 23, b >> 23);
#if IF_DEBUG_GRAPHIC_SCALE
                XWINDOW_INFO("%p = 0x%02x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b, dptr->a);
#endif
                dptr++;
            }
        }
    }
}

#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
/* scale by area sampling - IGNORE the ALPHA byte*/
void xGraphicScale::ScaleAARGB(xGraphicScaleInfo* pScaleInfo, xPixel* dest, int dxx, int dyy,
                               int dx, int dy, int dw, int dh, int dow, int sow)
{
    if (NULL == pScaleInfo || NULL == dest)
    {
        return;
    }

#if IF_DEBUG_GRAPHIC_SCALE
    XWINDOW_INFO("ScaleAARGB ScaleInfo:%p dest:%p dxx:%d dyy:%d dx:%d dy:%d dw:%d dh:%d dow:%d sow:%d",
                 pScaleInfo, dest, dxx, dyy, dx, dy, dw, dh, dow, sow);
#endif

    xPixel* sptr = NULL;
    xPixel* dptr = NULL;
    int x, y, end;
    xPixel** ypoints = pScaleInfo->m_ppYPoints;
    int *xpoints = pScaleInfo->m_pXPoints;
    int *xapoints = pScaleInfo->m_pXAPoints;
    int *yapoints = pScaleInfo->m_pYAPoints;
    end = dxx + dw;
    /* scaling up both ways */
    if (xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_XY == pScaleInfo->m_nScaleType)
    {
        /* go through every scanline in the output buffer */
        for (y = 0; y < dh; y++)
        {
            /* calculate the source line we'll scan from */
            dptr = dest + dx + ((y + dy) * dow);
            sptr = ypoints[dyy + y];
            if (YAP > 0)
            {
                for (x = dxx; x < end; x++)
                {
                    int r = 0, g = 0, b = 0;
                    int rr = 0, gg = 0, bb = 0;
                    xPixel* pix = NULL;

                    if (XAP > 0)
                    {
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = pix->r * INV_XAP;
                        g = pix->g * INV_XAP;
                        b = pix->b * INV_XAP;
                        pix++;
                        r += pix->r * XAP;
                        g += pix->g * XAP;
                        b += pix->b * XAP;
                        pix += sow;
                        rr = pix->r * XAP;
                        gg = pix->g * XAP;
                        bb = pix->b * XAP;
                        pix--;
                        rr += pix->r * INV_XAP;
                        gg += pix->g * INV_XAP;
                        bb += pix->b * INV_XAP;
                        r = ((rr * YAP) + (r * INV_YAP)) >> 16;
                        g = ((gg * YAP) + (g * INV_YAP)) >> 16;
                        b = ((bb * YAP) + (b * INV_YAP)) >> 16;
                        *dptr = xPixel(r, g, b);
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b);
#endif
                        dptr++;
                    }
                    else
                    {
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = pix->r * INV_YAP;
                        g = pix->g * INV_YAP;
                        b = pix->b * INV_YAP;
                        pix += sow;
                        r += pix->r * YAP;
                        g += pix->g * YAP;
                        b += pix->b * YAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        *dptr = xPixel(r, g, b);
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b);
#endif
                        dptr++;
                    }
                }
            }
            else
            {
                for (x = dxx; x < end; x++)
                {
                    int r = 0, g = 0, b = 0;
                    xPixel* pix;

                    if (XAP > 0)
                    {
                        pix = ypoints[dyy + y] + xpoints[x];
                        r = pix->r * INV_XAP;
                        g = pix->g * INV_XAP;
                        b = pix->b * INV_XAP;
                        pix++;
                        r += pix->r * XAP;
                        g += pix->g * XAP;
                        b += pix->b * XAP;
                        r >>= 8;
                        g >>= 8;
                        b >>= 8;
                        *dptr = xPixel(r, g, b);
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b);
#endif
                        dptr++;
                    }
                    else
                    {
                        *dptr = sptr[xpoints[x]];
#if IF_DEBUG_GRAPHIC_SCALE
                        XWINDOW_INFO("%p = 0x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b);
#endif
                        dptr++;
                    }
                }
            }
        }
    }
    /* if we're scaling down vertically */
    else if (xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_X == pScaleInfo->m_nScaleType)
    {
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cy, j;
        xPixel* pix = NULL;
        int r, g, b, rr, gg, bb;
        int yap;

        /* go through every scanline in the output buffer */
        for (y = 0; y < dh; y++)
        {
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for (x = dxx; x < end; x++)
            {
                pix = ypoints[dyy + y] + xpoints[x];
                r = pix->r * yap;
                g = pix->g * yap;
                b = pix->b * yap;
                pix += sow;
                for (j = (1 << 14) - yap; j > Cy; j -= Cy)
                {
                    r += pix->r * Cy;
                    g += pix->g * Cy;
                    b += pix->b * Cy;
                    pix += sow;
                }
                if (j > 0)
                {
                    r += pix->r * j;
                    g += pix->g * j;
                    b += pix->b * j;
                }
                if (XAP > 0)
                {
                    pix = ypoints[dyy + y] + xpoints[x] + 1;
                    rr = pix->r * yap;
                    gg = pix->g * yap;
                    bb = pix->b * yap;
                    pix += sow;
                    for (j = (1 << 14) - yap; j > Cy; j -= Cy)
                    {
                        rr += pix->r * Cy;
                        gg += pix->g * Cy;
                        bb += pix->b * Cy;
                        pix += sow;
                    }
                    if (j > 0)
                    {
                        rr += pix->r * j;
                        gg += pix->g * j;
                        bb += pix->b * j;
                    }
                    r = r * INV_XAP;
                    g = g * INV_XAP;
                    b = b * INV_XAP;
                    r = (r + ((rr * XAP))) >> 12;
                    g = (g + ((gg * XAP))) >> 12;
                    b = (b + ((bb * XAP))) >> 12;
                }
                else
                {
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                }
                *dptr = xPixel(r >> 10, g >> 10, b >> 10);
#if IF_DEBUG_GRAPHIC_SCALE
                XWINDOW_INFO("%p = 0x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b);
#endif
                dptr++;
            }
        }
    }
    /* if we're scaling down horizontally */
    else if (xGraphicScaleInfo::GRAPHIC_SCALE_TYPE_Y == pScaleInfo->m_nScaleType)
    {
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cx, j;
        xPixel* pix = NULL;
        int r, g, b, rr, gg, bb;
        int xap;

        /* go through every scanline in the output buffer */
        for (y = 0; y < dh; y++)
        {
            dptr = dest + dx + ((y + dy) * dow);
            for (x = dxx; x < end; x++)
            {
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                pix = ypoints[dyy + y] + xpoints[x];
                r = pix->r * xap;
                g = pix->g * xap;
                b = pix->b * xap;
                pix++;
                for (j = (1 << 14) - xap; j > Cx; j -= Cx)
                {
                    r += pix->r * Cx;
                    g += pix->g * Cx;
                    b += pix->b * Cx;
                    pix++;
                }
                if (j > 0)
                {
                    r += pix->r * j;
                    g += pix->g * j;
                    b += pix->b * j;
                }
                if (YAP > 0)
                {
                    pix = ypoints[dyy + y] + xpoints[x] + sow;
                    rr = pix->r * xap;
                    gg = pix->g * xap;
                    bb = pix->b * xap;
                    pix++;
                    for (j = (1 << 14) - xap; j > Cx; j -= Cx)
                    {
                        rr += pix->r * Cx;
                        gg += pix->g * Cx;
                        bb += pix->b * Cx;
                        pix++;
                    }
                    if (j > 0)
                    {
                        rr += pix->r * j;
                        gg += pix->g * j;
                        bb += pix->b * j;
                    }
                    r = r * INV_YAP;
                    g = g * INV_YAP;
                    b = b * INV_YAP;
                    r = (r + ((rr * YAP))) >> 12;
                    g = (g + ((gg * YAP))) >> 12;
                    b = (b + ((bb * YAP))) >> 12;
                }
                else
                {
                    r >>= 4;
                    g >>= 4;
                    b >>= 4;
                }
                *dptr = xPixel(r >> 10, g >> 10, b >> 10);
#if IF_DEBUG_GRAPHIC_SCALE
                XWINDOW_INFO("%p = 0x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b);
#endif
                dptr++;
            }
        }
    }
    /* fully optimized (i think) - onyl change of algorithm can help */
    /* if we're scaling down horizontally & vertically */
    else
    {
        /*\ 'Correct' version, with math units prepared for MMXification \*/
        int Cx, Cy, i, j;
        xPixel* pix;
        int r, g, b, rx, gx, bx;
        int xap, yap;

        for (y = 0; y < dh; y++)
        {
            Cy = YAP >> 16;
            yap = YAP & 0xffff;

            dptr = dest + dx + ((y + dy) * dow);
            for (x = dxx; x < end; x++)
            {
                Cx = XAP >> 16;
                xap = XAP & 0xffff;

                sptr = ypoints[dyy + y] + xpoints[x];
                pix = sptr;
                sptr += sow;
                rx = pix->r * xap;
                gx = pix->g * xap;
                bx = pix->b * xap;
                pix++;
                for (i = (1 << 14) - xap; i > Cx; i -= Cx)
                {
                    rx += pix->r * Cx;
                    gx += pix->g * Cx;
                    bx += pix->b * Cx;
                    pix++;
                }
                if (i > 0)
                {
                    rx += pix->r * i;
                    gx += pix->g * i;
                    bx += pix->b * i;
                }

                r = (rx >> 5) * yap;
                g = (gx >> 5) * yap;
                b = (bx >> 5) * yap;

                for (j = (1 << 14) - yap; j > Cy; j -= Cy)
                {
                    pix = sptr;
                    sptr += sow;
                    rx = pix->r * xap;
                    gx = pix->g * xap;
                    bx = pix->b * xap;
                    pix++;
                    for (i = (1 << 14) - xap; i > Cx; i -= Cx)
                    {
                        rx += pix->r * Cx;
                        gx += pix->g * Cx;
                        bx += pix->b * Cx;
                        pix++;
                    }
                    if (i > 0)
                    {
                        rx += pix->r * i;
                        gx += pix->g * i;
                        bx += pix->b * i;
                    }

                    r += (rx >> 5) * Cy;
                    g += (gx >> 5) * Cy;
                    b += (bx >> 5) * Cy;
                }
                if (j > 0)
                {
                    pix = sptr;
                    sptr += sow;
                    rx = pix->r * xap;
                    gx = pix->g * xap;
                    bx = pix->b * xap;
                    pix++;
                    for (i = (1 << 14) - xap; i > Cx; i -= Cx)
                    {
                        rx += pix->r * Cx;
                        gx += pix->g * Cx;
                        bx += pix->b * Cx;
                        pix++;
                    }
                    if (i > 0)
                    {
                        rx += pix->r * i;
                        gx += pix->g * i;
                        bx += pix->b * i;
                    }

                    r += (rx >> 5) * j;
                    g += (gx >> 5) * j;
                    b += (bx >> 5) * j;
                }

                *dptr = xPixel(r >> 23, g >> 23, b >> 23);
#if IF_DEBUG_GRAPHIC_SCALE
                XWINDOW_INFO("%p = 0x%02x%02x%02x", dptr, dptr->r, dptr->g, dptr->b);
#endif
                dptr++;
            }
        }
    }
}
#endif
#endif
#endif
