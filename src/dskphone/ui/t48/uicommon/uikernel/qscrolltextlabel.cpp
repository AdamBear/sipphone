#include "config.h"
#include "qscrolltextlabel.h"
#include <qcommonstyle.h>
#include "languagehelper.h"
#include "qtcommon/qmisc.h"

//滚动间隔时间
#define TIME_INTERVAL_SCROLL    300
//移动到端点停留次数
#define COUNT_STOP              4
//选择文字内容滚动方式
//0方式：从起点左滚到右端点，再从右端点滚到起点左，如此循环
//1方式：从起点左滚到右端点，再从起点左滚到右端点，如此循环
#define TEXT_LABEL_SCROLL_WAY   1

// 用于设置Label Style 类型
static QCommonStyle s_CommonStyle;

//Label滚动文字间隔时间
static int GetTimeIntervalScroll()
{
    int nValue = 500;
    return nValue < TIME_INTERVAL_SCROLL ? TIME_INTERVAL_SCROLL : nValue;
}
//Label滚动文字内容到端点，停歇次数。
static int GetCountStop()
{
    int nValue = GetTimeIntervalScroll();
    //Label滚动文字内容到端点时停留时间
    int nSum = 500;
    if (0 != nValue)
    {
        return nSum / nValue + 1;
    }
    return COUNT_STOP;
}
////////////////////////////////////////////////////////////////////////////
qScrollTextLabel::qScrollTextLabel(QWidget * parent /* = 0 */, Qt::WindowFlags flag /* = 0 */)
    : QLabel(parent, flag)
    , m_alignLast(0)
    , m_bScroll(TRUE)
{
    this->installEventFilter(this);
    connect(&m_timerScrollText, SIGNAL(timeout()), this, SLOT(UpdateText()));
    m_nCount = 0;
    m_bGoAhead = TRUE;
    m_strShowText = "";
    resetScroolTimer();

    setStyle(&s_CommonStyle);
}

qScrollTextLabel::~qScrollTextLabel()
{
    this->removeEventFilter(this);
    StopScrollTimer();
}

bool qScrollTextLabel::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pObj == this
            && NULL != pEvent)
    {
        switch (pEvent->type())
        {
        case QEvent::FocusIn:
            {
                StartScrollTimer();
            }
            break;
        case QEvent::Resize:
        case QEvent::FontChange:
            {
                StartScrollTimer();
//              if (this->hasFocus())
//              {
//                  StartScrollTimer();
//              }
//              else
//              {
//                  StopScrollTimer();
//              }
            }
            break;
        case QEvent::FocusOut:
            {
                StopScrollTimer(true);
            }
            break;
        case QEvent::Show:
            {
                OnReshowEvent();
            }
            break;
        default:
            break;
        }
    }
    return  QLabel::eventFilter(pObj, pEvent);
}

void qScrollTextLabel::setFocus(Qt::FocusReason reason)
{
    QLabel::setFocus(reason);
}

void qScrollTextLabel::setText(const QString & text)
{
    m_strRealText = text;
    m_strShowText = LANG_PARSE(
                        text); // 显示的文字需要先解析,截断的方式会导致现在的翻译方式出问题
    ResetScrollStatus();
    /*  QString strDisplayText = text;
        yl::wstring strTextW = yl::to_utf16(fromQString(strDisplayText));
        if (IsStringContainHebrew(strTextW))
        {
            setLayoutDirection(Qt::RightToLeft);
            setAlignment(alignment() | Qt::AlignRight);

            //若为(Empty)则改为)Empty(
            if (strDisplayText.at(4) == QChar('(') && strDisplayText.right(1) == QChar(')'))
            {
                strDisplayText.replace(4, 1, toQString(")"));
                strDisplayText.replace(strDisplayText.length()-1, 1, toQString("("));
            }

            //如果字符串的前几位是表示序号，如 1. 2. ，则序号不翻转
            int i = 0;
            BOOL bHeadDigitChar = FALSE;
            BOOL bFindIt = FALSE;
            while (strTextW[i] != L'\0')
            {
                if (!bHeadDigitChar)
                {
                    if (IS_DIGIT_WCHAR(strTextW[i]))
                    {
                        bHeadDigitChar = TRUE;
                    }
                    else if (strTextW[i] != L' ')
                    {
                        // 如果起始字符不是数字也不是空格，则退出
                        break;
                    }
                }
                else //如果已经有数字开头
                {
                    if (!IS_DIGIT_WCHAR(strTextW[i]) && strTextW[i] != L'.')
                    {
                        break;
                    }
                    else if (strTextW[i] == L'.')
                    {
                        bFindIt = TRUE;
                        break;
                    }
                }

                i++;
            }

            if (bFindIt)
            {
                //为正确显示含有希伯来语的字符串，做以下操作：
                //如 " 12. XXXXXXXX:" 改为 ":XXXXXXXX .12 "  注意其中的空格
                QString strTemp = strDisplayText.left(i);
                strTemp = toQString(" .") + strTemp.right(i-1) + toQString(" ");
                int nLength = strTextW.length();
                if (strTextW[nLength-1] == L':')
                {
                    strDisplayText.replace(nLength-1, 1, strTemp);
                    strDisplayText.replace(0, i+2, toQString(":"));
                }
                else
                {
                    strDisplayText = strDisplayText + strTemp;
                    strDisplayText.replace(0, i+2, toQString(""));
                }
            }
        }
        else
        {
            setLayoutDirection(Qt::LeftToRight);
            setAlignment(alignment() | Qt::AlignLeft);
        }

        QLabel::setText(strDisplayText);
    */
}

void qScrollTextLabel::UpdateText()
{
//TEXT_LABEL_SCROLL_WAY [1]方式：从起点左滚到右端点，再从右端点滚到起点左，如此循环
#if TEXT_LABEL_SCROLL_WAY
    if (m_nCount > m_nDiffLength + getCountStop())
    {
        m_nCount = 0 - getCountStop();//到端点，应该停留片刻
    }
    int nCount = m_nCount < 0 ? 0 : m_nCount > m_nDiffLength ? m_nDiffLength : m_nCount;
    QLabel::setText(m_strShowText.right(m_strShowText.length() - nCount));
    ++m_nCount;
#else
//TEXT_LABEL_SCROLL_WAY [0]方式：从起点左滚到右端点，再从起点左滚到右端点，如此循环
    if (m_nCount > m_nDiffLength + getCountStop())
    {
        m_bGoAhead = FALSE;
        m_nCount = m_nDiffLength;//到端点，应该停留片刻
    }
    else if (m_nCount < 0 - getCountStop())
    {
        m_bGoAhead = TRUE;
        m_nCount = 0;//到端点，应该停留片刻
    }
    int nCount = m_nCount < 0 ? 0 : m_nCount > m_nDiffLength ? m_nDiffLength : m_nCount;
    QLabel::setText(m_strShowText.right(m_strShowText.length() - nCount));
    m_bGoAhead ? ++m_nCount : --m_nCount;
#endif
}

void qScrollTextLabel::StartScrollTimer()
{
    StopScrollTimer();
    QFontMetrics fm(this->font());
    int nDiff = fm.width(m_strShowText) - this->rect().width();
    if (nDiff > 0)
    {
        // 文字宽度大于控件宽度时居中会造成截断
        if (this->alignment() & Qt::AlignHCenter)
        {
            m_alignLast = this->alignment();
            Qt::Alignment alignNew = this->alignment() & ~Qt::AlignHCenter;
            this->setAlignment(alignNew | Qt::AlignLeft);
        }

        m_nDiffLength = 0;
        while (nDiff > fm.width(m_strShowText.left(++m_nDiffLength)));

        m_timerScrollText.start(m_iScroolTimer);
    }
    else if (m_alignLast != this->alignment()
             && m_alignLast != 0)
    {
        this->setAlignment(m_alignLast);
        QLabel::setText(m_strShowText);
    }
}

void qScrollTextLabel::StopScrollTimer(bool bEllipsis/* = false*/)
{
    m_nCount = 0;
    m_bGoAhead = TRUE;

    if (m_timerScrollText.isActive())
    {
        m_timerScrollText.stop();
    }

    QFontMetrics fm(this->font());
    if (bEllipsis && fm.width(m_strShowText) > this->rect().width()
            && !this->hasFocus())
    {
#ifdef IF_FEATURE_ARABIC
        QString strShowText = CutOffText(m_strShowText, fm, this->width() - 3);
#else
        int nShowWidth = this->rect().width() - fm.width("...");
        int nShowLenght = 0;
        while (nShowWidth >= fm.width(m_strShowText.left(++nShowLenght)));
        QString strShowText = m_strShowText.left(--nShowLenght) + "...";
#endif
        QLabel::setText(strShowText);
    }
    else
    {
        QLabel::setText(m_strShowText);
    }
}

int qScrollTextLabel::getCountStop()
{
    return 500 / m_iScroolTimer + 1;
}

void qScrollTextLabel::setScroll(BOOL bScroll)
{
    if (m_bScroll != bScroll)
    {
        m_bScroll = bScroll;
        ResetScrollStatus();
    }
}

void qScrollTextLabel::setScroolTimer(int iTimer)
{
    m_iScroolTimer = (iTimer <= 0) ? GetTimeIntervalScroll() : chMID(100, iTimer, 1000 * 10);
}

void qScrollTextLabel::resetScroolTimer()
{
    m_iScroolTimer = GetTimeIntervalScroll();
}

void qScrollTextLabel::OnReshowEvent()
{
    setText(m_strRealText);
}

void qScrollTextLabel::ResetScrollStatus()
{
    if (m_bScroll && isVisible())
    {
        StartScrollTimer();
    }
    else
    {
        StopScrollTimer(true);
    }
}
