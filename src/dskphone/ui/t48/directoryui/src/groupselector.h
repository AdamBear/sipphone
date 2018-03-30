
#ifndef __GROUP_SELECTOR_H__
#define __GROUP_SELECTOR_H__

#include <ylstring.h>
#include <ylvector.h>
#include "baseui/framelistdelegate.h"


typedef struct
{
    int nGroupId;
    yl::string strGroupName;
} ST_SIMPLE_GROUP_NODE;
typedef ST_SIMPLE_GROUP_NODE * ST_SIMPLE_GROUP_NODE_PTR;
typedef YLVector<ST_SIMPLE_GROUP_NODE_PTR> VEC_SIMPLE_GROUP_NODE;

class CGroupSelector : public CFrameListDelegate
{
public:
    CGroupSelector(void);
    virtual ~CGroupSelector(void);

public:
    void Init(int nContactType, int nExcludeGroupId, bool bRootInclude);

    int GetSelectGroupId();

    int GetGroupAmount();

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

protected:
    void ClearGroupNode();

private:
    int m_nContactType;
    int m_nSelIndex;
    VEC_SIMPLE_GROUP_NODE m_vecSimpleGroupNode;
};

#endif
