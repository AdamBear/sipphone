#include <QtGui>
#include <ETLLib/ETLLib.hpp>
#include "maskframe.h"

namespace
{
#define MASK_FRAME_BG_COLOR QColor(0, 0, 0, 122)
}

CMaskFrame::CMaskFrame(QWidget * parent)
    : QWidget(parent)
    , m_bDown(false)
{
#if WINDOWS_SYSTEM
    setWindowFlags(Qt::FramelessWindowHint);
#endif

    //setAttribute(Qt::WA_TranslucentBackground, true);
    QPalette pal = palette();

    pal.setColor(QPalette::Background, QColor(0, 0, 0, 0));
    setPalette(pal);
}

CMaskFrame::~CMaskFrame()
{

}

void CMaskFrame::ResetDown()
{
    m_bDown = false;
}

void CMaskFrame::SetMaskRect(const QRect & rcMask)
{
    m_rcMask = rcMask;
}

bool CMaskFrame::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject != this)
    {
        return false;
    }
    if (NULL == pEvent)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::MouseButtonPress:
        {
            m_bDown = true;
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        {
            bool bDown = m_bDown;

            m_bDown = false;

            if (bDown)
            {
                emit click();
            }

            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

void CMaskFrame::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    QColor colorBG = MASK_FRAME_BG_COLOR;

    if (m_rcMask.isEmpty())
    {
        QRect rcDlg = rect();

        stylePainter.fillRect(rcDlg, colorBG);
    }
    else
    {
        stylePainter.fillRect(m_rcMask, colorBG);
    }
}
