
#ifndef __GENBAND_DETAIL_LAYOUT_H__
#define __GENBAND_DETAIL_LAYOUT_H__
#if IF_FEATURE_GBBOOK
#include "detaillistitemlayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_GENBAND         = DETAIL_LIST_ITEM_LAYOUT_BASE + 3,
};

enum
{
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_NICK_NAME         = 0,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_GROUP,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_PRIMARY_CONTACT,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_FIRST_NAME,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_LAST_NAME,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_BUSINESS_NO,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_HOME_NO,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_PAGER,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_FAX,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_EMAIL,
    GENBAND_CONTACT_DETAIL_ITEM_INDEX_CONF_URL,
};

// 本地群组的详情界面顺序
enum
{
    GENBAND_GROUP_DETAIL_ITEM_INDEX_NAME                = 0,
};

class CBaseDialog;
class CEditListItem;
class CComboBoxListItem;
class CDetailListItem;

struct DirGroupRecord;
struct ContactGBBookUnit;

class CGenbandDetailLayout : public CDetailListItemLayout
{
public:
    explicit CGenbandDetailLayout(CBaseDialog * pDlg, bool bNewContact, bool bCopyLayout);
    virtual ~CGenbandDetailLayout(void);

    static bool IsGenbandDetailLayout(CDetailListItemLayoutPtr pLayout);

    enum
    {
        GENBAND_DETAIL_LAYOUT_TYPE_NONE     = 0,
        GENBAND_DETAIL_LAYOUT_TYPE_CONTACT  = 1,
        GENBAND_DETAIL_LAYOUT_TYPE_GROUP        = 2,
    };

public:
    virtual void Init();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    virtual bool IsInputEmpty();

    int GetDetailId();

    CEditListItem * GetNameEditItem();

    CEditListItem * GetFirstNameEditItem();
    CEditListItem * GetLastNameEditItem();

    CEditListItem * GetBusinessNoEditItem();
    CEditListItem * GetMobileNoEditItem();
    CEditListItem * GetHomeNoEditItem();

    CEditListItem * GetPagerEditItem();
    CEditListItem * GetFaxEditItem();
    CEditListItem * GetEmailEditItem();
    CEditListItem * GetConfUrlEditItem();
    CEditListItem * GetPriContactEditItem();

    CComboBoxListItem * GetGroupComboBoxItem();

    bool IsCopyLayout() const;
    bool IsAdditionLayout() const;
    bool UpdateContact(int nContactId, ContactGBBookUnit * pContact);
    bool UpdateGroup(int nGroupId, DirGroupRecord * pGroup);

protected:
    // 初始化Name
    void InitNameCtrl();

    // 初始化Name
    void InitNameCtrl(yl::string strNameTitle);

    void InitNumberCtrl(yl::string strNumTitle);



    // 初始化群组ComboBox
    void InitGroupCtrl();


protected:
    // 布局子类型
    int m_nSubType;
    // 是否是添加界面
    bool m_bAddition;
    // 是否是复制联系人的布局
    bool m_bCopyLayout;
    int m_nDetailId;
};

typedef CGenbandDetailLayout * CGenbandDetailLayoutPtr;

#endif
#endif
