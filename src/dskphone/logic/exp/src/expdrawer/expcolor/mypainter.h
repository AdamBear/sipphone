#ifndef _EXP_MY_PAINTER_H
#define _EXP_MY_PAINTER_H

#include "expdefine.h"

#ifdef _USE_SKIA
#include "core/SkCanvas.h"
#include "core/SkPaint.h"
#include "core/SkBitmap.h"
#include "core/SkImage.h"
#include "core/SkRect.h"
#include "core/SkTypeface.h"
#include "core/SkImageDecoder.h"
#include "effects/SkGradientShader.h"

#include <mutex>

#include "picpath.h"
#include "t4xpicpath.h"
#elif defined _XWIN
#include "common/xwindow/xGraphic.h"
#include "xexppainter.h"
#else
#include <QImage>
#include <QRect>
#include <QPainter>
#include <QString>
#include <QColor>
#include <QMutex>

#include "imagemanager/modimagemanager.h"
#include "imagemanager/imagecacheaider.h"
#include "imagemanager/imagepoint9manager.h"
#include "imagemanager/imagepathconvertor.h"
#endif

enum MYPAINTER_FORMAT
{
    MYPAINTER_FORMAT_16,
    MYPAINTER_FORMAT_32,
};

#ifdef _USE_SKIA
struct Point9Info
{
    //是否水平拉伸
    bool            m_bHStretch;
    //是否竖直拉伸
    bool            m_bVStretch;

    //水平拉伸关键点位置
    int         m_nHStart;
    int         m_nHOver;

    //竖直拉伸关键点位置
    int         m_nVStart;
    int         m_nVOver;

    Point9Info()
    {
        ResetPoint9Info();
    }

    void ResetPoint9Info()
    {
        m_bHStretch = false;
        m_bVStretch = false;

        m_nHStart = 0;
        m_nHOver = 0;
        m_nVStart = 0;
        m_nVOver = 0;
    }
};
#endif

class CMyImage
#ifdef _USE_SKIA
    : public SkBitmap
#elif defined _XWIN
    : public xPixmap
#else
    : public QImage
#endif
{
public:
    CMyImage();
    CMyImage(int nWidth, int nHeight, MYPAINTER_FORMAT ePainterFormat);
    CMyImage(const yl::string & strPic);

    virtual ~CMyImage();

public:
    bool LoadImage(const yl::string & strPic);
    yl::string GetImagePath();

    void * Getbits();

    int GetWidth();
    int GetHeight();

    bool GetIsNull();

    void SaveImageToFile(const yl::string & strPicPath);

public:
    yl::string m_strPic;
    bool m_bLoadSuccess;

#ifdef _USE_SKIA
public:
    Point9Info  m_Point9Info;
#elif defined _XWIN
public:
    xGraphicPtr m_pFlushGraphic;
    int nRotateAngle;
    chRect m_rcPaint;
#endif
};

class CMyRect
#ifdef _USE_SKIA
    : public SkRect
#elif defined _XWIN
    : public chRect
#else
    : public QRect
#endif
{
public:
    CMyRect();
    CMyRect(int nLeft, int nTop, int nWidth, int nHeight);

//  virtual ~CMyRect();

    void SetRect(int nLeft, int nTop, int nWidth, int nHeight);

    int GetLeft();
    int GetTop();
    int GetWidth();
    int GetHeight();

public:
    bool GetValid()
    {
        return m_bValid;
    }

private:
    bool m_bValid;

};

class CMyPainter
#ifdef _USE_SKIA
    : public SkCanvas
#elif defined _XWIN
    : public xEXPPainter
#else
    : public QPainter
#endif
{
public:
    explicit CMyPainter(CMyImage & myImage);
//  virtual ~CMyPainter();

public:
    void SetAntiAlias(bool SetOn);

public:
    void DrawImage(CMyRect rectDes, CMyImage & img, CMyRect rectSrc = CMyRect());
    void DrawLine(int nStartLeft, int nStartTop, int nOverLeft, int nOverTop);

    void DrawColor(CMyRect rectDes, int nRed, int nGreen, int nBlue);
    void DrawGradient(CMyRect rectDes, int nRed, int nGreen, int nBlue);

    void SetPenColor(int nRed, int nGreen, int nBlue, int nAlpha = 255);
    void SetFontSize(int nSize);

    void DrawText(CMyRect rectDes, yl::string strText, bool bVCenter = true, bool bSingleLine = true, int nFlag = DSSKEY_ALIGN_NORMOL);

    void Translate(int nX, int nY);
    void Rotate(int nAngle);

    void Save();
    void Restore();

    yl::string CutOffText(const yl::string & strText, int iWidth);
#ifdef _XWIN
    void SetOffset(int x, int y)
    {
        m_ptViewOffset = chPoint(x, y);
    }
#endif // _XWIN

private:
    int GetTextWidth(const yl::string & strText);

    static bool IsRTL(unsigned short wch);
    static bool Lang_IsRTL(const yl::string & qStr);

#ifdef _USE_SKIA
protected:
    SkPaint pen;
#elif defined _XWIN
public:
    xFont               m_Font;
    xColor              m_Color;
    xFont               m_FontOld;
    xColor              m_ColorOld;
    CMyImage *          m_pMyImagePaint;
#endif
};

class CMyImageInfo
{
public:
    CMyImageInfo();
    ~CMyImageInfo();

public:
    void LoadImageInfo(const yl::string & strPic);

    bool IsImageNull();

    void DrawPointImage(CMyPainter & myPainter, CMyRect myRect);

    static yl::string GetPicRootPath();
protected:
#if (defined _USE_SKIA) || (defined _XWIN)
    CMyImage        *        m_myImage;
#else
    CImageBaseInfo   *   m_myImage;
#endif
};

#endif //_EXP_MY_PAINTER_H
