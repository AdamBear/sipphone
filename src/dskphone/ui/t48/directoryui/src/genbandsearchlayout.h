#ifndef __GENBAND_SEARCH_LAYOUT_H__
#define __GENBAND_SEARCH_LAYOUT_H__

#include "detaillistitemlayout.h"
#include "directorycommon.h"
#if IF_FEATURE_GBBOOK
enum
{
    DETAIL_LIST_ITEM_LAYOUT_GENBAND_SEARCH      = DETAIL_LIST_ITEM_LAYOUT_BASE + 4,
};

enum
{
    GB_SEARCH_ITEM_INDEX_SEARCH_TYPE = 0,
    GB_SEARCH_ITEM_INDEX_NAME,
    GB_SEARCH_ITEM_INDEX_MAX,
};

class CBaseDialog;
class CEditListItem;
class CComboBoxListItem;

class CGenbandSearchLayout : public CDetailListItemLayout
{
public:
    explicit CGenbandSearchLayout(CBaseDialog * pDlg);
    virtual ~CGenbandSearchLayout();

public:
    virtual void Init();
    virtual void Init(GBBOOK_OPERATION_TYPE eMode);

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    void SetSearchMode(GBBOOK_OPERATION_TYPE eMode)
    {
        m_eSearchMode = eMode;
    }

    GBBOOK_OPERATION_TYPE GetSearchMode()
    {
        return m_eSearchMode;
    }

    void ClearEdit();

    CComboBoxListItem * GetSearchTypeItem();
    CEditListItem * GetSearchKeyItem();

protected:
    void InitSearchTypeCtrl();
    void InitNameCtrl();
    void InitNameCtrl(GBBOOK_OPERATION_TYPE eSearchType);
    void InitNumberCtrl();
    void GetNameBySearchType(GBBOOK_OPERATION_TYPE eSearchType, QString & strDisName);
    GBBOOK_OPERATION_TYPE m_eSearchMode;
};

typedef CGenbandSearchLayout * CGenbandSearchLayoutPtr;
#endif
#endif
