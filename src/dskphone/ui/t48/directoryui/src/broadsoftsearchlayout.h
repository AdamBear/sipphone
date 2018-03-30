#ifndef __BROADSOFT_SEARCH_LAYOUT_H__
#define __BROADSOFT_SEARCH_LAYOUT_H__

#include "detaillistitemlayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_BSFT_SEARCH     = DETAIL_LIST_ITEM_LAYOUT_BASE + 3,
};

enum
{
    BS_SEARCH_ITEM_INDEX_FIRST_NAME     = 0,
    BS_SEARCH_ITEM_INDEX_LAST_NAME,
    BS_SEARCH_ITEM_INDEX_NUMBER,
    BS_SEARCH_ITEM_INDEX_EXTENSION,
    BS_SEARCH_ITEM_INDEX_MOBILE,
    BS_SEARCH_ITEM_INDEX_DEPARTMENT,
    BS_SEARCH_ITEM_INDEX_EMAIL,
    BS_SEARCH_ITEM_INDEX_GROUP,
    BS_SEARCH_ITEM_INDEX_MAX,
};

class CBaseDialog;
class CEditListItem;
class CComboBoxListItem;

class CBroadSoftSearchLayout : public CDetailListItemLayout
{
public:
    explicit CBroadSoftSearchLayout(CBaseDialog * pDlg);
    virtual ~CBroadSoftSearchLayout();

public:
    virtual void Init();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    void ClearEdit();

    CEditListItem * GetFirstNameItem();
    CEditListItem * GetLastNameItem();
    CEditListItem * GetNumberItem();
    CEditListItem * GetExtensionItem();
    CEditListItem * GetMobileItem();
    CEditListItem * GetDepartmentItem();
    CEditListItem * GetEmailItem();
    CComboBoxListItem * GetGroupItem();

protected:
    void InitFirstNameCtrl();
    void InitLastNameCtrl();
    void InitNumberCtrl();
    void InitExtensionCtrl();
    void InitMobileCtrl();
    void InitDepartmentCtrl();
    void InitEMailCtrl();
    void InitGroupCtrl();
};

typedef CBroadSoftSearchLayout * CBroadSoftSearchLayoutPtr;

#endif
