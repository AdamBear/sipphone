#ifndef __TREELIST_H__
#define __TREELIST_H__

#include <vector>
#include <QWidget>
#include "baseui/treeitem.h"
#include "baseui/framelist.h"
#include "baseui/itreelistdelegate.h"

class CTreeList : public CFrameList
{
    Q_OBJECT

public:
    CTreeList(QWidget * parent = NULL);
    virtual ~CTreeList();

    // 根据Id删除节点
    bool DelItemById(int nItemId, bool bRefresh = false);

    // 获取Id对应的节点位置
    int GetIndexById(int nItemId);

    // 根据Id获取节点
    CTreeItemPtr GetItemById(int nItemId);

// IListItemDelegate
public:
    // 某行获得焦点
    virtual void OnItemFocusChange(CListItem * pItem, bool bPreFocus);
};

#endif // __TREELIST_H__
