#include "mypainter.h"

#include "regexp/modregexp.h"
#include "ylstl/ylstringalgorithm.h"
#include "imageformatdefine.h"
#include "common/xwindow/xgraphicproess.h"
#include "languagehelper/modlangtranslate.h"

#ifdef _USE_SKIA
#include "imagepoint9manager.h"
#elif defined _XWIN

#else
// 将ylString转换为QString
QString toQString(const yl::string & other, bool bCheckFormat = false);
#endif

CMyImage::CMyImage()
#ifdef _USE_SKIA
    : SkBitmap()
#elif defined _XWIN
    : xPixmap()
    , m_pFlushGraphic(NULL)
#else
    : QImage()
#endif
    , m_bLoadSuccess(false)
    , m_strPic("")
{
}

CMyImage::CMyImage(int nWidth, int nHeight, MYPAINTER_FORMAT ePainterFormat)
#ifdef _USE_SKIA
    : SkBitmap()
#elif defined _XWIN
    : xPixmap()
    , m_pFlushGraphic(NULL)
    , nRotateAngle(0)
    , m_rcPaint(chRect())
#else
    : QImage(nWidth, nHeight, MYPAINTER_FORMAT_16 == ePainterFormat ? QImage::Format_RGB16 :
             QImage::Format_RGB32)
#endif
    , m_bLoadSuccess(true)
    , m_strPic("")
{
#ifdef _USE_SKIA
    allocPixels(SkImageInfo::Make(nWidth, nHeight
                                  , MYPAINTER_FORMAT_16 == ePainterFormat ? kRGB_565_SkColorType : kRGBA_8888_SkColorType
                                  , kPremul_SkAlphaType));
#elif defined _XWIN
    Create(EXP_SCREEN_HEIGHT, EXP_SCREEN_HEIGHT);
    if (m_pFlushGraphic.IsEmpty())
    {
        xGraphic * pFlushGraphic = new xGraphic;
        if (pFlushGraphic != NULL)
        {
            if (pFlushGraphic->createGraphic(nWidth, nHeight))
            {
                m_pFlushGraphic = xGraphicPtr(pFlushGraphic);
#warning m_nFormat
                //m_pFlushGraphic->m_nFormat = GRAPHIC_FORMAT_RGB16;
            }
        }
    }

#endif
}

CMyImage::CMyImage(const yl::string & strPic)
#ifdef _USE_SKIA
    : SkBitmap()
#elif defined _XWIN
    : xPixmap(strPic)
    , m_pFlushGraphic(NULL)
#else
    : QImage(strPic.c_str())
#endif
    , m_bLoadSuccess(false)
    , m_strPic("")
{
#ifdef _USE_SKIA
    m_bLoadSuccess = SkImageDecoder::DecodeFile(strPic.c_str(), this);
#else

#endif
    m_strPic = strPic;
    m_bLoadSuccess = load(strPic);
}

bool CMyImage::LoadImage(const yl::string & strPic)
{
    m_strPic = strPic;

#ifdef _USE_SKIA
    m_bLoadSuccess = SkImageDecoder::DecodeFile(strPic.c_str(), this);
#elif defined _XWIN
    m_bLoadSuccess = load(strPic);
#else
    m_bLoadSuccess = load(strPic.c_str());
#endif

    return m_bLoadSuccess;
}

yl::string CMyImage::GetImagePath()
{
    return m_strPic;
}

void * CMyImage::Getbits()
{
#ifdef _USE_SKIA
    return getPixels();
#elif defined _XWIN
    if (!m_pFlushGraphic.IsEmpty())
    {
        // 逆时针旋转
        switch (nRotateAngle)
        {
        case 0:
            {
            }
            break;
        case 270:
            {
                //g_GraphicConverter.Rotate270(*(GetGraphic().Get()), *(m_pFlushGraphic.Get()), chRect(0, 0, GetGraphic()->width(), GetGraphic()->height()), chRect(0, 0, m_pFlushGraphic->width(), m_pFlushGraphic->height()));

                xGraphic& src = *(GetGraphic().Get());
                xGraphic& dst = *(m_pFlushGraphic.Get());
                int nLeft = src.height() - m_rcPaint.bottom;
                chRect rcDst = rectLTWH(nLeft, m_rcPaint.left, m_rcPaint.Height(), m_rcPaint.right);

                xGraphicOperate::tranGraphic270(src, m_rcPaint, dst, rcDst, &xGraphicConverter::Copy_BGR565);

                m_rcPaint.SetEmpty();
            }
            break;
        default:
            break;
        }

        return m_pFlushGraphic->pixels();
    }
#else
    return bits();
#endif
    // 为过编译
    return NULL;
}

int CMyImage::GetWidth()
{
#ifdef _USE_SKIA
    return width();
#else
    return width();
#endif
}

int CMyImage::GetHeight()
{
#ifdef _USE_SKIA
    return height();
#else
    return height();
#endif
}

bool CMyImage::GetIsNull()
{
    return !m_bLoadSuccess;
}

void CMyImage::SaveImageToFile(const yl::string & strPicPath)
{
#ifdef _USE_SKIA
    SkImageEncoder::EncodeFile(strPicPath.c_str(), *this,
                               SkImageEncoder::kPNG_Type, /*quality range 0-100*/80);
#elif defined _XWIN
    save(strPicPath, chSize(width(), height()));
#else
    save(strPicPath.c_str());
#endif
}

CMyImage::~CMyImage()
{
#ifdef _XWIN
    if (!m_pFlushGraphic.IsEmpty())
    {
        m_pFlushGraphic.Reset();
    }
#endif // _XWIN
}

CMyRect::CMyRect()
#ifdef _USE_SKIA
    : SkRect()
#elif defined _XWIN
    : chRect()
#else
    : QRect()
#endif
    , m_bValid(false)
{

}

CMyRect::CMyRect(int nLeft, int nTop, int nWidth, int nHeight)
#ifdef _USE_SKIA
    : SkRect()
#elif defined _XWIN
    : chRect(nLeft, nTop, nLeft + nWidth, nTop + nHeight)
#else
    : QRect(nLeft, nTop, nWidth, nHeight)
#endif
    , m_bValid(true)
{
#ifdef _USE_SKIA
    set(nLeft, nTop, nLeft + nWidth, nTop + nHeight);
#endif
}

void CMyRect::SetRect(int nLeft, int nTop, int nWidth, int nHeight)
{
#ifdef _USE_SKIA
    set(nLeft, nTop, nLeft + nWidth, nTop + nHeight);
#elif defined _XWIN
    SetRect(nLeft, nTop, nLeft + nWidth, nTop + nHeight);
#else
    setRect(nLeft, nTop, nWidth, nHeight);
#endif
    m_bValid = true;
}

int CMyRect::GetLeft()
{
#ifdef _USE_SKIA
    return x();
#elif defined _XWIN
    return left;
#else
    return left();
#endif
}

int CMyRect::GetTop()
{
#ifdef _USE_SKIA
    return y();
#elif defined _XWIN
    return top;
#else
    return top();
#endif
}

int CMyRect::GetWidth()
{
#ifdef _USE_SKIA
    return width();
#elif defined _XWIN
    return Width();
#else
    return width();
#endif
}

int CMyRect::GetHeight()
{
#ifdef _USE_SKIA
    return height();
#elif defined _XWIN
    return Height();
#else
    return height();
#endif
}

CMyPainter::CMyPainter(CMyImage & myImage)
#ifdef _USE_SKIA
    : SkCanvas(myImage)
#elif defined _XWIN
    : xEXPPainter(myImage.GetGraphic().Get(), chPoint(0, 0))
    , m_Font()
    , m_Color()
    , m_pMyImagePaint(NULL)
#else
    : QPainter()
#endif
{
#ifdef _USE_SKIA
    SkTypeface * font = SkTypeface::CreateFromFile(FONT_PATH);
    if (NULL != font)
    {
        pen.setTypeface(font);
        font->unref();
    }
#elif defined _XWIN
    m_pMyImagePaint = &myImage;
#else
    begin(&myImage);
#endif
}

void CMyPainter::SetAntiAlias(bool SetOn)
{
#ifdef _USE_SKIA
    pen.setAntiAlias(SetOn);
#elif defined _XWIN
#else
    setRenderHint(SetOn ? QPainter::HighQualityAntialiasing : QPainter::NonCosmeticDefaultPen);
#endif
}

void CMyPainter::DrawImage(CMyRect rectDes, CMyImage & img, CMyRect rectSrc)
{
#ifdef _USE_SKIA
    if (rectSrc.GetValid())
    {
        drawBitmapRectToRect(img, &rectSrc, rectDes, NULL);
    }
    else
    {
        drawBitmapRect(img, rectDes, NULL);
    }
#elif defined _XWIN
    if (img.isNull())
    {
        return;
    }
    if (rectSrc.GetValid())
    {
        drawItemPixmapEx(rectDes, img, rectSrc);
    }
    else
    {
        drawItemPixmap(rectDes, img);
    }
    m_pMyImagePaint->m_rcPaint |= rectDes;
#else
    if (rectSrc.GetValid())
    {
        drawImage(rectDes, img, rectSrc);
    }
    else
    {
        drawImage(rectDes, img);
    }
#endif
}

void CMyPainter::DrawLine(int nStartLeft, int nStartTop, int nOverLeft, int nOverTop)
{
#ifdef _USE_SKIA
    drawLine(nStartLeft, nStartTop, nOverLeft, nOverTop, pen);
#elif defined _XWIN
    drawLine(nStartLeft, nStartTop, nOverLeft, nOverTop, m_Color);
    m_pMyImagePaint->m_rcPaint |= chRect(nStartLeft, nStartTop, nOverLeft, nOverTop);
#else
    drawLine(nStartLeft, nStartTop, nOverLeft, nOverTop);
#endif
}

void CMyPainter::DrawColor(CMyRect rectDes, int nRed, int nGreen, int nBlue)
{
#ifdef _USE_SKIA
    drawARGB(255, nRed, nGreen, nBlue);
#elif defined _XWIN
    fillRect(rectDes, xColor(nRed, nGreen, nBlue));
    m_pMyImagePaint->m_rcPaint |= rectDes;
#else
    fillRect(rectDes, QColor(nRed, nGreen, nBlue));
#endif
}

void CMyPainter::DrawGradient(CMyRect rectDes, int nRed, int nGreen, int nBlue)
{
#ifdef _USE_SKIA
    SkPoint points[2] =
    {
        SkPoint::Make(EXP_SCREEN_WIDTH / 2, 0),
        SkPoint::Make(EXP_SCREEN_WIDTH / 2, EXP_SCREEN_HEIGHT)
    };
    SkColor colors[2] = { SK_ColorWHITE, SK_ColorBLACK };
    colors[1] = SkColorSetRGB(nRed, nGreen, nBlue);

    SkScalar scalar[2] = { 0, 1 };

    SkPaint tmpPen = pen;
    pen.setShader(SkGradientShader::CreateLinear(
                      points, colors, scalar, 2,
                      SkShader::kClamp_TileMode));
    drawPaint(pen);
    pen = tmpPen;
#elif defined _XWIN
    drawRect(chRect(0, 0, EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT), xColor(nRed, nGreen, nBlue));
    m_pMyImagePaint->m_rcPaint |= chRect(0, 0, EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT);
#else
    QLinearGradient linearGradient(0, 0, 0, EXP_SCREEN_HEIGHT);
    linearGradient.setColorAt(0.0, QColor(nRed, nGreen, nBlue));
    linearGradient.setColorAt(1.0, Qt::white);

    setPen(QColor(nRed, nGreen, nBlue));
    setBrush(QBrush(linearGradient));

    drawRect(0, 0, EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT);
#endif
}

void CMyPainter::SetPenColor(int nRed, int nGreen, int nBlue, int nAlpha/* = 255*/)
{
#ifdef _USE_SKIA
    pen.setColor(SkColorSetARGB(nAlpha, nRed, nGreen, nBlue));
#elif defined _XWIN
    m_Color = xColor(nAlpha, nRed, nGreen, nBlue);
#else
    setPen(QColor(nRed, nGreen, nBlue, nAlpha));
#endif
}

void CMyPainter::SetFontSize(int nSize)
{
#ifdef _USE_SKIA
    pen.setTextSize(nSize);
#elif defined _XWIN
    m_Font.setPointSize(nSize);
#else
    QFont font;
    font.setPointSize(nSize);
    setFont(font);
#endif
}

void CMyPainter::DrawText(CMyRect rectDes, yl::string strText, bool bVCenter /*= true*/,
                          bool bSingleLine /*= true*/, int nFlag /*= DSSKEY_ALIGN_NORMOL*/)
{
#ifdef _USE_SKIA
    drawText(strText.c_str(), strText.size(), rectDes.GetLeft(),
             rectDes.GetTop() + (bVCenter ? TEXT_ALIGN_VERTICAL_MARGIN : 0), pen);
#elif defined _XWIN
    if (nFlag == DSSKEY_AlIGN_RIGHT)
    {
        drawText(rectDes, _TRAN(strText).c_str(), (bVCenter ? align_v_center : align_center) | align_right, m_Font, m_Color);
        return;
    }
    drawText(rectDes, _TRAN(strText).c_str(), (bVCenter ? align_v_center : align_center), m_Font, m_Color);
    m_pMyImagePaint->m_rcPaint |= rectDes;
#else
    if (nFlag == DSSKEY_AlIGN_RIGHT)
    {
        drawText(rectDes
                 , (bVCenter ? Qt::AlignVCenter : Qt::AlignCenter) | (bSingleLine ? Qt::TextSingleLine :
                         Qt::TextWordWrap) | Qt::AlignRight
                 , toQString(strText));
        return;
    }
    drawText(rectDes
             , (bVCenter ? Qt::AlignVCenter : Qt::AlignCenter) | (bSingleLine ? Qt::TextSingleLine :
                     Qt::TextWordWrap)
             , toQString(strText));
#endif
}

void CMyPainter::Translate(int nX, int nY)
{
#ifdef _USE_SKIA
    translate(nX, nY);
#elif defined _XWIN
    //m_ptViewOffset = chPoint(nX, nY);
#else
    translate(nX, nY);
#endif
}

void CMyPainter::Rotate(int nAngle)
{
#ifdef _USE_SKIA
    rotate(nAngle);
#elif defined _XWIN
    if (m_pMyImagePaint != NULL)
    {
        m_pMyImagePaint->nRotateAngle = 360 - nAngle;
    }
#else
    rotate(nAngle);
#endif
}

void CMyPainter::Save()
{
#ifdef _USE_SKIA
    save();
#elif defined _XWIN
    m_ColorOld = m_Color;
    m_FontOld = m_Font;
#else
    save();
#endif
}

void CMyPainter::Restore()
{
#ifdef _USE_SKIA
    restore();
#elif defined _XWIN
    m_Color = m_ColorOld;
    m_Font = m_FontOld;
#else
    restore();
#endif
}

#if (not defined _USE_SKIA) && (not defined _XWIN)
QString toQString(const yl::string & other, bool bCheckFormat/* = false*/)
{
    QString strTemp = QString::fromUtf8(other.c_str());

    if (bCheckFormat)
    {
        yl::string ylstring = strTemp.toUtf8().data();

        if (ylstring != other)
        {
            // 根据不同的编码类型转换编码
            return QString::fromLatin1(other.c_str());
        }
    }

    return strTemp;
}
#endif

int CMyPainter::GetTextWidth(const yl::string & strText)
{
#ifdef _USE_SKIA
    return pen.measureText(strText.c_str(), strText.size());
#elif defined _XWIN
    return m_Font.textBound(strText).cx;
#else
    return fontMetrics().width(toQString(strText));
#endif
}

// 截断过长的Text
// iWidth为Text显示的宽度
yl::string CMyPainter::CutOffText(const yl::string & strText, int iWidth)
{
    int iTextWidth = GetTextWidth(strText);

    // 如下情况返回原字符
    // 字符串为空
    // 显示宽度大于文字长度
    if (strText.empty()
            || iWidth >= iTextWidth)
    {
        return strText;
    }

    // 显示宽度小于末端符号返回原字符
    yl::string strEndSymbol("...");
    iWidth -= GetTextWidth(strEndSymbol);
    if (iWidth <= 0)
    {
        return strText;
    }

    // 二分法确定截断位置的范围
    int iMinIndex = 0;
    int iMaxIndex = strText.size() - 1;
    int iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    int iInterval = 2;

    // 循环确定截断的范围,范围大小为0~2*iInterval
    while (iMaxIndex > iMidIndex + iInterval)
    {
        if (iWidth >= GetTextWidth(strText.substr(0, iMidIndex)))
        {
            iMinIndex = iMidIndex;
        }
        else
        {
            iMaxIndex = iMidIndex - 1;
        }

        iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    }

    // 逆向遍历找出第一个符合的位置
    while (iMinIndex <= iMaxIndex)
    {
        if (iWidth > GetTextWidth(strText.substr(0, iMaxIndex)))
        {
            break;
        }

        --iMaxIndex;
    }

    yl::string strSrcW = strText;
    if (Lang_IsRTL(strSrcW))
    {
        static yl::string exp_index_dot("\\d+[.]{1}.*");
        if (0 == regExp_IsMatchedExact(exp_index_dot.c_str(), strText.c_str()))
        {
            // 处理 1. ABCDEFG  截断，处理之后为1. ...ABC
            int iPos = strSrcW.find('.');
            if (iPos != -1 && iPos + 1 < strSrcW.length())
            {
                yl::string strPrefix = strSrcW.substr(0, iPos + 1);

                int iFrom = iPos;
                while ((strSrcW[iFrom] == ' ' || strSrcW[iFrom] == ':' || strSrcW[iFrom] == '.')
                        && iFrom < strSrcW.length())
                {
                    ++iFrom;
                }

                if (iFrom < strSrcW.length())
                {
                    yl::string strSuffer = strSrcW.substr(0, iFrom);

                    yl::string strPre = strPrefix + " ";
                    if (iWidth - GetTextWidth(strPre) <= 0)
                    {
                        return strText;
                    }

                    return strPre + CutOffText(strSuffer, iWidth - GetTextWidth(strPre));
                }

                return strText;
            }
        }

        // ABCDEFG => ...ABC
        return strEndSymbol + strText.substr(0, iMaxIndex);
    }

    return strText.substr(0, iMaxIndex) + strEndSymbol;
}

bool CMyPainter::IsRTL(unsigned short wch)
{
    return (wch >= 0x0590 && wch <= 0x05FF) || (wch >= 0x0600 && wch <= 0x06FF)
           || (wch >= 0xFB50 && wch <= 0xFDFF) || (wch >= 0xFE70 && wch <= 0xFEFF);
}

bool CMyPainter::Lang_IsRTL(const yl::string & qStr)
{
    yl::wstring wStr = yl::to_utf16(qStr);

    if (wStr.size() <= 1)
    {
        return false;
    }

    for (int i = 0; i < wStr.size(); ++i)
    {
        if (IsRTL(wStr[i]))
        {
            return true;
        }
    }

    return false;
}

CMyImageInfo::CMyImageInfo()
    : m_myImage(NULL)
{
#if (defined _USE_SKIA) || (defined _XWIN)
    m_myImage = new CMyImage();
#else
    m_myImage = GetImageInfoByType(DIT_THEME);
#endif
}

CMyImageInfo::~CMyImageInfo()
{
    if (NULL != m_myImage)
    {
        delete m_myImage;
    }
}

void CMyImageInfo::LoadImageInfo(const yl::string & strPic)
{
    if (NULL == m_myImage)
    {
        return;
    }

#ifdef _USE_SKIA
    m_myImage->LoadImage(strPic);
    GetPoint9Info(*m_myImage);
#elif defined _XWIN
    m_myImage->LoadImage(strPic);
#else
    m_myImage->LoadImageInfo(toQString(strPic));
    GetImagePoint9Manager().GetPoint9Info(m_myImage);
#endif
}

bool CMyImageInfo::IsImageNull()
{
    if (NULL == m_myImage)
    {
        return true;
    }

#if (defined _USE_SKIA) || (defined _XWIN)
    return m_myImage->GetIsNull();
#else
    return m_myImage->IsNull();
#endif
}

void CMyImageInfo::DrawPointImage(CMyPainter & myPainter, CMyRect myRect)
{
    if (NULL == m_myImage)
    {
        return;
    }

#ifdef _USE_SKIA
    PaintDraw9PixByImageInfo(myPainter
                             , *m_myImage
                             , myRect);
#elif defined _XWIN
    myPainter.DrawDot9Pixmap(*m_myImage, myRect);
    myPainter.m_pMyImagePaint->m_rcPaint |= myRect;
#else
    Theme_Draw_9_Pix_ImageInfo(myPainter
                               , m_myImage
                               , myRect);
#endif
}

yl::string CMyImageInfo::GetPicRootPath()
{
#if (defined _USE_SKIA) || (defined _XWIN)
    return PIC_ROOT_PATH;
#else
    return GetImagePathConvertor().GetPicPathRoot().c_str();
#endif
}
