#ifndef __SCROLL_TEXT_H__
#define __SCROLL_TEXT_H__

#include <QWidget>
#include <QString>
#include <QTimer>
#include "colortextdelegateT9.h"
// 选择文字内容滚动方式
// 0方式：从起点左滚到右端点，再从右端点滚到起点左，如此循环
// 1方式：从起点左滚到右端点，再从起点左滚到右端点，如此循环
#define TEXT_SCROLL_WAY 1

// 支持同一个Widget多个文本同时滚动
class CScrollText
{
public:
    explicit CScrollText(QWidget * pWidget);
    virtual ~CScrollText();

    static bool IsNeedScroll(QWidget * pWidget, int nShowWidth, QString & strText);

    static void ResetInterval();
    static void SetInterval(int nMilliSecond);

protected:
    static void UpdateScrollStopCount();

public:
    bool IsScrolling();
    bool StartScroll(int nWidth, QString & strText, const char * pszScrollMenber);
    bool StopScroll(const char * pszScrollMenber);
    void OnScrollInterval();
    void GetShowText(QString & strRealText, QString & strShowText);
    int GetShowTextAlign();

protected:
    bool UpdateScrollInfo(int nWidth, QString & strText);

protected:
    QWidget * m_pWidget;

    // 是否滚动中
    bool m_bScrolling;

    int m_nScrollRange;
    int m_nScrollPos;
    int m_nScrollStopCount;

#if !TEXT_SCROLL_WAY
    bool m_bScrollToLeft;
#endif

    // 滚动间隔时间(ms)
    static int s_nScrollInterval;
    // 滚动计数器
    static int s_nScrollCount;
    // 滚动端点停止的定时器次数
    static int s_nScrollStopCount;
    // 滚动定时器
    static QTimer s_tmScroll;
};

#define SCROLL_TEXT_DECLARATION(Attr) \
public:\
    void SetScroll##Attr(bool bScroll);\
protected:\
    void CheckScroll##Attr();\
protected:\
    bool m_bScroll##Attr;\
    CScrollText* m_pScroll##Attr;\
    static QString s_strReal##Attr;\
    static QString s_strShow##Attr;

#define SCROLL_TEXT_IMPLEMENTATION(Class,Attr,Value,ShowWidth) \
QString Class::s_strReal##Attr;\
QString Class::s_strShow##Attr;\
void Class::SetScroll##Attr(bool bScroll)\
{\
    m_bScroll##Attr = bScroll;\
\
    CheckScroll##Attr();\
}\
\
void Class::CheckScroll##Attr()\
{\
    if (m_bScroll##Attr)\
    {\
        if (IsFocus())\
        {\
            QString strValue = LANG_PARSE(Value);\
\
            RTL_GetRealString(strValue);\
            s_strReal##Attr = strValue;\
        }\
\
        if (IsFocus() && CScrollText::IsNeedScroll(this, ShowWidth, s_strReal##Attr))\
        {\
            if (NULL == m_pScroll##Attr)\
            {\
                m_pScroll##Attr = new CScrollText(this);\
\
                if (NULL == m_pScroll##Attr)\
                {\
                    return;\
                }\
            }\
\
            if (m_pScroll##Attr->StartScroll(ShowWidth, s_strReal##Attr, SLOT(Scroll##Attr())))\
            {\
                m_pScroll##Attr->GetShowText(s_strReal##Attr, s_strShow##Attr);\
            }\
        }\
        else if (NULL != m_pScroll##Attr && m_pScroll##Attr->IsScrolling())\
        {\
            m_pScroll##Attr->StopScroll(SLOT(Scroll##Attr()));\
        }\
    }\
    else\
    {\
        if (NULL != m_pScroll##Attr)\
        {\
            m_pScroll##Attr->StopScroll(SLOT(Scroll##Attr()));\
\
            delete m_pScroll##Attr;\
            m_pScroll##Attr = NULL;\
        }\
    }\
}\

#define SCROLL_TEXT_CHECK(Attr) CheckScroll##Attr();

#define SCROLL_TEXT_INIT(Attr) m_bScroll##Attr = false;\
    m_pScroll##Attr = NULL;

#define SCROLL_TEXT_UNINIT(Attr) m_bScroll##Attr = false;\
    CheckScroll##Attr();

#define SCROLL_TEXT_PAINT(Painter,Rect,Attr,Value) \
    if (NULL != m_pScroll##Attr && m_pScroll##Attr->IsScrolling())\
    {\
        int nHAlign = m_pScroll##Attr->GetShowTextAlign();\
\
        Painter.drawText(Rect, (nHAlign | Qt::AlignVCenter), s_strShow##Attr);\
    }\
    else\
    {\
        Painter.drawItemText(Rect, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(), Value);\
    }

#define SCROLL_TEXT_INTERVAL(Attr) \
    if (NULL == m_pScroll##Attr)\
    {\
        return;\
    }\
\
    m_pScroll##Attr->OnScrollInterval();\
    m_pScroll##Attr->GetShowText(s_strReal##Attr, s_strShow##Attr);

#define SCROLL_TEXT_PAINT_COLOR(Painter,Rect,Attr,Value) \
    if (NULL != m_pScroll##Attr && m_pScroll##Attr->IsScrolling())\
    {\
        m_colorTextDelegate.SetIsNeedCutOff(false); \
        m_colorTextDelegate.PaintHighLightScrollText(Painter, Value, s_strShow##Attr, Rect, HIGH_LIGHT_COLOR); \
    }\
    else\
    {\
        m_colorTextDelegate.SetIsNeedCutOff(true);\
        m_colorTextDelegate.PaintHighLightText(Painter, Value, Rect, HIGH_LIGHT_COLOR); \
    }

#endif
