#ifndef __MTSW_CONTACT_DETIAL_LAYOUT_H__
#define __MTSW_CONTACT_DETIAL_LAYOUT_H__

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "detaillayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_MTSW_CONTACT        = DETAIL_LIST_ITEM_LAYOUT_BASE + 6,
};

enum
{
    MTSW_DETAIL_ITEM_INDEX_FIRST_NAME       = 0,
    MTSW_DETAIL_ITEM_INDEX_LAST_NAME,
    MTSW_DETAIL_ITEM_INDEX_NICK_NAME,
    MTSW_DETAIL_ITEM_INDEX_JOBTITLE,
    MTSW_DETAIL_ITEM_INDEX_ORGANISATION,
    MTSW_DETAIL_ITEM_INDEX_HOME_NUMBER,
    MTSW_DETAIL_ITEM_INDEX_WORK_NUMBER,
    MTSW_DETAIL_ITEM_INDEX_MOBILE,
    MTSW_DETAIL_ITEM_INDEX_FAX_NUMBER,
    MTSW_DETAIL_ITEM_INDEX_OTHER_NUMBER,
    MTSW_DETAIL_ITEM_INDEX_MAX,
};

class CEditListItem;
class CComboBoxListItem;
struct ContactMetaSwitchContactList;

class CMTSWContactLayout : public CDetailLayout
{
public:
    explicit CMTSWContactLayout();
    virtual ~CMTSWContactLayout();

public:
    virtual void UnInit();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    void Init();

    void SetAction(int nAction);

    int GetAction() const;
    int GetDetailId() const;

    void ClearEdit();

    bool IsCopyLayout() const;
    bool IsAddLayout() const;

    bool UpdateContact(int nContactId, ContactMetaSwitchContactList * pContact);

    CEditListItem* GetFirstNameItem();
    CEditListItem* GetLastNameItem();
    CEditListItem* GetNickNameItem();
    CEditListItem* GetJobTitleItem();
    CEditListItem* GetOrgItem();
    CEditListItem* GetHomeNumberItem();
    CEditListItem* GetWorkNumberItem();
    CEditListItem* GetMobileItem();
    CEditListItem* GetFaxNumberItem();
    CEditListItem* GetOtherNumberItem();
    CComboBoxListItem* GetHomeNumberComboBoxItem();
    CComboBoxListItem* GetWorkNumberComboBoxItem();
    CComboBoxListItem* GetMobileComboBoxItem();
    CComboBoxListItem* GetFaxNumberComboBoxItem();
    CComboBoxListItem* GetOtherNumberComboBoxItem();

protected:
    void InitEditItem(int nDataIndex, int nMaxLength, const yl::string& strDisp, bool bAutoIme = true);
    void InitComboboxItem(int nDataIndex, const yl::string& strDisp);
    void SetNumberByName(ContactMetaSwitchContactList* pContact, const yl::string& strName, CEditListItem* pEditItem);
    CEditListItem* GetEditItemIndex(int nIndex);
    CComboBoxListItem* GetComboboxItemByIndex(int nIndex);
private:
    int m_nAction;
    int m_nDetailId;
};

typedef CMTSWContactLayout * CMTSWContactLayoutPtr;

#endif

#endif//#if IF_FEATURE_METASWITCH_DIRECTORY
