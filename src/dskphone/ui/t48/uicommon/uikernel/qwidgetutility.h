
#ifndef __Q_WIDGET_UNILITY_H__
#define __Q_WIDGET_UNILITY_H__

#include <ETLLib/ETLLib.hpp>
#include <QWidget>

class qWidgetUtility
{
public:
    qWidgetUtility(void);
    ~qWidgetUtility(void);

public:
    // 设置字体的风格
    // nPointSize:字体的大小 nWeightSize:字体的宽度
    // pObject:控件的指针    bBoldType:是否加粗
    static void setFontStyle(QWidget * pObject, int nPointSize, int nWeight = QFont::Normal,
                             bool bBoldType = true);

    // 设置背景图片
    static void SetBackGround(QWidget * pWgt, LPCSTR picPath, bool bScale = false);
    static void SetBackGround(QWidget * pWgt, const QPixmap & pic, bool bScale = false);

    //将控件的某一区域映射到全局的区域
    static QRect qMapRectToGlobal(QWidget * pWidget, QRect rcSrc);

    //将全局的某一区域映射到控件上的区域
    static QRect qMapRectFromGlobal(QWidget * pWidget, QRect rcSrc);
};

#endif
