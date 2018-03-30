
#ifndef __GROUP_TREE_NODE_H__
#define __GROUP_TREE_NODE_H__

#include <ylstring.h>
#include "baseui/treelistdelegate.h"

class CGroupTreeNode : public CTreeDataNode
{
public:
    explicit CGroupTreeNode(int nId, int nParentId, bool bHadChildren, int nType, int nGroupId);
    virtual ~CGroupTreeNode(void);

public:
    // 设置群组节点名称
    void SetName(const char * pszName);
    // 设置群组节点名称
    void SetName(const yl::string & strName);

    // 获取类型
    int GetType() const;
    // 获取群组ID
    int GetGroupId() const;

    // 获取群组名
    const yl::string & GetName() const;

private:
    // 类型
    int m_nType;
    // 群组ID
    int m_nGroupId;
    // 群组名
    yl::string m_strName;
};
typedef CGroupTreeNode * CGroupTreeNodePtr;

#endif
