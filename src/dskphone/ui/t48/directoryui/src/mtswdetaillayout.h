
#ifndef __MTSW_DETAIL_LAYOUT_H__
#define __MTSW_DETAIL_LAYOUT_H__

#include "detaillistitemlayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_MTSW_CONTACT        = DETAIL_LIST_ITEM_LAYOUT_BASE + 4,
};

// 本地联系人的详情界面顺序
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
struct LocalDirContactRecord;
struct ContactMetaSwitchContactList;

class CMTSWDetailLayout : public CDetailListItemLayout
{
public:
    explicit CMTSWDetailLayout(CBaseDialog * pDlg, bool bNewContact, bool bCopyLayout);
    virtual ~CMTSWDetailLayout(void);

    static bool IsMTSWDetailLayout(CDetailListItemLayoutPtr pLayout);

public:
    virtual void Init();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    bool IsInputEmpty();


    CEditListItem * GetFirstNameItem();
    CEditListItem * GetLastNameItem();
    CEditListItem * GetNickNameItem();
    CEditListItem * GetJobTitleItem();
    CEditListItem * GetOrgItem();

    CEditListItem * GetHomeNumberItem();
    CEditListItem * GetWorkNumberItem();
    CEditListItem * GetMobileItem();
    CEditListItem * GetFaxNumberItem();
    CEditListItem * GetOtherNumberItem();

    CComboBoxListItem * GetHomeNumberComboBoxItem();
    CComboBoxListItem * GetWorkNumberComboBoxItem();
    CComboBoxListItem * GetMobileComboBoxItem();
    CComboBoxListItem * GetFaxNumberComboBoxItem();
    CComboBoxListItem * GetOtherNumberComboBoxItem();

    bool IsCopyLayout() const;
    bool IsNewLayout() const;
    bool UpdateContact(int nContactId, ContactMetaSwitchContactList * pContact);

protected:
    void InitFirstNameCtrl();
    void InitLastNameCtrl();
    void InitNickNameCtrl();
    void InitJobTiTleCtrl();
    void InitOrgCtrl();

    void InitHomeNumberCtrl();
    void InitWorkNumberCtrl();
    void InitMobileCtrl();
    void InitFaxNumberCtrl();
    void InitOtherNumberCtrl();

protected:
    bool m_bNewContact;
    // 是否是复制联系人的布局
    bool m_bCopyLayout;
    int m_nDetailId;
};

typedef CMTSWDetailLayout * CMTSWDetailLayoutPtr;

#endif
