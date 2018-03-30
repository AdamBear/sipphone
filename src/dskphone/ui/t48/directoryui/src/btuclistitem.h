#ifndef __BTUC_LIST_ITEM_H__
#define __BTUC_LIST_ITEM_H__

#include "directorycommon.h"
#include "baseui/contactbaseitem.h"

class QPoint;
class QStylePainter;

enum
{
    LIST_ITEM_SUB_TYPE_BTUC_CONTACT     = 0x00000200,
};

class CBTUCListItem : public CContactBaseItem
{
    Q_OBJECT

public:
    explicit CBTUCListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CBTUCListItem();

    static int GetBTUCItemType();

// CListItem
public:
    // 清除内容
    virtual void ClearContent();

public:
    // 设置头像
    void SetPhoto(const yl::string & strPhoto);
    // 设置状态
    void SetPresence(int nPresence);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintStatus(QStylePainter & stylePainter, const QPoint & ptContact);

protected:
    // 状态
    int m_nPresence;
    // 头像
    QPixmap m_pmPhoto;
};
typedef CBTUCListItem * CBTUCListItemPtr;

#endif
