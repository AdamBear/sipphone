#ifndef _X_DATE_TIME_VIEW_H__
#define _X_DATE_TIME_VIEW_H__

#include "xwindow/xWindow.h"
#include "xwindow-kernel/include/xthreadtimer.h"
#include "baseui/include/baseuidefine.h"

bool PopupAndroidTimeDateFormat(xml_node & node, const char * pszName, int & value);
bool PopupAndroidDateFormat(xml_node & node, const char * pszName, int & value);
bool PopupAndroidTimeFormat(xml_node & node, const char * pszName, int & value);


class xDateTimeView : public xTextView, public xTimerHandler
{
    DECLARE_VIEW_DUMP_CLASS(xDateTimeView)

public:
    enum
    {
        DATE_TIME_SHOW_FORMAT_NONE                  = -1,       // Invalid date time format type.
        DATE_TIME_SHOW_FORMAT_TIME_DATE             = 0,        // Time Date
        DATE_TIME_SHOW_FORMAT_TWO_LINE              = 1,        // 两行，第一行时间，第二行日期，字体不一样
        DATE_TIME_SHOW_FORMAT_LEFT_DATE_MID_TIME    = 2,        // Date      Time       (right)
        DATE_TIME_SHOW_FORMAT_TIME             = 3,
        DATE_TIME_SHOW_FORMAT_MAX
    };

    enum
    {
        TIME_SHOW_FORMAT_NONE                       = -1,       // Invalid time format type.
        TIME_SHOW_FORMAT_HH_MM_APM                  = 0,        // HH:MM A/PM
        TIME_SHOW_FORMAT_HH_MM                      = 1,        // HH:MM
        TIME_SHOW_FORMAT_HH_MM_SS                   = 2,        // HH:MM:SS
        TIME_SHOW_FORMAT_HH_MM_SMALL_SS             = 3,        // HH:MM ss
        TIME_SHOW_FORMAT_SMALL_HH_MM_SS             = 4,        // hh:mm ss
        TIME_SHOW_FORMAT_MAX
    };

    struct ST_DATE_TIME_CACHE_DATA
    {
        yl::string  strDate;
        yl::string  strHour;
        yl::string  strMinute;
        yl::string  strSecond;
        chRect      rcColon;
        chSize      szDate;
        chSize      szTime;
        chSize      szSecond;

        void Reset()
        {
            strDate = "";
            strHour = "";
            strMinute = "";
            strSecond = "";
            rcColon.SetEmpty();
            szDate.cx = szDate.cy = 0;
            szTime.cx = szTime.cy = 0;
            szSecond.cx = szSecond.cy = 0;
        }
    };

public:
    // xTextView
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();
    virtual void loadAttributes(xml_node & node);

public:
    xDateTimeView();
    ~xDateTimeView();

    static void RegisterDataTimeView();

    void Init();

    // Date Time Show Format
    void SetDateTimeShowFormat(int nFormat);
    int GetDateTimeShowFormat()
    {
        return m_nDateTimeShowFormat;
    }

    // Time Show Format
    void SetTimeShowFormat(int nFormat);
    int GetTimeShowFormat()
    {
        return m_nTimeShowFormat;
    }

    void SetDateFormat(int nFormat);
    void SetTimeFormat(int nFormat);

    virtual void OnTextChange();

    //设置日期、时间字体大小
    void SetDateTimeSize(int nDateSize, int nTimeSize);

public:
    // 显示或隐藏信号槽
    void OnAppearSlot(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

protected:
    virtual bool onPaintContent(xPainter & p);

    void DrawTimeDateFormat(xPainter & p);
    void DrawTwoLineFormat(xPainter & p);
    void DrawDateTimeFormat(xPainter & p);
    void DrawTimeFormat(xPainter& p);

private:
    void OnTimerTick(xThreadTimer* timer, LPVOID pExtraData);

    void UpdateDateTimeInfo();

    void PaintTime(xPainter &, chRect & rcTime);
    void PaintDate(xPainter &, chRect & rcDate);

    void CachePaintData();
    void CacheDateSize();
    void CacheTimeSize();

    yl::string GetTimeShowString();
    int GetTimeWidth();

#if IF_SUPPORT_DATETIME_SHADOW
    void DrawTextShadow(xPainter& p, const chRect& rcTextBound,
                        const char* pszText, int nAlign,
                        const xFont& font, const xColor& color);
#endif

protected:
    int                     m_nDateTimeShowFormat;      // 时间、日期组合显示类型
    int                     m_nTimeShowFormat;          // 日期的显示类型
    int                     m_nDateFormat;              // 日期格式
    int                     m_nTimeFormat;              // 时间格式
    int                     m_nDateSize;                // 日期字体大小
    int                     m_nTimeSize;                // 时间字体大小

    int
    m_nLineSpacing;             // 多行显示时的行间距，默认1.5倍时间高度

    xThreadTimer            m_timerTick;                // 刷新计时器
    ST_DATE_TIME_CACHE_DATA m_stCacheData;              // 缓存绘制信息
    bool                    m_bShowColon;               // 是否显示冒号
#if IF_SUPPORT_DATETIME_SHADOW
    bool                    m_bShowShadow;              // 是否显示阴影
    xColor                  m_clrTimeShadow;
    xColor                  m_clrDateShadow;
#endif
};

#endif
