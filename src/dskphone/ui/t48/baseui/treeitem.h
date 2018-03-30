#ifndef __TREE_ITEM_H__
#define __TREE_ITEM_H__

#include <QWidget>
#include "uikernel/listitem.h"

enum
{
    TREE_ITEM_LEVEL_NONE        = 255,
    TREE_ITEM_LEVEL_FIRST       = 0,            // 第一层节点
    TREE_ITEM_LEVEL_SECOND      = 1,            // 第二层节点
    TREE_ITEM_LEVEL_MAX         = 2,            // 最大节点层数
};

enum
{
    LIST_ITEM_TYPE_TREE         = 0x01000000,
};

class QStylePainter;
class CTreeItem : public CListItem
{
    Q_OBJECT

public:
    explicit CTreeItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CTreeItem();

    // 设置通知更改处理对象
    virtual void SetDelegate(IListItemDelegate * pDelegate);

    // 清除内容
    virtual void ClearContact();

    // 设置父节点属性
    void SetHadChildren(bool bHadChildren);

    // 设置是否展开
    void SetExpand(bool bExpand);

    // 设置层级
    void SetLevel(unsigned char ucLevel);

    // 设置ID
    void SetId(int nId);

    bool IsHadChildren() const;
    bool IsExpand() const;
    unsigned char GetLevel() const;
    int GetId() const;

protected:
    // 画树形箭头，ptBegin会返回画完后的位置
    void PaintExpand(QStylePainter & stylePainter, QPoint & ptBegin, const char * pszExand,
                     const char * pszCurl);
    void PaintRawIcon(QStylePainter & stylePainter, QPoint & ptBegin, const char * pszIcon);

protected:
    // 是否是父节点
    bool m_bHadChildren;
    // 是否展开状态
    bool m_bExpand;
    // 第几层节点
    unsigned char m_ucLevel;
    // 节点ID
    int m_nId;

    // 内容偏移
    int m_nContextLeft;
};

typedef CTreeItem * CTreeItemPtr;

#endif // __TREE_ITEM_H__
