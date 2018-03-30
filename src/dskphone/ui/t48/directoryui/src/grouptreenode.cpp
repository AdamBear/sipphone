#include "grouptreenode.h"

CGroupTreeNode::CGroupTreeNode(int nId, int nParentId, bool bHadChildren, int nType, int nGroupId)
    : CTreeDataNode(nId, nParentId, bHadChildren)
    , m_nType(nType)
    , m_nGroupId(nGroupId)
{
}

CGroupTreeNode::~CGroupTreeNode(void)
{
}

void CGroupTreeNode::SetName(const char * pszName)
{
    if (NULL == pszName)
    {
        return;
    }

    m_strName = pszName;
}

void CGroupTreeNode::SetName(const yl::string & strName)
{
    m_strName = strName;
}

int CGroupTreeNode::GetType() const
{
    return m_nType;
}

int CGroupTreeNode::GetGroupId() const
{
    return m_nGroupId;
}

const yl::string & CGroupTreeNode::GetName() const
{
    return m_strName;
}
