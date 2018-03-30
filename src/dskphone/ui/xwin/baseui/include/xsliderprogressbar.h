#ifndef __SLIDER_PROGRESS_BAR__
#define __SLIDER_PROGRESS_BAR__

#include "xwindow/xWindow.h"

class XWINDOW_EXPORT xSliderProgressBar : public xProgressBar
{
    DECLARE_VIEW_DUMP_CLASS(xSliderProgressBar)

public:
    xSliderProgressBar();
    ~xSliderProgressBar();

public:
    //设置滑块图片
    void SetSliderPixmap(xPixmap& pmSlider);
    //设置滑块颜色
    void SetSliderColor(xColor colSlider, chRect rcSlider);

protected:
    //设置绘制起始位置
    virtual void SetPaintPosition();

    virtual void PaintProgress(xPainter& p);
    virtual void PaintProgressBG(xPainter& p);
    virtual void PaintProgressBar(xPainter& p);
    virtual void PaintSlider(xPainter& p);

protected:
    virtual void loadAttributes(xml_node& node);
    virtual bool onPaintContent(xPainter&);

protected:
    xBrush m_brushSlider;
    chRect m_rcSlider;

    int m_nLeftPadding;
    int m_nTopPadding;
    int m_nProgressBarWidth;
};

#endif //__SLIDER_PROGRESS_BAR__
