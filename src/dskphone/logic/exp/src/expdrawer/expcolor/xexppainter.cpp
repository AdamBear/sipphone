#include "xexppainter.h"
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include "common/xwindow/modfreetype.cpp"


void xEXPPainter::drawItemPixmap(const chRect& rect, xPixmap& pixmap)
{
    if (pixmap.isNull())
    {
        return;
    }
    drawItemPixmapEx(rect, pixmap, chRect(0, 0, pixmap.width(), pixmap.height()));
}


void xEXPPainter::drawItemPixmapEx(const chRect& rect, xPixmap& pixmap, const chRect& srcRect)
{
    if (pixmap.isNull() || rect.IsEmpty())
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);
    //m_pScreen->DrawPixmap(rcScreen, pixmap, srcRect);

    DrawRawPixmap(pixmap, srcRect, rcScreen);

}


void xEXPPainter::DrawRawPixmap(xPixmap& pixmap, const chRect& rcPixmap, const chRect& rcDst
#if IF_GRAPHIC_ZOOM
                                , bool bZoomTile /*= false*/
#endif
                               )
{
    if (pixmap.isNull())
    {
        return;
    }

    xGraphic * pGraphic = pixmap.GetGraphic().Get();

    if (rcPixmap.Width() == rcDst.Width() && rcPixmap.Height() == rcDst.Height())
    {
#warning blendFrom+ copyFrom
//#if IF_PIXMAP_MARK_ALPHA_CHANNEL
//        if (pixmap.hasAlphaChannel())
//        {
//            {
//                m_pGraphic->blendFrom(*pGraphic, rcPixmap, chPoint(rcDst.left, rcDst.top), m_rgnClip);
//            }
//            return;
//        }
//#endif
//        m_pGraphic->copyFrom(*pGraphic, rcPixmap, chPoint(rcDst.left, rcDst.top), m_rgnClip);
        return;


    }

#warning tileZoomFrom + zoomFrom
//#if IF_GRAPHIC_ZOOM
//    bool bAlpha = false;
//
//#if IF_PIXMAP_MARK_ALPHA_CHANNEL
//    bAlpha = pixmap.hasAlphaChannel();
//#endif
//    if (bZoomTile)
//    {
//        m_pGraphic->tileZoomFrom(*pGraphic, rcPixmap, rcDst, m_rgnClip);
//    }
//    else
//    {
//        m_pGraphic->zoomFrom(*pGraphic, bAlpha, rcPixmap, rcDst, m_rgnClip);
//    }
//#endif
}


bool xEXPPainter::DrawDot9Pixmap(xPixmap& pixmap, const chRect& rect)
{
    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);
    xDot9Info* pDot9Info = pixmap.GetDot9Info();

    if (NULL == m_pGraphic || rcScreen.IsEmpty() || NULL == pDot9Info || !pDot9Info->IsStretch())
    {
        return false;
    }

    const xGraphicPtr& pData = pixmap.GetGraphic();
    const xGraphic* pGraphic = pData.Get();

    if (NULL == pGraphic)
    {
        return false;
    }

    bool bHorization = pDot9Info->IsHorizontalStretch();
    bool bVertical = pDot9Info->IsVerticalStretch();
    bool bCanTile = pDot9Info->IsCanTile();

#if IF_PIXMAP_PARTLY_SHARED
    chRect rcPixmap = pixmap.GetPixmapRect();
#else
    chRect rcPixmap = rectLTWH(0, 0, pGraphic->width(), pGraphic->height());
#endif

    chRect rcSrc;
    chRect rcDst;

    if (bHorization && bVertical)
    {
        int arrSrcX[4] = { rcPixmap.left + 1, rcPixmap.left + pDot9Info->GetBeginH(),
                           rcPixmap.left + pDot9Info->GetEndH(), rcPixmap.right
                         };
        int arrSrcY[4] = { rcPixmap.top + 1, rcPixmap.top + pDot9Info->GetBeginV(),
                           rcPixmap.top + pDot9Info->GetEndV(), rcPixmap.bottom
                         };
        int arrDstX[4] = { rcScreen.left, rcScreen.left + pDot9Info->GetBeginH() - 1,
                           rcScreen.right - (arrSrcX[3] - arrSrcX[2]) + 1, rcScreen.right
                         };
        int arrDstY[4] = { rcScreen.top, rcScreen.top + pDot9Info->GetBeginV() - 1,
                           rcScreen.bottom - (arrSrcY[3] - arrSrcY[2]) + 1, rcScreen.bottom
                         };

        for (int i = 0; i < 3; ++i)
        {
            rcSrc.top = arrSrcY[i];
            rcSrc.bottom = arrSrcY[i + 1];
            rcDst.top = arrDstY[i];
            rcDst.bottom = arrDstY[i + 1];

            for (int j = 0; j < 3; ++j)
            {
                rcSrc.left = arrSrcX[j];
                rcSrc.right = arrSrcX[j + 1];
                rcDst.left = arrDstX[j];
                rcDst.right = arrDstX[j + 1];

                DrawRawPixmap(pixmap, rcSrc, rcDst, bCanTile);
            }
        }

        return true;
    }

    if (bHorization)
    {
        rcPixmap.top += 1;
        rcPixmap.bottom -= 1;

        int arrSrcX[4] = { rcPixmap.left + 1, rcPixmap.left + pDot9Info->GetBeginH(),
                           rcPixmap.left + pDot9Info->GetEndH(), rcPixmap.right
                         };
        int arrDstX[4] = { rcScreen.left, rcScreen.left + pDot9Info->GetBeginH() - 1,
                           rcScreen.right - (arrSrcX[3] - arrSrcX[2]) + 1, rcScreen.right
                         };

        rcSrc.top = rcPixmap.top;
        rcSrc.bottom = rcPixmap.bottom;
        rcDst.top = rcScreen.top;
        rcDst.bottom = rcScreen.bottom;

        for (int i = 0; i < 3; ++i)
        {
            rcSrc.left = arrSrcX[i];
            rcSrc.right = arrSrcX[i + 1];
            rcDst.left = arrDstX[i];
            rcDst.right = arrDstX[i + 1];

            DrawRawPixmap(pixmap, rcSrc, rcDst, bCanTile);
        }

        return true;
    }

    rcPixmap.left += 1;
    rcPixmap.right -= 1;

    int arrSrcY[4] = { rcPixmap.top + 1, rcPixmap.top + pDot9Info->GetBeginV(),
                       rcPixmap.top + pDot9Info->GetEndV(), rcPixmap.bottom
                     };
    int arrDstY[4] = { rcScreen.top, rcScreen.top + pDot9Info->GetBeginV() - 1,
                       rcScreen.bottom - (arrSrcY[3] - arrSrcY[2]) + 1, rcScreen.bottom
                     };

    rcSrc.left = rcPixmap.left;
    rcSrc.right = rcPixmap.right;
    rcDst.left = rcScreen.left;
    rcDst.right = rcScreen.right;

    for (int i = 0; i < 3; ++i)
    {
        rcSrc.top = arrSrcY[i];
        rcSrc.bottom = arrSrcY[i + 1];
        rcDst.top = arrDstY[i];
        rcDst.bottom = arrDstY[i + 1];

        DrawRawPixmap(pixmap, rcSrc, rcDst, bCanTile);
    }

    return true;
}


void xEXPPainter::drawText(const chRect& rcTextBound, const char* pszText, int nAlign, const xFont& font, const xColor& color)
{
    if (NULL == m_pGraphic || rcTextBound.IsEmpty())
    {
        return;
    }
    chRect rcScreen = OffsetRect(rcTextBound, m_ptViewOffset.x, m_ptViewOffset.y);

    if (NULL == pszText || strlen(pszText) <= 0)
    {
        return;
    }

#if IF_USE_DOB_FONT
    dobFont* pDobFont = font.GetDobFont();

    if (NULL != pDobFont)
    {
        chSize szScreen(m_nWidthPixels, m_nHeightPixels);
        const typeRectList& listRect = m_rgnClip.GetRectList();

        for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
        {
            pDobFont->layoutText32Bound(rcScreen, pszText, nAlign, color, (LPVOID)pixels(), szScreen, *it);
        }

        return;
    }
#endif

#if IF_USE_FREETYPE_LIB
    FreeType_DrawText(*m_pGraphic, rcScreen, pszText, nAlign, font, color, m_rgnClip);
#endif
}

void xEXPPainter::fillRect(const chRect& rect, const xColor& color)
{
    if (NULL == m_pGraphic || rect.IsEmpty())
    {
        return;
    }
    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

#warning fillRect
    //m_pGraphic->fillRect(rcScreen, color, m_rgnClip);
}


void xEXPPainter::blendRect(const chRect& rect, xPixmap& pixmap)
{
    if (NULL == m_pGraphic || rect.IsEmpty())
    {
        return;
    }
    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

#if FB_DEBUG
    XWINDOW_INFO("BlendRect rect:%d %d %d %d pixmap:%p", rcScreen.left, rcScreen.top, rcScreen.right, rcScreen.bottom, &pixmap);
#endif
#if IF_PIXMAP_SUPPORT_DOT_9
    if (DrawDot9Pixmap(pixmap, rcScreen))
    {
        return;
    }
#endif

    const xGraphicPtr& pData = pixmap.GetGraphic();
    const xGraphic* pGraphic = pData.Get();

    if (NULL == pGraphic || rect.IsEmpty())
    {
        return;
    }

#if IF_PIXMAP_PARTLY_SHARED
    const chRect& rcPixmap = pixmap.GetPixmapRect();
#else
    chRect rcPixmap(0, 0, pGraphic->width(), pGraphic->height());
#endif

    DrawRawPixmap(pixmap, rcPixmap, rect);
}

void xEXPPainter::drawRect(const chRect& rect, const xColor& color)
{
    if (NULL == m_pGraphic || rect.IsEmpty())
    {
        return;
    }
    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    chPoint ptTopLeft(rcScreen.left, rcScreen.top);
    chPoint ptTopRight(rcScreen.right, rcScreen.top);
    chPoint ptBottomLeft(rcScreen.left, rcScreen.bottom);
    chPoint ptBottomRight(rcScreen.right, rcScreen.bottom);

    drawLine(rcScreen.left, rcScreen.top, rcScreen.right, rcScreen.top, color);
    drawLine(rcScreen.left, rcScreen.top, rcScreen.left, rcScreen.bottom, color);
    drawLine(rcScreen.right, rcScreen.top, rcScreen.right, rcScreen.bottom, color);

    drawLine(rcScreen.left, rcScreen.bottom, rcScreen.right, rcScreen.bottom, color);
}

void xEXPPainter::drawLine(int x1, int y1, int x2, int y2, const xColor& color)
{
    if (NULL == m_pGraphic)
    {
        return;
    }

    int nX1 = chMIN(x1, x2) + m_ptViewOffset.x;
    int nX2 = chMAX(x1, x2) + m_ptViewOffset.x;
    int nY1 = chMIN(y1, y2) + m_ptViewOffset.y;
    int nY2 = chMAX(y1, y2) + m_ptViewOffset.y;
    chPoint ptBegin(nX1, nY1);
    chPoint ptEnd(nX2, nY2);

#warning blendLine_Y
    //if (ptBegin.x == ptEnd.x)
    //{
    //    m_pGraphic->blendLine_Y(ptBegin.x, ptBegin.y, ptEnd.y, color, m_rgnClip);
    //}
    //else if (ptBegin.y == ptEnd.y)
    //{
    //    m_pGraphic->blendLine_X(ptBegin.y, ptBegin.x, ptEnd.x, color, m_rgnClip);
    //}
}

xEXPPainter::xEXPPainter(xGraphic* pGraphic, const chPoint& ptOffset)
    : m_pGraphic(pGraphic)
    , m_ptViewOffset(ptOffset)
{
    if (m_pGraphic != NULL)
    {
        m_rgnClip = chRect(0, 0, m_pGraphic->width(), m_pGraphic->height());
    }
}

xEXPPainter::~xEXPPainter()
{
    if (m_pGraphic != NULL)
    {
        m_pGraphic == NULL;
    }
}
