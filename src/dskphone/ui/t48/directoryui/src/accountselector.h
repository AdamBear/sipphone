
#ifndef __ACCOUNT_SELECTOR_H__
#define __ACCOUNT_SELECTOR_H__

#include "baseui/framelistdelegate.h"
#include "directorycommon.h"

typedef struct
{
    int nAccountId;
    yl::string strName;
    yl::string strValue;
} ST_ACCOUNT_NODE;
//typedef ST_ACCOUNT_NODE* ST_ACCOUNT_NODE_PTR;
typedef YLVector<ST_ACCOUNT_NODE> VEC_ACCOUNT_NODE;

class CAccountSelector : public CFrameListDelegate
{
public:
    CAccountSelector();
    virtual ~CAccountSelector(void);

    int GetDialAccountId() const;

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

private:
    int m_nSelIndex;
    VEC_ACCOUNT_NODE m_vecAccountNode;
};

#endif
