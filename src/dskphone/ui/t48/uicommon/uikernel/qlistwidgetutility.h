
#ifndef __Q_LIST_WIDGET_UTILITY_H__
#define __Q_LIST_WIDGET_UTILITY_H__

#include <QListWidget>

class QListWidgetUtility
{
public:
    QListWidgetUtility(void);
    ~QListWidgetUtility(void);

public:
    //QListWidget
    /****************************************************
    函数名称：SetQListWidgetTransparent(QListWidget* pListWgt)
    函数意义：设置QListWidget透明
    参数意义：
    参数1：pListwgt --QListWidget*类型，所要修改属性的QListWidget 控件指针
    返回值：无返回值
    ***************************************************/
    static void SetQListWidgetTransparent(QListWidget * pListWgt);

    /****************************************************
    函数名称：SetQListWgtNormalProperty(QListWidget* pListwgt)
    函数意义：设置QListWidget基本属性
    参数意义：
    参数1：pListwgt --QListWidget*类型，所要修改属性的QListWidget 控件指针
    返回值：无返回值
    ***************************************************/
    static void SetQListWgtNormalProperty(QListWidget * pListwgt);
};

#endif
