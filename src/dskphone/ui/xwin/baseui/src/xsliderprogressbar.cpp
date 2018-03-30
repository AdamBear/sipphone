#include "xsliderprogressbar.h"

namespace
{
const char* XWINDOW_NAME_PROGRESSBAR_PADDINGLEFT = "android:progressbarpaddingleft";
const char* XWINDOW_NAME_PROGRESSBAR_PADDINGTOP = "android:progressbarpaddingtop";
const char* XWINDOW_NAME_PROGRESSBAR_WIDTH = "android:progressbarwidth";

const char* XWINDOW_NAME_PROGRESSBAR_SLIDER = "android:progressbarslider";
}

EXPORT_VIEW_CLASS(xSliderProgressBar);

xSliderProgressBar::xSliderProgressBar()
{
    REGISTER_VIEW_DUMP_CLASS(xSliderProgressBar)
}

xSliderProgressBar::~xSliderProgressBar()
{
}

void xSliderProgressBar::loadAttributes(xml_node& node)
{
    xProgressBar::loadAttributes(node);

    PopupAndroidBrush(node, XWINDOW_NAME_SLIDE_ICON, m_brushSlider);

    PopupAndroidPixel(node, XWINDOW_NAME_PROGRESSBAR_PADDINGLEFT, m_nLeftPadding);
    PopupAndroidPixel(node, XWINDOW_NAME_PROGRESSBAR_PADDINGTOP, m_nTopPadding);
    PopupAndroidPixel(node, XWINDOW_NAME_PROGRESSBAR_WIDTH, m_nProgressBarWidth);

    PopupAndroidBrush(node, XWINDOW_NAME_PROGRESSBAR_SLIDER, m_brushSlider);

    SetPaintPosition();
}

void xSliderProgressBar::SetSliderPixmap(xPixmap& pmSlider)
{
    m_brushSlider.setPixmap(pmSlider);
}

void xSliderProgressBar::SetSliderColor(xColor colSlider, chRect rcSlider)
{
    m_brushSlider.setColor(colSlider);
    m_rcSlider = rcSlider;
}

void xSliderProgressBar::SetPaintPosition()
{
    if (m_nRange <= 0 || m_nPosition < 0)
    {
        return;
    }

    float fShowPercent = (float)m_nPosition / m_nRange;
    m_nPaintPosition = (int)(fShowPercent * m_nProgressBarWidth);
}

bool xSliderProgressBar::onPaintContent(xPainter& p)
{
    if (m_nPosition < 0 || m_nRange <= 0)
    {
        return true;
    }

    PaintProgress(p);

    return true;
}

void xSliderProgressBar::PaintProgress(xPainter& p)
{
    SetPaintPosition();

    PaintProgressBG(p);
    PaintProgressBar(p);

    PaintSlider(p);
}

void xSliderProgressBar::PaintProgressBG(xPainter& p)
{
    if (m_brushBG.isNull())
    {
        return;
    }

    xPixmap pmBG = m_brushBG.pixmap();
    if (m_nBarWidth <= 0 && !pmBG.isNull())
    {
        m_nBarWidth = pmBG.GetRealHeight();
    }

    if (m_nBarWidth <= 0)
    {
        return;
    }

    chRect rcBG(m_nLeftPadding, m_nTopPadding, m_nLeftPadding + m_nProgressBarWidth, m_nTopPadding + m_nBarWidth);
    if (!pmBG.isNull())
    {
        p.fillRect(rcBG, pmBG);
    }
    else
    {
        xColor colBG = m_brushBG.GetColor();
        p.blendRect(rcBG, colBG);
    }
}

void xSliderProgressBar::PaintProgressBar(xPainter& p)
{
    if (m_brushBar.isNull())
    {
        return;
    }

    xPixmap pmBar = m_brushBar.pixmap();
    if (m_nBarWidth <= 0 && !pmBar.isNull())
    {
        m_nBarWidth = pmBar.GetRealHeight();
    }

    if (m_nBarWidth <= 0)
    {
        return;
    }

    chRect rcBar(m_nLeftPadding, m_nTopPadding, m_nLeftPadding + m_nPaintPosition, m_nTopPadding + m_nBarWidth);
    chRect rcSrc(rcBar);
    rcSrc.OffsetRect(-rcBar.left, -rcBar.top);

    if (!pmBar.isNull())
    {
        p.drawItemPixmapEx(rcBar, pmBar, rcSrc);
    }
    else
    {
        xColor colBar = m_brushBar.GetColor();
        p.blendRect(rcBar, colBar);
    }
}

void xSliderProgressBar::PaintSlider(xPainter& p)
{
    if (m_brushSlider.isNull())
    {
        return;
    }

    int nSliderWidth = 0;
    int nSliderHeight = 0;

    xPixmap pmSlider = m_brushSlider.pixmap();
    if (!pmSlider.isNull())
    {
        nSliderWidth = pmSlider.GetRealWidth();
        nSliderHeight = pmSlider.GetRealHeight();
    }
    else
    {
        nSliderWidth = m_rcSlider.Width();
        nSliderHeight = m_rcSlider.Height();
    }

    if (nSliderWidth <= 0 || nSliderHeight <= 0)
    {
        return;
    }

    int nProgressCenterLeft = m_nLeftPadding + m_nPaintPosition + (m_nPaintOffset >> 1);
    int nProgressCenterTop = m_nTopPadding + (m_nBarWidth >> 1);

    chRect rcBar(nProgressCenterLeft - (nSliderWidth >> 1)
                 , nProgressCenterTop - (nSliderHeight >> 1)
                 , nProgressCenterLeft + (nSliderWidth >> 1)
                 , nProgressCenterTop + (nSliderHeight >> 1));

    if (!pmSlider.isNull())
    {
        p.fillRect(rcBar, pmSlider);
    }
    else
    {
        xColor colBG = m_brushSlider.GetColor();
        p.blendRect(rcBar, colBG);
    }
}
