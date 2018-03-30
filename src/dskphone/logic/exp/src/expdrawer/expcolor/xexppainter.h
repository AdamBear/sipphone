#ifndef __X_EXP_PAINTER_H__
#define __X_EXP_PAINTER_H__

#include "common/xwindow/xGraphic.h"
#include "common/fbscreen/src/fbscreendefine.h"

class xEXPPainter
{
public:
    void drawItemPixmap(const chRect& rect, xPixmap& pixmap);
    void drawItemPixmapEx(const chRect& rect, xPixmap& pixmap, const chRect& srcRect);

    void DrawRawPixmap(xPixmap& pixmap,
                       const chRect& rcPixmap, const chRect& rcDst
#if IF_GRAPHIC_ZOOM
                       , bool bZoomTile = false
#endif
                      );

    bool DrawDot9Pixmap(xPixmap& pixmap, const chRect& rect);

    void drawText(const chRect& rcTextBound, const char* pszText, int nAlign,
                  const xFont& font, const xColor& color);

    void fillRect(const chRect& rect, const xColor& color);

#if IF_PIXEL_ALPHA
    void blendRect(const chRect& rect, xPixmap& pixmap);
#endif

    void drawRect(const chRect& rect, const xColor& color);

    void drawLine(int x1, int y1, int x2, int y2, const xColor& color);

public:
    explicit xEXPPainter(xGraphic* pGraphic, const chPoint& ptOffset);
    ~xEXPPainter();

public:
    xGraphic*        m_pGraphic;
    chPoint          m_ptViewOffset;
    xRegion          m_rgnClip;
};

#endif //__X_EXP_PAINTER_H__
