// 带图标的菜单列表项：图标 + Menu + ">图标"
#ifndef __ICON_MENU_LIST_ITEM_H__
#define __ICON_MENU_LIST_ITEM_H__

#include "baseui/menulistitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_ICON_MENU        = 0x00010000,
};

class CIconMenuListItem : public CMenuListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();

    QRect m_rcIcon;
#endif
public:
    explicit CIconMenuListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CIconMenuListItem(void);

    static int GetIconMenuListItemType();

// CListItem
public:
    // 清除内容
    virtual void ClearContent();

public:
    // 设置Icon图片
    void SetIconPath(const yl::string & strPath);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    // 按默认偏移位置及大小画图标
    void PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon);
    // 画图标
    void PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon);

protected:
    // Icon图片路径
    yl::string m_strIconPath;
};
typedef CIconMenuListItem * CIconMenuListItemPtr;

#endif
