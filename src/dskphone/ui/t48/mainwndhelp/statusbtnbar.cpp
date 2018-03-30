#include <QtGui>
#include "statusbtnbar.h"
#include "statusbutton.h"

namespace
{
#define STATUS_BTN_X_SPACE 0
}

CStatusBtnBar::CStatusBtnBar(QWidget * pWidget, bool bAutoDelBtn)
    : CBtnBarWrapper(pWidget, bAutoDelBtn)
    , m_nAlgin(STATUS_BTN_ALIGN_CENTER)
{
}

CStatusBtnBar::~CStatusBtnBar(void)
{
}

void CStatusBtnBar::SetAlign(int nAlign)
{
    m_nAlgin = nAlign;
}

void CStatusBtnBar::SetRect(const QRect & rect)
{
    m_rcBar = rect;
}

void CStatusBtnBar::SetRect(int nX, int nY, int nWidth, int nHeight)
{
    m_rcBar.setRect(nX, nY, nWidth, nHeight);
}

const QRect & CStatusBtnBar::GetRect() const
{
    return m_rcBar;
}

void CStatusBtnBar::SetBtnShowCount(int nCount)
{
    m_iBtnShowCount = nCount;
}

void CStatusBtnBar::ReLayoutButton()
{
    int nBtnCount = m_iBtnShowCount;
    int nTotalWidth = nBtnCount * CStatusButton::GetStatusBtnWidth();

    if (nBtnCount > 1)
    {
        nTotalWidth = nTotalWidth + (nBtnCount - 1) * STATUS_BTN_X_SPACE;
    }

    int nBtnLeft = 0;
    switch (m_nAlgin)
    {
    case STATUS_BTN_ALIGN_LEFT:
        {
            nBtnLeft = m_rcBar.left();
        }
        break;
    case STATUS_BTN_ALIGN_CENTER:
        {
            nBtnLeft = m_rcBar.left() + (m_rcBar.width() - nTotalWidth) / 2;
        }
        break;
    case STATUS_BTN_ALIGN_RIGHT:
        {
            nBtnLeft = m_rcBar.left() + m_rcBar.width() - nTotalWidth;
        }
        break;
    default:
        break;
    }

    for (int i = 0; i < nBtnCount; ++i)
    {
        CStatusButton * pStatusBtn = (CStatusButton *)GetBtn(i);

        if (NULL == pStatusBtn)
        {
            continue;
        }

        const QRect & rcBtn = pStatusBtn->GetRect();
        QRect rcNewBtn(nBtnLeft, rcBtn.top(), rcBtn.width(), rcBtn.height());

        pStatusBtn->SetRect(rcNewBtn);
        nBtnLeft += (rcBtn.width() + STATUS_BTN_X_SPACE);
    }
}

void CStatusBtnBar::PaintStatusBtnBar(QStylePainter & stylePainter, const char * pszBtnClick)
{
    int i = 0;
    int nBtnCount = m_iBtnShowCount;

    for (; i < nBtnCount; ++i)
    {
        CStatusButton * pBtnBar = (CStatusButton *)GetBtn(i);

        if (NULL == pBtnBar)
        {
            continue;
        }

        pBtnBar->PaintStatusButton(stylePainter, pszBtnClick);
    }
}
