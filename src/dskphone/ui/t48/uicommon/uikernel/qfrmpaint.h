#ifndef QFRAME_Paint
#define QFRAME_Paint

#include <QtGui>
#include <list>
#include <ylstring.h>
#include <ETLLib/ETLLib.hpp>

// 绘制背景和线条的控件
class qFramePaint : public QFrame
{
public:
    // 重命名类内部使用类型
    typedef std::pair<QPoint, QPoint>    LineData;
    typedef std::list<LineData>          LineDataList;
    typedef LineDataList::const_iterator LineDataCIter;

    typedef std::pair<QRect, int>        RectData;
    typedef std::list<RectData>          RectDataList;
    typedef RectDataList::const_iterator RectDataCIter;

    typedef std::pair<QPoint, yl::string> ImageData;
    typedef std::list<ImageData>           ImageDataList;
    typedef ImageDataList::const_iterator  ImageDataCIter;

public:
    qFramePaint(QWidget * parent = 0);
    ~qFramePaint();

    // 绘制的线条是由两条线条FirstLine,SecondLine组成
    void SetFirstLinePen(const QPen & pen);
    void SetSecondLinePen(const QPen & pen);

    // 添加一条水平线条的绘制信息，可调用该接口反复添加
    // 添加的线条会按顺序依次被绘制
    // | pointStart | in 线条起始坐标
    // | pointEnd | in   线条结束坐标
    void AddHorizontalLine(const QPoint & pointStart, int iWidth);
    void CleanHorizontalLineData();// 清空所有的水平线条信息

    // 添加一条竖直线条的绘制信息，可调用该接口反复添加
    // 添加的线条会按顺序依次被绘制
    // | pointStart | in 线条起始坐标
    // | pointEnd | in   线条结束坐标
    void AddVerticalLine(const QPoint & pointStart, int iHeight);
    void CleanVerticalLineData();// 清空所有的竖直线条信息

    // 添加一个矩形的绘制信息，在一个矩形中填充距离为nInterval的线条，目前只支持横向的填充
    // 可调用该接口反复添加，添加的矩形会按顺序依次被绘制
    // | rectRange | in  矩形的区域
    // | nInterval | in  线条的间距
    void AddDrawRect(const QRect & rectRange, int nInterval);
    void CleanRectData();// 清空所有的绘制矩形信息

    // 添加一个图片的绘制信息，绘制的图片按照原大小进行绘制
    // 可调用该接口反复添加，添加的图片会按顺序依次被绘制
    // | pointStart | in  绘制图片的起始坐标
    // | nInterval | in  线条的间距
    void AddDrawImage(const QPoint & pointStart, const yl::string & strPath);
    void CleanImageData();// 清空所有的绘制图片信息

protected:
    void paintEvent(QPaintEvent * event);

private:
    enum LINE_TYPE
    {
        LINE_TYPE_HOR = 0,
        LINE_TYPE_VER,
    };

    // 绘制背景线条
    void DrawMayLine(QPainter & objPainter, QPoint startPoint, QPoint endPoint, LINE_TYPE eType);

private:
    QPen            m_FirstLinePen;     // 绘制水平线条的画笔
    QPen            m_SecondLinelPen;           // 绘制竖直线条的画笔
    LineDataList    m_ListHorizontalLineData;   // 保存绘制水平Line的信息
    LineDataList    m_ListVerticalLineData;     // 保存绘制竖直Line的信息
    RectDataList    m_ListRectData;  // 保存绘制Rect的信息
    ImageDataList   m_ListImageData; // 保存绘制Image的信息
};

#endif // QFRAME_Paint
