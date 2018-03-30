#ifndef __LOACL_DETAIL_LAYOUT_H__
#define __LOACL_DETAIL_LAYOUT_H__

#include "detaillayout.h"
#include "ringplayer.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_LOCAL           = DETAIL_LIST_ITEM_LAYOUT_BASE + 1,
};

// 本地联系人的详情界面顺序
enum
{
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_NAME            = 0,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_OFFICE_NO,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_ACCOUNT,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_RING,
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_GROUP,
#ifdef PHOTO_SHOW
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_PHOTO,
#endif
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_AUTO_DIVERT,
#if IF_FEATURE_FAVORITE_DIR
    LOCAL_CONTACT_DETAIL_ITEM_INDEX_FAVORITE_INDEX,
#endif
};

// 本地群组的详情界面顺序
enum
{
    LOCAL_GROUP_DETAIL_ITEM_INDEX_NAME              = 0,
    LOCAL_GROUP_DETAIL_ITEM_INDEX_RING              = 1,
};

class CEditListItem;
class CComboBoxListItem;
class CDetailListItem;
struct LocalDirContactRecord;
struct DirGroupRecord;

class CLocalDetailLayout : public CDetailLayout
{
public:
    explicit CLocalDetailLayout();
    virtual ~CLocalDetailLayout();

    static bool IsLocalDetailLayout(CDetailLayoutPtr pLayout);

public:
    virtual void UnInit();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    void Init(int nGroupId);

    void SetAction(int nAction);

    int GetAction() const;
    int GetDetailId() const;
    bool IsContactLayout() const;
    bool IsGroupLayout() const;
    bool IsCopyLayout() const;
    bool IsAdditionLayout() const;
#if IF_FEATURE_FAVORITE_DIR
    bool IsDsskeyAddLayout() const;
#endif

    bool IsInputEmpty();

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
#ifdef PHOTO_SHOW
    CDetailListItem * GetPhotoListItem();
#endif
    CEditListItem* GetAutoDivertEditItem();
#if IF_FEATURE_FAVORITE_DIR
    CEditListItem* GetFavoriteIndexEditItem();
    bool UpdateFavoriteIndex();
#endif

    bool UpdateContact(int nContactId, LocalDirContactRecord * pContact);
    bool UpdateGroup(int nGroupId, DirGroupRecord * pGroup);

#ifdef PHOTO_SHOW
    void SetPhoto(yl::string & strPhoto);
#endif

    void OnRingSwitch(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void OnFocusChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void OnKeyEvent(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

protected:
    CEditListItem* GetEditItemByIndex(int nIndex);
    CComboBoxListItem* GetComboboxItemByIndex(int nIndex);
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
#ifdef PHOTO_SHOW
    // 初始化头像ComboBox
    void InitPhotoCtrl();
#endif
    void InitDivertCtrl();
#if IF_FEATURE_FAVORITE_DIR
    void InitFavoriteIndexCtrl();
#endif

    void AddEditListItem(int nDataIndex, const yl::string& strTitle, int nMaxLength
                         , bool bAutoIme, const char * pszSupportIME, const char * pszIME);
    void AddComboboxItem(int nDataIndex, const yl::string& strTitle);
    void SetComboboxItemParam(CComboBoxListItem* pComboboxItem, int nDataIndex, const yl::string& strTitle);

    void ConnectStationSlot();
    void DisconnectStationSlot();

protected:
    int m_nAction;
    int m_nDetailId;
    CRingPlayer m_ring;
};

typedef CLocalDetailLayout * CLocalDetailLayoutPtr;

#endif
