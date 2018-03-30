#ifndef __GENBAND_SEARCH_LAYOUT_H__
#define __GENBAND_SEARCH_LAYOUT_H__

#if IF_FEATURE_GBBOOK
#include "detaillayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_GENBAND_SEARCH      = DETAIL_LIST_ITEM_LAYOUT_BASE + 4,
};

class CEditListItem;

class CGenbandSearchLayout : public CDetailLayout
{
public:
    explicit CGenbandSearchLayout();
    virtual ~CGenbandSearchLayout();

public:
    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();
    void Init(int eMode);

    void SetSearchMode(int eMode);
    int GetSearchMode();

    void ClearEdit();

    CEditListItem * GetFirstItem();

protected:
    void InitNameCtrl(int eSearchType);
    void InitNumberCtrl();
    yl::string GetTitleByType(int eSearchType);

protected:
    int m_eSearchMode;
};

typedef CGenbandSearchLayout * CGenbandSearchLayoutPtr;
#endif
#endif
