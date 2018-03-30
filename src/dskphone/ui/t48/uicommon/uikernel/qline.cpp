#include "qline.h"

qLine::qLine(QRect rect, QWidget * parent):
    QWidget(parent),
    m_vecColor(Qt::black)
{
    if (!rect.isEmpty())
    {
        this->setGeometry(rect);

        //线长和线宽根据父控件大小确定
        if (this->width() > this->height()) //画横线
        {
            m_bHorz = true;
            m_nLength = this->width();
            m_nWidth = this->height();
        }
        else    //画竖线
        {
            m_bHorz = false;
            m_nLength = this->height();
            m_nWidth = this->width();
        }
    }
}

void qLine::paintEvent(QPaintEvent * pEvent)
{
    QPainter * paint = new QPainter(this);
    if (paint != NULL)
    {
        //设置画笔参数
        QPen pen;
        pen.setWidth(m_nWidth * 2);     //设置线宽，2倍是为了实现正常线宽，为什么?
        pen.setColor(m_vecColor);       //设置直线颜色
        paint->setPen(pen);

        if (m_bHorz)
        {
            //画横线
            paint->drawLine(0, 0, this->width(), 0);
        }
        else
        {
            //画竖线
            paint->drawLine(0, 0, 0, this->height());
        }
    }
}

void qLine::SetLineWidth(int nWidth)
{
    m_nWidth = nWidth;
    //设置线宽只需重设控件大小即可
    if (m_bHorz)
    {
        this->setFixedHeight(m_nWidth);
    }
    else
    {
        this->setFixedWidth(m_nWidth);
    }
}

void qLine::SetLineColor(QColor color)
{
    m_vecColor = color;
}

qLine::~qLine()
{

}
