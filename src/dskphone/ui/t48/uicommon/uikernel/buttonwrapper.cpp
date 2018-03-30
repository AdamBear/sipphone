#include <QtGui>
#include "buttonwrapper.h"
#include "keymap.h"
#include "imagemanager/modimagemanager.h"

CButtonWrapper::CButtonWrapper(QWidget * pWidget)
    : m_pWidget(pWidget)
    , m_bDown(false)
    , m_nDownKey(PHONE_KEY_NONE)
    , m_rect(0, 0, 0, 0)
{
}

CButtonWrapper::~CButtonWrapper(void)
{
}

void CButtonWrapper::SetRect(const QRect & rect)
{
    m_rect = rect;
}

void CButtonWrapper::SetRect(int nX, int nY, int nWidth, int nHeight)
{
    m_rect.setRect(nX, nY, nWidth, nHeight);
}

void CButtonWrapper::ResetDown()
{
    bool bDown = m_bDown;

    m_bDown = false;

    if (bDown)
    {
        RepaintButton();
    }
}

void CButtonWrapper::SetDownKey(int nDownKey)
{
    m_nDownKey = nDownKey;
}

const QRect & CButtonWrapper::GetRect() const
{
    return m_rect;
}

bool CButtonWrapper::IsDown() const
{
    return m_bDown;
}

bool CButtonWrapper::ProcessMousePress(QMouseEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    const QPoint & pt = pEvent->pos();

    if (m_rect.contains(pt))
    {
        if (!m_bDown)
        {
            m_bDown = true;

            RepaintButton();
        }

        pEvent->accept();
    }
    else if (m_bDown)
    {
        m_bDown = false;

        RepaintButton();
    }

    return m_bDown;
}

bool CButtonWrapper::ProcessMouseRelease(QMouseEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    bool bDown = m_bDown;

    m_bDown = false;

    const QPoint & pt = pEvent->pos();

    if (m_rect.contains(pt))
    {
        pEvent->accept();

        if (bDown)
        {
            RepaintButton();
        }

        return bDown;
    }

    if (bDown)
    {
        RepaintButton();
    }

    return false;
}

bool CButtonWrapper::ProcessKeyPress(QKeyEvent * pEvent)
{
    if (PHONE_KEY_NONE == m_nDownKey || NULL == pEvent)
    {
        return false;
    }

    int nKey = pEvent->key();

    if (nKey == m_nDownKey)
    {
        if (!m_bDown)
        {
            RepaintButton();
        }

        m_bDown = true;
    }
    else if (m_bDown)
    {
        RepaintButton();
        m_bDown = false;
    }

    return m_bDown;
}

bool CButtonWrapper::ProcessKeyRelease(QKeyEvent * pEvent)
{
    if (PHONE_KEY_NONE == m_nDownKey || NULL == pEvent)
    {
        return false;
    }

    bool bDown = false;
    int nKey = pEvent->key();

    if (nKey == m_nDownKey)
    {
        bDown = m_bDown;
    }

    m_bDown = false;

    if (bDown)
    {
        RepaintButton();
    }

    return bDown;
}

void CButtonWrapper::RepaintButton()
{
    if (NULL != m_pWidget)
    {
        m_pWidget->update(m_rect);
    }
}

void CButtonWrapper::PaintButton(QStylePainter & stylePainter, const char * pszNoraml,
                                 const char * pszClick/* = NULL*/,
                                 const char * pszDisable/* = NULL*/, QRect rcBtn/* = QRect()*/)
{
    if (NULL == m_pWidget)
    {
        return;
    }

    if (!rcBtn.isValid())
    {
        rcBtn = m_rect;
    }

    bool bPaint = false;

    if (m_pWidget->isEnabled())
    {
        if (m_bDown && NULL != pszClick)
        {
            QPixmap pm = THEME_GET_BMP(pszClick);

            if (!pm.isNull())
            {
                bPaint = true;
                stylePainter.drawPixmap(rcBtn, pm);
            }
        }
    }
    else if (NULL == pszDisable)
    {
        QPixmap pm = THEME_GET_BMP(pszDisable);

        if (!pm.isNull())
        {
            bPaint = true;
            stylePainter.drawPixmap(rcBtn, pm);
        }
    }

    if (!bPaint && NULL != pszNoraml)
    {
        QPixmap pm = THEME_GET_BMP(pszNoraml);

        if (!pm.isNull())
        {
            stylePainter.drawPixmap(rcBtn, pm);
        }
    }
}

void CButtonWrapper::PaintButton(QStylePainter & stylePainter, QPixmap & pm,
                                 QRect rcBtn/* = QRect()*/)
{
    if (NULL == m_pWidget)
    {
        return;
    }

    if (!rcBtn.isValid())
    {
        rcBtn = m_rect;
    }

    if (!pm.isNull())
    {
        stylePainter.drawPixmap(rcBtn, pm);
    }
}

const QWidget * CButtonWrapper::GetParent() const
{
    return m_pWidget;
}