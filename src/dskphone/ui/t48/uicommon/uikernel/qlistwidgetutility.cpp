#include "qlistwidgetutility.h"

QListWidgetUtility::QListWidgetUtility(void)
{
}

QListWidgetUtility::~QListWidgetUtility(void)
{
}

/****************************************************
函数名称：SetQListWidgetTransparent(QListWidget* pListWgt)
函数意义：设置QListWidget透明
参数意义：
参数1：pListwgt --QListWidget*类型，所要修改属性的QListWidget 控件指针
返回值：无返回值
***************************************************/
void QListWidgetUtility::SetQListWidgetTransparent(QListWidget * pListWgt)
{
    if (NULL == pListWgt)
    {
        return;
    }

    //设置无虚线
    pListWgt->setFrameShape(QFrame::WinPanel);
    //设置无焦点
    pListWgt->setFocusPolicy(Qt::NoFocus);

    QBrush brush(QColor(10, 36, 106, 255));
    brush.setStyle(Qt::SolidPattern);
    QPalette palette1;
    QBrush brush2(QColor(0, 0, 0, 255));
    brush2.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::WindowText, brush2);
    QBrush brush3(QColor(212, 208, 200, 0));
    brush3.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Button, brush3);
    QBrush brush4(QColor(255, 255, 255, 0));
    brush4.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Light, brush4);
    QBrush brush5(QColor(233, 231, 227, 0));
    brush5.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Midlight, brush5);
    QBrush brush6(QColor(106, 104, 100, 0));
    brush6.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Dark, brush6);
    QBrush brush7(QColor(141, 139, 133, 0));
    brush7.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Mid, brush7);
    palette1.setBrush(QPalette::Active, QPalette::Text, brush2);
    QBrush brush8(QColor(255, 255, 255, 255));
    brush8.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::BrightText, brush8);
    palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush2);
    palette1.setBrush(QPalette::Active, QPalette::Base, brush4);
    palette1.setBrush(QPalette::Active, QPalette::Window, brush3);
    QBrush brush9(QColor(0, 0, 0, 0));
    brush9.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Shadow, brush9);
    QBrush brush10(QColor(10, 36, 106, 0));
    brush10.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Highlight, brush10);
    palette1.setBrush(QPalette::Active, QPalette::HighlightedText, brush2);
    QBrush brush11(QColor(233, 231, 227, 127));
    brush11.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush11);
    QBrush brush12(QColor(255, 255, 220, 255));
    brush12.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::ToolTipBase, brush12);
    palette1.setBrush(QPalette::Active, QPalette::ToolTipText, brush2);
    palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
    palette1.setBrush(QPalette::Inactive, QPalette::Button, brush3);
    palette1.setBrush(QPalette::Inactive, QPalette::Light, brush4);
    palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush5);
    palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush6);
    palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush7);
    palette1.setBrush(QPalette::Inactive, QPalette::Text, brush2);
    palette1.setBrush(QPalette::Inactive, QPalette::BrightText, brush8);
    palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush2);
    palette1.setBrush(QPalette::Inactive, QPalette::Base, brush4);
    palette1.setBrush(QPalette::Inactive, QPalette::Window, brush3);
    palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush9);
    palette1.setBrush(QPalette::Inactive, QPalette::Highlight, brush10);
    palette1.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush2);
    palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush11);
    palette1.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush12);
    palette1.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush2);
    palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush6);
    palette1.setBrush(QPalette::Disabled, QPalette::Button, brush3);
    palette1.setBrush(QPalette::Disabled, QPalette::Light, brush4);
    palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush5);
    palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush6);
    palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush7);
    palette1.setBrush(QPalette::Disabled, QPalette::Text, brush6);
    palette1.setBrush(QPalette::Disabled, QPalette::BrightText, brush8);
    palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush6);
    palette1.setBrush(QPalette::Disabled, QPalette::Base, brush3);
    palette1.setBrush(QPalette::Disabled, QPalette::Window, brush3);
    palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush9);
    palette1.setBrush(QPalette::Disabled, QPalette::Highlight, brush);
    palette1.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush2);
    palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush3);
    palette1.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush12);
    palette1.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush2);
    pListWgt->setPalette(palette1);
}

/****************************************************
函数名称：SetQListWgtNormalProperty(QListWidget* pListwgt)
函数意义：设置QListWidget基本属性
参数意义：
参数1：pListwgt --QListWidget*类型，所要修改属性的QListWidget 控件指针
返回值：无返回值
***************************************************/
void QListWidgetUtility::SetQListWgtNormalProperty(QListWidget * pListwgt)
{
    if (NULL == pListwgt)
    {
        return;
    }

    pListwgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//关闭垂直滚动条
    pListwgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//关闭水平滚动条
    pListwgt->setEditTriggers(QAbstractItemView::NoEditTriggers);// 不可编辑
    pListwgt->setMovement(QListView::Static);//不可移动
    pListwgt->setLayoutMode(QListView::Batched);
    pListwgt->setViewMode(QListView::IconMode);
    pListwgt->setResizeMode(QListView::Adjust);
    pListwgt->setFocusPolicy(Qt::NoFocus);
}
