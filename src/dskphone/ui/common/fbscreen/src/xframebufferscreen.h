#ifndef __X_FRAME_BUFFER_SCREEN_H__
#define __X_FRAME_BUFFER_SCREEN_H__

#include "xwindow/xGraphic.h"
#include "xscreendevice.h"
#include "fbscreendefine.h"
#include "iscreendevice.h"


#define IF_SCREEN_BUFFER_DUMP 0

class xFrameBufferScreen : public xGraphic
    , public xScreen
{
public:
    xFrameBufferScreen();
    ~xFrameBufferScreen();

public:
    virtual bool Init(const char* pszFile);
    virtual bool UnInit();

    virtual void OnBeginPaint();
    virtual void OnEndPaint();
    virtual bool Flush();

    virtual void SetClipRegion(const xRegion& rgnClip, bool bMarkDirty = false);

#if IF_SCREEN_SUPPORT_BG_RENDER
    virtual void PaintScreenBGImage(const chRect& rect, xPixmap& pixmap);
#endif

    virtual void FillRect(const chRect& rect, xPixmap& pixmap);
    virtual void FillRect(const chRect& rect, const xColor& color);

#if IF_PIXEL_ALPHA
    virtual void BlendRect(const chRect& rect, xPixmap& pixmap);
    virtual void BlendRect(const chRect& rect, const xColor& color);
#endif

    virtual void SetLineType(int nType);
    virtual void SetLineWidth(int nWidth);
    virtual void DrawLine(const chPoint& pt1, const chPoint& pt2, const xColor& color);

#if IF_ROUND_RECTANGLE
    void DrawLine_X(int y, int x1, int x2, const xColor &color);
    void DrawLine_Y(int x, int y1, int y2, const xColor &color);

    virtual void FillRoundedRect(const chRect& rect, int nRoundPixel,
                                 const xColor& color, int nCornerType = CORNER_TYPE_ALL);
    virtual void DrawRoundedRect(const chRect& rect, int nRoundPixel, const xColor& color,
                                 int nCornerType = CORNER_TYPE_ALL);
#endif

    virtual void DrawPixmap(const chRect& rect, xPixmap& pixmap, const chRect& srcRect);

    virtual void DrawText(const chRect& rcBound, const char* pszText, int nAlign,
                          const xFont& font, const xColor& color);
    virtual void DrawTextEx(const chRect& rcBound, const char* pszText, int nAlign,
                            chPoint& ptOffset, const xFont& font, const xColor& color);

#if IF_SCREEN_SET_PIXEL
    virtual void drawPoint(int x, int y, const xColor& color);
#endif

#if IF_INVERT_EFFECT
    virtual void InvertRect(const chRect& rect);
#endif

#if IF_XWINDOW_STATION_ALPHA
    virtual void BlendGlobalAlpha(int nAlpha);
#endif

    virtual bool Capture(xPixmap& pixmap);

    virtual int Width() const;
    virtual int Height() const;
    virtual int GetLineType() const;
    virtual int GetLineWidth() const;

#if IF_CUSTOM_SCREEN_DEVICE
    void SetScreenDevice(IScreenDevice* pDevice);
#endif

protected:
    void DrawRawPixmap(xPixmap& pixmap, const xGraphic* pGraphic,
                       const chRect& rcPixmap, const chRect& rcDst
#if IF_GRAPHIC_ZOOM
                       , bool bZoomTile = false
#endif
                      );

#if IF_PIXMAP_SUPPORT_DOT_9
    bool DrawDot9Pixmap(xPixmap& pixmap, const chRect& rect);
#endif

#if !IF_SCREEN_SUPPORT_DOUBLE_BUFFER
    bool CreateFrameBuffer();
#endif

#if IF_SCREEN_BUFFER_DUMP
    void DumpScreenBuffer();
#endif

private:
#if IF_CUSTOM_SCREEN_DEVICE
    IScreenDevice*  m_pScreenDevice;
    bool            m_bCustomDevice;
#else
    xScreenDevice*  m_pScreenDevice;
#endif
#if !IF_SCREEN_SUPPORT_DOUBLE_BUFFER
    chMapFile*      m_pFrameBuffer;
#endif

    xRegion         m_rgnClip;
    xRegion         m_rgnDirty;
    int             m_nLineType;
    int             m_nLineWidth;
#if !IF_ONLY_USE_GRADATION
    int             m_nLogicWidth;
    int             m_nLogicHeight;
    int             m_nBufferSize;
    u16int          m_u16FillColor;
    xColor          m_clrFill;
#endif
#if IF_SCREEN_BUFFER_DUMP
    bool            m_bDump;
    bool            m_bFlushImmediately;
#endif
};

#endif
