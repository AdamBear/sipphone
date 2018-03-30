#include "xdatetimeview.h"
#include "devicelib/networkserver.h"
#include "languagehelper/modlangtranslate.h"
#include "commonapi/stringhelperapi.h"
#include "xwindow/modviewdump.h"
#include "baseuicommon.h"

#include "dsklog/log.h"


namespace
{
#ifdef _CP920
#define DATE_TIME_SECOND_FONT_SIZE 9
#else
#define DATE_TIME_SECOND_FONT_SIZE 10
#endif
#if IF_SUPPORT_DATETIME_SHADOW
const char* BASE_UI_ATTR_SHOW_SHADOW = "android:bShowShadow";       // 是否显示阴影
const char* BASE_UI_ATTR_SHADOW_COLOR_TIME = "android:timeShadowColor";       // 时间阴影颜色
const char* BASE_UI_ATTR_SHADOW_COLOR_DATE = "android:dateShadowColor";       // 日期阴影颜色
#define DATE_TIEM_SHADOW_OFFSET     1

#endif
}

static int GetMax(int x, int y)
{
    return (x > y ? x : y);
}

static int GetMin(int x, int y)
{
    return (x < y ? x : y);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PopupAndroidTimeDateFormat(xml_node & node, const char * pszName, int & value)
{
    // 设置默认格式
    value = xDateTimeView::DATE_TIME_SHOW_FORMAT_TIME_DATE;

    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();
    strAttr = strAttr.trim_both();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    if (BASE_UI_ENUM_DATE_TIME_FORMAT_TWO_LINE == strAttr)
    {
        value = xDateTimeView::DATE_TIME_SHOW_FORMAT_TWO_LINE;
    }
    else if (BASE_UI_ENUM_DATE_TIME_FORMAT_LEFT_DATE_MID_TIME == strAttr)
    {
        value = xDateTimeView::DATE_TIME_SHOW_FORMAT_LEFT_DATE_MID_TIME;
    }
    else if (BASE_UI_ENUM_DATE_TIME_FORMAT_TIME == strAttr)
    {
        value = xDateTimeView::DATE_TIME_SHOW_FORMAT_TIME;
    }

    return true;
}

bool PopupAndroidTimeFormat(xml_node & node, const char * pszName, int & value)
{
    // 设置默认格式
    value = xDateTimeView::TIME_SHOW_FORMAT_HH_MM_APM;

    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();
    strAttr = strAttr.trim_both();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    if (BASE_UI_ENUM_TIME_FORMAT_HH_MM == strAttr)
    {
        value = xDateTimeView::TIME_SHOW_FORMAT_HH_MM;
    }
    else if (BASE_UI_ENUM_TIME_FORMAT_HH_MM_SS == strAttr)
    {
        value = xDateTimeView::TIME_SHOW_FORMAT_HH_MM_SS;
    }
    else if (BASE_UI_ENUM_TIME_FORMAT_HH_MM_SMALL_SS == strAttr)
    {
        value = xDateTimeView::TIME_SHOW_FORMAT_HH_MM_SMALL_SS;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xDateTimeView);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_VIEW_DUMP_BEGIN(xDateTimeView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(datetimeformat, m_nDateTimeShowFormat, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(dateformat, m_nDateFormat, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(timeformat, m_nTimeShowFormat, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(datesize, m_nDateSize, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(timesize, m_nTimeSize, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(showcolon, m_bShowColon, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xDateTimeView, xTextView)
IMPLEMENT_VIEW_DUMP_PROPERTY(datetimeformat)
IMPLEMENT_VIEW_DUMP_PROPERTY(dateformat)
IMPLEMENT_VIEW_DUMP_PROPERTY(timeformat)
IMPLEMENT_VIEW_DUMP_PROPERTY(datesize)
IMPLEMENT_VIEW_DUMP_PROPERTY(timesize)
IMPLEMENT_VIEW_DUMP_PROPERTY(showcolon)
IMPLEMENT_VIEW_DUMP_END(xDateTimeView)

//////////////////////////////////////////////////////////////////////////
// class xDateTimeView : public xTextView
xDateTimeView::xDateTimeView()
    : m_nDateTimeShowFormat(DATE_TIME_SHOW_FORMAT_TIME_DATE)
    , m_nTimeShowFormat(TIME_SHOW_FORMAT_HH_MM)
    , m_nDateFormat(0)
    , m_nTimeFormat(0)
    , m_nDateSize(-1)
    , m_nTimeSize(-1)
    , m_nLineSpacing(-1)
    , m_bShowColon(true)
#if IF_SUPPORT_DATETIME_SHADOW
    , m_bShowShadow(false)
    , m_clrTimeShadow(xColor(0x00, 0x00, 0x00))
    , m_clrDateShadow(xColor(0x00, 0x00, 0x00))
#endif
{
    REGISTER_VIEW_DUMP_CLASS(xDateTimeView)

    SLOT_CONNECT(this, signalAppear, this, xDateTimeView::OnAppearSlot);
}

xDateTimeView::~xDateTimeView()
{
    SLOT_DISCONNECT(this, signalAppear, this, xDateTimeView::OnAppearSlot);
}

void xDateTimeView::RegisterDataTimeView()
{
    // T2X编译成.a时，如果没有调用这个函数，EXPORT_VIEW_CLASS无效
}

void xDateTimeView::Init()
{
    CachePaintData();
}

void xDateTimeView::loadAttributes(xml_node & node)
{
    xTextView::loadAttributes(node);

    PopupAndroidPixel(node, BASE_UI_ATTR_TIME_SIZE, m_nTimeSize);
    PopupAndroidPixel(node, BASE_UI_ATTR_DATE_SIZE, m_nDateSize);

    if (m_nTimeSize < 0)
    {
        m_nTimeSize = m_nDateSize;
    }

    if (m_nDateSize < 0)
    {
        m_nDateSize = m_nTimeSize;
    }

    PopupAndroidPixel(node, BASE_UI_ATTR_LINE_SPACING, m_nLineSpacing);
    PopupAndroidTimeDateFormat(node, BASE_UI_ATTR_DATE_TIME_FORMAT, m_nDateTimeShowFormat);
    PopupAndroidTimeFormat(node, BASE_UI_ATTR_TIME_FORMAT, m_nTimeShowFormat);
#if IF_SUPPORT_DATETIME_SHADOW
    PopupAndroidBool(node, BASE_UI_ATTR_SHOW_SHADOW, m_bShowShadow);
    PopupAndroidColor(node, BASE_UI_ATTR_SHADOW_COLOR_TIME, m_clrTimeShadow);
    PopupAndroidColor(node, BASE_UI_ATTR_SHADOW_COLOR_DATE, m_clrDateShadow);
#endif
}

void xDateTimeView::SetDateTimeShowFormat(int nFormat)
{
    if (nFormat == m_nDateTimeShowFormat)
    {
        return;
    }

    m_nDateTimeShowFormat = nFormat;

    onContentChanged();

    CachePaintData();
}

void xDateTimeView::SetTimeShowFormat(int nFormat)
{
    if (nFormat == m_nTimeShowFormat)
    {
        return;
    }

    m_nTimeShowFormat = nFormat;

    CacheTimeSize();

    onContentChanged();
}

void xDateTimeView::SetDateFormat(int nFormat)
{
    if (nFormat == m_nDateFormat)
    {
        return;
    }

    m_nDateFormat = nFormat;

    CacheDateSize();

    onContentChanged();
}

void xDateTimeView::SetTimeFormat(int nFormat)
{
    if (nFormat == m_nTimeFormat)
    {
        return;
    }

    m_nTimeFormat = nFormat;

    CacheTimeSize();

    onContentChanged();
}

void xDateTimeView::OnTextChange()
{
    CacheDateSize();

    xTextView::OnTextChange();
}

void xDateTimeView::SetDateTimeSize(int nDateSize, int nTimeSize)
{
    if (nDateSize >= 0 && nTimeSize >= 0)
    {
        m_nDateSize = nDateSize;
        m_nTimeSize = nTimeSize;
    }
}

void xDateTimeView::OnAppearSlot(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                 LPVOID pData)
{
    m_timerTick.KillTimer();

    if (1 == wParam)
    {
        UpdateDateTimeInfo();
        m_timerTick.SetTimer(1000, TIMER_METHOD(this, xDateTimeView::OnTimerTick));
    }
}

void xDateTimeView::CachePaintData()
{
    m_stCacheData.strDate = Date2String();

    SetText(m_stCacheData.strDate);
    CacheDateSize();

    CacheTimeSize();

    if (DATE_TIME_SHOW_FORMAT_TWO_LINE == m_nDateTimeShowFormat && m_nLineSpacing < 0)
    {
        m_nLineSpacing = m_stCacheData.szTime.cy >> 1;
    }
}

void xDateTimeView::CacheDateSize()
{
    font.setPointSize(m_nDateSize);

    m_stCacheData.szDate = font.textBound(m_strText);
}

void xDateTimeView::CacheTimeSize()
{
    ClockTime2String(m_stCacheData.strHour, m_stCacheData.strMinute, m_stCacheData.strSecond);

    yl::string strTime = GetTimeShowString();
    if (TIME_SHOW_FORMAT_SMALL_HH_MM_SS == m_nTimeShowFormat)
    {
        font.setPointSize(m_nTimeSize);
        chSize szTime = font.textBound(strTime);
        font.setPointSize(DATE_TIME_SECOND_FONT_SIZE);
        m_stCacheData.szTime = font.textBound(strTime);
        m_stCacheData.szTime.cy = szTime.cy;
    }
    else
    {
        font.setPointSize(m_nTimeSize);
        m_stCacheData.szTime = font.textBound(strTime);
    }

    chSize szHour = font.textBound(m_stCacheData.strHour);
    yl::string strPreMin = m_stCacheData.strHour + " ";
    chSize szPreMin = font.textBound(strPreMin);

    m_stCacheData.rcColon.SetRect(szHour.cx, 0, szPreMin.cx, szHour.cy);

    if (TIME_SHOW_FORMAT_HH_MM_SMALL_SS == m_nTimeShowFormat
            || TIME_SHOW_FORMAT_SMALL_HH_MM_SS == m_nTimeShowFormat)
    {
        int nSecondFontSize = DATE_TIME_SECOND_FONT_SIZE;
        yl::string strSecond = _TRAN(m_stCacheData.strSecond);

        strSecond = " " + strSecond;
        font.setPointSize(nSecondFontSize);
        m_stCacheData.szSecond = font.textBound(strSecond);
    }
    else
    {
        m_stCacheData.szSecond = chSize(0, 0);
    }

}

yl::string xDateTimeView::GetTimeShowString()
{
    yl::string strTime;

    switch (m_nTimeShowFormat)
    {
    case TIME_SHOW_FORMAT_HH_MM_APM:    // HH:MM A/PM
        {
            strTime = m_stCacheData.strHour + " " + m_stCacheData.strMinute + " " + m_stCacheData.strSecond;
        }
        break;
    case TIME_SHOW_FORMAT_HH_MM:    // HH:MM
        {
            strTime = m_stCacheData.strHour + " " + m_stCacheData.strMinute;
        }
        break;
    case TIME_SHOW_FORMAT_HH_MM_SS: // HH:MM:SS
        {
            strTime = m_stCacheData.strHour + ":" + m_stCacheData.strMinute + ":" + m_stCacheData.strSecond;
        }
        break;
    case TIME_SHOW_FORMAT_HH_MM_SMALL_SS:   // HH:MM ss
    case TIME_SHOW_FORMAT_SMALL_HH_MM_SS:   // hh:mm ss
        {
            strTime = m_stCacheData.strHour + " " + m_stCacheData.strMinute;
        }
        break;
    default:
        break;
    }

    return strTime;
}

void xDateTimeView::OnTimerTick(xThreadTimer* timer, LPVOID pExtraData)
{
    UpdateDateTimeInfo();
}

void xDateTimeView::UpdateDateTimeInfo()
{
    ClockTime2String(m_stCacheData.strHour, m_stCacheData.strMinute, m_stCacheData.strSecond);

    yl::string strDate = Date2String();

    if (strDate != m_stCacheData.strDate)
    {
        SetText(strDate);
        m_stCacheData.strDate = strDate;
    }

    if (TIME_SHOW_FORMAT_HH_MM_APM == m_nTimeShowFormat || TIME_SHOW_FORMAT_HH_MM == m_nTimeShowFormat)
    {
        m_bShowColon = !m_bShowColon;
    }

    update();
}

bool xDateTimeView::onPaintContent(xPainter & p)
{
    switch (m_nDateTimeShowFormat)
    {
    case DATE_TIME_SHOW_FORMAT_TIME_DATE:
        {
            DrawTimeDateFormat(p);
        }
        break;
    case DATE_TIME_SHOW_FORMAT_TWO_LINE:
        {
            DrawTwoLineFormat(p);
        }
        break;
    case DATE_TIME_SHOW_FORMAT_LEFT_DATE_MID_TIME:
        {
            DrawDateTimeFormat(p);
        }
        break;
    case DATE_TIME_SHOW_FORMAT_TIME:
        {
            DrawTimeFormat(p);
        }
        break;
    default:
        break;
    }

    return true;
}

void xDateTimeView::DrawTimeDateFormat(xPainter & p)
{
    chSize sizeBlank = font.textBound(" ");
    chSize sizeWgt(sizeBlank.cx + m_stCacheData.szTime.cx + m_stCacheData.szDate.cx, sizeBlank.cy);
    chRect rectTextBound(chPoint(0, 0), sizeWgt);

    adjustRectByGravity(contentRect(), gravity, rectTextBound);

    // time
    chRect rcTime(chPoint(rectTextBound.left, rectTextBound.top), m_stCacheData.szTime);

    PaintTime(p, rcTime);

    // date
    chRect rcDate(chPoint(rectTextBound.left + m_stCacheData.szTime.cx + sizeBlank.cx,
                          rectTextBound.top), m_stCacheData.szDate);

    PaintDate(p, rcDate);
}

void xDateTimeView::DrawTwoLineFormat(xPainter & p)
{
    int nTimeWidth = GetTimeWidth();
    int nTextWidth = GetMax(nTimeWidth, m_stCacheData.szDate.cx);
    int nTextHeight = m_stCacheData.szTime.cy + m_stCacheData.szDate.cy + m_nLineSpacing;
    chRect rectTextBound(0, 0, nTextWidth, nTextHeight);

    adjustRectByGravity(contentRect(), gravity, rectTextBound);

    // time
    chRect rcTime(0, 0, nTimeWidth, m_stCacheData.szTime.cy);

    adjustRectByGravity(rectTextBound, gravity & gravity_x_mask | gravity_top, rcTime);

    PaintTime(p, rcTime);

    // date
    rectTextBound.top = rectTextBound.top + rcTime.Height() + m_nLineSpacing;

    chRect rcDate(chPoint(0, 0), m_stCacheData.szDate);

    adjustRectByGravity(rectTextBound, gravity & gravity_x_mask | gravity_top, rcDate);

    PaintDate(p, rcDate);
}

void xDateTimeView::DrawDateTimeFormat(xPainter & p)
{
    chRect rcContent = contentRect();

    // date
    chRect rectDate(chPoint(0, 0), m_stCacheData.szDate);

    adjustRectByGravity(rcContent, gravity_left | gravity_y_center, rectDate);
    PaintDate(p, rectDate);

    // time
    chRect rectTime(chPoint(0, 0), m_stCacheData.szTime);

    adjustRectByGravity(rcContent, align_center, rectTime);
    PaintTime(p, rectTime);
}

void xDateTimeView::DrawTimeFormat(xPainter& p)
{
#if IF_SHOW_TWOLINE_TIME
    int nTimeWidth = GetTimeWidth();
    int nTextWidth = GetMax(nTimeWidth, m_stCacheData.szDate.cx);
    int nTextHeight = m_stCacheData.szTime.cy + m_stCacheData.szDate.cy + m_nLineSpacing;
    chRect rectTextBound(0, 0, nTextWidth, nTextHeight);
    adjustRectByGravity(contentRect(), gravity, rectTextBound);
    // time
    chRect rcTime(0, 0, nTimeWidth, m_stCacheData.szTime.cy);
    adjustRectByGravity(rectTextBound, gravity & gravity_x_mask | gravity_top, rcTime);
    PaintTime(p, rcTime);
#else
    chRect rcContent = contentRect();
    // time
    chRect rectTime(chPoint(0, 0), m_stCacheData.szTime);
    adjustRectByGravity(rcContent, align_center, rectTime);
    PaintTime(p, rectTime);
#endif
}

#if IF_SUPPORT_DATETIME_SHADOW
void xDateTimeView::DrawTextShadow(xPainter& p, const chRect& rcTextBound,
                                   const char* pszText, int nAlign,
                                   const xFont& font, const xColor& color)
{
    chRect rcShadow = rcTextBound;
    rcShadow.OffsetRect(DATE_TIEM_SHADOW_OFFSET, DATE_TIEM_SHADOW_OFFSET);
    p.drawText(rcShadow, pszText, nAlign, font, color);
}
#endif

void xDateTimeView::PaintDate(xPainter & p, chRect & rcDate)
{
    font.setPointSize(m_nDateSize);

#if IF_SUPPORT_DATETIME_SHADOW
    if (m_bShowShadow)
    {
        m_rcText = rcDate;
        m_rcText.OffsetRect(DATE_TIEM_SHADOW_OFFSET, DATE_TIEM_SHADOW_OFFSET);
        DrawText(p, m_clrDateShadow);
    }
#endif
    // paint date
    m_rcText = rcDate;
    DrawText(p, textColor);
}

void xDateTimeView::PaintTime(xPainter & p, chRect & rcTime)
{
    chRect rcPaintTime = rcTime;
    if (TIME_SHOW_FORMAT_SMALL_HH_MM_SS == m_nTimeShowFormat)
    {
        font.setPointSize(DATE_TIME_SECOND_FONT_SIZE);
        // 实际该格式 时分和秒的字体大小一致，顶部偏移也一致，
        rcPaintTime = chRect(rcPaintTime.left, rcPaintTime.bottom - m_stCacheData.szSecond.cy,
                             rcPaintTime.right, rcPaintTime.bottom);
    }
    else
    {
        font.setPointSize(m_nTimeSize);
    }

    int nAlign = align & gravity_y_mask | align_left;
    yl::string strTime = GetTimeShowString();

    if (TIME_SHOW_FORMAT_HH_MM_APM == m_nTimeShowFormat
            || TIME_SHOW_FORMAT_HH_MM_SS == m_nTimeShowFormat)
    {
        strTime = _TRAN(strTime);
    }

#if IF_SUPPORT_DATETIME_SHADOW
    if (m_bShowShadow)
    {
        DrawTextShadow(p, rcPaintTime, strTime.c_str(), nAlign, font, m_clrTimeShadow);
    }
#endif
    p.drawText(rcPaintTime, strTime.c_str(), nAlign, font, textColor);

    if (m_bShowColon)
    {
        chRect rcColon = m_stCacheData.rcColon;

        rcColon.OffsetRect(rcPaintTime.left, rcPaintTime.top);

#if IF_SUPPORT_DATETIME_SHADOW
        if (m_bShowShadow)
        {
            DrawTextShadow(p, rcColon, ":", align_center, font, m_clrTimeShadow);
        }
#endif
        p.drawText(rcColon, ":", align_center, font, textColor);
    }

    if (TIME_SHOW_FORMAT_HH_MM_SMALL_SS == m_nTimeShowFormat
            || TIME_SHOW_FORMAT_SMALL_HH_MM_SS == m_nTimeShowFormat)
    {
        chRect rcSecond(rcPaintTime.left + m_stCacheData.szTime.cx,
                        rcPaintTime.bottom - m_stCacheData.szSecond.cy,
                        rcPaintTime.right, rcPaintTime.bottom);
        if (TIME_SHOW_FORMAT_HH_MM_SMALL_SS == m_nTimeShowFormat)
        {
            // 偏移使秒数底部与时分对齐
            rcSecond.OffsetRect(0, -3);
        }

        yl::string strSecond = _TRAN(m_stCacheData.strSecond);
        strSecond = " " + strSecond;
        font.setPointSize(DATE_TIME_SECOND_FONT_SIZE);
#if IF_SUPPORT_DATETIME_SHADOW
        if (m_bShowShadow)
        {
            DrawTextShadow(p, rcSecond, strSecond.c_str(), nAlign, font, m_clrTimeShadow);
        }
#endif
        p.drawText(rcSecond, strSecond.c_str(), nAlign, font, textColor);
    }
}

int xDateTimeView::GetTimeWidth()
{
    if (TIME_SHOW_FORMAT_HH_MM_SMALL_SS == m_nTimeShowFormat
            || TIME_SHOW_FORMAT_SMALL_HH_MM_SS == m_nTimeShowFormat)
    {
        return (m_stCacheData.szTime.cx + m_stCacheData.szSecond.cx);
    }
    else
    {
        return m_stCacheData.szTime.cx;
    }
}

int xDateTimeView::wrapContentWidth()
{
    if (DATE_TIME_SHOW_FORMAT_TWO_LINE == m_nDateTimeShowFormat)
    {
        return GetMax(m_stCacheData.szDate.cx, GetTimeWidth());
    }
    else
    {
        return (m_stCacheData.szDate.cx + GetTimeWidth());
    }
}

int xDateTimeView::wrapContentHeight()
{
    if (DATE_TIME_SHOW_FORMAT_TWO_LINE == m_nDateTimeShowFormat)
    {
        return (m_stCacheData.szTime.cy + m_nLineSpacing + m_stCacheData.szDate.cy);
    }
    else
    {
        return m_stCacheData.szTime.cy;
    }
}
