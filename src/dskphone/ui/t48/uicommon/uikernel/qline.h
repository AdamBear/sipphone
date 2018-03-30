#ifndef Q_LINE_H
#define Q_LINE_H

#include <QtGui>
#include <QWidget>

/*
两个参数分别为：
控件位置大小，
父控件
*/
class  qLine : public QWidget
{
    Q_OBJECT
public:
    qLine(QRect rect, QWidget * parent);

    ~qLine();

protected:
    void paintEvent(QPaintEvent * pEvent);

    //这里提供的接口均为对外接口，用于自行设置属性
    //如果不设置，则采取默认值
public:
    void SetLineWidth(int nWidth);
    //设置线条颜色
    void SetLineColor(QColor color);

private:
    QColor m_vecColor /*= Qt::black*/;      //线条颜色，默认为黑色
    int m_nWidth;                       //线条宽度
    int m_nLength;                      //线条长度
    bool m_bHorz;                       //线条方向，true表示横向
};
#endif // Q_LINE_H
