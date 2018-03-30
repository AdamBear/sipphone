#include <QtGui>
#include "uicommon.h"
#include "localcontactuihelper.h"
#include "localcontactcontroler.h"
#include "directorymgr.h"
#include "groupselector.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "moddirectoryui.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "dlgdirnew.h"
#include "commonunits/commonunits_def.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/editlistitem.h"
#include "baseui/framelist.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include <resource/modresource.h>
#include "baseui/t4xpicpath.h"


CLocalContactUIHelper::CLocalContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_LOCAL)
    , m_pDetailLayout(NULL)
    , m_bDetailEditing(false)
{
}

CLocalContactUIHelper::~CLocalContactUIHelper()
{
}

CLocalContactUIHelper * CLocalContactUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_LOCAL != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CLocalContactUIHelper *)pUIHelper.Get();
}

void CLocalContactUIHelper::GetGroupDisplayName(int nGroupId, const yl::string & strGroupName,
        QString & strDisplayName)
{
    if (kszBlackListGroupId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_BLACKLIST);
    }
    else if (kszAllContactId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_ALL_CONTACTS);
    }
    else
    {
        strDisplayName = toQString(strGroupName, true);
    }
}

void CLocalContactUIHelper::SetDetailLayout(CLocalDetailLayoutPtr pDetailLayout)
{
    m_pDetailLayout = pDetailLayout;

    if (NULL == m_pDetailLayout)
    {
        return;
    }

    DIRUI_INFO("SetDetailLayout NewLayout:%d CopyLayout:%d", (m_pDetailLayout->IsNewLayout() ? 1 : 0),
               (m_pDetailLayout->IsCopyLayout() ? 1 : 0));

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
            && IsFromDsskeyAdd())
    {
        m_pDetailLayout->UpdateFavoriteIndex();
    }
#endif

    if (m_pDetailLayout->IsNewLayout())
    {
        if (m_pDetailLayout->IsCopyLayout())
        {
            InitCopyLayout();
        }
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty() || DIR_UI_STATE_DETAIL != pDelegate->GetState())
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return;
    }

    CComboBoxListItemPtr pGroupComboBoxItem = m_pDetailLayout->GetGroupComboBoxItem();

    if (NULL == pGroupComboBoxItem)
    {
        return;
    }

    // 初始化普通群组
    VEC_CONTACT_ID vecGroupId;
    yl::string strGroupName;
    QString strGroupShowName;

    pLocalContact->GetMenuGroupId(vecGroupId);

    int nSize = vecGroupId.size();

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                && nGroupId == kszFavoriteGroupId)
        {
            continue;
        }
#endif

        if (!pLocalContact->GetGroupName(nGroupId, strGroupName))
        {
            continue;
        }

        GetGroupDisplayName(nGroupId, strGroupName, strGroupShowName);

        pGroupComboBoxItem->AddValue(strGroupShowName, QVariant::fromValue(nGroupId));
    }

    pLocalContact->GetGroupName(kszBlackListGroupId, strGroupName);
    strGroupShowName = LANG_TRANSLATE(strGroupName.c_str());

    pGroupComboBoxItem->AddValue(strGroupShowName, QVariant::fromValue((int)kszBlackListGroupId));
}

void CLocalContactUIHelper::InitCopyLayout()
{
    if (NULL == m_pDetailLayout)
    {
        return;
    }

    int nSrcDirType = GetAttributeType();
    yl::string strName;
    yl::string strNumber1;
    yl::string strNumber2;
    yl::string strNumber3;
    YLList<yl::string> listSelectNum;

    GetAttributeByName(0, ATTR_NAME, strName);

    if (DIRECTORY_TYPE_BLUETOOTH == nSrcDirType)
    {
        GetAttributeByName(0, ATTR_OFFICENUMBER, strNumber1);
        GetAttributeByName(0, ATTR_MOBILENUMBER, strNumber2);
        GetAttributeByName(0, ATTR_OTHERNUMBER, strNumber3);
    }
    else if (DIRECTORY_TYPE_LDAP == nSrcDirType)
    {
        int iAttributeSize = GetAttributeSize();
        for (int i = 0; i < iAttributeSize - 1; ++i)
        {
            yl::string strNumber = "";

            GetAttributeByName(i, ATTR_NUMBER, strNumber);
            if (!strNumber.empty())
            {
                listSelectNum.push_back(strNumber);

                if (strNumber1.empty())
                {
                    strNumber1 = strNumber;
                }
                else if (strNumber2.empty())
                {
                    strNumber2 = strNumber;
                }
                else if (strNumber3.empty())
                {
                    strNumber3 = strNumber;
                }
            }
        }
    }
    else
    {
        GetAttributeByName(0, ATTR_NUMBER, strNumber1);
        GetAttributeByName(1, ATTR_NUMBER, strNumber2);
    }


    DIRUI_INFO("InitCopyLayout Name:%s Number1:%s Number2:%s Number3:%s",
               strName.c_str(), strNumber1.c_str(), strNumber2.c_str(), strNumber3.c_str());

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL != pNameItem)
    {
        //int nSrcDirType = GetAttributeType();

        // 兼容ldap名字为空的情况
        if (DIRECTORY_TYPE_LDAP == nSrcDirType && strName.empty())
        {
            QString strNameUnknown = LANG_TRANSLATE(TRID_NAME_UNKNOWN);

            strNameUnknown = LANG_PARSE(strNameUnknown);
            pNameItem->SetValue(strNameUnknown);
        }
        else if (CALL_LOG_TYPE_LOCAL == nSrcDirType)
        {
            QString strTransName;

            TransBsftFACText(strName, strTransName);

            strTransName = LANG_PARSE(strTransName);
            pNameItem->SetValue(strTransName);
        }
        else
        {
            pNameItem->SetValue(toQString(strName, true));
        }
    }

    // 本地联系人复制界面赋值
    CComboBoxListItemPtr pOfficeNoItem = m_pDetailLayout->GetOfficeNoComboBoxItem();
    CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoComboBoxItem();
    CComboBoxListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNoComboBoxItem();

    if (NULL == pOfficeNoItem || NULL == pMobileNoItem || NULL == pOtherNoItem)
    {
        return;
    }

    if (DIRECTORY_TYPE_LDAP == nSrcDirType)
    {
        FillNumberComboBoxList(pOfficeNoItem, listSelectNum);
        pOfficeNoItem->SetValue(toQString(strNumber1));

        FillNumberComboBoxList(pMobileNoItem, listSelectNum);
        pMobileNoItem->SetValue(toQString(strNumber2));

        FillNumberComboBoxList(pOtherNoItem, listSelectNum);
        pOtherNoItem->SetValue(toQString(strNumber3));
    }
    else
    {
        FillNumberComboBoxList(pOfficeNoItem, strNumber1, strNumber2, strNumber3);
        pOfficeNoItem->SetValue(toQString(strNumber1));

        FillNumberComboBoxList(pMobileNoItem, strNumber1, strNumber2, strNumber3);
        pMobileNoItem->SetValue(toQString(strNumber2));

        FillNumberComboBoxList(pOtherNoItem, strNumber1, strNumber2, strNumber3);
        pOtherNoItem->SetValue(toQString(strNumber3));
    }
}

void CLocalContactUIHelper::SetInputFocus(const char * pszAttr)
{
    if (NULL == m_pDetailLayout || NULL == pszAttr)
    {
        return;
    }

    // 添加联系人界面没有FrameList
    if (NULL != m_pFrameList)
    {
        CListItemPtr pFirstItem = m_pFrameList->GetItemByIndex(0);

        if (NULL == pFirstItem || 0 != pFirstItem->GetDataIndex())
        {
            CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

            if (!pDelegate.IsEmpty())
            {
                pDelegate->SetPageDataIndex(0);
                pDelegate->SetFocusIndex(0);
                pDelegate->RefreshFrameList();
            }
        }
    }

    if (0 == strcmp(pszAttr, ATTR_NAME))
    {
        CEditListItem * pNameItem = m_pDetailLayout->GetNameEditItem();

        if (NULL != pNameItem)
        {
            setCurrentPageForItem(pNameItem);
            pNameItem->FocusItem(true);
        }
        return;
    }

    if (0 == strcmp(pszAttr, ATTR_OFFICENUMBER))
    {
        CEditListItem * pOfficeNoItem = m_pDetailLayout->GetOfficeNoEditItem();

        if (NULL != pOfficeNoItem)
        {
            setCurrentPageForItem(pOfficeNoItem);
            pOfficeNoItem->FocusItem(true);
        }
        return;
    }

    if (0 == strcmp(pszAttr, ATTR_MOBILENUMBER))
    {
        CEditListItem * pMobileNoItem = m_pDetailLayout->GetMobileNoEditItem();

        if (NULL != pMobileNoItem)
        {
            setCurrentPageForItem(pMobileNoItem);
            pMobileNoItem->FocusItem(true);
        }
        return;
    }

    if (0 == strcmp(pszAttr, ATTR_OTHERNUMBER))
    {
        CEditListItem * pOtherNoItem = m_pDetailLayout->GetOtherNoEditItem();

        if (NULL != pOtherNoItem)
        {
            setCurrentPageForItem(pOtherNoItem);
            pOtherNoItem->FocusItem(true);
        }
    }

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        if (0 == strcmp(pszAttr, ATTR_FAVORITE_INDEX))
        {
            CEditListItem * pIndexItem = m_pDetailLayout->GetFavoriteIndexEditItem();

            if (NULL != pIndexItem)
            {
                setCurrentPageForItem(pIndexItem);
                pIndexItem->FocusItem(true);
            }
        }
    }
#endif
}

void CLocalContactUIHelper::OnDetailCtrlFocus(bool bFocus)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact || !pLocalContact->IsDetailState())
    {
        return;
    }

    if (bFocus == m_bDetailEditing)
    {
        return;
    }

    m_bDetailEditing = bFocus;
    InitDetailTitle();
}

bool CLocalContactUIHelper::GetLocalGroupData(ContactGroupData & groupData)
{
    return false;
}

bool CLocalContactUIHelper::GetLocalContactData(int & nGroupId, ContactGeneralItemData & itemData)
{
    if (NULL == m_pDetailLayout)
    {
        DIRUI_ERR("m_pDetailLayout is null when GetLocalContactData");
        return false;
    }

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL == pNameItem)
    {
        DIRUI_ERR("GetNameEditItem() is null when GetLocalContactData");
        return false;
    }

    itemData.m_strDisplayName = pNameItem->GetValue().trimmed().toUtf8().data();

    if (m_pDetailLayout->IsCopyLayout())
    {
        CComboBoxListItemPtr pOfficeNoItem = m_pDetailLayout->GetOfficeNoComboBoxItem();

        if (NULL != pOfficeNoItem)
        {
            itemData.m_strOfficePhoneNumber = pOfficeNoItem->GetItemStringData();
        }

        CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoComboBoxItem();

        if (NULL != pMobileNoItem)
        {
            itemData.m_strMobilePhoneNumber = pMobileNoItem->GetItemStringData();
        }

        CComboBoxListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNoComboBoxItem();

        if (NULL != pOtherNoItem)
        {
            itemData.m_strOtherPhoneNumber = pOtherNoItem->GetItemStringData();
        }
    }
    else
    {
        CEditListItemPtr pOfficeNoItem = m_pDetailLayout->GetOfficeNoEditItem();

        if (NULL != pOfficeNoItem)
        {
            itemData.m_strOfficePhoneNumber = pOfficeNoItem->GetValue().trimmed().toUtf8().data();
        }

        CEditListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoEditItem();

        if (NULL != pMobileNoItem)
        {
            itemData.m_strMobilePhoneNumber = pMobileNoItem->GetValue().trimmed().toUtf8().data();
        }

        CEditListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNoEditItem();

        if (NULL != pOtherNoItem)
        {
            itemData.m_strOtherPhoneNumber = pOtherNoItem->GetValue().trimmed().toUtf8().data();
        }
    }

    CComboBoxListItemPtr pAccountItem = m_pDetailLayout->GetAccountComboBoxItem();

    if (NULL != pAccountItem)
    {
        qComboBoxEx * pComboBox = pAccountItem->GetComboBox();

        if (NULL != pComboBox)
        {
            int nCurIndex = pComboBox->currentIndex();

            itemData.m_nLine = pComboBox->itemData(nCurIndex).value<int>();
        }
        else
        {
            itemData.m_nLine = AUTO_ACCOUNT_IDENTIFY;
        }
    }
    else
    {
        itemData.m_nLine = AUTO_ACCOUNT_IDENTIFY;
    }

    if (m_pDetailLayout->IsNewLayout())
    {
        CDlgDirNew * pDlgDirNew = (CDlgDirNew *)UIManager_GetPrivateDlg(DLG_CDlgDirNew);

        if (NULL != pDlgDirNew)
        {
            nGroupId = pDlgDirNew->GetGroupId();
        }
    }
    else
    {
        CComboBoxListItemPtr pGroupItem = m_pDetailLayout->GetGroupComboBoxItem();

        if (NULL != pGroupItem)
        {
            nGroupId = pGroupItem->GetItemData();
        }
    }

    CEditListItemPtr pAutoDivertItem = m_pDetailLayout->GetAutoDivertItem();

    if (NULL != pAutoDivertItem)
    {
        itemData.m_strAutoDivert = pAutoDivertItem->GetValue().trimmed().toUtf8().data();
    }

    if (kszBlackListGroupId == nGroupId)
    {
        return true;
    }

    CComboBoxListItemPtr pRingItem = m_pDetailLayout->GetRingComboBoxItem();

    if (NULL != pRingItem)
    {
        itemData.m_strRing = pRingItem->GetItemStringData();
    }

    CComboBoxListItemPtr pPhotoItem = m_pDetailLayout->GetPhotoComboBoxItem();

    if (NULL != pPhotoItem)
    {
        itemData.m_strDefaultPhoto = pPhotoItem->GetItemStringData();

        res_toHidePath(itemData.m_strDefaultPhoto, PHFILE_TYPE_CONTACT_IMAGE);
    }

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        CEditListItemPtr pFavoriteItem = m_pDetailLayout->GetFavoriteIndexEditItem();

        if (NULL != pFavoriteItem)
        {
            yl::string strTemp = pFavoriteItem->GetValue().trimmed().toUtf8().data();
            if (strTemp.empty())
            {
                itemData.m_nFavoriteIndex = -1;
            }
            else if (commonAPI_IsDigital(strTemp))
            {
                itemData.m_nFavoriteIndex = atoi(strTemp.c_str());
            }
        }
    }
#endif

    return true;
}

bool CLocalContactUIHelper::IsSameContactRing(yl::string & strRing, yl::string & strContactRing)
{
    if (strRing == strContactRing)
    {
        return true;
    }

    if ("Auto" != strRing)
    {
        return false;
    }

    if ("" == strContactRing)
    {
        return true;
    }

    CComboBoxListItemPtr pRingItem = NULL;

    if (NULL != m_pDetailLayout)
    {
        pRingItem = m_pDetailLayout->GetRingComboBoxItem();
    }

    // 如果是不存在的铃声，也使用默认铃声
    if (NULL != pRingItem && !pRingItem->IsHadItemData(strContactRing))
    {
        return true;
    }

    return false;
}

bool CLocalContactUIHelper::IsSameContactPhoto(yl::string & strPhoto, yl::string & strContactPhoto)
{
    // strPhoto是新的图片路径，strContactPhoto是逻辑层现有的图片路径
    if (strPhoto == strContactPhoto)
    {
        return true;
    }

    if (strPhoto != CONTACT_PHOTO_DEFAULT_CONTACT_IMAGE)
    {
        return false;
    }

    if (strContactPhoto.empty())
    {
        return true;
    }

    yl::string strPhotoPath = strContactPhoto;

    res_toRealPath(strPhotoPath, PHFILE_TYPE_CONTACT_IMAGE);

    CComboBoxListItemPtr pPhotoItem = NULL;

    if (NULL != m_pDetailLayout)
    {
        pPhotoItem = m_pDetailLayout->GetPhotoComboBoxItem();
    }

    // 如果是不存在的图片，也使用默认图片
    if (NULL != pPhotoItem && !pPhotoItem->IsHadItemData(strPhotoPath))
    {
        return true;
    }

    return false;
}

bool CLocalContactUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    LocalDirContactRecord * pContact = CLocalContactControler::GetLocalContact(pDetail);

    DIRUI_INFO("CLocalContactUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p Layout:%p",
               nId, pContact, nDataIndex, nDataCount, m_pFrameList, m_pDetailLayout);

    if (NULL == pContact || NULL == m_pFrameList || NULL == m_pDetailLayout)
    {
        return false;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        DIRUI_ERR("CLocalContactUIHelper::FillDetail() fail when delegate is empty");
        return false;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        DIRUI_ERR("CLocalContactUIHelper::FillDetail() fail when controler is empty");
        return false;
    }

    m_pFrameList->ClearContent();

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nTotalCount = m_pDetailLayout->GetItemCount();

    if (kszBlackListGroupId == pLocalContact->GetGroupId() && nTotalCount > 3)
    {
        nTotalCount -= 3;

#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            nTotalCount -= 1;
        }
#endif
    }

    if (nDataIndex < 0)
    {
        nDataIndex = 0;
    }
    else if (nDataIndex >= nTotalCount)
    {
        if (0 != nMaxPageTotalCount && 0 != nTotalCount && nMaxPageTotalCount != nTotalCount)
        {
            nDataIndex = nTotalCount - (nTotalCount % nMaxPageTotalCount);
        }
        else
        {
            nDataIndex = 0;
        }
    }

    m_pFrameList->SetItemAttach();
    m_pFrameList->SetTotalDataCount(nTotalCount);

    m_pDetailLayout->UpdateDetail(nId, pContact);

    for (int i = 0; i < nMaxPageTotalCount && nDataIndex < nTotalCount; ++i, ++nDataIndex)
    {
        CDetailListItemBasePtr pItem = m_pDetailLayout->GetDetailListItem(nDataIndex);

        if (NULL == pItem)
        {
            continue;
        }

        if (0 == i)
        {
            pItem->SetDataIndex(INVALID_ITEM_INDEX);
        }

        pItem->SetAction(pDelegate.Get());
        m_pFrameList->AddItem(pItem);

        pItem->SetDataIndex(nDataIndex);

        DIRUI_INFO("AddItem(%p) ItemIndex:%d DataIndex=%d", pItem, i, nDataIndex);
    }

    m_pFrameList->update();

    return true;
}

void CLocalContactUIHelper::OnGroupChange()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    int nState = pDelegate->GetState();

    if (DIR_UI_STATE_SETTING == nState)
    {
        InitSettingTitle();
    }
}

void CLocalContactUIHelper::OnAttachFrameList()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    int nState = pDelegate->GetState();

    if (DIR_UI_STATE_SETTING == nState)
    {
        InitSettingTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_DETAIL == nState)
    {
        InitDetailTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_DETAIL_ITEM_COUNT);
        return;
    }

    CCommonUIHelper::OnAttachFrameList();
}

bool CLocalContactUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    LocalDirContactRecord * pContact = CLocalContactControler::GetLocalContact(pDetail);

    if (NULL == pContact)
    {
        return false;
    }

    yl::string strHead = "";

    if (!pContact->m_strDefaultPhoto.empty())
    {
        strHead = pContact->m_strDefaultPhoto;

        yl::string strPath;
        yl::string strFile;

        res_toRealPath(strHead, PHFILE_TYPE_CONTACT_IMAGE);
        ParseFilePath(strHead, strPath, strFile);

        DIRUI_INFO("Path:%s File:%s", strPath.c_str(), strFile.c_str());

        strHead = strPath + strFile;
    }

    yl::string strNumber = "";

    //按顺序获取显示号码
    if (!pContact->GetNumberByName(kszOfficeNumber, strNumber) || strNumber.empty())
    {
        if (!pContact->GetNumberByName(kszMobileNumber, strNumber) || strNumber.empty())
        {
            pContact->GetNumberByName(kszOtherNumber, strNumber);
        }
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
            && !pDelegate.IsEmpty())
    {
        CDirectoryControlerPtr pControler = pDelegate->GetControler();
        if (!pDelegate.IsEmpty()
                && kszFavoriteGroupId == pControler->GetGroupId())
        {
            return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                                      strNumber, strHead, pContact->m_nFavoriteIndex);
        }
    }
#endif

    if (!pDelegate.IsEmpty())
    {
        //如果是搜索状态，则要显示的号码应为搜索结果匹配的号码，而不是第一个号码
        if (DIR_UI_STATE_SEARCH == pDelegate->GetState())
        {
            CDirectoryControlerPtr pControler = pDelegate->GetControler();
            strNumber = pContact->GetSearchShowNumber(pControler->GetSearchKey());
        }
    }

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNumber, strHead);
}

void CLocalContactUIHelper::InitSettingTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    bool bAdd = true;
    CContactListTitlePtr pContactListTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactListTitle::IsContactListTitle(pTitle))
    {
        bAdd = false;
        pContactListTitle = (CContactListTitlePtr)pTitle;
    }
    else
    {
        pContactListTitle = new CContactListTitle;
        if (NULL == pContactListTitle)
        {
            return;
        }
    }

    pContactListTitle->SetAction(pDelegate.Get());
#if IF_FEATURE_GBBOOK
    pContactListTitle->SetCheckable(true);
#endif

    int nGroupId = pControler->GetGroupId();
#if 1//zzl
    int iBtnCount = 1;
    bool bShowAddBlackList = false;
    bool bShowMove = false;

    if (kszBlackListGroupId != nGroupId
            && CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        bShowAddBlackList = true;
        iBtnCount++;
    }

    if ((kszBlackListGroupId == nGroupId
            || IsCanMoveOtherGroup(nGroupId)
#if IF_FEATURE_FAVORITE_DIR
            || (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1 && kszFavoriteGroupId != nGroupId)
#endif
        ))
    {
        bShowMove = true;
        iBtnCount++;
    }

    pContactListTitle->SetBtnCount(iBtnCount);

    if (bShowMove)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_MOVE));
    }

    if (bShowAddBlackList)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
    }

    pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
#else
    if (kszBlackListGroupId == nGroupId)
    {
        pContactListTitle->SetBtnCount(2);
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_MOVE));
        pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
    else if (IsCanMoveOtherGroup(nGroupId))
    {
        pContactListTitle->SetBtnCount(3);
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_MOVE));
        pContactListTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
        pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
    else
    {
        pContactListTitle->SetBtnCount(2);
        pContactListTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
        pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
#endif
    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pContactListTitle);
    }
    else
    {
        pContactListTitle->RefreshTitle();
    }
}

void CLocalContactUIHelper::InitDetailTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    bool bAdd = true;
    CContactDetailTitlePtr pDetailTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactDetailTitle::IsContactDetailTitle(pTitle))
    {
        bAdd = false;
        pDetailTitle = (CContactDetailTitlePtr)pTitle;
    }
    else
    {
        pDetailTitle = new CContactDetailTitle;
        if (NULL == pDetailTitle)
        {
            return;
        }
    }

    if (kszBlackListGroupId == pControler->GetGroupId())
    {
        if (m_bDetailEditing)
        {
            pDetailTitle->SetBtnCount(1);
        }
        else
        {
            pDetailTitle->SetBtnCount(2);
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        }

        pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
#if IF_FEATURE_FAVORITE_DIR
    else if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
             && kszFavoriteGroupId == pControler->GetGroupId())
    {
        pDetailTitle->SetBtnCount(2);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_REMOVE_FAVORITE,
                                LANG_TRANSLATE(TRID_REMOVE_FROM_FAVORITE));
    }
#endif
    else
    {
#if 1//zzl
        int iBtnCount = 1;
        bool bShowAddBlackList = false;

        if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
        {
            bShowAddBlackList = true;
            iBtnCount++;
        }

        if (!m_bDetailEditing)
        {
            iBtnCount++;
        }

#if IF_FEATURE_FAVORITE_DIR
        int nContactId = -1;

        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);
            if (pLocalContact != NULL)
            {
                nContactId = pLocalContact->GetContactId();
                iBtnCount++;
            }
        }
#endif

        pDetailTitle->SetBtnCount(iBtnCount);

        if (!m_bDetailEditing)
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        }

        if (bShowAddBlackList)
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
        }

#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            if (LocalDir_IsContactInFavorite(nContactId))
            {
                pDetailTitle->AddButton(LIST_TITLE_ACTION_REMOVE_FAVORITE,
                                        LANG_TRANSLATE(TRID_FAVORITE));
            }
            else
            {
                pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_FAVORITE, LANG_TRANSLATE(TRID_FAVORITE));
            }
        }
#endif

        pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
#else
        if (m_bDetailEditing)
        {
            pDetailTitle->SetBtnCount(2);
        }
        else
        {
            pDetailTitle->SetBtnCount(3);
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        }

        pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
#endif
    }

    pDetailTitle->SetAction(pDelegate.Get());

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pDetailTitle);
    }
    else
    {
        pDetailTitle->RefreshTitle();
    }
}

int CLocalContactUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    switch (nTitleAction)
    {
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            return DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT;
        }
        break;
    case LIST_TITLE_ACTION_ADD_BLACKLIST:
        {
            if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_LOCAL_MOVE_TO_BLACKLIST;
            }
            else if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST;
            }
        }
        break;
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_LOCAL_DEL_CONTACT;
            }
            else if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_LOCAL_BATCH_DEL_CONTACT;
            }
        }
        break;
#if IF_FEATURE_FAVORITE_DIR
    case LIST_TITLE_ACTION_ADD_FAVORITE:
        {
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                    && DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_LOCAL_COPY_TO_FAVORITE;
            }
        }
        break;
    case LIST_TITLE_ACTION_REMOVE_FAVORITE:
        {
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                    && DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE;
            }
        }
        break;
#endif
    default:
        break;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

bool CLocalContactUIHelper::IsCanMoveOtherGroup(int nGroupId)
{
    bool bIncludeRoot = false;

    if (kszBlackListGroupId == nGroupId)
    {
        bIncludeRoot = true;
    }

    CGroupSelector groupSelector;

    groupSelector.Init(DIRECTORY_TYPE_LOCAL, nGroupId, bIncludeRoot);

    return (groupSelector.GetGroupAmount() > 0);
}

void CLocalContactUIHelper::FillNumberComboBoxList(CComboBoxListItem * pCmbNumberItem,
        yl::string & strFirstSelectNumber,
        yl::string & strSecondSelectNumber, yl::string & strThirdSelectNumber)
{
    if (NULL == pCmbNumberItem)
    {
        return;
    }

    pCmbNumberItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                             QVariant::fromValue<yl::string>(yl::string("")));

    if (!strFirstSelectNumber.empty())
    {
        pCmbNumberItem->AddValue(toQString(strFirstSelectNumber, true),
                                 QVariant::fromValue<yl::string>(strFirstSelectNumber));
    }

    if (!strSecondSelectNumber.empty())
    {
        pCmbNumberItem->AddValue(toQString(strSecondSelectNumber, true),
                                 QVariant::fromValue<yl::string>(strSecondSelectNumber));
    }

    if (!strThirdSelectNumber.empty())
    {
        pCmbNumberItem->AddValue(toQString(strThirdSelectNumber, true),
                                 QVariant::fromValue<yl::string>(strThirdSelectNumber));
    }
}

void CLocalContactUIHelper::FillNumberComboBoxList(CComboBoxListItem * pCmbNumberItem,
        YLList<yl::string> & listSelectNumber)
{
    if (NULL == pCmbNumberItem)
    {
        return;
    }

    pCmbNumberItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                             QVariant::fromValue<yl::string>(yl::string("")));

    YLList<yl::string>::iterator it = listSelectNumber.begin();
    for (; it != listSelectNumber.end(); ++it)
    {
        yl::string strNumber = *it;
        if (!strNumber.empty())
        {
            pCmbNumberItem->AddValue(toQString(strNumber, true), QVariant::fromValue<yl::string>(strNumber));
        }
    }
}

void CLocalContactUIHelper::LoadLocalAdditionData(int nAction, int nDataIndex, int nDataCount)
{
    FillContactDetail(knInvalidRId, NULL, nDataIndex, nDataCount);
}

bool CLocalContactUIHelper::FillContactDetail(int nId, LocalDirContactRecord * pContact,
        int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CLocalContactUIHelper::FillContactDetail(Id:%d pContact:%p DataIndex:%d DataCount:%d) FrameList:%p Layout:%p",
               nId, pContact, nDataIndex, nDataCount, m_pFrameList, m_pDetailLayout);

    if (NULL == m_pFrameList || NULL == m_pDetailLayout)
    {
        DIRUI_ERR("CLocalContactUIHelper::FillContactDetail() fail");
        return false;
    }

    if (pContact == NULL)
    {
        m_pFrameList->SetMaxPageItemCount(7);
    }

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        m_pDetailLayout->UpdateFavoriteIndex();
    }
#endif

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        DIRUI_ERR("CLocalContactUIHelper::FillDetail() fail when delegate is empty");
        return false;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        DIRUI_ERR("CLocalContactUIHelper::FillDetail() fail when controler is empty");
        return false;
    }

    m_pFrameList->ClearContent();

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nTotalCount = m_pDetailLayout->GetItemCount();

    if (kszBlackListGroupId == pLocalContact->GetGroupId() && nTotalCount > 3)
    {
        nTotalCount -= 3;

#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            nTotalCount -= 1;
        }
#endif
    }

    if (nDataIndex < 0)
    {
        nDataIndex = 0;
    }
    else if (nDataIndex >= nTotalCount)
    {
        if (0 != nMaxPageTotalCount && 0 != nTotalCount && nMaxPageTotalCount != nTotalCount)
        {
            nDataIndex = nTotalCount - (nTotalCount % nMaxPageTotalCount);
        }
        else
        {
            nDataIndex = 0;
        }
    }

    m_pFrameList->SetItemAttach();
    m_pFrameList->SetTotalDataCount(nTotalCount);

    if (pContact != NULL)
    {
        m_pDetailLayout->UpdateDetail(nId, pContact);
    }

    for (int i = 0; i < nMaxPageTotalCount && nDataIndex < nTotalCount; ++i, ++nDataIndex)
    {
        CDetailListItemBasePtr pItem = m_pDetailLayout->GetDetailListItem(nDataIndex);

        if (NULL == pItem)
        {
            continue;
        }

        if (0 == i)
        {
            pItem->SetDataIndex(INVALID_ITEM_INDEX);
        }

        pItem->SetAction(pDelegate.Get());
        m_pFrameList->AddItem(pItem);

        pItem->SetDataIndex(nDataIndex);

        DIRUI_INFO("AddItem(%p) ItemIndex:%d DataIndex=%d", pItem, i, nDataIndex);
    }

    m_pFrameList->SetItemFocusByIndex(0);

    m_pFrameList->update();

    return true;
}

#if IF_FEATURE_FAVORITE_DIR
bool CLocalContactUIHelper::IsFromDsskeyAdd()
{
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
            && m_pDetailLayout != NULL)
    {
        return m_pDetailLayout->IsFromDsskeyAdd();
    }
    return false;
}

bool CLocalContactUIHelper::GetLocalContactFavorite(yl::string & strFavoriteIndex)
{
    CEditListItemPtr pFavoriteItem = m_pDetailLayout->GetFavoriteIndexEditItem();

    if (NULL != pFavoriteItem)
    {
        strFavoriteIndex = pFavoriteItem->GetValue().trimmed().toUtf8().data();
    }
    return true;
}
#endif

