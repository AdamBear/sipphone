#ifndef __DETAIL_LAYOUT_H__
#define __DETAIL_LAYOUT_H__

#include "baseui/include/detaillistitembase.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_BASE            = 0,
};


class CDetailLayout : public xSlotHandler
{
public:
    explicit CDetailLayout(int nType);
    virtual ~CDetailLayout();

public:
    virtual void UnInit();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit()                = 0;

    void ReleaseAllItem();

    bool IsLayoutItem(xListViewItemPtr pListItem);

    int GetType() const;
    int GetItemCount() const;

    CDetailListItemBasePtr GetDetailListItem(int nIndex);

    void OnDetailIMEChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                            LPVOID pData);

protected:
    // 布局类型
    int m_nType;

    // 列表控件
    VEC_DETAIL_LIST_ITEM m_vecListItem;
};
typedef CDetailLayout * CDetailLayoutPtr;

#endif
