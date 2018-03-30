
#ifndef __LOACL_DETAIL_LAYOUT_H__
#define __LOACL_DETAIL_LAYOUT_H__

#include "detaillistitemlayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_LOCAL           = DETAIL_LIST_ITEM_LAYOUT_BASE + 1,
};

// 本地联系人的详情界面顺序
enum
{
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_GROUP       = 0,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_NAME,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_OFFICE_NO,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_ACCOUNT,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_RING,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_PHOTO,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_AUTO_DIVERT,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_FAVORITE_INDEX,
};

class CBaseDialog;
class CEditListItem;
class CComboBoxListItem;
struct LocalDirContactRecord;

class CLocalDetailLayout : public CDetailListItemLayout
{
public:
    explicit CLocalDetailLayout(CBaseDialog * pDlg, bool bNewContact, bool bCopyLayout);
    virtual ~CLocalDetailLayout(void);

    static bool IsLocalDetailLayout(CDetailListItemLayoutPtr pLayout);

public:
    virtual void Init();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    virtual bool IsInputEmpty();

    CEditListItem * GetNameEditItem();
    CEditListItem * GetOfficeNoEditItem();
    CEditListItem * GetMobileNoEditItem();
    CEditListItem * GetOtherNoEditItem();
    CComboBoxListItem * GetOfficeNoComboBoxItem();
    CComboBoxListItem * GetMobileNoComboBoxItem();
    CComboBoxListItem * GetOtherNoComboBoxItem();
    CComboBoxListItem * GetAccountComboBoxItem();
    CComboBoxListItem * GetRingComboBoxItem();
    CComboBoxListItem * GetGroupComboBoxItem();
    CComboBoxListItem * GetPhotoComboBoxItem();
    CEditListItem * GetAutoDivertItem();
#if IF_FEATURE_FAVORITE_DIR
    CEditListItem * GetFavoriteIndexEditItem();

    bool UpdateFavoriteIndex();

    void SetIsFromDsskeyAdd(bool bFavoriteAdd)
    {
        m_bIsDsskeyAdd = bFavoriteAdd;
    }

    bool IsFromDsskeyAdd()
    {
        return m_bIsDsskeyAdd;
    }
#endif

    bool IsCopyLayout() const;
    bool IsNewLayout() const;
    bool UpdateDetail(int nContactId, LocalDirContactRecord * pContact);

protected:
    // 初始化Name
    void InitNameCtrl();
    // 初始化OfficeNo
    void InitOfficeNoCtrl();
    // 初始化MobileNo
    void InitMobileNoCtrl();
    // 初始化OtherNo
    void InitOtherNoCtrl();
    // 初始化帐号ComboBox
    void InitAccountCtrl();
    // 初始化铃声ComboBox
    void InitRingCtrl();
    // 初始化群组ComboBox
    void InitGroupCtrl();
    // 初始化头像ComboBox
    void InitPhotoCtrl();
    // 初始化auto divert
    void InitAutoDivertCtrl();
#if IF_FEATURE_FAVORITE_DIR
    void InitFavoriteIndexCtrl();
#endif

protected:
    bool m_bNewContact;
    // 是否是复制联系人的布局
    bool m_bCopyLayout;
    int m_nDetailId;
#if IF_FEATURE_FAVORITE_DIR
    bool m_bIsDsskeyAdd;
#endif
};

typedef CLocalDetailLayout * CLocalDetailLayoutPtr;

#endif
