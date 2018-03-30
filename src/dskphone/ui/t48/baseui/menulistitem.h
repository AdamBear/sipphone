// 普通Menu列表项：Menu + ">图标"
#ifndef __MENU_LIST_ITEM__
#define __MENU_LIST_ITEM__

#include "uikernel/listitem.h"
#include "uikernel/buttonwrapper.h"
#include <ylstring.h>

class QString;
class QPoint;
class QMouseEvent;
class QPaintEvent;
class QStylePainter;

enum
{
    LIST_ITEM_TYPE_MENU         = 0x02000000,
};

class CMenuListItem : public CListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
public:
    QRect m_rcTitle;
#endif
public:
    explicit CMenuListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CMenuListItem(void);

// CListItem
public:
    // 清除内容
    virtual void ClearContent();

public:
    void SetId(int nMenuId);

    // 设置标题
    void SetMenu(const QString & strMenu);
    void SetMenu(const yl::string & strMenu);

    int GetId() const;

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

    // 按默认偏移位置和大小画Menu文本
    void PaintMenu(QStylePainter & stylePainter, const QPoint & ptMenu);
    // 画Menu文本
    void PaintMenu(QStylePainter & stylePainter, const QRect & rcMenu);

    // 画>按钮
    void PaintEnterBtn(QStylePainter & stylePainter);

protected:
    int m_nMenuId;
    // Menu标题
    QString m_strMenu;
    // >按钮
    CButtonWrapper m_btnEnter;
};

#endif
