#include <QtGui>
#include "titlebutton.h"
#include "imagemanager/modimagemanager.h"

namespace
{
#define TITLE_ICON_COMMON_TOP 6
#define TITLE_ICON_COMMON_WIDTH 29
#define TITLE_ICON_COMMON_HEIGHT 26
#define TITLE_TEXT_COMMON_TOP 32
#define TITLE_TEXT_COMMON_HEIGHT 26
}

CTitleButton::CTitleButton(QWidget * pWidget)
    : CButtonWrapper(pWidget)
    , m_rcIcon(0, TITLE_ICON_COMMON_TOP, TITLE_ICON_COMMON_WIDTH, TITLE_ICON_COMMON_HEIGHT)
    , m_rcText(0, TITLE_TEXT_COMMON_TOP, 0, TITLE_TEXT_COMMON_HEIGHT)
{
#if IF_BUG_26252
    m_bTextScroll = false;
    m_pTextLabel = new qScrollTextLabel(pWidget);
    m_pTextLabel->setGeometry(m_rcText);
#endif
}

CTitleButton::~CTitleButton(void)
{
}

#if IF_BUG_26252
void CTitleButton::SetTextScroll(bool bScroll)
{
    if (m_bTextScroll != bScroll)
    {
        m_bTextScroll = bScroll;
        if (NULL != m_pTextLabel)
        {
            m_pTextLabel->setScroll(m_bTextScroll);
            if (!m_bTextScroll)
            {
                m_pTextLabel->setText("");
            }
        }
    }
}
#endif

void CTitleButton::SetDefaultPos()
{
    SetIconTop(TITLE_ICON_COMMON_TOP);
    SetTextTop(TITLE_TEXT_COMMON_TOP);
}

void CTitleButton::SetIconTop(int nTop)
{
    int nLeft = m_rect.left();
    int nWidth = m_rect.width();
    int nIconLeft = nLeft + (nWidth - TITLE_ICON_COMMON_WIDTH) / 2;

    m_rcIcon.setRect(nIconLeft, nTop, TITLE_ICON_COMMON_WIDTH, TITLE_ICON_COMMON_HEIGHT);
}

void CTitleButton::SetTextTop(int nTop)
{
    int nLeft = m_rect.left();
    int nWidth = m_rect.width();

    m_rcText.setRect(nLeft, nTop, nWidth, TITLE_TEXT_COMMON_HEIGHT);
#if IF_BUG_26252
    m_pTextLabel->setGeometry(m_rcText);
#endif
}

void CTitleButton::SetIconRect(const QRect & rcIcon)
{
    m_rcIcon = rcIcon;
}

void CTitleButton::SetIconRect(int nX, int nY, int nWidth, int nHeight)
{
    m_rcIcon.setRect(nX, nY, nWidth, nHeight);
}

void CTitleButton::SetTextRect(const QRect & rcText)
{
    m_rcText = rcText;
#if IF_BUG_26252
    m_pTextLabel->setGeometry(m_rcText);
#endif
}

void CTitleButton::SetTextRect(int nX, int nY, int nWidth, int nHeight)
{
    m_rcText.setRect(nX, nY, nWidth, nHeight);
#if IF_BUG_26252
    m_pTextLabel->setGeometry(m_rcText);
#endif
}

void CTitleButton::PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon,
                             const char * pszIcon)
{
    if (NULL == pszIcon)
    {
        return;
    }

    QPixmap pmIcon = THEME_GET_BMP(pszIcon);

    if (!pmIcon.isNull())
    {
        stylePainter.drawPixmap(rcIcon, pmIcon);
    }
}

void CTitleButton::PaintText(QStylePainter & stylePainter, int nTextAlign, const QRect & rcText,
                             const QString & strText)
{
    if (NULL == m_pWidget)
    {
        return;
    }

#if IF_BUG_26252
    // 如果需要滚动则使用qScrollTextLabel来显示，否则直接画text
    if (m_bTextScroll)
    {
        if (strText != m_pTextLabel->text())
        {
            m_pTextLabel->setText(strText);
        }
    }
    else
#endif
    {
        stylePainter.drawItemText(rcText, nTextAlign, m_pWidget->palette(), m_pWidget->isEnabled(),
                                  strText);
    }
#ifdef IF_ENABLE_TESTABILITY
    m_strText = strText;
#endif
}

void CTitleButton::PaintVButton(QStylePainter & stylePainter, const char * pszSelectBg,
                                const char * pszIcon, const QString & strText)
{
    if (NULL != pszSelectBg)
    {
        CButtonWrapper::PaintButton(stylePainter, NULL, pszSelectBg);
    }

    PaintIcon(stylePainter, m_rcIcon, pszIcon);

    PaintText(stylePainter, (Qt::AlignTop | Qt::AlignHCenter), m_rcText, strText);
}

void CTitleButton::PaintHButton(QStylePainter & stylePainter, const char * pszSelectBg,
                                const char * pszIcon, const QString & strText)
{
    if (NULL != pszSelectBg)
    {
        CButtonWrapper::PaintButton(stylePainter, NULL, pszSelectBg);
    }

    PaintIcon(stylePainter, m_rcIcon, pszIcon);

    PaintText(stylePainter, (Qt::AlignLeft | Qt::AlignVCenter), m_rcText, strText);
}

#ifdef IF_ENABLE_TESTABILITY
const QString & CTitleButton::GetText()
{
    return m_strText;
}
#endif