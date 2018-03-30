#ifndef __X_GRAPHIC_H__
#define __X_GRAPHIC_H__

#include <ylstring.h>
#include <yllist.h>
#include <ylsmartptr.h>
#include "ETLLib/ETLLib.hpp"
#include <xwindow/xwindowdefine.h>
#include <xwindow/xscreendefine.h>
#include "xwindow-kernel/include/xkernelcommon.h"


USING_XKERNEL_NAMESPACE

#if IF_USE_GRADATION
class dobFont;
#else
#ifndef u16int
typedef unsigned short u16int;
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
// gravity & align
#define gravity_left            0x01
#define gravity_x_center        0x02
#define gravity_right           0x04
#define gravity_top             0x10
#define gravity_y_center        0x20
#define gravity_bottom          0x40

#define gravity_undefined       0x00
#define gravity_top_left        0x11
#define gravity_center          0x22
#define gravity_x_mask          0x07
#define gravity_y_mask          0x70

#define align_left              gravity_left
#define align_h_center          gravity_x_center
#define align_right             gravity_right
#define align_top               gravity_top
#define align_v_center          gravity_y_center
#define align_bottom            gravity_bottom

#define align_center            (gravity_x_center | gravity_y_center)

//////////////////////////////////////////////////////////////////////////////////////////////////
// xRegion
typedef YLList<chRect> typeRectList;
class XWINDOW_EXPORT xRegion
{
public:
    static xRegion OffsetRegion(const xRegion& region, int x, int y);
public:
    bool empty() const;
    const chRect& bound() const;
    bool operator == (const xRegion& src) const;

    xRegion& operator = (const chRect& rect);
    xRegion& operator = (const xRegion& src);

    xRegion operator - (const chRect& src) const;
    xRegion& operator -= (const chRect& src);

    xRegion operator & (const chRect& src) const;
    xRegion& operator &= (const chRect& src);

    xRegion operator | (const chRect& src) const;
    xRegion& operator |= (const chRect& src);

    xRegion operator - (const xRegion& src) const;
    xRegion& operator -= (const xRegion& src);

    xRegion operator & (const xRegion& src) const;
    xRegion& operator &= (const xRegion& src);

    xRegion operator | (const xRegion& src) const;
    xRegion& operator |= (const xRegion& src);

    xRegion& moveRegion(int x, int y);
    int percent() const;
    void clear();
    int getArea() const;

    bool hasIntersect(const chRect& rect) const;
    bool hasIntersect(const xRegion& src) const;

    const typeRectList& GetRectList() const;

#if IF_XGRAPHIC_DEBUG
    void Dump() const;
    static void DumpRectList(const typeRectList& listRect);
#endif

private:
    void push_rect(const chRect& rect);
public:
    xRegion();
    xRegion(const xRegion& rgn);
    explicit xRegion(const chRect& rect);
private:
    typeRectList    m_listRect;
    chRect          m_rectRound;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphic
enum GRAPHIC_FORMAT
{
    GRAPHIC_FORMAT_INVALID = 0,
    GRAPHIC_FORMAT_BGR8,
    GRAPHIC_FORMAT_BGR16_444,
    GRAPHIC_FORMAT_BGR16_444_COLOR_KEY,
    GRAPHIC_FORMAT_BGR16_555,
    GRAPHIC_FORMAT_BGR16_555_COLOR_KEY,
    GRAPHIC_FORMAT_BGR16_565,
    GRAPHIC_FORMAT_BGR16_565_COLOR_KEY,
    GRAPHIC_FORMAT_BGR24,
    GRAPHIC_FORMAT_RGB24,
    GRAPHIC_FORMAT_BGRA32,
    GRAPHIC_FORMAT_BGRA32_PRE_MULT,
    GRAPHIC_FORMAT_RGBA32,
    GRAPHIC_FORMAT_RGBA32_PRE_MULT,
    GRAPHIC_FORMAT_GRADATION_8B,
    GRAPHIC_FORMAT_GRADATION_1B,
};

typedef struct _ST_RGBA32
{
    _ST_RGBA32()
        : r(0)
        , g(0)
        , b(0)
        , a(0)
    {
    }

    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
} ST_RGBA32;

typedef struct _ST_RGB24
{
    _ST_RGB24()
        : r(0)
        , g(0)
        , b(0)
    {
    }

    BYTE r;
    BYTE g;
    BYTE b;
} ST_RGB24;

#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
typedef struct _ST_BGR24
{
    _ST_BGR24()
        : b(0)
        , g(0)
        , r(0)
    {
    }

    BYTE b;
    BYTE g;
    BYTE r;
} ST_BGR24;
#endif

// Color format is BGRA(BYTE is ARGB(from low to height))
class XWINDOW_EXPORT xColor
{
public:
    xColor();
    xColor(BYTE _r, BYTE _g, BYTE _b);
#if IF_ONLY_USE_GRADATION
    xColor(BYTE byGray);
#else
#if IF_PIXEL_ALPHA
    xColor(BYTE _a, BYTE _r, BYTE _g, BYTE _b);
    inline bool isTransparent() const
    {
        return a != 0xFF;
    }

    inline bool isFullTransparent() const
    {
        return 0x00 == a;
    }
#endif

    operator DWORD () const;
#endif

#if IF_USE_GRADATION
    operator BYTE() const;
#endif

public:
#if IF_ONLY_USE_GRADATION
    BYTE gray;
#else
    BYTE b;
    BYTE g;
    BYTE r;
#endif
#if IF_PIXEL_ALPHA
    BYTE a;
#endif
};

typedef xColor          xPixel;

class xGraphicConverter;
class xGraphicScale;

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
class xGraphic;
class XWINDOW_EXPORT xScreenGraphic
{
public:
    xScreenGraphic() {};
    virtual ~xScreenGraphic() {};

public:
    virtual void OnAttachGraphic(xGraphic* pGraphic)            = 0;
    virtual void OnDetachGraphic(xGraphic* pGraphic)            = 0;
};
#endif

class XWINDOW_EXPORT xGraphic
{
    friend class xGraphicOperate;
    friend class xGraphicScale;
    friend class xGraphicConverter;
    friend class xPixmap;

public:
    xGraphic();
    virtual ~xGraphic();

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL || IF_GRAPHIC_MARK_ROTATE
    enum
    {
        GRAPHIC_FLAG_NONE                       = 0,
        GRAPHIC_FLAG_ALPHA_MASK                 = 0x00FF,       // 透明度使用第1个字节
        GRAPHIC_FLAG_OPACITY                    = 0x0001,       // 不透明
        GRAPHIC_FLAG_TRANSPARENT                = 0x0002,       // 有透明像素
        GRAPHIC_FLAG_FULL_TRANSPARENT           = 0x0010,       // 透明像素全透明
        GRAPHIC_FLAG_ROTATE_MASK                = 0xFF00,       // 旋转使用使用第2个字节
        GRAPHIC_FLAG_ROTATE_90                  = 0x0100,       // 旋转90度
        GRAPHIC_FLAG_ROTATE_180                 = 0x0200,       // 旋转180度
        GRAPHIC_FLAG_ROTATE_270                 = 0x0400,       // 旋转270度
    };
#endif

    static int GetDefaultFormat();
    static void SetDefaultFormat(int nFormat);
#if IF_PIXEL_ALPHA
    static int GetDefaultTranparentFormat();
    static void SetDefaultTranparentFormat(int nFormat);
#endif

public:
    bool createGraphic(int nWidth, int nHeight);
    void destroyGraphic();
    void SetFormat(int nFormat);

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    bool AttachScreenGraphic(xScreenGraphic* pScreenGraphic);
    bool DetachScreenGraphic();

    inline xScreenGraphic* GetScreenGraphic() const
    {
        return m_pScreenGraphic;
    }
#endif

    inline int GetFormat() const
    {
        return m_nFormat;
    }

    inline BYTE* pixels() const
    {
        return m_pPixmap;
    }

    inline int width() const
    {
        return m_nWidthPixels;
    }

    inline int height() const
    {
        return m_nHeightPixels;
    }

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    void setAlphaFlag(bool bOpacity, bool bFullTransparent);
    void setAlphaFlag(int nAlphaFlag);

    inline bool hasAlphaPixel() const
    {
        return GRAPHIC_FLAG_OPACITY != (GRAPHIC_FLAG_ALPHA_MASK & m_nFlag);
    }

    inline bool IsAlphaFullTransparent() const
    {
        return GRAPHIC_FLAG_FULL_TRANSPARENT == (GRAPHIC_FLAG_FULL_TRANSPARENT & m_nFlag);
    }

    inline bool IsAlphaTranslucence() const
    {
        return GRAPHIC_FLAG_TRANSPARENT == (GRAPHIC_FLAG_ALPHA_MASK & m_nFlag);
    }
#endif

#if IF_GRAPHIC_MARK_ROTATE
    void setRotateFlag(int nAngle);

    inline int getRotateFlag() const
    {
        return (GRAPHIC_FLAG_ROTATE_MASK & m_nFlag);
    }
#endif

private:
    xGraphic& operator = (const xGraphic& src);
    xGraphic(const xGraphic& src);

protected:
    int             m_nFormat;

    BYTE*           m_pPixmap;
    int             m_nWidthPixels;
    int             m_nHeightPixels;

    // 标记图片中是否有透明的像素
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL || IF_GRAPHIC_MARK_ROTATE
    int            m_nFlag;
#endif

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    xScreenGraphic* m_pScreenGraphic;
#endif

    static int      s_nDefaultFormat;
#if IF_PIXEL_ALPHA
    static int      s_nDefaultTransparentFormat;
#endif

#if IF_GRAPHIC_SUPPORT_COLOR_KEY
    static xColor   s_clrKey;
    static u16int   s_u16Key;
#endif
};

typedef YLSmartPtr<xGraphic> xGraphicPtr;

#if IF_PIXMAP_SUPPORT_DOT_9
#define INVALID_DOT_9_VALUE -1
class xPixmap;
class xDot9Info
{
    friend class xPixmap;
public:
    xDot9Info()
        : m_nBeginH(INVALID_DOT_9_VALUE)
        , m_nEndH(INVALID_DOT_9_VALUE)
        , m_nBeginV(INVALID_DOT_9_VALUE)
        , m_nEndV(INVALID_DOT_9_VALUE)
        , m_bTile(true)
    {
    }

    void Reset()
    {
        m_nBeginH = INVALID_DOT_9_VALUE;
        m_nEndH = INVALID_DOT_9_VALUE;
        m_nBeginV = INVALID_DOT_9_VALUE;
        m_nEndV = INVALID_DOT_9_VALUE;
        m_bTile = true;
    }

    int GetBeginH() const
    {
        return m_nBeginH;
    }
    int GetEndH() const
    {
        return m_nEndH;
    }
    int GetBeginV() const
    {
        return m_nBeginV;
    }
    int GetEndV() const
    {
        return m_nEndV;
    }

    bool IsHorizontalStretch() const
    {
        return (INVALID_DOT_9_VALUE != m_nBeginH && INVALID_DOT_9_VALUE != m_nEndH);
    }
    bool IsVerticalStretch() const
    {
        return (INVALID_DOT_9_VALUE != m_nBeginV && INVALID_DOT_9_VALUE != m_nEndV);
    }
    bool IsStretch() const
    {
        return (IsHorizontalStretch() || IsVerticalStretch());
    }
    bool IsCanTile() const
    {
        return m_bTile;
    }

protected:
    int m_nBeginH;
    int m_nEndH;
    int m_nBeginV;
    int m_nEndV;
    // 拉伸时是否可以用平铺方式填充(默认方式)
    bool m_bTile;
};

typedef YLSmartPtr<xDot9Info> xDot9InfoPtr;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
// xPixmap
class XWINDOW_EXPORT xPixmap
{
public:
    bool Create(int nWidth, int nHeight, int nFormat = GRAPHIC_DEFAULT_FORMAT);
    bool load(const char* pFlow, int iFlowSize);

    bool load(const yl::string& strFile);

#if IF_ONLY_USE_GRADATION
    bool save(const yl::string& strFile, chSize picSize);
    bool LoadFromDobData(BYTE* pData, int nWidth = 0, int nHeight = 0);
#else
    static void InitImageFormatHandle();
    static bool GetImageSize(const yl::string&strFileName, chSize& szImage);

    bool convertFormat(int nFormat);
    bool convertDefaultFormat();

    bool save(const yl::string& strFile, chSize picSize, const yl::string& strFormat = "");
#endif

#if IF_GRAPHIC_ZOOM
    bool ScaleCopy(const xPixmap& src);
#endif
    bool Copy(xGraphic* pGraphic);

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    bool AttachScreenGraphic(xScreenGraphic* pScreenGraphic, const chRect& rc);
    bool DetachScreenGraphic();
    xScreenGraphic* GetScreenGraphic() const;
#endif

#if IF_PIXMAP_PARTLY_SHARED
    void SetPixmapRect(const chRect& rcPixmap)
    {
        m_rcPixmap = rcPixmap;
    }
    const chRect& GetPixmapRect() const
    {
        return m_rcPixmap;
    }
#endif

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    bool hasAlphaChannel() const;
#endif

#if IF_PIXMAP_SUPPORT_DOT_9
    void InitDot9Info(bool bTile);
    xDot9Info* GetDot9Info() const
    {
        return m_pDot9Info.Get();
    }
#endif

    void setNull();
    bool isNull() const
    {
        return m_pGraphic.IsEmpty();
    }
    int width() const;
    int height() const;
    int GetRealWidth();
    int GetRealHeight();

    const xGraphicPtr& GetGraphic() const
    {
        return m_pGraphic;
    };

public:
    xPixmap();
    xPixmap(const xPixmap& src);
    explicit xPixmap(const yl::string& strFile);
    ~xPixmap();

    xPixmap& operator = (const xPixmap& src);

#if IF_PIXMAP_SUPPORT_DOT_9
protected:
    bool GetDot9HorizontalInfo(const xPixel* pPixel, int nStride, int nWidth, int& nBeginH, int& nEndH);
    bool GetDot9VerticalInfo(const xPixel* pPixel, int nStride, int nHeight, int& nBeginV, int& nEndV);
#endif

private:
    xGraphicPtr m_pGraphic;

#if IF_PIXMAP_PARTLY_SHARED
    chRect m_rcPixmap;
#endif

#if IF_PIXMAP_SUPPORT_DOT_9
    xDot9InfoPtr m_pDot9Info;
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// xFont
class XWINDOW_EXPORT xFont
{
public:
    static void SetDefaultSize(int nDefaultSize);

    void setPointSize(int pointSize);
    void setFamily(const yl::string& family);
    void setBold(bool bBold);

    int getPointSize() const;

#if IF_USE_FREETYPE_LIB
    yl::string getFamily() const
    {
        return m_strFamily;
    }
#endif

#if IF_USE_DOB_FONT
    dobFont* GetDobFont() const
    {
        return m_pDobFont;
    }
#endif

    chSize textBound(const yl::string& text) const;

public:
    xFont();
    xFont(const yl::string& family, int pointSize, bool bold = false);
    xFont(const xFont& font);
    ~xFont();

    xFont& operator = (const xFont& src);

protected:
    static int s_nDefaultSize;
#if IF_USE_FREETYPE_LIB
    int         m_nPointSize;
    bool        m_bBold;
    yl::string  m_strFamily;
#endif
#if IF_USE_DOB_FONT
    dobFont* m_pDobFont;
#endif
};

//函数声明：根据传入的字体、显示宽度,将字符串strText进行省略处理
yl::string CutOffText(const yl::string& strText, xFont& objFont, int nShowWidth);

#if IF_USE_DOB_FONT
class dobFont;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
// xBrush
class XWINDOW_EXPORT xBrush
{
    friend class xPainter;
public:
    bool isNull() const;
    bool isColor() const;
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    bool isTransparent() const;
#endif

    void setPixmap(const xPixmap& pixmap);
    void setColor(const xColor& color);

    const xColor& GetColor() const;

    xPixmap& pixmap();
public:
    xBrush();
    xBrush(const xBrush& src);
    explicit xBrush(const xPixmap& pixmap);
    explicit xBrush(const xColor& color);
    ~xBrush();

    xBrush& operator = (const xBrush& src);

private:
    xPixmap         m_objPixmap;
    xColor          m_objColor;
    bool            m_bIsNull;
};

// 裁剪区域类型
enum
{
    CHIP_OPERATION_NO_CHIP      = 0,
    CHIP_OPERATION_REPLACE      = 1,
    CHIP_OPERATION_INTERSECT    = 2,
    CHIP_OPERATION_UNITE        = 3,
};

// 直线类型
enum
{
    LINE_TYPE_NONE              = 0,
    LINE_TYPE_SOLID             = 1,
    LINE_TYPE_DASH              = 2,
};

enum CORNER_TYPE
{
    CORNER_TYPE_LEFT_TOP        = 0x1,
    CORNER_TYPE_RIGHT_TOP       = 0x1 << 1,
    CORNER_TYPE_LEFT_BOTTOM     = 0x1 << 2,
    CORNER_TYPE_RIGHT_BOTTOM    = 0x1 << 3,
    CORNER_TYPE_ALL             = 0xf,
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// xScreen
class xScreen
{
public:
    xScreen() {};
    virtual ~xScreen() {};

public:
    virtual bool Init(const char* pszFile)      = 0;
    virtual bool UnInit()                       = 0;

    virtual void OnBeginPaint()                                                                 = 0;
    virtual void OnEndPaint()                                                                   = 0;
    virtual bool Flush()                                                                        = 0;

    virtual void SetClipRegion(const xRegion& rgnClip, bool bMarkDirty = false)                 = 0;

#if IF_SCREEN_SUPPORT_BG_RENDER
    virtual void PaintScreenBGImage(const chRect& rect, xPixmap& pixmap)                        = 0;
#endif

    virtual void FillRect(const chRect& rect, xPixmap& pixmap)                                  = 0;
    virtual void FillRect(const chRect& rect, const xColor& color)                              = 0;

#if IF_PIXEL_ALPHA
    virtual void BlendRect(const chRect& rect, xPixmap& pixmap)                                 = 0;
    virtual void BlendRect(const chRect& rect, const xColor& color)                             = 0;
#endif

    virtual void SetLineType(int nType)         = 0;
    virtual void SetLineWidth(int nWidth)       = 0;
    virtual void DrawLine(const chPoint& pt1, const chPoint& pt2, const xColor& color)          = 0;

#if IF_ROUND_RECTANGLE
    virtual void FillRoundedRect(const chRect& rect, int nRoundPixel,
                                 const xColor& color, int nCornerType = CORNER_TYPE_ALL)        = 0;

    virtual void DrawRoundedRect(const chRect& rect, int nRoundPixel,
                                 const xColor& color, int nCornerType = CORNER_TYPE_ALL)        = 0;
#endif

    virtual void DrawPixmap(const chRect& rect, xPixmap& pixmap, const chRect& srcRect)         = 0;

    virtual void DrawText(const chRect& rcBound, const char* pszText, int nAlign,
                          const xFont& font, const xColor& color)                               = 0;
    virtual void DrawTextEx(const chRect& rcBound, const char* pszText, int nAlign,
                            chPoint& ptOffset, const xFont& font, const xColor& color)          = 0;

#if IF_SCREEN_SET_PIXEL
    virtual void drawPoint(int x, int y, const xColor& color)                                   = 0;
#endif

#if IF_INVERT_EFFECT
    virtual void InvertRect(const chRect& rect) = 0;
#endif

#if IF_XWINDOW_STATION_ALPHA
    virtual void BlendGlobalAlpha(int nAlpha) = 0;
#endif

    virtual bool Capture(xPixmap& pixmap)       = 0;

    virtual int Width() const                   = 0;
    virtual int Height() const                  = 0;
    virtual int GetLineType() const             = 0;
    virtual int GetLineWidth() const            = 0;

#if IF_SCREEN_SUPPORT_DEV_PIXEL_SCALE
    virtual int GetDevPixelX(int nX)
    {
        return nX;
    }
    virtual int GetDevPixelY(int nY)
    {
        return nY;
    }
#endif
};

typedef xScreen* (*FUN_CREATE_SCREEN)();
typedef bool (*FUN_RELEASE_SCREEN)(xScreen*);

//////////////////////////////////////////////////////////////////////////////////////////////////
// xPainter
class xView;
class XWINDOW_EXPORT xPainter
{
public:
    void drawItemPixmap(const chRect& rect, xPixmap& pixmap);
    void drawItemPixmapEx(const chRect& rect, xPixmap& pixmap, const chRect& srcRect);

    void fillRoundPixmap(const chRect& rect, xPixmap& src);
    void fillExpandPixmap(const chRect& rect, xPixmap& src);

    void drawText(const chRect& rcTextBound, const char* pszText, int nAlign,
                  const xFont& font, const xColor& color);
    void drawTextEx(const chRect& rcTextBound, const char* pszText, int nAlign,
                    chPoint& ptOffset, const xFont& font, const xColor& color);

#if IF_INVERT_EFFECT
    void invertRect(const chRect& rect);
#endif

    void fillRect(const chRect& rect, xBrush& brush);
    void fillRect(const chRect& rect, xPixmap& pixmap);
    void fillRect(const chRect& rect, const xColor& color);
#if IF_ROUND_RECTANGLE
    void fillRoundedRect(const chRect& rect, int nRoundPixel, const xColor& color,
                         int nCornerType = CORNER_TYPE_ALL);
#endif

#if IF_PIXEL_ALPHA
    void blendRect(const chRect& rect, xBrush& brush);
    void blendRect(const chRect& rect, xPixmap& pixmap);
    void blendRect(const chRect& rect, const xColor& color);
#endif

    void drawRect(const chRect& rect, const xColor& color);
#if IF_ROUND_RECTANGLE
    void drawRoundedRect(const chRect& rect, int nRoundPixel, const xColor& color,
                         int nCornerType = CORNER_TYPE_ALL);
#endif
    void drawLineX(int y, int x1, int x2, const xColor& color);
    void drawLineY(int x, int y1, int y2, const xColor& color);

#if IF_SCREEN_SET_PIXEL
    void drawPoint(int x, int y, const xColor& color);
#endif

    void setClipRect(const chRect& rect, int nClipOperation, bool bMarkDirty = false);
    void setLineType(int nType);
    void setLineWidth(int nWidth);

    const xRegion& GetClipRegion() const;
    void SetClipRegion(const xRegion& rgnClip);

public:
    explicit xPainter(xScreen* pScreen, const chPoint& ptOffset, const xRegion& region);
    explicit xPainter(xScreen* pScreen, xView* pView, const xRegion& region);
    ~xPainter();

protected:
    bool IsInvalidDrawRgn(const chRect& rcDraw);

protected:
    xRegion         m_regionClip;
    xScreen*        m_pScreen;
    chPoint         m_ptViewOffset;
};

#if !IF_PIXEL_ALPHA
#define blendRect fillRect
#endif

#endif // __X_GRAPHIC_H__
