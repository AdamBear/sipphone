#ifndef __GENBAND_DETAIL_LAYOUT_H__
#define __GENBAND_DETAIL_LAYOUT_H__

#if IF_FEATURE_GBBOOK
#include "detaillayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_GENBAND         = DETAIL_LIST_ITEM_LAYOUT_BASE + 5,
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

class CEditListItem;
class CComboBoxListItem;

struct DirGroupRecord;
struct ContactGBBookUnit;

class CGenbandDetailLayout : public CDetailLayout
{
public:
    explicit CGenbandDetailLayout();
    virtual ~CGenbandDetailLayout();

    static bool IsGenbandDetailLayout(CDetailLayout* pLayout);

public:
    void Init();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

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

    void SetAction(int nAction);
    int GetAction() const;
    bool IsContactLayout();
    bool IsGroupLayout();

    bool UpdateContact(int nContactId, ContactGBBookUnit * pContact);
    bool UpdateGroup(int nGroupId, DirGroupRecord * pGroup);

protected:
    void InitNameCtrl(const yl::string& strTtle, int nDataIndex);
    void InitNumberCtrl(const yl::string& strNumTitle, int nDataIndex);
    void InitGroupCtrl();
    CEditListItem* GetEditItemByIndex(int nIndex, bool bCheckLayout = true);
    void UpdateEditValueByName(CEditListItem* pEditItem, ContactGBBookUnit * pContact, const yl::string& strName);


protected:
    int m_nAction;
    int m_nDetailId;
};

typedef CGenbandDetailLayout * CGenbandDetailLayoutPtr;

#endif
#endif
