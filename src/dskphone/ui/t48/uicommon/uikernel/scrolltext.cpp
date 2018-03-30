#include "scrolltext.h"


namespace
{
#define SCROLL_TEXT_SCROLL_INTERVAL_DEFAULT 500
#define SCROLL_TEXT_SCROLL_INTERVAL_MIN 300
#define SCROLL_TEXT_SCROLL_STOP_INTERVAL 2000
}

QTimer CScrollText::s_tmScroll;
int CScrollText::s_nScrollCount = 0;
int CScrollText::s_nScrollInterval = SCROLL_TEXT_SCROLL_INTERVAL_DEFAULT;
int CScrollText::s_nScrollStopCount = 1;


CScrollText::CScrollText(QWidget * pWidget)
    : m_pWidget(pWidget)
    , m_bScrolling(false)
#if !TEXT_SCROLL_WAY
    , m_bScrollToLeft(false)
#endif
    , m_nScrollRange(0)
    , m_nScrollPos(0)
    , m_nScrollStopCount(0)
{
}

CScrollText::~CScrollText()
{
}

bool CScrollText::IsNeedScroll(QWidget * pWidget, int nShowWidth, QString & strText)
{
    if (NULL == pWidget)
    {
        return false;
    }

    QFontMetrics fm(pWidget->font());

    if (fm.width(strText) > nShowWidth)
    {
        return true;
    }

    return false;
}

void CScrollText::ResetInterval()
{
    s_nScrollInterval = SCROLL_TEXT_SCROLL_INTERVAL_DEFAULT;
    UpdateScrollStopCount();
}

void CScrollText::SetInterval(int nMilliSecond)
{
    if (nMilliSecond < SCROLL_TEXT_SCROLL_INTERVAL_MIN)
    {
        s_nScrollInterval = SCROLL_TEXT_SCROLL_INTERVAL_MIN;
        UpdateScrollStopCount();
        return;
    }

    s_nScrollInterval = nMilliSecond;
    UpdateScrollStopCount();
}

void CScrollText::UpdateScrollStopCount()
{
    if (s_nScrollInterval > SCROLL_TEXT_SCROLL_STOP_INTERVAL || s_nScrollInterval <= 0)
    {
        s_nScrollStopCount = 1;
        return;
    }

    s_nScrollStopCount = (SCROLL_TEXT_SCROLL_STOP_INTERVAL + s_nScrollInterval - 1) / s_nScrollInterval;
}

bool CScrollText::IsScrolling()
{
    return m_bScrolling;
}

bool CScrollText::StartScroll(int nWidth, QString & strText, const char * pszScrollMenber)
{
    if (NULL == pszScrollMenber || NULL == m_pWidget || s_nScrollInterval <= 0)
    {
        return false;
    }

    //http://10.2.1.199/Bug.php?BugID=99911
    if (m_bScrolling)
    {
        StopScroll(pszScrollMenber);
    }

    if (!UpdateScrollInfo(nWidth, strText))
    {
        return false;
    }

    QObject::connect(&s_tmScroll, SIGNAL(timeout()), m_pWidget, pszScrollMenber);

    m_bScrolling = true;
    ++s_nScrollCount;

    if (!s_tmScroll.isActive())
    {
        s_tmScroll.start(s_nScrollInterval);
    }

    return true;
}

bool CScrollText::UpdateScrollInfo(int nWidth, QString & strText)
{
    if (NULL == m_pWidget || strText.isEmpty() || nWidth <= 0)
    {
        return false;
    }

    QFontMetrics fm(m_pWidget->font());
    int nOverflowWidth = fm.width(strText) - nWidth;

    if (nOverflowWidth <= 0)
    {
        return false;
    }

    int nTextWidth = 0;

    m_nScrollRange = 0;
    m_nScrollStopCount = 0;

    do
    {
        ++m_nScrollRange;

        nTextWidth = fm.width(strText.left(m_nScrollRange));
    }
    while (nTextWidth < nOverflowWidth);

    m_nScrollPos = 0;

#if !TEXT_SCROLL_WAY
    m_bScrollToLeft = false;
#endif

    return true;
}

bool CScrollText::StopScroll(const char * pszScrollMenber)
{
    if (NULL == pszScrollMenber || NULL == m_pWidget)
    {
        return false;
    }

    if (!m_bScrolling)
    {
        return true;
    }

    QObject::disconnect(&s_tmScroll, SIGNAL(timeout()), m_pWidget, pszScrollMenber);

    m_bScrolling = false;
    --s_nScrollCount;

    if (s_nScrollCount <= 0)
    {
        s_nScrollCount = 0;
        s_tmScroll.stop();
    }

    return true;
}

void CScrollText::OnScrollInterval()
{
    if (m_nScrollPos != m_nScrollRange)
    {
#if TEXT_SCROLL_WAY
        if (0 == m_nScrollPos)
        {
            if (m_nScrollStopCount < s_nScrollStopCount)
            {
                ++m_nScrollStopCount;
                return;
            }

            m_nScrollStopCount = 0;
        }
        ++m_nScrollPos;
        return;
#else
        if (!m_bScrollToLeft)
        {
            ++m_nScrollPos;
            return;
        }
#endif
    }

#if TEXT_SCROLL_WAY
    if (m_nScrollStopCount < s_nScrollStopCount)
    {
        ++m_nScrollStopCount;
    }
    else
    {
        m_nScrollStopCount = 0;
        m_nScrollPos = 0;
    }
#else
    if (m_bScrollToLeft)
    {
        if (m_nScrollPos >= 0)
        {
            --m_nScrollPos;
            return;
        }

        if (m_nScrollStopCount < s_nScrollStopCount)
        {
            ++m_nScrollStopCount;
        }
        else
        {
            m_nScrollStopCount = 0;
            m_nScrollPos = 0;
            m_bScrollToLeft = false;
        }
    }
    else
    {
        if (m_nScrollStopCount < s_nScrollStopCount)
        {
            ++m_nScrollStopCount;
        }
        else
        {
            m_nScrollStopCount = 0;
            m_nScrollPos = m_nScrollRange;
            m_bScrollToLeft = true;
        }
    }
#endif
}

void CScrollText::GetShowText(QString & strRealText, QString & strShowText)
{
    int nLen = strRealText.length();

    if (0 >= m_nScrollPos || m_nScrollPos >= nLen)
    {
        strShowText = strRealText;
        return;
    }

#if TEXT_SCROLL_WAY
    strShowText = strRealText.right(nLen - m_nScrollPos);
#else
    if (m_bScrollToLeft)
    {
        strShowText = strRealText.left(nLen - (m_nScrollRange - m_nScrollPos));
    }
    else
    {
        strShowText = strRealText.right(nLen - m_nScrollPos);
    }
#endif
}

int CScrollText::GetShowTextAlign()
{
    if (!m_bScrolling)
    {
        return Qt::AlignLeft;
    }

#if TEXT_SCROLL_WAY
    if (0 != m_nScrollRange && m_nScrollPos == m_nScrollRange)
    {
        return Qt::AlignRight;
    }

    return Qt::AlignLeft;
#else
    if (0 != m_nScrollRange)
    {
        if (m_nScrollPos <= 0)
        {
            return Qt::AlignLeft;
        }
        else if (m_nScrollPos >= m_nScrollRange)
        {
            return Qt::AlignRight;
        }

        if (m_bScrollToLeft)
        {
            return Qt::AlignRight;
        }
        else
        {
            return Qt::AlignLeft;
        }
    }

    return Qt::AlignLeft;
#endif
}
