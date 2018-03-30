
#ifndef __TREE_LIST_DELEGATE_H__
#define __TREE_LIST_DELEGATE_H__

#include <map>
#include <vector>
#include "baseui/framelistdelegatebase.h"
#include "baseui/itreelistdelegate.h"
#include "uikernel/ilistitemaction.h"

class CTreeDataNode
{
public:
    explicit CTreeDataNode(int nId, int nParentId, bool bHadChildren);
    virtual ~CTreeDataNode();

    // 设置是否是父节点
    void SetHadChildren(bool bHadChildren);
    // 设置父节点状态
    void SetExpanded(bool bExpanded);

    // 获取节点ID
    int GetId() const;
    // 获取父节点ID
    int GetParentId() const;
    // 是否是父节点
    bool IsHadChildren() const;
    // 是否展开
    bool IsExpanded() const;

private:
    // 树节点ID
    int m_nId;
    // 父节点ID
    int m_nParentId;
    // 是否有子节点
    bool m_bHadChildren;
    // 是否扩展
    bool m_bExpanded;
};

typedef CTreeDataNode * CTreeDataNodePtr;
typedef std::map<int, CTreeDataNodePtr> MAP_TREE_DATA_NODE;

class CTreeItem;
class CTreeList;

class CTreeListDelegate : public CFrameListDelegateBase, public IListItemAction,
    public ITreeListDelegate
{
public:
    CTreeListDelegate();
    virtual ~CTreeListDelegate(void);

public:
    // 初始化
    virtual void Init()                 = 0;

    // 根据ID获得树节点数据
    CTreeDataNodePtr GetTreeDataNode(int nTreeId);

    CTreeDataNodePtr GetSelectTreeDataNode();

    int GetSelectId() const;

// IFrameListDelegate
public:
    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext();
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious();
    // 数据清空
    virtual void OnCleanContent(CFrameListBase * pFrameList);
    // 是否已经绑定特定的列表框
    virtual bool IsFrameListAttach(CFrameListBase * pFrameList);
    // 数据代理解除绑定
    virtual void OnFrameListDetach(CFrameListBase * pFrameList);
    // 数据代理绑定
    virtual void OnFrameListAttach(CFrameListBase * pFrameList);
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

#if IF_FEATURE_MUTI_CALLLOG
    virtual bool IsIdenticalTypeNote(int nParentID, int nTreeID)
    {
        return true;
    };
#endif

public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

// ITreeListDelegate
public:
    // 父节点展开
    virtual void OnItemExpand(CTreeList * pTreeList, CTreeItem * pTreeItem);
    // 父节点收起
    virtual void OnItemCurl(CTreeList * pTreeList, CTreeItem * pTreeItem);

// CFrameListDelegateBase
protected:
    virtual IFrameListDelegatePtr GetDelegate() const;

protected:
    void Clear();

    typedef std::vector<int> VEC_TREE_ID;

    // 生成TreeId(从1开始生成树节点ID)
    int GetTreeId();
    // 获取一个节点的逐层父节点ID
    void GetParentTreeIdSet(VEC_TREE_ID & vecTreeId, int nTreeId);
    // 获取最底下被展开的节点
    CTreeDataNodePtr GetMaxExpandedNode();

    // 展开一个节点
    void ExpandNode(CTreeDataNodePtr pNode);
    // 收缩一个节点
    void CurlNode(CTreeDataNodePtr pNode);
    // 收缩所有的节点
    void CurlAllNode();

    // 刷新树控件
    virtual void RefreshTreeListItem(int nCount, int nIndex)                = 0;

protected:
    // 记录被选中的项
    int m_nUISelectId;

    // 界面上需要的所有节点ID(不包括未展开)
    VEC_TREE_ID m_vecTreeId;
    // 所有树形控件的数据（包括未展开）
    MAP_TREE_DATA_NODE m_mapTreeNode;

private:
    // 已经使用的最大树节点ID
    int m_nMaxTreeId;
};
typedef CTreeListDelegate * CTreeListDelegatePtr;

#endif
