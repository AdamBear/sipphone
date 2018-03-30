
#ifndef __DETAIL_LIST_ITEM_LAYOUT_H__
#define __DETAIL_LIST_ITEM_LAYOUT_H__

#include "baseui/detaillistitembase.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_BASE            = 0,
};

class CBaseDialog;
class CDetailListItemLayout
{
public:
    explicit CDetailListItemLayout(int nType, CBaseDialog * pDlg);
    virtual ~CDetailListItemLayout(void);

public:
    // 初始化
    virtual void Init();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit()                = 0;

    virtual bool IsInputEmpty()
    {
        return true;
    };

    void ReleaseAllItem();

    bool IsLayoutItem(CListItemPtr pListItem);

    int GetType() const;
    int GetItemCount() const;

    CDetailListItemBasePtr GetDetailListItem(int nIndex);

    bool IsFirstItemFocus();
    bool IsLastItemFocus();

    void SetFirstItemFocus();
    void SetLastItemFocus();
    void SwitchFocusToPre();
    void SwitchFocusToNext();
    int GetFocusIndex();

    bool IsEditItem(int nIndex);
    bool IsLastEditItem();
    bool IsNextLastEditItem();
    int GetLastEditItemIndex();

protected:

    void SetItemFocus(CDetailListItemBasePtr pDetailItem);

    CDetailListItemBasePtr GetFirstVisbileItem(int & nItemIndex);
    CDetailListItemBasePtr GetLastVisbileItem(int & nItemIndex);

    bool IsCanFocus(CDetailListItemBasePtr pDetailItem);

protected:
    // 布局类型
    int m_nType;

    // 列表控件
    VEC_DETAIL_LIST_ITEM m_vecListItem;

    // 布局对应的窗口
    CBaseDialog * m_pDlg;
};
typedef CDetailListItemLayout * CDetailListItemLayoutPtr;

#endif
