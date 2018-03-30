
#ifndef __GROUP_TREE_ITEM_H__
#define __GROUP_TREE_ITEM_H__

#include <ylstring.h>
#include "baseui/treeitem.h"

class QString;
class QPainterEvent;
class QStylePainter;

enum
{
    LIST_ITEM_SUB_TYPE_GROUP_TREE       = 0x00010000,
};

class CGroupTreeItem : public CTreeItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();

    QRect m_rcText;
    int ichoose;
#endif
public:
    explicit CGroupTreeItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CGroupTreeItem(void);

    void SetGroupName(const yl::string & strGroupName);
    void SetGroupName(const QString & strGroupName);

    const QString & GetGroupName() const;

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintGroupTree(QStylePainter & stylePainter, QPoint & ptEnd);
    void PaintGroupText(QStylePainter & stylePainter, const QPoint & ptBegin, int nTextRight = 0);

protected:
    QString m_strGroupName;
};
typedef CGroupTreeItem * CGroupTreeItemPtr;

#endif
