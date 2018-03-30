
#ifndef __BROADSOFT_DETAIL_LAYOUT_H__
#define __BROADSOFT_DETAIL_LAYOUT_H__

#include "detaillistitemlayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_BROADSOFT           = DETAIL_LIST_ITEM_LAYOUT_BASE + 2,
};

// BroadSoft联系人的界面顺序
enum
{
    BROADSOFT_CONTACT_DETAIL_ITEM_INDEX_GROUP   = 0,
    BROADSOFT_CONTACT_DETAIL_ITEM_NAME,
    BROADSOFT_CONTACT_DETAIL_ITEM_NUMBER,
};

class CEditListItem;
class CDetailListItem;

class CBroadSoftDetailLayout : public CDetailListItemLayout
{
public:
    explicit CBroadSoftDetailLayout(CBaseDialog * pDlg);
    virtual ~CBroadSoftDetailLayout(void);

    static bool IsBroadSoftDetailLayout(CDetailListItemLayoutPtr pLayout);

public:
    virtual void Init();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    CEditListItem * GetNameEditItem();
    CEditListItem * GetNumberEditItem();
    CEditListItem * GetGroupEditItem();

    // 初始化Name
    void InitNameCtrl();
    // 初始化Number
    void InitNumberCtrl();
    // 初始化群组Group
    void InitGroupCtrl();
};
typedef CBroadSoftDetailLayout * CBroadSoftDetailLayoutPtr;

#endif
