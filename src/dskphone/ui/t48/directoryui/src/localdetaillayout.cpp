#include <QtGui>
#include "baseui/comboboxlistitem.h"
#include "baseui/editlistitem.h"
#include "uimanager/basedialog.h"

#include "localdetaillayout.h"

#include "directorycommon.h"
#include "translateiddefine.h"
#include "moddirectoryui.h"
#include "uicommon.h"
#include "account/include/modaccount.h"
#include "commonunits/modcommonunits.h"
#include "commonunits/commonunits_def.h"
#include "inputmethod/inputmethod.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/languagehelper.h"
#include <resource/modresource.h>


typedef YLSmartPtr<BaseResItem> CResDataPtr;
typedef YLList<CResDataPtr> LIST_RESOURCE;


CLocalDetailLayout::CLocalDetailLayout(CBaseDialog * pDlg, bool bNewContact, bool bCopyLayout)
    : CDetailListItemLayout(DETAIL_LIST_ITEM_LAYOUT_LOCAL, pDlg)
    , m_bNewContact(bNewContact)
    , m_bCopyLayout(bCopyLayout)
    , m_nDetailId(knInvalidRId)
#if IF_FEATURE_FAVORITE_DIR
    , m_bIsDsskeyAdd(false)
#endif
{
}

CLocalDetailLayout::~CLocalDetailLayout(void)
{
}

bool CLocalDetailLayout::IsLocalDetailLayout(CDetailListItemLayoutPtr pLayout)
{
    if (NULL != pLayout && DETAIL_LIST_ITEM_LAYOUT_LOCAL == pLayout->GetType())
    {
        return true;
    }

    return false;
}

void CLocalDetailLayout::Init()
{
    InitGroupCtrl();
    InitNameCtrl();
    InitOfficeNoCtrl();
    InitMobileNoCtrl();
    InitOtherNoCtrl();
    InitAccountCtrl();
    InitRingCtrl();
    InitPhotoCtrl();
    InitAutoDivertCtrl();
#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        InitFavoriteIndexCtrl();
    }
#endif
}

// 初始化Name
void CLocalDetailLayout::InitNameCtrl()
{
    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_NAME));
    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);

        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);

        if (FullKeyboardEnabled())
        {
            pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

// 初始化OfficeNo
void CLocalDetailLayout::InitOfficeNoCtrl()
{
    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_OFFICE_NUMBER));
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_OFFICE_NUMBER));

    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

// 初始化MobileNo
void CLocalDetailLayout::InitMobileNoCtrl()
{
    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_MOBILE_NUMBER));
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_MOBILE_NUMBER));
    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

// 初始化OtherNo
void CLocalDetailLayout::InitOtherNoCtrl()
{
    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_OTHER_NUMBER));
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_OTHER_NUMBER));
    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

// 初始化帐号ComboBox
void CLocalDetailLayout::InitAccountCtrl()
{
    CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_ACCOUNT));
    m_vecListItem.push_back(pComboBoxItem);

    qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

    if (NULL == pComboBox)
    {
        return;
    }

    if (NULL != m_pDlg)
    {
        pComboBox->installEventFilter(m_pDlg);
    }

    pComboBox->addItem(LANG_TRANSLATE(TRID_AUTO), QVariant::fromValue<int>(AUTO_ACCOUNT_IDENTIFY));

    //获取账号总个数，进行添加
    int nAccountNum = acc_AccountNum();

    for (int i = 0; i < nAccountNum; ++i)
    {
        QString strIndex;
        QString strDisplayName = LANG_TRANSLATE(TRID_LINE) + " " + strIndex.setNum(i + 1);

        pComboBox->addItem(strDisplayName, QVariant::fromValue<int>(i));
    }
}

// 初始化铃声ComboBox
void CLocalDetailLayout::InitRingCtrl()
{
    CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_RING));
    m_vecListItem.push_back(pComboBoxItem);

    qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

    if (NULL == pComboBox)
    {
        return;
    }

    if (NULL != m_pDlg)
    {
        pComboBox->installEventFilter(m_pDlg);
    }

    //第一项设置为Auto铃声，对应为空
    yl::string strAuto = TRID_AUTO;
    pComboBox->addItem(LANG_TRANSLATE(TRID_AUTO), QVariant::fromValue<yl::string>(strAuto));

    yl::string strResType;
    LIST_RESOURCE lsRes;

    if (Res_GetResType(DIR_RES_TYPE_RING, strResType)
            && 0 == res_getResItemsByResType(lsRes, strResType.c_str()))
    {
        LIST_RESOURCE::iterator iter = lsRes.begin();

        for (; iter != lsRes.end(); iter++)
        {
            CResDataPtr & pItem = *iter;

            if (pItem.IsEmpty())
            {
                continue;
            }

            pComboBox->addItem(toQString(pItem->resItemDispLabel),
                               QVariant::fromValue<yl::string>(pItem->resPath));
        }
    }
}

// 初始化群组ComboBox
void CLocalDetailLayout::InitGroupCtrl()
{
    CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_GROUP));
    m_vecListItem.push_back(pComboBoxItem);

    qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

    if (NULL != pComboBox && NULL != m_pDlg)
    {
        pComboBox->installEventFilter(m_pDlg);
    }
}

// 初始化头像ComboBox
void CLocalDetailLayout::InitPhotoCtrl()
{
    CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_PHOTO));
    m_vecListItem.push_back(pComboBoxItem);

    qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

    if (NULL == pComboBox)
    {
        return;
    }

    if (NULL != m_pDlg)
    {
        pComboBox->installEventFilter(m_pDlg);
    }

    yl::string strResType;
    LIST_RESOURCE lsRes;

    if (Res_GetResType(DIR_RES_TYPE_PHOTO, strResType)
            && 0 == res_getResItemsByResType(lsRes, strResType.c_str(), true))
    {
        LIST_RESOURCE::iterator iter = lsRes.begin();

        for (; iter != lsRes.end(); iter++)
        {
            CResDataPtr & pItem = *iter;

            if (pItem.IsEmpty())
            {
                continue;
            }

            QString strName = LANG_TRANSLATE(DM_GetContactImageName(pItem->resItemDispLabel).c_str());

            pComboBox->addItem(strName, QVariant::fromValue<yl::string>(pItem->resPath));
        }
    }
}

void CLocalDetailLayout::InitAutoDivertCtrl()
{
    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_AUTO_DIVERT));

    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

#if IF_FEATURE_FAVORITE_DIR
void CLocalDetailLayout::InitFavoriteIndexCtrl()
{
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) != 1)
    {
        return;
    }

    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_FAVORITE_INDEX));
    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        pEditListItem->SetInputInfo(EDIT_TYPE_LOCAL_FAVORITE_INDEX, IME_Number, IME_123);

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}
#endif

CEditListItem * CLocalDetailLayout::GetNameEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CLocalDetailLayout::GetOfficeNoEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OFFICE_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CLocalDetailLayout::GetMobileNoEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CLocalDetailLayout::GetOtherNoEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CLocalDetailLayout::GetOfficeNoComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OFFICE_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CLocalDetailLayout::GetMobileNoComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CLocalDetailLayout::GetOtherNoComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CLocalDetailLayout::GetAccountComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_ACCOUNT);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CLocalDetailLayout::GetRingComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_RING);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CLocalDetailLayout::GetGroupComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_GROUP);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CLocalDetailLayout::GetPhotoComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_PHOTO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CLocalDetailLayout::GetAutoDivertItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_AUTO_DIVERT);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

#if IF_FEATURE_FAVORITE_DIR
CEditListItem * CLocalDetailLayout::GetFavoriteIndexEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(
            LOCAL_CONTACT_DETAIL_ITEM_INDEX_FAVORITE_INDEX);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}
#endif

bool CLocalDetailLayout::IsCopyLayout() const
{
    return m_bCopyLayout;
}

bool CLocalDetailLayout::IsNewLayout() const
{
    return m_bNewContact;
}

void CLocalDetailLayout::ReturnAllEdit()
{
    //设置处于预输入状态的字符也有效
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL != pNameEditItem)
    {
        pNameEditItem->ReturnAllInput();
    }

    CEditListItemPtr pOfficeNoEditItem = GetOfficeNoEditItem();

    if (NULL != pOfficeNoEditItem)
    {
        pOfficeNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pMobileNoEditItem = GetMobileNoEditItem();

    if (NULL != pMobileNoEditItem)
    {
        pMobileNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pOtherNoEditItem = GetOtherNoEditItem();

    if (NULL != pOtherNoEditItem)
    {
        pOtherNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pAutoDivertEditItem = GetAutoDivertItem();

    if (NULL != pAutoDivertEditItem)
    {
        pAutoDivertEditItem->ReturnAllInput();
    }

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        CEditListItemPtr pFavoriteIndexEditItem = GetFavoriteIndexEditItem();

        if (NULL != pFavoriteIndexEditItem)
        {
            pFavoriteIndexEditItem->ReturnAllInput();
        }
    }
#endif
}

bool CLocalDetailLayout::UpdateDetail(int nContactId, LocalDirContactRecord * pContact)
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
        pNameItem->SetValue(toQString(pContact->GetDisplayName(), true));
    }

    CEditListItemPtr pOfficeNoItem = GetOfficeNoEditItem();

    if (NULL != pOfficeNoItem)
    {
        yl::string strOfficNo;

        pContact->GetNumberByName(kszOfficeNumber, strOfficNo);
        pOfficeNoItem->SetValue(toQString(strOfficNo, true));
    }

    CEditListItemPtr pMobileNoItem = GetMobileNoEditItem();

    if (NULL != pMobileNoItem)
    {
        yl::string strMobileNo;

        pContact->GetNumberByName(kszMobileNumber, strMobileNo);
        pMobileNoItem->SetValue(toQString(strMobileNo, true));
    }

    CEditListItemPtr pOtherNoItem = GetOtherNoEditItem();

    if (NULL != pOtherNoItem)
    {
        yl::string strOtherNo;

        pContact->GetNumberByName(kszOtherNumber, strOtherNo);
        pOtherNoItem->SetValue(toQString(strOtherNo, true));
    }

    CComboBoxListItemPtr pAccountItem = GetAccountComboBoxItem();

    if (NULL != pAccountItem)
    {
        if (AUTO_ACCOUNT_IDENTIFY == pContact->m_nLine)
        {
            pAccountItem->SetValue(LANG_TRANSLATE(TRID_AUTO));
        }
        else
        {
            QString strIndex;
            QString strLine = LANG_TRANSLATE(TRID_LINE) + " " + strIndex.setNum(pContact->m_nLine + 1);

            pAccountItem->SetValue(strLine);
        }
    }

    CComboBoxListItemPtr pGroupItem = GetGroupComboBoxItem();

    if (NULL != pGroupItem)
    {
        pGroupItem->SetItemData(pContact->m_nParentId);
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

    CComboBoxListItemPtr pPhotoItem = GetPhotoComboBoxItem();

    if (NULL != pPhotoItem)
    {
        yl::string strImagePath = pContact->m_strDefaultPhoto;

        if (!res_toRealPath(strImagePath, PHFILE_TYPE_CONTACT_IMAGE) || strImagePath.empty())
        {
            strImagePath = PIC_CONTACTS_IMAGE_DEFAULT;
        }

        pPhotoItem->SetItemData(strImagePath);
    }

    CEditListItemPtr pAutoDivertItem = GetAutoDivertItem();

    if (NULL != pAutoDivertItem)
    {
        pAutoDivertItem->SetValue(toQString(pContact->m_strDivert, true));
    }

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        CEditListItemPtr pFavoriteItem = GetFavoriteIndexEditItem();

        if (NULL != pFavoriteItem)
        {
            yl::string strIndex = "";
            if (pContact->m_nFavoriteIndex > 0)
            {
                strIndex = commonAPI_FormatString("%d", pContact->m_nFavoriteIndex);
            }

            pFavoriteItem->SetValue(toQString(strIndex, true));
        }
    }
#endif

    return true;
}

bool CLocalDetailLayout::IsInputEmpty()
{
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL == pNameEditItem)
    {
        return true;
    }

    QString strName = pNameEditItem->GetValue().trimmed();

    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pOfficeNoItem = GetOfficeNoComboBoxItem();

        if (NULL == pOfficeNoItem || !pOfficeNoItem->isVisible())
        {
            return strName.isEmpty();
        }

        yl::string strOfficeNo = pOfficeNoItem->GetItemStringData();
        CComboBoxListItemPtr pMobileNoItem = GetMobileNoComboBoxItem();

        if (NULL == pMobileNoItem || !pMobileNoItem->isVisible())
        {
            return (strName.isEmpty() && strOfficeNo.empty());
        }

        yl::string strMobileNo = pMobileNoItem->GetItemStringData();
        CComboBoxListItemPtr pOtherNoItem = GetOtherNoComboBoxItem();

        if (NULL == pOtherNoItem || !pOtherNoItem->isVisible())
        {
            return (strName.isEmpty() && strOfficeNo.empty() && strMobileNo.empty());
        }

        yl::string strOtherNo = pOtherNoItem->GetItemStringData();

        return (strName.isEmpty() && strOfficeNo.empty() && strMobileNo.empty() && strOtherNo.empty());
    }

    CEditListItemPtr pOfficeNoItem = GetOfficeNoEditItem();

    if (NULL == pOfficeNoItem || !pOfficeNoItem->isVisible())
    {
        return strName.isEmpty();
    }

    QString strOfficeNo = pOfficeNoItem->GetValue().trimmed();
    CEditListItemPtr pMobileNoItem = GetMobileNoEditItem();

    if (NULL == pMobileNoItem || !pMobileNoItem->isVisible())
    {
        return (strName.isEmpty() && strOfficeNo.isEmpty());
    }

    QString strMobileNo = pMobileNoItem->GetValue().trimmed();
    CEditListItemPtr pOtherNoItem = GetOtherNoEditItem();

    if (NULL == pOtherNoItem || !pOtherNoItem->isVisible())
    {
        return (strName.isEmpty() && strOfficeNo.isEmpty() && strMobileNo.isEmpty());
    }

    QString strOtherNo = pOtherNoItem->GetValue().trimmed();

    return (strName.isEmpty() && strOfficeNo.isEmpty() && strMobileNo.isEmpty()
            && strOtherNo.isEmpty());
}

#if IF_FEATURE_FAVORITE_DIR
bool CLocalDetailLayout::UpdateFavoriteIndex()
{
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) != 1)
    {
        return false;
    }

    if (!m_bIsDsskeyAdd)
    {
        return false;
    }
    CEditListItemPtr pFavoriteItem = GetFavoriteIndexEditItem();

    if (NULL != pFavoriteItem)
    {
        yl::string strIndex = "";
        int nIndex = LocalDir_GetFavoriteIndex();
        if (nIndex > 0)
        {
            strIndex = commonAPI_FormatString("%d", nIndex);
        }

        pFavoriteItem->SetValue(toQString(strIndex, true));
    }
    return true;
}
#endif
