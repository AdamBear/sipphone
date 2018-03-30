// 普通Menu列表项：Menu + ">图标"
#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_

#include "uikernel/listitem.h"
#include <ylstring.h>

class QString;
class QPoint;
class QMouseEvent;
class QPaintEvent;
class QStylePainter;

enum
{
    LIST_ITEM_TYPE_TEXT_MENU            = 0x02000000,
};

class CTextMenuItem : public CListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
    QRect m_rcText;
    QString m_strDraw;
    int ichoose;
#endif
public:
    explicit CTextMenuItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CTextMenuItem(void);

public:
    // 清除内容
    virtual void ClearContent();

public:
    void SetId(int nMenuId);
    int GetId() const;

    // 设置标题
    void SetMenu(const QString & strMenu);
    void SetMenu(const yl::string & strMenu);

    void SetClickAction(const QString & strClickAction);
    QString GetClickAction();

    void SetShowIndex(bool bShowIndex);
    bool IsShowIndex();

    void SetTextColor(QColor & clrNormal, QColor & clrSelect);

    void SetChangeColorOnPress(bool bChangeOnPress);

    void SetPressColor(QColor & clrPress);

protected:
//  virtual void mousePressEvent(QMouseEvent* pEvent);
//  virtual void mouseReleaseEvent(QMouseEvent* pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    QColor & GetTextPaintColor();

protected:
    int m_nMenuId;

    QString m_strText;
    QString m_strClickAction;

    bool m_bShowIndex;
    bool m_bChangeColorOnPress;
    QColor m_clrNormal;
    QColor m_clrSelect;
    QColor m_clrPress;
};

#endif
