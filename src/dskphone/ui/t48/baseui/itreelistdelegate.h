
#ifndef __I_TREE_LIST_DELEGATE_H__
#define __I_TREE_LIST_DELEGATE_H__

#include "uikernel/iframelistdelegate.h"

class CTreeItem;
class CTreeList;

class ITreeListDelegate : public IFrameListDelegate
{
public:
    ITreeListDelegate() {};
    virtual ~ITreeListDelegate() {};

public:
    // 父节点展开
    virtual void OnItemExpand(CTreeList * pTreeList, CTreeItem * pTreeItem)           = 0;
    // 父节点收起
    virtual void OnItemCurl(CTreeList * pTreeList, CTreeItem * pTreeItem)             = 0;
};

#endif
