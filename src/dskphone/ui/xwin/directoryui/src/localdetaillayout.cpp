#include "localdetaillayout.h"
#include "directoryuicommon.h"
#include "moddirectoryui.h"
#include <directorydefine.h>

#include <resource/modresource.h>
#include "translateiddefine.h"
#include "xwindow/xWindowManager.h"

#include "account/include/modaccount.h"
#include "voice/include/modvoice.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "languagehelper/modlangtranslate.h"


#ifdef PHOTO_SHOW
typedef YLSmartPtr<BaseResItem> CPhotoDataPtr;
typedef YLList<CPhotoDataPtr> LIST_PHOTO;
#endif

namespace
{
// 铃声播放延迟防抖时间
#define TIME_DELAY_PLAY_RING 500
// 铃声焦点延迟播放时间
#define TIME_DELAY_FOCUS_RING 2000
// 音量的最小值
#define RING_VOLUME_MIN 0
// 音量的最大值
#define RING_VOLUME_MAX 15
#if IF_FEATURE_FAVORITE_DIR
#define FAVORITE_MAX_LENGTH 9
#endif
}

CLocalDetailLayout::CLocalDetailLayout()
    : CDetailLayout(DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    , m_nAction(DIR_ACTION_NONE)
    , m_nDetailId(knInvalidRId)
{
}

CLocalDetailLayout::~CLocalDetailLayout(void)
{
    m_ring.StopRingPlay();

    DisconnectStationSlot();
}

void CLocalDetailLayout::ConnectStationSlot()
{
    xWorkStation & objStation = g_WorkStation;

    SLOT_CONNECT(&objStation, signalFocusEvent, this, CLocalDetailLayout::OnFocusChanged);
    SLOT_CONNECT(&objStation, signalKeyEvent, this, CLocalDetailLayout::OnKeyEvent);
}

void CLocalDetailLayout::DisconnectStationSlot()
{
    xWorkStation & objStation = g_WorkStation;

    SLOT_DISCONNECT(&objStation, signalFocusEvent, this, CLocalDetailLayout::OnFocusChanged);
    SLOT_DISCONNECT(&objStation, signalKeyEvent, this, CLocalDetailLayout::OnKeyEvent);
}

bool CLocalDetailLayout::IsLocalDetailLayout(CDetailLayoutPtr pLayout)
{
    if (NULL != pLayout && DETAIL_LIST_ITEM_LAYOUT_LOCAL == pLayout->GetType())
    {
        return true;
    }

    return false;
}

void CLocalDetailLayout::Init(int nGroupId)
{
    ReleaseAllItem();

    if (IsContactLayout())
    {
        InitNameCtrl();
        InitOfficeNoCtrl();
        InitMobileNoCtrl();
        InitOtherNoCtrl();
        InitAccountCtrl();

        if (kszBlackListGroupId != nGroupId)
        {
            InitRingCtrl();

            InitGroupCtrl();
#ifdef PHOTO_SHOW
            InitPhotoCtrl();
#endif
            InitDivertCtrl();
#if IF_FEATURE_FAVORITE_DIR
            if (LocalDir_IsFavoriteEnable())
            {
                InitFavoriteIndexCtrl();
            }
#endif
        }
    }
    else if (IsGroupLayout())
    {
        InitNameCtrl();
        InitRingCtrl();
    }

    ConnectStationSlot();

    CComboBoxListItemPtr pRingItem = GetRingComboBoxItem();

    if (NULL != pRingItem)
    {
        pRingItem->SetupValueChangedSlot(this, static_cast<slotMethod>(&CLocalDetailLayout::OnRingSwitch));
    }
}

void CLocalDetailLayout::UnInit()
{
    m_ring.StopRingPlay();

    DisconnectStationSlot();

    CComboBoxListItemPtr pRingItem = GetRingComboBoxItem();

    if (NULL != pRingItem)
    {
        pRingItem->RemoveValueChangedSlot(this, static_cast<slotMethod>(&CLocalDetailLayout::OnRingSwitch));
    }

    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL != pNameEditItem)
    {
        pNameEditItem->DisconnectIMEChanged(this,
                                            static_cast<slotMethod>(&CDetailLayout::OnDetailIMEChanged));
    }

    CDetailLayout::UnInit();

    m_nAction = DIR_ACTION_NONE;
    m_nDetailId = knInvalidRId;
}

// 初始化Name
void CLocalDetailLayout::InitNameCtrl()
{
    int nIndex = m_vecListItem.size();

    AddEditListItem(nIndex, _LANG(TRID_NAME), MAX_LEN_DISPLAY_NAME, true, IME_DIAL, IME_123_Dial);
}

// 初始化OfficeNo
void CLocalDetailLayout::InitOfficeNoCtrl()
{
    int nIndex = m_vecListItem.size();

    if (IsCopyLayout())
    {
        AddComboboxItem(nIndex, _LANG(TRID_OFFICE_NUMBER));

        return;
    }

    AddEditListItem(nIndex, _LANG(TRID_OFFICE_NUMBER), MAX_LEN_PHONE_NUMBER_LONG, false, IME_DIAL, IME_123_Dial);
}

// 初始化MobileNo
void CLocalDetailLayout::InitMobileNoCtrl()
{
    int nIndex = m_vecListItem.size();

    if (IsCopyLayout())
    {
        AddComboboxItem(nIndex, _LANG(TRID_MOBILE_NUMBER));

        return;
    }

    AddEditListItem(nIndex, _LANG(TRID_MOBILE_NUMBER), MAX_LEN_PHONE_NUMBER_LONG, false, IME_DIAL, IME_123_Dial);
}

// 初始化OtherNo
void CLocalDetailLayout::InitOtherNoCtrl()
{
    int nIndex = m_vecListItem.size();

    if (IsCopyLayout())
    {
        AddComboboxItem(nIndex, _LANG(TRID_OTHER_NUMBER));

        return;
    }

    AddEditListItem(nIndex, _LANG(TRID_OTHER_NUMBER), MAX_LEN_PHONE_NUMBER_LONG, false, IME_DIAL, IME_123_Dial);
}

// 初始化帐号ComboBox
void CLocalDetailLayout::InitAccountCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CComboBoxListItem::GetComboBoxListItemType());
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);

    if (NULL == pComboBoxItem)
    {
        return;
    }

    int nIndex = m_vecListItem.size();

    SetComboboxItemParam(pComboBoxItem, nIndex, _LANG(TRID_ACCOUNT));

    m_vecListItem.push_back(pComboBoxItem);

    pComboBoxItem->AddValue(_LANG(TRID_AUTO), AUTO_ACCOUNT_IDENTIFY);

    //获取账号总个数，进行添加
    int nAccountNum = acc_AccountNum();
    char szBuffer[32];

    for (int i = 0; i < nAccountNum; ++i)
    {
        _snprintf(szBuffer, sizeof(szBuffer), "%d", (i + 1));
        szBuffer[sizeof(szBuffer) - 1] = '\0';

        yl::string strDisplayName = _LANG(TRID_LINE);

        strDisplayName = strDisplayName + " " + szBuffer;

        pComboBoxItem->AddValue(strDisplayName, i);
    }

#ifdef IF_FEATURE_PSTN
    // 获取用户信息
    ListAccounts listAccountId = acc_GetAccountList(ACCOUNT_TYPE_PSTN);
    ListAccounts::ListIterator iter = listAccountId.begin();

    for (; iter != listAccountId.end(); ++iter)
    {
        int nAccountId = *iter;
        yl::string strLine = acc_GetLabel(nAccountId);

        pComboBoxItem->AddValue(_LANG(strLine), nAccountId);
    }
#endif
}

// 初始化铃声ComboBox
void CLocalDetailLayout::InitRingCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CComboBoxListItem::GetComboBoxListItemType());
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);

    if (NULL == pComboBoxItem)
    {
        return;
    }

    int nIndex = m_vecListItem.size();

    SetComboboxItemParam(pComboBoxItem, nIndex, _LANG(TRID_RING));

    m_vecListItem.push_back(pComboBoxItem);

    //第一项设置为Auto铃声，对应为空
    pComboBoxItem->AddValue(_LANG(TRID_AUTO), TRID_AUTO);

    typedef YLSmartPtr<BaseResItem> CRingDataPtr;
    typedef YLList<CRingDataPtr> LIST_RING;
    yl::string strResType;
    LIST_RING lsRing;

    if (Res_GetResType(DIR_RES_TYPE_RING, strResType)
            && 0 == res_getResItemsByResType(lsRing, strResType.c_str()))
    {
        LIST_RING::iterator iter = lsRing.begin();

        for (; iter != lsRing.end(); iter++)
        {
            CRingDataPtr & pItem = *iter;

            if (pItem.IsEmpty())
            {
                continue;
            }

            pComboBoxItem->AddValue(pItem->resItemDispLabel, pItem->resPath);
        }
    }
}

// 初始化群组ComboBox
void CLocalDetailLayout::InitGroupCtrl()
{
    int nIndex = m_vecListItem.size();
    AddComboboxItem(nIndex, _LANG(TRID_GROUP));
}

#ifdef PHOTO_SHOW
// 初始化头像ComboBox
void CLocalDetailLayout::InitPhotoCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CDetailListItem::GetDetailListItemType());
    CDetailListItemPtr pPhotoItem = CDetailListItem::GetDetailListItem(pItem);

    if (NULL == pPhotoItem)
    {
        return;
    }

    yl::string strValue = _LANG(TRID_PHOTO_CUSTOM);
    int nIndex = m_vecListItem.size();

    pPhotoItem->SetDataIndex(nIndex);
    pPhotoItem->SetShowIndex(true);
    pPhotoItem->SetTitle(_LANG(TRID_PHOTO));
    pPhotoItem->SetValue(strValue);


    m_vecListItem.push_back(pPhotoItem);
}
#endif

void CLocalDetailLayout::InitDivertCtrl()
{
    AddEditListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_AUTO_DIVERT, _LANG(TRID_AUTO_DIVERT)
                    , MAX_LEN_PHONE_NUMBER_LONG, false, IME_DIAL, IME_123_Dial);
}

#if IF_FEATURE_FAVORITE_DIR
void CLocalDetailLayout::InitFavoriteIndexCtrl()
{
    if (!LocalDir_IsFavoriteEnable())
    {
        return;
    }

    AddEditListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_FAVORITE_INDEX, _LANG(TRID_FAVORITE_INDEX)
                    , FAVORITE_MAX_LENGTH, false, IME_Number, IME_123);
}
#endif

void CLocalDetailLayout::AddEditListItem(int nDataIndex, const yl::string& strTitle, int nMaxLength, bool bAutoIme, const char * pszSupportIME, const char * pszIME)
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditListItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditListItem)
    {
        return;
    }

    int nIndex = m_vecListItem.size();

    pEditListItem->SetDataIndex(nDataIndex);

    bool bShowIndex = false;
#if IF_DIR_DETAIL_SHOW_INDEX
    bShowIndex = true;
#endif
    pEditListItem->SetShowIndex(bShowIndex);

    pEditListItem->SetTitle(strTitle);

    pEditListItem->SetMaxLength(nMaxLength);

    m_vecListItem.push_back(pEditListItem);

    if (bAutoIme)
    {
        yl::string strSupportIME;
        yl::string strIME;

        DIRUI_GetNameIME(strSupportIME, strIME);

        pEditListItem->InstallIME(strSupportIME.c_str(), strIME.c_str());
        pEditListItem->ConnectIMEChanged(this, static_cast<slotMethod>(&CDetailLayout::OnDetailIMEChanged));
    }
    else
    {
        pEditListItem->InstallIME(pszSupportIME, pszIME);
    }
}

void CLocalDetailLayout::AddComboboxItem(int nDataIndex, const yl::string& strTitle)
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CComboBoxListItem::GetComboBoxListItemType());
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);

    if (NULL != pComboBoxItem)
    {
        SetComboboxItemParam(pComboBoxItem, nDataIndex, strTitle);

        m_vecListItem.push_back(pComboBoxItem);
    }
}

void CLocalDetailLayout::SetComboboxItemParam(CComboBoxListItem* pComboboxItem, int nDataIndex, const yl::string& strTitle)
{
    if (NULL == pComboboxItem)
    {
        return;
    }

    pComboboxItem->SetDataIndex(nDataIndex);

    bool bShowIndex = false;
#if IF_DIR_DETAIL_SHOW_INDEX
    bShowIndex = true;
#endif
    pComboboxItem->SetShowIndex(bShowIndex);

    pComboboxItem->SetTitle(strTitle);
}

void CLocalDetailLayout::SetAction(int nAction)
{
    m_nAction = nAction;
}


int CLocalDetailLayout::GetAction() const
{
    return m_nAction;
}

int CLocalDetailLayout::GetDetailId() const
{
    return m_nDetailId;
}

bool CLocalDetailLayout::IsContactLayout() const
{
    return (DIR_ACTION_LOCAL_EDIT_GROUP != m_nAction && DIR_ACTION_LOCAL_ADD_GROUP != m_nAction);
}

bool CLocalDetailLayout::IsGroupLayout() const
{
    return (DIR_ACTION_LOCAL_EDIT_GROUP == m_nAction || DIR_ACTION_LOCAL_ADD_GROUP == m_nAction);
}

bool CLocalDetailLayout::IsCopyLayout() const
{
    return (DIR_ACTION_LOCAL_COPY_TO_CONTACTS == m_nAction
            || DIR_ACTION_LOCAL_COPY_TO_BLACKLIST == m_nAction);
}

bool CLocalDetailLayout::IsAdditionLayout() const
{
    if (DIR_ACTION_LOCAL_EDIT_CONTACT == m_nAction || DIR_ACTION_LOCAL_EDIT_GROUP == m_nAction)
    {
        return false;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (IsDsskeyAddLayout())
    {
        return true;
    }
#endif

    return (DIR_ACTION_LOCAL_COPY_TO_CONTACTS != m_nAction
            && DIR_ACTION_LOCAL_COPY_TO_BLACKLIST != m_nAction);
}

#if IF_FEATURE_FAVORITE_DIR
bool CLocalDetailLayout::IsDsskeyAddLayout() const
{
    return DIR_ACTION_LOCAL_DSSKEY_ADD_CONTACT == m_nAction;
}
#endif

CEditListItem * CLocalDetailLayout::GetNameEditItem()
{
    int nItemIndex = -1;

    if (IsContactLayout())
    {
        nItemIndex = LOCAL_CONTACT_DETAIL_ITEM_INDEX_NAME;
    }
    else
    {
        nItemIndex = LOCAL_GROUP_DETAIL_ITEM_INDEX_NAME;
    }

    return GetEditItemByIndex(nItemIndex);
}

CEditListItem * CLocalDetailLayout::GetOfficeNoEditItem()
{
    return GetEditItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OFFICE_NO);
}

CEditListItem * CLocalDetailLayout::GetMobileNoEditItem()
{
    return GetEditItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO);
}

CEditListItem * CLocalDetailLayout::GetOtherNoEditItem()
{
    return GetEditItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO);
}

CComboBoxListItem * CLocalDetailLayout::GetOfficeNoComboBoxItem()
{
    return GetComboboxItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OFFICE_NO);
}

CComboBoxListItem * CLocalDetailLayout::GetMobileNoComboBoxItem()
{
    return GetComboboxItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO);
}

CComboBoxListItem * CLocalDetailLayout::GetOtherNoComboBoxItem()
{
    return GetComboboxItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO);
}

CComboBoxListItem * CLocalDetailLayout::GetAccountComboBoxItem()
{
    return GetComboboxItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_ACCOUNT);
}

CComboBoxListItem * CLocalDetailLayout::GetRingComboBoxItem()
{
    int nItemIndex = -1;

    if (IsContactLayout())
    {
        nItemIndex = LOCAL_CONTACT_DETAIL_ITEM_INDEX_RING;
    }
    else
    {
        nItemIndex = LOCAL_GROUP_DETAIL_ITEM_INDEX_RING;
    }

    return GetComboboxItemByIndex(nItemIndex);
}

CComboBoxListItem * CLocalDetailLayout::GetGroupComboBoxItem()
{
    return GetComboboxItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_GROUP);
}

#ifdef PHOTO_SHOW
CDetailListItem * CLocalDetailLayout::GetPhotoListItem()
{
    if (IsGroupLayout())
    {
        return NULL;
    }

    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_PHOTO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_ITEM == pDetailItem->GetDetailSubType())
    {
        return static_cast<CDetailListItem *>(pDetailItem);
    }

    return NULL;
}
#endif

CEditListItem* CLocalDetailLayout::GetAutoDivertEditItem()
{
    return GetEditItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_AUTO_DIVERT);
}

#if IF_FEATURE_FAVORITE_DIR
CEditListItem* CLocalDetailLayout::GetFavoriteIndexEditItem()
{
    return GetEditItemByIndex(LOCAL_CONTACT_DETAIL_ITEM_INDEX_FAVORITE_INDEX);
}

bool CLocalDetailLayout::UpdateFavoriteIndex()
{
    CEditListItemPtr pFavoriteItem = GetFavoriteIndexEditItem();
    if (NULL == pFavoriteItem)
    {
        return false;
    }

    yl::string strIndex = "";
    int nIndex = LocalDir_GetFavoriteIndex();
    if (nIndex > 0)
    {
        strIndex = commonAPI_FormatString("%d", nIndex);
    }

    pFavoriteItem->SetValue(strIndex);
    return true;
}
#endif

CEditListItem* CLocalDetailLayout::GetEditItemByIndex(int nIndex)
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(nIndex);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CComboBoxListItem* CLocalDetailLayout::GetComboboxItemByIndex(int nIndex)
{
    if (IsGroupLayout())
    {
        return NULL;
    }

    CDetailListItemBasePtr pDetailItem = GetDetailListItem(nIndex);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return static_cast<CComboBoxListItem *>(pDetailItem);
    }

    return NULL;
}

void CLocalDetailLayout::ReturnAllEdit()
{
    //设置处于预输入状态的字符也有效
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL != pNameEditItem && pNameEditItem->IsItemFocus())
    {
        pNameEditItem->ReturnAllInput();

        return;
    }

    CEditListItemPtr pOfficeNoEditItem = GetOfficeNoEditItem();

    if (NULL != pOfficeNoEditItem && pOfficeNoEditItem->IsItemFocus())
    {
        pOfficeNoEditItem->ReturnAllInput();

        return;
    }

    CEditListItemPtr pMobileNoEditItem = GetMobileNoEditItem();

    if (NULL != pMobileNoEditItem && pMobileNoEditItem->IsItemFocus())
    {
        pMobileNoEditItem->ReturnAllInput();

        return;
    }

    CEditListItemPtr pOtherNoEditItem = GetOtherNoEditItem();

    if (NULL != pOtherNoEditItem && pOtherNoEditItem->IsItemFocus())
    {
        pOtherNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pAutoDivertEditItem = GetAutoDivertEditItem();

    if (NULL != pAutoDivertEditItem)
    {
        pAutoDivertEditItem->ReturnAllInput();
    }

#if IF_FEATURE_FAVORITE_DIR
    if (LocalDir_IsFavoriteEnable())
    {
        CEditListItemPtr pFavoriteItem = GetFavoriteIndexEditItem();

        if (NULL != pFavoriteItem)
        {
            pFavoriteItem->ReturnAllInput();
        }
    }
#endif
}

bool CLocalDetailLayout::UpdateContact(int nContactId, LocalDirContactRecord * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    if (nContactId == m_nDetailId)
    {
        return true;
    }

    m_nDetailId = nContactId;

    CEditListItemPtr pNameItem = GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->SetValue(pContact->GetDisplayName());
    }

    CEditListItemPtr pOfficeNoItem = GetOfficeNoEditItem();

    if (NULL != pOfficeNoItem)
    {
        yl::string strOfficNo;

        pContact->GetNumberByName(kszOfficeNumber, strOfficNo);
        pOfficeNoItem->SetValue(strOfficNo);
    }

    CEditListItemPtr pMobileNoItem = GetMobileNoEditItem();

    if (NULL != pMobileNoItem)
    {
        yl::string strMobileNo;

        pContact->GetNumberByName(kszMobileNumber, strMobileNo);
        pMobileNoItem->SetValue(strMobileNo);
    }

    CEditListItemPtr pOtherNoItem = GetOtherNoEditItem();

    if (NULL != pOtherNoItem)
    {
        yl::string strOtherNo;

        pContact->GetNumberByName(kszOtherNumber, strOtherNo);
        pOtherNoItem->SetValue(strOtherNo);
    }

    CComboBoxListItemPtr pAccountItem = GetAccountComboBoxItem();

    if (NULL != pAccountItem)
    {
        pAccountItem->SetItemData(pContact->m_nLine);
    }

    if (kszBlackListGroupId == pContact->m_nParentId)
    {
        return true;
    }

    CComboBoxListItemPtr pRingItem = GetRingComboBoxItem();

    if (NULL != pRingItem)
    {
        if (TRID_AUTO == pContact->m_strRing)
        {
            pRingItem->SetItemData("");
        }
        else
        {
            pRingItem->SetItemData(pContact->m_strRing);
        }
    }

#ifdef PHOTO_SHOW
    SetPhoto(pContact->m_strDefaultPhoto);
#endif

    CEditListItemPtr pAutoDivertEditItem = GetAutoDivertEditItem();
    if (NULL != pAutoDivertEditItem)
    {
        pAutoDivertEditItem->SetValue(pContact->m_strDivert);
    }

#if IF_FEATURE_FAVORITE_DIR
    CEditListItemPtr pFavoriteItem = GetFavoriteIndexEditItem();
    if (NULL != pFavoriteItem)
    {
        yl::string strIndex = "";
        if (pContact->m_nFavoriteIndex > 0)
        {
            strIndex = commonAPI_FormatString("%d", pContact->m_nFavoriteIndex);
        }

        pFavoriteItem->SetValue(strIndex);
    }
#endif

    return true;
}

bool CLocalDetailLayout::UpdateGroup(int nGroupId, DirGroupRecord * pGroup)
{
    if (NULL == pGroup)
    {
        return false;
    }

    if (nGroupId == m_nDetailId)
    {
        return true;
    }

    m_nDetailId = nGroupId;

    CEditListItemPtr pNameItem = GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->SetValue(pGroup->GetDisplayName());
    }

    CComboBoxListItemPtr pRingItem = GetRingComboBoxItem();

    if (NULL != pRingItem)
    {
        if (TRID_AUTO == pGroup->m_strRing)
        {
            pRingItem->SetItemData("");
        }
        else
        {
            pRingItem->SetItemData(pGroup->m_strRing);
        }
    }

    return true;
}

#ifdef PHOTO_SHOW
void CLocalDetailLayout::SetPhoto(yl::string & strPhoto)
{
    CDetailListItemPtr pPhotoItem = GetPhotoListItem();

    if (NULL == pPhotoItem)
    {
        return;
    }

    yl::string strRealPath = strPhoto;
    yl::string strPhotoName;

    if (!strRealPath.empty())
    {
        res_toRealPath(strRealPath, PHFILE_TYPE_CONTACT_IMAGE);
    }

    if (Res_GetNameByPath(DIR_RES_TYPE_PHOTO, strRealPath, strPhotoName))
    {
        pPhotoItem->SetValue(strPhotoName);
        pPhotoItem->SetItemData(strPhoto);
    }
    else
    {
        pPhotoItem->SetValue(_LANG(TRID_PHOTO_CUSTOM));
        pPhotoItem->SetItemData("");
    }
}
#endif

bool CLocalDetailLayout::IsInputEmpty()
{
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL == pNameEditItem)
    {
        return true;
    }

    yl::string strName = pNameEditItem->GetValue().trim_both();

    if (IsCopyLayout())
    {
        CComboBoxListItemPtr pOfficeNoItem = GetOfficeNoComboBoxItem();

        if (NULL == pOfficeNoItem || !pOfficeNoItem->isVisible())
        {
            return strName.empty();
        }

        yl::string strOfficeNo = pOfficeNoItem->GetItemStringData();
        CComboBoxListItemPtr pMobileNoItem = GetMobileNoComboBoxItem();

        if (NULL == pMobileNoItem || !pMobileNoItem->isVisible())
        {
            return (strName.empty() && strOfficeNo.empty());
        }

        yl::string strMobileNo = pMobileNoItem->GetItemStringData();
        CComboBoxListItemPtr pOtherNoItem = GetOtherNoComboBoxItem();

        if (NULL == pOtherNoItem || !pOtherNoItem->isVisible())
        {
            return (strName.empty() && strOfficeNo.empty() && strMobileNo.empty());
        }

        yl::string strOtherNo = pOtherNoItem->GetItemStringData();

        return (strName.empty() && strOfficeNo.empty() && strMobileNo.empty() && strOtherNo.empty());
    }

    CEditListItemPtr pOfficeNoItem = GetOfficeNoEditItem();

    if (NULL == pOfficeNoItem || !pOfficeNoItem->isVisible())
    {
        return strName.empty();
    }

    yl::string strOfficeNo = pOfficeNoItem->GetValue().trim_both();
    CEditListItemPtr pMobileNoItem = GetMobileNoEditItem();

    if (NULL == pMobileNoItem || !pMobileNoItem->isVisible())
    {
        return (strName.empty() && strOfficeNo.empty());
    }

    yl::string strMobileNo = pMobileNoItem->GetValue().trim_both();
    CEditListItemPtr pOtherNoItem = GetOtherNoEditItem();

    if (NULL == pOtherNoItem || !pOtherNoItem->isVisible())
    {
        return (strName.empty() && strOfficeNo.empty() && strMobileNo.empty());
    }

    yl::string strOtherNo = pOtherNoItem->GetValue().trim_both();

    return (strName.empty() && strOfficeNo.empty() && strMobileNo.empty() && strOtherNo.empty());
}

void CLocalDetailLayout::OnRingSwitch(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                      int nDataBytes, LPVOID pData)
{
    CComboBoxListItemPtr pRingItem = GetRingComboBoxItem();

    if (NULL == pRingItem || ((!pRingItem->hasFocus()) && (!pRingItem->containFocus())))
    {
        return;
    }

    m_ring.StopRingPlay();

    yl::string strValue = pRingItem->GetItemStringData();

    if (strValue != TRID_AUTO)
    {
        m_ring.StartRingPlay(strValue, TIME_DELAY_PLAY_RING);
    }
}

void CLocalDetailLayout::OnFocusChanged(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                        int nDataBytes, LPVOID pData)
{
    CComboBoxListItemPtr pRingItem = GetRingComboBoxItem();

    if (NULL == pRingItem)
    {
        return;
    }

    m_ring.StopRingPlay();

    if (pRingItem->hasFocus() || pRingItem->containFocus())
    {
        yl::string strValue = pRingItem->GetItemStringData();

        if (strValue != TRID_AUTO)
        {
            m_ring.StartRingPlay(strValue, TIME_DELAY_FOCUS_RING);
        }
    }
}

void CLocalDetailLayout::OnKeyEvent(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                    LPVOID pData)
{
    int nKey = (int)wParam;

    // 提前判断Key值，防止Softkey-Back场景
    if (PHONE_KEY_VOLUME_INCREASE != nKey && PHONE_KEY_VOLUME_DECREASE != nKey)
    {
        return;
    }

    CComboBoxListItemPtr pRingItem = GetRingComboBoxItem();
    bool bPress = (1 == lParam);

    if (!bPress || NULL == pRingItem || ((!pRingItem->hasFocus()) && (!pRingItem->containFocus())))
    {
        return;
    }

    if (PHONE_KEY_VOLUME_INCREASE == nKey)
    {
        int nVolume = voice_GetVolume(VT_AUTO);

        ++nVolume;

        if (nVolume <= RING_VOLUME_MAX)
        {
            voice_SetVolume(nVolume, VT_AUTO, true);
        }
    }
    else if (PHONE_KEY_VOLUME_DECREASE == nKey)
    {
        int nVolume = voice_GetVolume(VT_AUTO);

        --nVolume;

        if (nVolume >= RING_VOLUME_MIN)
        {
            voice_SetVolume(nVolume, VT_AUTO, true);
        }
    }
}
