#include <QtGui>
#include "pagectrl.h"
#include "keymap.h"
#include "imagemanager/modimagemanager.h"
#include "t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

#define PAGE_NO_DEFAULT 0
#define PAGE_COUNT_DEFAULT 1

#define PAGE_CTRL_DEFAULT_WIDTH 48
#define PAGE_CTRL_BG_TOP_HEIGHT 28
#define PAGE_CTRL_BG_MID_HEIGHT 26
#define PAGE_CTRL_BG_BOTTOM_HEIGHT 28
#define PAGE_CTRL_ARROW_WIDTH 16
#define PAGE_CTRL_ARROW_HEIGHT 16
#define PAGE_CTRL_ARROW_LEFT 16
#define PAGE_CTRL_ARROW_CLICK_LEFT 2
#define PAGE_CTRL_ARROW_CLICK_WIDTH 44
#define PAGE_CTRL_UP_CLICK_TOP 2
#define PAGE_CTRL_DOWN_CLICK_BOTTOM 2
#define PAGE_CTRL_TEXT_LEFT 0
#define PAGE_CTRL_TEXT_WIDTH 48
#define PAGE_CTRL_TEXT_HEIGHT 100
#define PAGE_CTRL_RIGHT_MARGIN 2
#define PAGE_CTRL_TOP_MARGIN 2
#define PAGE_CTRL_BOTTOM_MARGIN 2

CPageCtrl::CPageCtrl(QWidget * parent, IScrollNotify * pScrollNotify)
    : QWidget(parent)
    , m_nPageNo(PAGE_NO_DEFAULT)
    , m_nPageCount(PAGE_COUNT_DEFAULT)
    , m_pScrollNotify(pScrollNotify)
    , m_btnUpArrow(this)
    , m_btnDownArrow(this)
    , m_btnBar(this, false)
{
    setGeometry(0, 0, PAGE_CTRL_DEFAULT_WIDTH, 0);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setObjectName("CPageCtrl");

    m_btnUpArrow.SetDownKey(PHONE_KEY_UP);
    m_btnDownArrow.SetDownKey(PHONE_KEY_DOWN);

    m_btnBar.SetBtnCount(2);
    m_btnBar.SetBtn(0, &m_btnUpArrow);
    m_btnBar.SetBtn(1, &m_btnDownArrow);

    ReLayout();
}

CPageCtrl::~CPageCtrl()
{

}

void CPageCtrl::SetScrollCtrlNotify(IScrollNotify * pScrollNotify)
{
    m_pScrollNotify = pScrollNotify;
}

void CPageCtrl::SetRect(int nX, int nY, int nWidth, int nHeight)
{
    const QRect rect(nX, nY, nWidth, nHeight);

    SetRect(rect);
}

void CPageCtrl::SetRect(const QRect & rect)
{
    if (rect.isValid())
    {
        setGeometry(rect);

        ReLayout();

        update();
    }
}

void CPageCtrl::SetScrollInfo(int nRange, int nPos)
{
    if (nRange < PAGE_COUNT_DEFAULT)
    {
        nRange = PAGE_COUNT_DEFAULT;
    }

    if (nPos < PAGE_NO_DEFAULT)
    {
        nPos = PAGE_NO_DEFAULT;
    }

    m_nPageCount = nRange;
    m_nPageNo = nPos;

    char szBuffer[32];

    snprintf(szBuffer, sizeof(szBuffer), "%d/%d", (m_nPageNo + 1), m_nPageCount);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    m_strPageInfo = QLatin1String(szBuffer);
}

void CPageCtrl::SetScrollPos(int nPos)
{
    if (nPos < PAGE_NO_DEFAULT)
    {
        nPos = PAGE_NO_DEFAULT;
    }

    m_nPageNo = nPos;

    char szBuffer[32];

    snprintf(szBuffer, sizeof(szBuffer), "%d/%d", (m_nPageNo + 1), m_nPageCount);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    m_strPageInfo = QLatin1String(szBuffer);

    update();
}

int CPageCtrl::GetPageNo()
{
    return m_nPageNo;
}

int CPageCtrl::GetPageCount()
{
    return m_nPageCount;
}

int CPageCtrl::GetDefaultWidth()
{
    return PAGE_CTRL_DEFAULT_WIDTH;
}

int CPageCtrl::GetRightMargin()
{
    return PAGE_CTRL_RIGHT_MARGIN;
}

int CPageCtrl::GetTopMargin() const
{
    return PAGE_CTRL_TOP_MARGIN;
}

int CPageCtrl::GetBottomMargin() const
{
    return PAGE_CTRL_BOTTOM_MARGIN;
}

bool CPageCtrl::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }

    if (QEvent::KeyPress == pEvent->type())
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;

        switch (pKeyEvent->key())
        {
        case PHONE_KEY_UP:
            {
                if (NULL != m_pScrollNotify)
                {
                    m_pScrollNotify->OnNotifyScrollPrevious();
                }
                return true;
            }
            break;
        case PHONE_KEY_DOWN:
            {
                if (NULL != m_pScrollNotify)
                {
                    m_pScrollNotify->OnNotifyScrollNext();
                }
                return true;
            }
            break;
        default:
            break;
        }
    }

    return false;
}

void CPageCtrl::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnBar.ProcessMousePress(pEvent);
}

void CPageCtrl::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (NULL == pBtn || NULL == m_pScrollNotify)
    {
        return;
    }

    if (pBtn == &m_btnUpArrow)
    {
        m_pScrollNotify->OnNotifyScrollPrevious();
    }
    else if (pBtn == &m_btnDownArrow)
    {
        m_pScrollNotify->OnNotifyScrollNext();
    }
}

void CPageCtrl::keyPressEvent(QKeyEvent * pEvent)
{
    m_btnBar.ProcessKeyPress(pEvent);
}

void CPageCtrl::keyReleaseEvent(QKeyEvent * pEvent)
{
    CButtonWrapper * pBtn = m_btnBar.ProcessKeyRelease(pEvent);

    if (NULL == pBtn || NULL == m_pScrollNotify)
    {
        return;
    }

    if (pBtn == &m_btnUpArrow)
    {
        m_pScrollNotify->OnNotifyScrollPrevious();
    }
    else if (pBtn == &m_btnDownArrow)
    {
        m_pScrollNotify->OnNotifyScrollNext();
    }
}

void CPageCtrl::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    // 画背景
    PaintBG(stylePainter);

    QRect rcCtrl = rect();
    int nCtrlCenterY = rcCtrl.height() / 2;
    int nArrowLeft = rcCtrl.left() + PAGE_CTRL_ARROW_LEFT;
    int nArrowCenter = (nCtrlCenterY - PAGE_CTRL_ARROW_HEIGHT) / 2;

    // 画前一页选中效果
    m_btnUpArrow.PaintButton(stylePainter, NULL, PIC_GENERAL_PAGE_CTRL_ARROW_UP_CLICK);

    // 画前一页箭头
    QRect rcUpArrow(nArrowLeft, rcCtrl.top() + nArrowCenter, PAGE_CTRL_ARROW_WIDTH,
                    PAGE_CTRL_ARROW_HEIGHT);
    QPixmap pmUpArrow = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_UP_ARROW);

    if (!pmUpArrow.isNull())
    {
        stylePainter.drawPixmap(rcUpArrow, pmUpArrow);
    }

    // 画后一页选中效果
    m_btnDownArrow.PaintButton(stylePainter, NULL, PIC_GENERAL_PAGE_CTRL_ARROW_DOWN_CLICK);

    // 画后一页箭头
    QRect rcDownArrow(nArrowLeft, rcCtrl.bottom() - nArrowCenter, PAGE_CTRL_ARROW_WIDTH,
                      PAGE_CTRL_ARROW_HEIGHT);
    QPixmap pmDownArrow = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_DOWN_ARROW);

    if (!pmDownArrow.isNull())
    {
        stylePainter.drawPixmap(rcDownArrow, pmDownArrow);
    }

    // 画页码
    stylePainter.drawItemText(m_rcText, Qt::AlignCenter, palette(), isEnabled(), m_strPageInfo);
}

void CPageCtrl::PaintBG(QStylePainter & stylePainter)
{
    QRect rcCtrl = rect();
    QRect rcTop(rcCtrl.left(), rcCtrl.top(), PAGE_CTRL_DEFAULT_WIDTH, PAGE_CTRL_BG_TOP_HEIGHT);
    QRect rcBottom(rcCtrl.left(), rcCtrl.bottom() - PAGE_CTRL_BG_BOTTOM_HEIGHT, PAGE_CTRL_DEFAULT_WIDTH,
                   PAGE_CTRL_BG_TOP_HEIGHT);
    QRect rcMid(rcCtrl.left(), rcTop.bottom(), PAGE_CTRL_DEFAULT_WIDTH,
                rcBottom.top() - rcTop.bottom());

    // 顶部
    QPixmap pmTop = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_BG_TOP);

    if (!pmTop.isNull())
    {
        stylePainter.drawPixmap(rcTop, pmTop);
    }

    // 中间
    QPixmap pmMid = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_BG_MID);

    if (!pmMid.isNull())
    {
        QRect rcSource(0, 0, PAGE_CTRL_DEFAULT_WIDTH, PAGE_CTRL_BG_MID_HEIGHT);

        stylePainter.drawPixmap(rcMid, pmMid, rcSource);
    }

    // 底部
    QPixmap pmBottom = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_BG_BOTTOM);

    if (!pmBottom.isNull())
    {
        stylePainter.drawPixmap(rcBottom, pmBottom);
    }
}

void CPageCtrl::ReLayout()
{
    QRect rc = rect();

    int nArrowLeft = rc.left() + PAGE_CTRL_ARROW_CLICK_LEFT;

    int nHalfHeight = rc.height() / 2;
    int nUpArrowTop = rc.top() + PAGE_CTRL_UP_CLICK_TOP;
    int nArrowHeight = nHalfHeight - PAGE_CTRL_UP_CLICK_TOP - PAGE_CTRL_DOWN_CLICK_BOTTOM;

    m_btnUpArrow.SetRect(nArrowLeft, nUpArrowTop, PAGE_CTRL_ARROW_CLICK_WIDTH, nArrowHeight);

    int nDownArrowTop = rc.bottom() - PAGE_CTRL_DOWN_CLICK_BOTTOM - nArrowHeight;

    m_btnDownArrow.SetRect(nArrowLeft, nDownArrowTop, PAGE_CTRL_ARROW_CLICK_WIDTH, nArrowHeight);

    int nTextLeft = rc.left() + PAGE_CTRL_TEXT_LEFT;
    int nTextTop = nHalfHeight - PAGE_CTRL_TEXT_HEIGHT / 2;

    m_rcText.setRect(nTextLeft, nTextTop, PAGE_CTRL_TEXT_WIDTH, PAGE_CTRL_TEXT_HEIGHT);
}

#ifdef IF_ENABLE_TESTABILITY
QString CPageCtrl::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    ptAbsolute = mapToGlobal(m_rcText.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcText.width();
    testInfo.height = m_rcText.height();
    testInfo.id = LABEL_INFO;
    testInfo.text = fromQString(m_strPageInfo);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    AddBtnNode(nodeRoot, m_btnUpArrow, BTN_UP);
    AddBtnNode(nodeRoot, m_btnDownArrow, BTN_DOWN);

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
