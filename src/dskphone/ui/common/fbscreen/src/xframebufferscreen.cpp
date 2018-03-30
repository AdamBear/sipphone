#include "xframebufferscreen.h"
#if IF_USE_FREETYPE_LIB
#include "xwindow/modfreetype.h"
#endif
#if IF_USE_DOB_FONT
#include "xwindow/gDobFont.h"
#endif
#include "xwindow/xgraphicproess.h"

#include "dsklog/log.h"
#include "ylstringalgorithm.h"


xFrameBufferScreen::xFrameBufferScreen()
    : m_pScreenDevice(NULL)
#if IF_CUSTOM_SCREEN_DEVICE
    , m_bCustomDevice(false)
#endif
#if !IF_SCREEN_SUPPORT_DOUBLE_BUFFER
    , m_pFrameBuffer(NULL)
#endif
    , m_nLineType(LINE_TYPE_SOLID)
    , m_nLineWidth(1)
#if !IF_ONLY_USE_GRADATION
    , m_nLogicWidth(0)
    , m_nLogicHeight(0)
    , m_nBufferSize(0)
    , m_u16FillColor(0)
    , m_clrFill(0, 0, 0, 0)
#endif
#if IF_SCREEN_BUFFER_DUMP
    , m_bDump(false)
    , m_bFlushImmediately(false)
#endif
{
}

xFrameBufferScreen::~xFrameBufferScreen()
{
}

bool xFrameBufferScreen::Init(const char* pszFile)
{
    UnInit();

#if IF_CUSTOM_SCREEN_DEVICE
    if (NULL != m_pScreenDevice)
    {
        if (!m_pScreenDevice->Open(NULL))
        {
            return false;
        }
    }
    else
    {
        m_bCustomDevice = false;
#endif
#ifdef _WIN32
        return false;
#else
        xScreenDevice* pScreenDevice = new xScreenDevice;
        m_pScreenDevice = pScreenDevice;

        if (NULL == m_pScreenDevice)
        {
            XWINDOW_ERR("new xScreenDevice fail in xFrameBufferScreen::Init");
            return false;
        }

        if (!pScreenDevice->Init(pszFile))
        {
            XWINDOW_ERR("xScreenDevice Init fail in xFrameBufferScreen::Init");
            return false;
        }
#endif
#if IF_CUSTOM_SCREEN_DEVICE
    }
#endif

#if IF_ONLY_USE_GRADATION
    if (!CreateFrameBuffer())
    {
        return false;
    }

    m_rgnClip = chRect(0, 0, m_nWidthPixels, m_nHeightPixels);
#else
    SetFormat(m_pScreenDevice->GetColorFormat());

    m_pPixmap = m_pScreenDevice->GetCanvasBuffer(m_nBufferSize);

    if (NULL == m_pPixmap)
    {
#if !IF_SCREEN_SUPPORT_DOUBLE_BUFFER
        if (!CreateFrameBuffer())
        {
            return false;
        }
#else
        return false;
#endif
    }
    else
    {
        m_nWidthPixels = m_pScreenDevice->GetScreenWidth();
        m_nHeightPixels = m_pScreenDevice->GetScreenHeight();
    }

    m_nLogicWidth = m_nWidthPixels;
    m_nLogicHeight = m_nHeightPixels;

#if IF_GRAPHIC_ROTATE
    xGraphicConverter::RotateRightAngle(m_nLogicWidth, m_nLogicHeight);
#endif

    m_rgnClip = chRect(0, 0, m_nLogicWidth, m_nLogicHeight);
#endif

    m_rgnDirty.clear();

    return true;
}

#if !IF_SCREEN_SUPPORT_DOUBLE_BUFFER
bool xFrameBufferScreen::CreateFrameBuffer()
{
    yl::string strMemName;

    if (!m_pScreenDevice->GetMemName(strMemName))
    {
        return false;
    }

    int nFBWidth = m_pScreenDevice->GetScreenWidth();
    int nFBHeight = m_pScreenDevice->GetScreenHeight();

    m_pFrameBuffer = new chMapFile;

    if (NULL == m_pFrameBuffer)
    {
        XWINDOW_ERR("new chMapFile fail in xFrameBufferScreen::Init");
        return false;
    }

#if IF_ONLY_USE_GRADATION
    int nBufferSize = nFBWidth * nFBHeight;
#else
    int nPixelSize = xGraphicOperate::getDataLength(m_nFormat, 1);
    int nBufferSize = nFBWidth * nFBHeight * nPixelSize;

    m_nBufferSize = nBufferSize;
#endif

    int nFBBytes = nBufferSize + sizeof(int) + sizeof(int);
    int* pData = (int*)m_pFrameBuffer->OpenMapFile(strMemName.c_str(), nFBBytes);

    if (m_pFrameBuffer->IsCreateByMe())
    {
        pData[0] = nFBWidth;
        pData[1] = nFBHeight;
    }
    else
    {
        nFBWidth = pData[0];
        nFBHeight = pData[1];
    }

    m_pPixmap = (BYTE*)(pData + 2);
    m_nWidthPixels = nFBWidth;
    m_nHeightPixels = nFBHeight;

    memset(m_pPixmap, 0, nBufferSize);

    return true;
}
#endif

bool xFrameBufferScreen::UnInit()
{
    if (NULL != m_pScreenDevice)
    {
        m_pScreenDevice->Close();

#if IF_CUSTOM_SCREEN_DEVICE
        if (!m_bCustomDevice)
        {
            delete m_pScreenDevice;
            m_pScreenDevice = NULL;
        }
#else
        delete m_pScreenDevice;
        m_pScreenDevice = NULL;
#endif
    }

#if !IF_SCREEN_SUPPORT_DOUBLE_BUFFER
    if (NULL != m_pFrameBuffer)
    {
        m_pFrameBuffer->CloseMapFile();

        delete m_pFrameBuffer;
        m_pFrameBuffer = NULL;
    }
#endif

    m_pPixmap = NULL;
    m_nWidthPixels = 0;
    m_nHeightPixels = 0;

#if !IF_ONLY_USE_GRADATION
    m_nLogicWidth = 0;
    m_nLogicHeight = 0;
#endif

    return true;
}

void xFrameBufferScreen::OnBeginPaint()
{
#if IF_ONLY_USE_GRADATION
    m_rgnClip = chRect(0, 0, m_nWidthPixels, m_nHeightPixels);
#else
    m_rgnClip = chRect(0, 0, m_nLogicWidth, m_nLogicHeight);
#endif

    m_rgnDirty.clear();
    m_nLineType = LINE_TYPE_SOLID;
    m_nLineWidth = 1;
}

void xFrameBufferScreen::OnEndPaint()
{
#if IF_ONLY_USE_GRADATION
    m_rgnClip = chRect(0, 0, m_nWidthPixels, m_nHeightPixels);
#else
    m_rgnClip = chRect(0, 0, m_nLogicWidth, m_nLogicHeight);
#endif

    Flush();
}

bool xFrameBufferScreen::Flush()
{
    if (NULL == m_pPixmap || NULL == m_pScreenDevice || m_rgnDirty.empty())
    {
        return false;
    }

    m_pScreenDevice->OnBeginFlush();

    if (m_pScreenDevice->IsFullScreenSync())
    {
#if IF_ONLY_USE_GRADATION
        chRect rcScreen(0, 0, m_nWidthPixels, m_nHeightPixels);
#else
        chRect rcScreen(0, 0, m_nLogicWidth, m_nLogicHeight);

#if IF_GRAPHIC_ROTATE
        xGraphicConverter::RotateRightAngle(rcScreen, m_nWidthPixels, m_nHeightPixels);
#endif
#endif

        m_pScreenDevice->Flush(*this, rcScreen);
    }
    else if (m_rgnDirty.percent() > 80)
    {
        chRect rcDirty = m_rgnDirty.bound();

#if !IF_ONLY_USE_GRADATION && IF_GRAPHIC_ROTATE
        xGraphicConverter::RotateRightAngle(rcDirty, m_nWidthPixels, m_nHeightPixels);
#endif

        m_pScreenDevice->Flush(*this, rcDirty);
    }
    else
    {
        m_pScreenDevice->Flush(*this, m_rgnDirty);
    }

    m_pScreenDevice->OnEndFlush();
    m_rgnDirty.clear();

    return true;
}

#if IF_SCREEN_BUFFER_DUMP
void xFrameBufferScreen::DumpScreenBuffer()
{
    if (m_bDump)
    {
        chRect rcScreen(0, 0, m_nWidthPixels, m_nHeightPixels);
        xGraphicPtr pScreen = xGraphicOperate::convertGraphic(*this, rcScreen, GRAPHIC_FORMAT_RGB24);
        xPixmap pmBuffer;

        pmBuffer.Create(m_nWidthPixels, m_nHeightPixels, GRAPHIC_FORMAT_RGB24);

        pmBuffer.Copy(pScreen.Get());

        chSize szScreen(m_nWidthPixels, m_nHeightPixels);

#if _WIN32
        pmBuffer.save("D:\\1.jpg", szScreen);
#else
        pmBuffer.save("/tmp/1.jpg", szScreen);
#endif
    }

    if (m_bFlushImmediately)
    {
        Flush();
    }
}
#endif

void xFrameBufferScreen::SetClipRegion(const xRegion& rgnClip, bool bMarkDirty/* = false*/)
{
    m_rgnClip = rgnClip;

#if IF_ONLY_USE_GRADATION
    chRect rcScreen(0, 0, m_nWidthPixels, m_nHeightPixels);
#else
    chRect rcScreen(0, 0, m_nLogicWidth, m_nLogicHeight);
#endif

    m_rgnClip &= rcScreen;

    if (bMarkDirty)
    {
        m_rgnDirty |= (rgnClip & rcScreen);
    }
}

#if IF_SCREEN_SUPPORT_BG_RENDER
void xFrameBufferScreen::PaintScreenBGImage(const chRect& rect, xPixmap& pixmap)
{
#if IF_GRAPHIC_ROTATE && IF_GRAPHIC_MARK_ROTATE
    xGraphic* pGraphic = pixmap.GetGraphic().Get();

    if (NULL == pGraphic)
    {
        return;
    }

    int nRotate = xGraphicConverter::GetRotateAngle();

    if (0 != nRotate && xGraphic::GRAPHIC_FLAG_NONE == pGraphic->getRotateFlag()
            && pGraphic->width() == m_nLogicWidth
            && pGraphic->height() == m_nLogicHeight
            && pGraphic->GetFormat() == m_nFormat)
    {
        xGraphicOperate::rotateGraphic(*pGraphic);
    }

    if (pGraphic->width() == m_nWidthPixels && pGraphic->height() == m_nHeightPixels
            && pGraphic->GetFormat() == m_nFormat)
    {
        const typeRectList& listRect = m_rgnClip.GetRectList();

        for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
        {
            chRect rcCopy = rect & *it;

            if (rcCopy.IsEmpty())
            {
                continue;
            }

#if IF_GRAPHIC_ROTATE
            xGraphicConverter::RotateRightAngle(rcCopy, m_nWidthPixels, m_nHeightPixels);
#endif

            xGraphicOperate::quickCopy(*pGraphic, *this, rcCopy, m_nBufferSize);
        }

#if IF_SCREEN_BUFFER_DUMP
        DumpScreenBuffer();
#endif

        return;
    }
#endif

#if IF_PIXEL_ALPHA
    BlendRect(rect, pixmap);
#else
    FillRect(rect, pixmap);
#endif
}
#endif

void xFrameBufferScreen::FillRect(const chRect& rect, xPixmap& pixmap)
{
#if IF_PIXEL_ALPHA
    BlendRect(rect, pixmap);
#else
    const xGraphicPtr& pData = pixmap.GetGraphic();
    const xGraphic* pGraphic = pData.Get();

    if (NULL == m_pPixmap || NULL == pGraphic || rect.IsEmpty())
    {
        return;
    }

#if IF_PIXMAP_PARTLY_SHARED
    const chRect& rcPixmap = pixmap.GetPixmapRect();
#else
    chRect rcPixmap(0, 0, pGraphic->width(), pGraphic->height());
#endif

    DrawRawPixmap(pixmap, pGraphic, rcPixmap, rect);
#endif
}

void xFrameBufferScreen::FillRect(const chRect& rect, const xColor& color)
{
    if (NULL == m_pPixmap)
    {
        return;
    }

#if !IF_ONLY_USE_GRADATION
    int nColorFormat = GRAPHIC_FORMAT_BGRA32;
    BYTE* pColor = (BYTE*)&color;

    if (GRAPHIC_FORMAT_BGR16_565 == m_nFormat)
    {
        nColorFormat = GRAPHIC_FORMAT_BGR16_565;

        m_u16FillColor = GET_BGR565(color.r, color.g, color.b);

        pColor = (BYTE*)&m_u16FillColor;
    }

    FUN_PIX_OPT pFunPixOpt = xGraphicConverter::GetFunTranPixOpt(nColorFormat, m_nFormat);

    if (NULL == pFunPixOpt)
    {
        return;
    }
#endif

#if IF_ONLY_USE_GRADATION
    xGraphicOperate::fillClipRect(*this, rect, m_rgnClip, (BYTE*)&color);
#else
    xGraphicOperate::fillClipRect(*this, rect, m_rgnClip, pColor, pFunPixOpt);
#endif

#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}

#if IF_PIXEL_ALPHA
void xFrameBufferScreen::BlendRect(const chRect& rect, xPixmap& pixmap)
{
#if FB_DEBUG
    XWINDOW_INFO("BlendRect rect:%d %d %d %d pixmap:%p", rect.left, rect.top, rect.right, rect.bottom, &pixmap);
#endif
#if IF_PIXMAP_SUPPORT_DOT_9
    if (DrawDot9Pixmap(pixmap, rect))
    {
#if IF_SCREEN_BUFFER_DUMP
        DumpScreenBuffer();
#endif
        return;
    }
#endif

    const xGraphicPtr& pData = pixmap.GetGraphic();
    const xGraphic* pGraphic = pData.Get();

    if (NULL == m_pPixmap || NULL == pGraphic || rect.IsEmpty())
    {
        return;
    }

#if IF_PIXMAP_PARTLY_SHARED
    const chRect& rcPixmap = pixmap.GetPixmapRect();
#else
    chRect rcPixmap(0, 0, pGraphic->width(), pGraphic->height());
#endif

    DrawRawPixmap(pixmap, pGraphic, rcPixmap, rect);

#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}
#endif

void xFrameBufferScreen::DrawRawPixmap(xPixmap& pixmap, const xGraphic* pGraphic,
                                       const chRect& rcPixmap, const chRect& rcDst
#if IF_GRAPHIC_ZOOM
                                       , bool bZoomTile
#endif
                                      )
{
    if (NULL == pGraphic || rcDst.IsEmpty())
    {
        return;
    }

#if FB_DEBUG
    XWINDOW_INFO("DrawRawPixmap pixmap:%p rcPixmap:%d %d %d %d rcDst:%d %d %d %d at %s %d",
                 &pixmap, rcPixmap.left, rcPixmap.top, rcPixmap.right, rcPixmap.bottom,
                 rcDst.left, rcDst.top, rcDst.right, rcDst.bottom, __FILE__, __LINE__);
#endif

    if (rcPixmap.Width() == rcDst.Width() && rcPixmap.Height() == rcDst.Height())
    {
#if IF_ONLY_USE_GRADATION
        xGraphicOperate::drawClipGraphic(*pGraphic, rcPixmap, *this, rcDst, m_rgnClip);
#else
        FUN_PIX_OPT pFunPixOpt = NULL;

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
        if (pGraphic->hasAlphaPixel())
        {
            pFunPixOpt = xGraphicConverter::GetFunBlendPixOpt(pGraphic->GetFormat(), m_nFormat);
        }
        else
        {
#endif
            pFunPixOpt = xGraphicConverter::GetFunTranPixOpt(pGraphic->GetFormat(), m_nFormat);
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
        }
#endif

        if (NULL == pFunPixOpt)
        {
            return;
        }

        xGraphicOperate::drawClipGraphic(*pGraphic, rcPixmap, (*this), rcDst, m_rgnClip, pFunPixOpt);
#endif

#if IF_SCREEN_BUFFER_DUMP
        DumpScreenBuffer();
#endif
        return;
    }

#if IF_GRAPHIC_ZOOM
    if (bZoomTile)
    {
        xGraphicOperate::tileDrawClipGraphic(*pGraphic, rcPixmap, (*this), rcDst, m_rgnClip);
    }
    else
    {
        xGraphicOperate::zoomDrawClipGraphic(*pGraphic, rcPixmap, (*this), rcDst, m_rgnClip);
    }
#endif
#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}

#if IF_PIXMAP_SUPPORT_DOT_9
bool xFrameBufferScreen::DrawDot9Pixmap(xPixmap& pixmap, const chRect& rect)
{
    xDot9Info* pDot9Info = pixmap.GetDot9Info();

    if (NULL == m_pPixmap || NULL == pDot9Info || !pDot9Info->IsStretch() || rect.IsEmpty())
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
#if IF_GRAPHIC_ZOOM
    bool bCanTile = pDot9Info->IsCanTile();
#endif

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
        int arrDstX[4] = { rect.left, rect.left + pDot9Info->GetBeginH() - 1,
                           rect.right - (arrSrcX[3] - arrSrcX[2]) + 1, rect.right
                         };
        int arrDstY[4] = { rect.top, rect.top + pDot9Info->GetBeginV() - 1,
                           rect.bottom - (arrSrcY[3] - arrSrcY[2]) + 1, rect.bottom
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

                DrawRawPixmap(pixmap, pGraphic, rcSrc, rcDst
#if IF_GRAPHIC_ZOOM
                              , bCanTile
#endif
                             );
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
        int arrDstX[4] = { rect.left, rect.left + pDot9Info->GetBeginH() - 1,
                           rect.right - (arrSrcX[3] - arrSrcX[2]) + 1, rect.right
                         };

        rcSrc.top = rcPixmap.top;
        rcSrc.bottom = rcPixmap.bottom;
        rcDst.top = rect.top;
        rcDst.bottom = rect.bottom;

        for (int i = 0; i < 3; ++i)
        {
            rcSrc.left = arrSrcX[i];
            rcSrc.right = arrSrcX[i + 1];
            rcDst.left = arrDstX[i];
            rcDst.right = arrDstX[i + 1];

            DrawRawPixmap(pixmap, pGraphic, rcSrc, rcDst
#if IF_GRAPHIC_ZOOM
                          , bCanTile
#endif
                         );
        }

        return true;
    }

    rcPixmap.left += 1;
    rcPixmap.right -= 1;

    int arrSrcY[4] = { rcPixmap.top + 1, rcPixmap.top + pDot9Info->GetBeginV(),
                       rcPixmap.top + pDot9Info->GetEndV(), rcPixmap.bottom
                     };
    int arrDstY[4] = { rect.top, rect.top + pDot9Info->GetBeginV() - 1,
                       rect.bottom - (arrSrcY[3] - arrSrcY[2]) + 1, rect.bottom
                     };

    rcSrc.left = rcPixmap.left;
    rcSrc.right = rcPixmap.right;
    rcDst.left = rect.left;
    rcDst.right = rect.right;

    for (int i = 0; i < 3; ++i)
    {
        rcSrc.top = arrSrcY[i];
        rcSrc.bottom = arrSrcY[i + 1];
        rcDst.top = arrDstY[i];
        rcDst.bottom = arrDstY[i + 1];

        DrawRawPixmap(pixmap, pGraphic, rcSrc, rcDst
#if IF_GRAPHIC_ZOOM
                      , bCanTile
#endif
                     );
    }

    return true;
}
#endif

#if IF_PIXEL_ALPHA
void xFrameBufferScreen::BlendRect(const chRect& rect, const xColor& color)
{
    if (NULL == m_pPixmap || color.isFullTransparent())
    {
        return;
    }

#if IF_ONLY_USE_GRADATION
    xGraphicOperate::fillClipRect(*this, rect, m_rgnClip, (BYTE*)&color);
#else
    if (!color.isTransparent())
    {
        FillRect(rect, color);

        return;
    }

    FUN_PIX_OPT pFunPixOpt = xGraphicConverter::GetFunBlendPixOpt(GRAPHIC_FORMAT_BGRA32_PRE_MULT, m_nFormat);

    if (NULL == pFunPixOpt)
    {
        return;
    }

    m_clrFill.a = color.a;

    PIXEL_PRE_MULT(color, m_clrFill)

    xGraphicOperate::fillClipRect(*this, rect, m_rgnClip, (BYTE*)&m_clrFill, pFunPixOpt);
#endif

#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}
#endif

void xFrameBufferScreen::SetLineType(int nType)
{
    m_nLineType = nType;
}

void xFrameBufferScreen::SetLineWidth(int nWidth)
{
    m_nLineWidth = nWidth;
}

void xFrameBufferScreen::DrawLine(const chPoint& pt1, const chPoint& pt2, const xColor& color)
{
    if (NULL == m_pPixmap)
    {
        return;
    }

    int nWidth = pt2.x - pt1.x;
    int nHeight = pt2.y - pt1.y;
    int nLeft = pt1.x;
    int nTop = pt1.y;

    if (nWidth < 0)
    {
        nLeft = pt2.x;
        nWidth = -nWidth;
    }

    if (nHeight < 0)
    {
        nTop = pt2.y;
        nHeight = -nHeight;
    }

    chRect rcBound;

    if (nWidth <= 0)
    {
        rcBound.SetRect(nLeft, nTop, nLeft + nWidth + m_nLineWidth, nTop + nHeight);
    }
    else if (nHeight <= 0)
    {
        rcBound.SetRect(nLeft, nTop, nLeft + nWidth, nTop + nHeight + m_nLineWidth);
    }
    else
    {
        rcBound.SetRect(nLeft, nTop, nLeft + nWidth, nTop + nHeight);
    }

#if IF_PIXEL_ALPHA
    BlendRect(rcBound, color);
#else
    FillRect(rcBound, color);
#endif

#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}

#if IF_ROUND_RECTANGLE
void xFrameBufferScreen::DrawLine_X(int y, int x1, int x2, const xColor &color)
{
    int nLeft = chMIN(x1, x2);
    int nRight = chMAX(x1, x2);

    chRect rcBound(nLeft, y, nRight, y + m_nLineWidth);

#if IF_PIXEL_ALPHA
    BlendRect(rcBound, color);
#else
    FillRect(rcBound, color);
#endif
}

void xFrameBufferScreen::DrawLine_Y(int x, int y1, int y2, const xColor &color)
{
    int nTop = chMIN(y1, y2);
    int nBottom = chMAX(y1, y2);

    chRect rcBound(x, nTop, x + m_nLineWidth, nBottom);

#if IF_PIXEL_ALPHA
    BlendRect(rcBound, color);
#else
    FillRect(rcBound, color);
#endif
}

void xFrameBufferScreen::FillRoundedRect(const chRect& rect, int nRoundPixel,
        const xColor& color, int nCornerType/* = CORNER_TYPE_ALL*/)
{
    if (NULL == m_pPixmap)
    {
        return;
    }

#if IF_INVERT_EFFECT
    FillRect(rect, color);

    xColor clrInvert = color;

    INVERT_GRADATION(clrInvert.gray)

    int nBottom = rect.bottom - 1;
    int nLeftCornerEnd = rect.left + nRoundPixel;
    int nRightCornerBegin = rect.right - nRoundPixel;

    // 左上角◤圆角
    if (CORNER_TYPE_LEFT_TOP == (nCornerType & CORNER_TYPE_LEFT_TOP))
    {
        for (int i = 0; i < nRoundPixel; ++i)
        {
            DrawLine_X(rect.top + i, rect.left, nLeftCornerEnd - i, clrInvert);
        }
    }

    // 右上角◥圆角
    if (CORNER_TYPE_RIGHT_TOP == (nCornerType & CORNER_TYPE_RIGHT_TOP))
    {
        for (int i = 0; i < nRoundPixel; ++i)
        {
            DrawLine_X(rect.top + i, nRightCornerBegin + i, rect.right, clrInvert);
        }
    }

    // 左下角◣圆角
    if (CORNER_TYPE_LEFT_BOTTOM == (nCornerType & CORNER_TYPE_LEFT_BOTTOM))
    {
        for (int i = 0; i < nRoundPixel; ++i)
        {
            DrawLine_X(nBottom - i, rect.left, nLeftCornerEnd - i, clrInvert);
        }
    }

    // 右下角◢圆角
    if (CORNER_TYPE_RIGHT_BOTTOM == (nCornerType & CORNER_TYPE_RIGHT_BOTTOM))
    {
        for (int i = 0; i < nRoundPixel; ++i)
        {
            DrawLine_X(nBottom - i, nRightCornerBegin + i, rect.right, clrInvert);
        }
    }
#endif
}

void xFrameBufferScreen::DrawRoundedRect(const chRect& rect, int nRoundPixel, const xColor& color,
        int nCornerType/* = CORNER_TYPE_ALL*/)
{
    if (NULL == m_pPixmap)
    {
        return;
    }

    int nLeftCornerX = rect.left + nRoundPixel - 1;
    int nRightCornerX = rect.right - nRoundPixel;
    int nTopCornerY = rect.top;
    int nBottomCornerY = rect.bottom - 1;
    int nMidX = (rect.left + rect.right) >> 1;
    int nMidY = (rect.top + rect.bottom) >> 1;
    int nRight = rect.right - 1;
    int nBottom = rect.bottom - 1;
    int i = 0;

    // 左上角◤圆角
    if (CORNER_TYPE_LEFT_TOP == (nCornerType & CORNER_TYPE_LEFT_TOP))
    {
        for (i = 0; i < nRoundPixel; ++i)
        {
            drawPoint(nLeftCornerX - i, nTopCornerY + i, color);
        }

        DrawLine_X(rect.top, rect.left + nRoundPixel, nMidX, color);
        DrawLine_Y(rect.left, rect.top + nRoundPixel, nMidY, color);
    }
    else
    {
        DrawLine_X(rect.top, rect.left, nMidX, color);
        DrawLine_Y(rect.left, rect.top, nMidY, color);
    }

    // 右上角◥圆角
    if (CORNER_TYPE_RIGHT_TOP == (nCornerType & CORNER_TYPE_RIGHT_TOP))
    {
        for (i = 0; i < nRoundPixel; ++i)
        {
            drawPoint(nRightCornerX + i, nTopCornerY + i, color);
        }

        DrawLine_X(rect.top, rect.right - nRoundPixel, nMidX, color);
        DrawLine_Y(nRight, rect.top + nRoundPixel, nMidY, color);
    }
    else
    {
        // 直角
        DrawLine_X(rect.top, rect.right, nMidX, color);
        DrawLine_Y(nRight, rect.top, nMidY, color);
    }

    // 左下角◣圆角
    if (CORNER_TYPE_LEFT_BOTTOM == (nCornerType & CORNER_TYPE_LEFT_BOTTOM))
    {
        for (i = 0; i < nRoundPixel; ++i)
        {
            drawPoint(nLeftCornerX - i, nBottomCornerY - i, color);
        }

        DrawLine_X(nBottom, rect.left + nRoundPixel, nMidX, color);
        DrawLine_Y(rect.left, rect.bottom - nRoundPixel, nMidY, color);
    }
    else
    {
        DrawLine_X(nBottom, rect.left, nMidX, color);
        DrawLine_Y(rect.left, rect.bottom, nMidY, color);
    }

    // 右下角◢圆角
    if (CORNER_TYPE_RIGHT_BOTTOM == (nCornerType & CORNER_TYPE_RIGHT_BOTTOM))
    {
        for (i = 0; i < nRoundPixel; ++i)
        {
            drawPoint(nRightCornerX + i, nBottomCornerY - i, color);
        }

        DrawLine_X(nBottom, rect.right - nRoundPixel, nMidX, color);
        DrawLine_Y(nRight, rect.bottom - nRoundPixel, nMidY, color);
    }
    else
    {
        DrawLine_X(nBottom, rect.right, nMidX, color);
        DrawLine_Y(nRight, rect.bottom, nMidY, color);
    }
}
#endif

void xFrameBufferScreen::DrawPixmap(const chRect& rect, xPixmap& pixmap, const chRect& srcRect)
{
    const xGraphicPtr& pData = pixmap.GetGraphic();
    const xGraphic* pGraphic = pData.Get();

    if (NULL == m_pPixmap || NULL == pGraphic || rect.IsEmpty())
    {
        return;
    }

#if IF_PIXMAP_PARTLY_SHARED
    const chRect& rcPixmap = pixmap.GetPixmapRect();

    if (srcRect == rcPixmap)
    {
        BlendRect(rect, pixmap);
        return;
    }

    // 画局部图片。如果是.9图画局部图片，先用设置裁剪区域，再画全部图片，然后再还原裁剪区域
    chRect rcRealPixmap = rcPixmap;

    rcRealPixmap.OffsetRect(srcRect.left, srcRect.top);
    rcRealPixmap.Resize(srcRect.Width(), srcRect.Height());
    rcRealPixmap = rcRealPixmap & rcPixmap;

    DrawRawPixmap(pixmap, pGraphic, rcRealPixmap, rect);
#else
    DrawRawPixmap(pixmap, pGraphic, srcRect, rect);
#endif
#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}

void xFrameBufferScreen::DrawText(const chRect& rcBound, const char* pszText, int nAlign,
                                  const xFont& font, const xColor& color)
{
    if (NULL == m_pPixmap || NULL == pszText || strlen(pszText) <= 0)
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
            pDobFont->layoutText32Bound(rcBound, pszText, nAlign, &(color.gray), m_pPixmap, szScreen, *it);
        }

        return;
    }
#endif

#if IF_USE_FREETYPE_LIB
    FreeType_DrawText(*this, rcBound, pszText, nAlign, font, color, m_rgnClip);
#endif
#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}

void xFrameBufferScreen::DrawTextEx(const chRect& rcBound, const char* pszText, int nAlign,
                                    chPoint& ptOffset, const xFont& font, const xColor& color)
{
    if (NULL == m_pPixmap || NULL == pszText || strlen(pszText) <= 0)
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
            pDobFont->layoutText32Bound(rcBound, pszText, nAlign, ptOffset, &(color.gray),
                                        m_pPixmap, szScreen, *it);
        }

        return;
    }
#endif

#if IF_USE_FREETYPE_LIB
    yl::string strText = pszText;
    chSize sizeText = FreeType_TextOutBound(font, pszText);
    if (sizeText.cx <= ptOffset.x || sizeText.cy <= ptOffset.y)
    {
        FreeType_DrawText(*this, rcBound, pszText, nAlign, font, color, m_rgnClip);
    }
    else
    {
        chRect rcText = OffsetRect(rcBound, -ptOffset.x, -ptOffset.y);
        int iX = ptOffset.x > 0 ? ptOffset.x : 0;
        int iY = ptOffset.y > 0 ? ptOffset.y : 0;
        rcText.Resize(rcText.Width() + iX, rcText.Height() + iY);
        FreeType_DrawText(*this, rcText, pszText, nAlign, font, color, m_rgnClip);
    }
#endif

#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}

#if IF_SCREEN_SET_PIXEL
void xFrameBufferScreen::drawPoint(int x, int y, const xColor& color)
{
    if (NULL == m_pPixmap)
    {
        return;
    }

#if IF_ONLY_USE_GRADATION
    const typeRectList& listRect = m_rgnClip.GetRectList();

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        const chRect& rect = *it;

        if (y >= rect.top && y < rect.bottom
                && x >= rect.left && x < rect.right)
        {
            BYTE* pData = m_pPixmap + y * m_nWidthPixels + x;

            (*pData) = color.gray;

            return;
        }
    }
#else
    chRect rcDst(x, y, x + 1, y + 1);

    BlendRect(rcDst, color);
#endif
}
#endif

#if IF_INVERT_EFFECT
void xFrameBufferScreen::InvertRect(const chRect& rect)
{
    if (NULL == m_pPixmap)
    {
        return;
    }

    const typeRectList& listRect = m_rgnClip.GetRectList();

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        chRect rcDst = rect & *it;

        if (rcDst.IsEmpty())
        {
            continue;
        }

        xGraphicOperate::invertRect(*this, rcDst);
    }
}
#endif

#if IF_XWINDOW_STATION_ALPHA
void xFrameBufferScreen::BlendGlobalAlpha(int nAlpha)
{
    if (NULL == m_pPixmap)
    {
        return;
    }

    const typeRectList& listRect = m_rgnDirty.GetRectList();
    chRect rcScreen(0, 0, m_nLogicWidth, m_nLogicHeight);

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        chRect rcDst = rcScreen & *it;

        if (rcDst.IsEmpty())
        {
            continue;
        }

#if IF_GRAPHIC_ROTATE
        xGraphicConverter::RotateRightAngle(rcDst, m_nWidthPixels, m_nHeightPixels);
#endif

        xGraphicOperate::blendAlphaBGR16(*this, rcDst, nAlpha);
    }

#if IF_SCREEN_BUFFER_DUMP
    DumpScreenBuffer();
#endif
}
#endif

bool xFrameBufferScreen::Capture(xPixmap& pixmap)
{
    return pixmap.Copy(this);
}

int xFrameBufferScreen::Width() const
{
#if IF_ONLY_USE_GRADATION
    return m_nWidthPixels;
#else
    return m_nLogicWidth;
#endif
}

int xFrameBufferScreen::Height() const
{
#if IF_ONLY_USE_GRADATION
    return m_nHeightPixels;
#else
    return m_nLogicHeight;
#endif
}

int xFrameBufferScreen::GetLineType() const
{
    return m_nLineType;
}

int xFrameBufferScreen::GetLineWidth() const
{
    return m_nLineWidth;
}

#if IF_CUSTOM_SCREEN_DEVICE
void xFrameBufferScreen::SetScreenDevice(IScreenDevice* pDevice)
{
    if (NULL == pDevice)
    {
        return;
    }

    if (NULL != m_pScreenDevice && !m_bCustomDevice)
    {
        delete m_pScreenDevice;

        m_pScreenDevice = NULL;
    }

    m_pScreenDevice = pDevice;
    m_bCustomDevice = true;
}
#endif
