#include "qfrmpaint.h"
#include "imagemanager/modimagemanager.h"

qFramePaint::qFramePaint(QWidget * parent)
    : QFrame(parent)
{

}

qFramePaint::~qFramePaint()
{

}

void qFramePaint::AddHorizontalLine(const QPoint & pointStart, int iWidth)
{
    // 判断参数是否有效
    if (iWidth > 0)
    {
        // 添加绘制信息
        QPoint pointEnd(pointStart.x() + iWidth, pointStart.y());
        m_ListHorizontalLineData.push_back(make_pair(pointStart, pointEnd));
    }
    else
    {
        UIKERNEL_WARN("AddHorizontalLine iWidth error");
    }
}
void qFramePaint::SetFirstLinePen(const QPen & pen)
{
    m_FirstLinePen = pen;
}

void qFramePaint::CleanHorizontalLineData()
{
    m_ListHorizontalLineData.clear();
}

void qFramePaint::AddVerticalLine(const QPoint & pointStart, int iHeight)
{
    // 判断参数是否有效
    if (iHeight > 0)
    {
        // 添加绘制信息
        QPoint pointEnd(pointStart.x(), pointStart.y() + iHeight);
        m_ListVerticalLineData.push_back(make_pair(pointStart, pointEnd));
    }
    else
    {
        UIKERNEL_WARN("AddVerticalLine iHeight error");
    }
}

void qFramePaint::SetSecondLinePen(const QPen & pen)
{
    m_SecondLinelPen = pen;
}

void qFramePaint::CleanVerticalLineData()
{
    m_ListVerticalLineData.clear();
}

void qFramePaint::AddDrawRect(const QRect & rectRange, int nInterval)
{
    // 判断矩形区域是否有效
    if (!rectRange.isValid())
    {
        UIKERNEL_WARN("rectRange is not Valid");
        return;
    }

    // 线条的间隔不能小于画笔的宽度
    if (nInterval < m_FirstLinePen.width())
    {
        UIKERNEL_WARN("nInterval less than pen width");
        return;
    }

    m_ListRectData.push_back(make_pair(rectRange, nInterval));
}

void qFramePaint::CleanRectData()
{
    m_ListRectData.clear();
}

void qFramePaint::AddDrawImage(const QPoint & pointStart, const yl::string & strPath)
{
    // 判断图片的路径是否能为空
    if (!strPath.empty())
    {
        m_ListImageData.push_back(std::make_pair(pointStart, strPath));
    }
    else
    {
        UIKERNEL_WARN("Image Path is Null");
    }
}

void qFramePaint::CleanImageData()
{
    m_ListImageData.clear();
}

void qFramePaint::DrawMayLine(QPainter & objPainter, QPoint startPoint,
                              QPoint endPoint, LINE_TYPE eType)
{
    objPainter.setPen(m_FirstLinePen);
    objPainter.drawLine(startPoint, endPoint);

    if (eType == LINE_TYPE_HOR)
    {
        startPoint.setY(m_FirstLinePen.width() + startPoint.y());
        endPoint.setY(m_FirstLinePen.width() + endPoint.y());
    }
    else if (eType == LINE_TYPE_VER)
    {
        startPoint.setX(m_FirstLinePen.width() + startPoint.x());
        endPoint.setX(m_FirstLinePen.width() + endPoint.x());
    }
    else
    {
        chWARNING2(false, "qFramePaint::DrawMayLine LINE_TYPE error");
        return;
    }

    objPainter.setPen(m_SecondLinelPen);
    objPainter.drawLine(startPoint, endPoint);
}

void qFramePaint::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);

    // 这里需要优先绘制图片，不然图片会将线条等遮住
    // 绘制图片
    if (!m_ListImageData.empty())
    {
        ImageDataCIter iterImageData = m_ListImageData.begin();
        for (/* */; iterImageData != m_ListImageData.end(); ++iterImageData)
        {
            painter.drawPixmap(iterImageData->first,
                               THEME_GET_BMP(iterImageData->second.c_str()));
        }
    }

    // 绘制矩形区域
    if (!m_ListRectData.empty())
    {
        RectDataCIter iterRectData = m_ListRectData.begin();
        for (/* */; iterRectData != m_ListRectData.end(); ++iterRectData)
        {
            // 绘制矩形区域顶部的线条
            QPoint pointLeft = iterRectData->first.topLeft();
            QPoint pointRight = iterRectData->first.topRight();

            // 填充线条
            DrawMayLine(painter, pointLeft, pointRight, LINE_TYPE_HOR);
            while (pointLeft.y() + iterRectData->second < iterRectData->first.bottom())
            {
                painter.setPen(m_SecondLinelPen);
                pointLeft.setY(pointLeft.y() + iterRectData->second);
                pointRight.setY(pointRight.y() + iterRectData->second);
                DrawMayLine(painter, pointLeft, pointRight, LINE_TYPE_HOR);
            }
        }
    }

    // 绘制水平线条
    if (!m_ListHorizontalLineData.empty())
    {
        LineDataCIter iterLineData = m_ListHorizontalLineData.begin();
        for (/* */; iterLineData != m_ListHorizontalLineData.end(); ++iterLineData)
        {
            DrawMayLine(painter, iterLineData->first, iterLineData->second, LINE_TYPE_HOR);
        }
    }

    // 绘制竖直线条
    painter.setPen(m_SecondLinelPen);
    if (!m_ListVerticalLineData.empty())
    {
        LineDataCIter iterLineData = m_ListVerticalLineData.begin();
        for (/* */; iterLineData != m_ListVerticalLineData.end(); ++iterLineData)
        {
            DrawMayLine(painter, iterLineData->first, iterLineData->second, LINE_TYPE_VER);
        }
    }
}
