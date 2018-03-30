#if IF_FEATURE_GBBOOK
#include <QtGui>
#include "genbandcontactuihelper.h"
#include "genbandcontactcontroler.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "settingdelegate.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "contactupdatelisttitle.h"
#include "moddirectoryui.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/detaillistitem.h"
#include "baseui/editlistitem.h"
#include "baseui/framelist.h"
#include "baseui/frametiptitle.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "dlgdirnew.h"
#include "uimanager/moduimanager.h"
#include "groupdelegate.h"
#include "contactgroupdelegate.h"

namespace
{
enum
{
    BS_DETAIL_ITEM_INDEX_FIRST_NAME     = 0,
    BS_DETAIL_ITEM_INDEX_LAST_NAME,
    BS_DETAIL_ITEM_INDEX_NUMBER,
    BS_DETAIL_ITEM_INDEX_EXTENSION,
    BS_DETAIL_ITEM_INDEX_MOBILE,
    BS_DETAIL_ITEM_INDEX_DEPARTMENT,
    BS_DETAIL_ITEM_INDEX_EMAIL,
    BS_DETAIL_ITEM_INDEX_GROUP,
    BS_DETAIL_ITEM_INDEX_MAX,
};
#define BROADSOFT_SUPER_SEARCH_ITEM_COUNT 6

enum
{
    GB_DETAIL_ITEM_INDEX_NAME = 0,
    GB_DETAIL_ITEM_INDEX_GROUP,
    GB_DETAIL_ITEM_INDEX_FIRST_NAME,
    GB_DETAIL_ITEM_INDEX_LAST_NAME,
    GB_DETAIL_ITEM_INDEX_BUSINESS_NUMBER,
    GB_DETAIL_ITEM_INDEX_HOME_NUMBER,
    GB_DETAIL_ITEM_INDEX_MOBILE,
    GB_DETAIL_ITEM_INDEX_PAGER,
    GB_DETAIL_ITEM_INDEX_FAX,
    GB_DETAIL_ITEM_INDEX_EMAIL,
    GB_DETAIL_ITEM_INDEX_CONF_URL,
    GB_DETAIL_ITEM_INDEX_PRIMARY_CONTACT,
    GB_DETAIL_ITEM_INDEX_MAX,
};

enum
{
    GAB_DETAIL_ITEM_INDEX_NAME = 0,
    GAB_DETAIL_ITEM_INDEX_FIRST_NAME,
    GAB_DETAIL_ITEM_INDEX_LAST_NAME,
    GAB_DETAIL_ITEM_INDEX_BUSINESS_NUMBER,
    GAB_DETAIL_ITEM_INDEX_HOME_NUMBER,
    GAB_DETAIL_ITEM_INDEX_MOBILE,
    GAB_DETAIL_ITEM_INDEX_PAGER,
    GAB_DETAIL_ITEM_INDEX_FAX,
    GAB_DETAIL_ITEM_INDEX_EMAIL,
    GAB_DETAIL_ITEM_INDEX_CONF_URL,
    GAB_DETAIL_ITEM_INDEX_PRIMARY_CONTACT,
    GAB_DETAIL_ITEM_INDEX_MAX,
};
}


CGenbandContactUIHelper::CGenbandContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_GENBAND)
    , m_pDetailLayout(NULL)
    , m_pSearchLayout(NULL)
    , m_bDetailEditing(false)
{
}

CGenbandContactUIHelper::~CGenbandContactUIHelper()
{
}

CGenbandContactUIHelper * CGenbandContactUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_GENBAND != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CGenbandContactUIHelper *)pUIHelper.Get();
}

void CGenbandContactUIHelper::GetGroupDisplayName(int nGroupId, const yl::string & strGroupName,
        QString & strDisplayName)
{
    if (kszAllContactId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_GB_PABBOOK);
    }
    else if (kszGabBookGroupId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_GB_GABBOOK);
    }
    else if (kszPabBookGroupId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_MANAGE_GROUP);
    }
    else if (kszGbNAGroupId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_NO_GROUP);
    }
    else if (!strGroupName.empty())
    {
        strDisplayName = toQString(strGroupName, true);
    }

}

void CGenbandContactUIHelper::SetDetailLayout(CGenbandDetailLayoutPtr pDetailLayout)
{
    m_pDetailLayout = pDetailLayout;

    if (NULL == m_pDetailLayout)
    {
        return;
    }

    DIRUI_INFO("SetDetailLayout NewLayout:%d CopyLayout:%d",
               (m_pDetailLayout->IsAdditionLayout() ? 1 : 0),
               (m_pDetailLayout->IsCopyLayout() ? 1 : 0));

    if (m_pDetailLayout->IsAdditionLayout() && m_pDetailLayout->IsCopyLayout())
    {
        InitCopyLayout();
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGBContact)
    {
        return;
    }

    int nCurGroupId = pGBContact->GetGroupId();

    CComboBoxListItemPtr pGroupComboBoxItem = m_pDetailLayout->GetGroupComboBoxItem();

    if (NULL == pGroupComboBoxItem)
    {
        return;
    }

    // 初始化普通群组
    VEC_CONTACT_ID vecGroupId;
    yl::string strGroupName;
    QString strGroupShowName;


    ContactListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 0;

    Dir_GetContactListByGroupId(&listData, kszPabBookGroupId, GB_DIR);
    vecGroupId = listData.m_listId;

    int nSize = vecGroupId.size();

    int iIndex = 0;
    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (!pGBContact->GetGroupName(nGroupId, strGroupName))
        {
            continue;
        }

        if (nGroupId == kszGbNAGroupId)
        {
            iIndex = i;
        }

        GetGroupDisplayName(nGroupId, strGroupName, strGroupShowName);

        pGroupComboBoxItem->AddValue(strGroupShowName, QVariant::fromValue(nGroupId));
    }

    pGroupComboBoxItem->SetCurrentIndex(iIndex);
}

void CGenbandContactUIHelper::SetSearchLayout(CGenbandSearchLayoutPtr pSearchLayout)
{
    m_pSearchLayout = pSearchLayout;

    if (NULL == m_pSearchLayout)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
    {
        return;
    }

    // 获取组名列表数据成功, 加入可选择的组
    int nGroupId = pGenbandContact->GetGroupId();
    VEC_CONTACT_ID vecId;

    pGenbandContact->GetAllGroup(vecId);

    YLVector<yl::string> vecGroupName;
    int nSize = vecId.size();
    int nIndex = 0;

    for (int i = 0; i < nSize; ++i)
    {
        int nId = vecId[i];

        if (nId == nGroupId)
        {
            nIndex = i;
        }

        SmartPtr pDetail = pGenbandContact->GetDetail(nId);

        if (pDetail.IsEmpty())
        {
            continue;
        }

        QString strDisplayName;

        GetGroupDisplayName(nId, pDetail->GetDisplayName(), strDisplayName);

    }
}

void CGenbandContactUIHelper::SetInputFocus(const char * pszAttr)
{
    if (NULL == m_pDetailLayout || NULL == pszAttr)
    {
        return;
    }

    if (NULL != m_pFrameList)
    {
        // 第二页时候，设置焦点不会切换到第一页，切换页
        m_pFrameList->SwitchPage(0);
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

    if (0 == strcmp(pszAttr, kszGbBookprimaryContact))
    {
        CEditListItem * pNumberItem = m_pDetailLayout->GetPriContactEditItem();

        if (NULL != pNumberItem)
        {
            setCurrentPageForItem(pNumberItem);
            pNumberItem->FocusItem(true);
        }
    }
}

void CGenbandContactUIHelper::OnDetailCtrlFocus(bool bFocus)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact || !pGenbandContact->IsDetailState())
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

void CGenbandContactUIHelper::InitSettingTitle()

{
    DIRUI_INFO("CGenbandContactUIHelper::InitSettingTitle");
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

    if (pControler.IsEmpty())
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
    pContactListTitle->SetBtnCount(1);
    pContactListTitle->SetCheckable(false);

    int nGroupId = pControler->GetGroupId();

    if (kszGabBookGroupId != nGroupId)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
    else
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_PERSONAL));
    }

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pContactListTitle);
    }
    else
    {
        pContactListTitle->RefreshTitle();
    }
}

void CGenbandContactUIHelper::InitDetailTitle()
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
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
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

    if (!CGenbandContactControler::IsExitPabGroup())
    {
        pDetailTitle->SetBtnCount(1);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    }
    else
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

        if (pGenbandContact->IsPabContact(pGenbandContact->GetContactId()))
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
        }
        else
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_PERSONAL));
        }
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

int CGenbandContactUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        if (LIST_TITLE_ACTION_CONTACT_UPDATE == nTitleAction)
        {
            return DIR_ACTION_GENBAND_UPDATE;
        }

        return DIR_ACTION_NONE;
    }

    switch (nTitleAction)
    {
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_GENBAND_BATCH_DEL_PERSONAL;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_GENBAND_DEL_PAB;
            }
        }
        break;
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_GENBAND_BATCH_MOVE_TO_PERSONAL;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_GENBAND_ENTER_ADD_TO_PAB;
            }
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

void CGenbandContactUIHelper::InitPreciseSearchTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    bool bAdd = true;
    CFrameTipTitlePtr pTipTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CFrameTipTitle::IsTipTitle(pTitle))
    {
        bAdd = false;
        pTipTitle = (CFrameTipTitlePtr)pTitle;
    }
    else
    {
        pTipTitle = new CFrameTipTitle;
        if (NULL == pTipTitle)
        {
            return;
        }
    }

    pTipTitle->SetTip(LANG_TRANSLATE(TRID_PLEASE_ENTER_FILTER_CONDITIONS));

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pTipTitle);
    }
    else
    {
        pTipTitle->RefreshTitle();
    }
}

void CGenbandContactUIHelper::CheckSettingTitle()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

    if (NULL == pSettingDelegate)
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
    {
        return;
    }

    InitSettingTitle();

    yl::string strGroupName;

    pGenbandContact->GetGroupName(pGenbandContact->GetGroupId(), strGroupName);

    SetUpdateTitleGroupName(toQString(strGroupName, true));

    int nTotalCount = GetAllContactAmount();

    RefreshUpdateTitle(nTotalCount);
}

void CGenbandContactUIHelper::OnAttachFrameList()
{
    DIRUI_INFO("CGenbandContactUIHelper::OnAttachFrameList m_pFrameList:%p", m_pFrameList);
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

    if (DIR_UI_STATE_BROWSER == nState)
    {
        InitUpdateTitle();

        int nGroupId = pDelegate->GetGroupId();
        bool bShowUpdate = kszGabBookGroupId == nGroupId ? true : false;
        if (bShowUpdate
                && (GAB_GetCurentGabKey().empty() || PAB_GAB_NONE == GAB_GetCurrentGabOptType()))
        {
            bShowUpdate = false;
        }
        SetShowUpdate(bShowUpdate);

        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }
    else if (DIR_UI_STATE_SETTING == nState)
    {
        InitSettingTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }
    else if (DIR_UI_STATE_DETAIL == nState)
    {
        InitDetailTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_DETAIL_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_PRECISE_SEARCH == nState)
    {
        CDirectoryControlerPtr pControler = pDelegate->GetControler();
        CGenbandContactControler * pGenbandControler = CGenbandContactControler::GetControler(pControler);

        // 从详情退回高级搜索界面时FrameList保持原来的设置
        if (NULL != pGenbandControler && pGenbandControler->IsPreciseSearchResult())
        {
            return;
        }

        InitPreciseSearchTitle();
        m_pFrameList->SetOnlyTitleLine(true);
        m_pFrameList->SetMaxPageItemCount(BROADSOFT_SUPER_SEARCH_ITEM_COUNT);
        m_pFrameList->SetItemClickBG("");
        return;
    }

    CCommonUIHelper::OnAttachFrameList();
}

void CGenbandContactUIHelper::OnGroupChange()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
    {
        return;
    }

    int nGroupId = pGenbandContact->GetGroupId();

    DIRUI_INFO("CGenbandContactUIHelper::OnGroupChange() GroupId:%d Setting:%d",
               nGroupId, (pGenbandContact->IsSettingState()));

    if (pGenbandContact->IsSettingState())
    {
        CheckSettingTitle();
    }
    else if (!IsShowUpateTitile())
    {
        return;
    }

    yl::string strGroupName;

    if (kszAllContactId != nGroupId && !pGenbandContact->GetGroupName(nGroupId, strGroupName))
    {
        DIRUI_ERR("CGenbandContactControler::GetGroupName(%d) fail in OnGroupChange", nGroupId);
    }

    bool bShowUpdate = kszGabBookGroupId == nGroupId ? true : false;
    if (bShowUpdate
            && (GAB_GetCurentGabKey().empty() || PAB_GAB_NONE == GAB_GetCurrentGabOptType()))
    {
        bShowUpdate = false;
    }

    SetShowUpdate(bShowUpdate);
    QString strDisplayName;

    GetGroupDisplayName(nGroupId, strGroupName, strDisplayName);

    SetUpdateTitleGroupName(strDisplayName, true);
}

bool CGenbandContactUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex,
        int nDataCount)
{
    DIRUI_INFO("CGenbandContactUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pDetail.Get(), nDataIndex, nDataCount, m_pFrameList);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    switch (pDetail->GetType())
    {
    case DIR_TYPE_GB_UNIT:
        {
            int nParentGroupId = Dir_GetParentIdById(nId, GB_DIR);
            if (nParentGroupId != kszGabBookGroupId)
            {
                return FillGenbandEdit(nId, (ContactGBBookUnit *)pDetail.Get(), nDataIndex, nDataCount);
            }
            else
            {
                return FillGenbandDetail(nDataIndex, nDataCount, (ContactGBBookUnit *)pDetail.Get(),
                                         nParentGroupId != kszGabBookGroupId);
            }
        }
        break;
    default:
        {
            DIRUI_ERR("CGenbandContactUIHelper::FillDetail fail, Type=%d", pDetail->GetType());
        }
        break;
    }

    return false;
}

bool CGenbandContactUIHelper::FillGenbandEdit(int nId, ContactGBBookUnit * pDetail, int nDataIndex,
        int nDataCount)
{
    DIRUI_INFO("CGenbandContactUIHelper::FillGenbandEdit(Id:%d Detail:%p DataIndex:%d DataCount:%d)",
               nId, pDetail, nDataIndex, nDataCount);

    if (NULL == m_pFrameList || NULL == m_pDetailLayout)
    {
        DIRUI_INFO("FillContactDetail FrameList:%p DetailLayout:%p", m_pFrameList, m_pDetailLayout);
        return false;
    }

    if (pDetail == NULL)
    {
        m_pFrameList->SetMaxPageItemCount(7);
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        DIRUI_ERR("CGenbandContactUIHelper::FillContactDetail() fail when delegate is empty");
        return false;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
    {
        DIRUI_ERR("CGenbandContactUIHelper::FillDetail() fail when controler is empty");
        return false;
    }

    m_pFrameList->ClearContent();

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nTotalCount = m_pDetailLayout->GetItemCount();

    if (nDataIndex < 0)
    {
        nDataIndex = 0;
    }
    else if (nDataIndex >= nTotalCount)
    {
        if (0 != nMaxPageTotalCount && 0 != nTotalCount && nMaxPageTotalCount != nTotalCount)
        {
            nDataIndex = nTotalCount - (nTotalCount % nMaxPageTotalCount);
            nDataCount = nMaxPageTotalCount - nDataIndex;
        }
        else
        {
            nDataIndex = 0;
            nDataCount = nMaxPageTotalCount;
        }
    }

    m_pFrameList->SetItemAttach();
    m_pFrameList->SetTotalDataCount(nTotalCount);

    if (NULL != pDetail)
    {
        m_pDetailLayout->UpdateContact(nId, pDetail);
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

    m_pFrameList->update();

    return true;
}

bool CGenbandContactUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    int nDetailType = pDetail->GetType();

    if (DIR_TYPE_GROUP == nDetailType || DIR_TYPE_GB_MENU == nDetailType)
    {
        QString strDisplayName;

        GetGroupDisplayName(nId, pDetail->GetDisplayName(), strDisplayName);

        return RefreshGroupMenuItem(nId, nItemIndex, nDataIndex, nDataCount, strDisplayName);
    }

    switch (nDetailType)
    {
    case DIR_TYPE_GB_UNIT:
        {
            return FillGenbandItem(nId, nItemIndex, nDataIndex, nDataCount, (ContactGBBookUnit *)pDetail.Get());
        }
        break;
    default:
        break;
    }

    DIRUI_ERR("CGenbandContactUIHelper::FillListItem(Id:%d ItemIndex:%d DataIndex:%d DataCount:%d Detail:%p) fail",
              nId, nItemIndex, nDataIndex, nDataCount, pDetail.Get());

    return false;
}

bool CGenbandContactUIHelper::FillGenbandItem(int nId, int nItemIndex, int nDataIndex,
        int nDataCount, ContactGBBookUnit * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strDisplayName = pContact->m_strFirstName;
    if (!pContact->m_strLastName.empty())
    {
        strDisplayName.append(" ");
        strDisplayName.append(pContact->m_strLastName);
    }

    if (strDisplayName.empty())
    {
        strDisplayName = pContact->GetDisplayName();
    }

    yl::string strNum = "";

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);
    if (NULL != pGenbandContact)
    {
        if (pGenbandContact->IsSearchState())
        {
            strNum = pContact->GetSearchShowNumber(pGenbandContact->GetSearchKey());
        }
    }

    if (strNum.empty())
    {
        if (!pContact->GetNumberByName(kszGbBookbusinessNumber, strNum) || strNum.empty())
        {
            if (!pContact->GetNumberByName(kszGbBookhomeNumber, strNum) || strNum.empty())
            {
                pContact->GetNumberByName(kszGbBookmobile, strNum);
            }
        }
    }

    yl::string strHead = "";
    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, strDisplayName, strNum, strHead);
}

bool CGenbandContactUIHelper::FillNormalDetail(int nDataIndex, int nTotalCount,
        VEC_ATTRIBUTE & vecAttr)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    int nSize = vecAttr.size();
    int nItemIndex = 0;

    for (; nItemIndex < nSize; ++nItemIndex)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = vecAttr[nItemIndex];
        QString strTitle = LANG_TRANSLATE(stAttr.m_strName.c_str());
        QString strValue = toQString(stAttr.m_strValue, true);
        yl::string strData = "";

        if (TRID_BUSINESS_NUMBER == stAttr.m_strName)
        {
            strData = kszGbBookbusinessNumber;
        }
        else if (TRID_HOME_NUMBER == stAttr.m_strName)
        {
            strData = kszGbBookhomeNumber;
        }
        else if (TRID_MOBILE_NUMBER == stAttr.m_strName)
        {
            strData = kszGbBookmobile;
        }
        else if (TRID_PAGER == stAttr.m_strName)
        {
            strData = kszGbBookpager;
        }
        else if (TRID_FAX == stAttr.m_strName)
        {
            strData = kszGbBookFax;
        }
        else if (TRID_CONFERENCE_URL == stAttr.m_strName)
        {
            strData = kszGbconferenceURL;
        }
        else if (TRID_PRIMARYCONTACT == stAttr.m_strName)
        {
            strData = kszGbBookprimaryContact;
        }

        RefreshDetailItem(nItemIndex, (nDataIndex + nItemIndex), nTotalCount,
                          strTitle, strValue);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

bool CGenbandContactUIHelper::FillGenbandDetail(int nDataIndex, int nDataCount,
        ContactGBBookUnit * pContact, bool bIsPab/* = true*/)
{
    if (NULL == pContact)
    {
        return false;
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;
    int nTotalAmount = GB_DETAIL_ITEM_INDEX_MAX;
    if (!bIsPab)
    {
        nTotalAmount--;
    }
    int nTmpDataIndex = nDataIndex;

    for (int i = 0; i < nDataCount && nTmpDataIndex < nTotalAmount; ++i, ++nTmpDataIndex)
    {
        if (bIsPab)
        {
            switch (nTmpDataIndex)
            {
            case GB_DETAIL_ITEM_INDEX_NAME:
                {
                    stAttr.m_strName = TRID_NICK_NAME;
                    stAttr.m_strValue = pContact->GetDisplayName();
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_FIRST_NAME:
                {
                    stAttr.m_strName = TRID_FIRST_NAME;
                    stAttr.m_strValue = pContact->m_strFirstName;
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_LAST_NAME:
                {
                    stAttr.m_strName = TRID_LAST_NAME;
                    stAttr.m_strValue = pContact->m_strLastName;
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_BUSINESS_NUMBER:
                {
                    stAttr.m_strName = TRID_BUSINESS_NUMBER;
                    if (!pContact->GetNumberByName(kszGbBookbusinessNumber, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_HOME_NUMBER:
                {
                    stAttr.m_strName = TRID_HOME_NUMBER;
                    if (!pContact->GetNumberByName(kszGbBookhomeNumber, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_MOBILE:
                {
                    stAttr.m_strName = TRID_MOBILE_NUMBER;
                    if (!pContact->GetNumberByName(kszGbBookmobile, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_PAGER:
                {
                    stAttr.m_strName = TRID_PAGER;
                    if (!pContact->GetNumberByName(kszGbBookpager, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_FAX:
                {
                    stAttr.m_strName = TRID_FAX;
                    if (!pContact->GetNumberByName(kszGbBookFax, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_EMAIL:
                {
                    stAttr.m_strName = TRID_EMAIL;
                    stAttr.m_strValue = pContact->m_strEmail;
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_CONF_URL:
                {
                    stAttr.m_strName = TRID_CONFERENCE_URL;
                    if (!pContact->GetNumberByName(kszGbconferenceURL, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_PRIMARY_CONTACT:
                {
                    stAttr.m_strName = TRID_PRIMARYCONTACT;
                    if (!pContact->GetNumberByName(kszGbBookprimaryContact, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GB_DETAIL_ITEM_INDEX_GROUP:
                {
                    stAttr.m_strName = TRID_GROUP;
                    stAttr.m_strValue = Dir_GetDisplayNameById(pContact->m_nParentId, GB_DIR);

                    vecAttr.push_back(stAttr);
                }
                break;
            default:
                break;
            }
        }
        else
        {
            switch (nTmpDataIndex)
            {
            case GAB_DETAIL_ITEM_INDEX_NAME:
                {
                    stAttr.m_strName = TRID_NICK_NAME;
                    stAttr.m_strValue = pContact->GetDisplayName();
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_FIRST_NAME:
                {
                    stAttr.m_strName = TRID_FIRST_NAME;
                    stAttr.m_strValue = pContact->m_strFirstName;
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_LAST_NAME:
                {
                    stAttr.m_strName = TRID_LAST_NAME;
                    stAttr.m_strValue = pContact->m_strLastName;
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_BUSINESS_NUMBER:
                {
                    stAttr.m_strName = TRID_BUSINESS_NUMBER;
                    if (!pContact->GetNumberByName(kszGbBookbusinessNumber, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_HOME_NUMBER:
                {
                    stAttr.m_strName = TRID_HOME_NUMBER;
                    if (!pContact->GetNumberByName(kszGbBookhomeNumber, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_MOBILE:
                {
                    stAttr.m_strName = TRID_MOBILE_NUMBER;
                    if (!pContact->GetNumberByName(kszGbBookmobile, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_PAGER:
                {
                    stAttr.m_strName = TRID_PAGER;
                    if (!pContact->GetNumberByName(kszGbBookpager, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_FAX:
                {
                    stAttr.m_strName = TRID_FAX;
                    if (!pContact->GetNumberByName(kszGbBookFax, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_EMAIL:
                {
                    stAttr.m_strName = TRID_EMAIL;
                    stAttr.m_strValue = pContact->m_strEmail;
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_CONF_URL:
                {
                    stAttr.m_strName = TRID_CONFERENCE_URL;
                    if (!pContact->GetNumberByName(kszGbconferenceURL, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            case GAB_DETAIL_ITEM_INDEX_PRIMARY_CONTACT:
                {
                    stAttr.m_strName = TRID_PRIMARYCONTACT;
                    if (!pContact->GetNumberByName(kszGbBookprimaryContact, stAttr.m_strValue))
                    {
                        stAttr.m_strValue = "";
                    }
                    vecAttr.push_back(stAttr);
                }
                break;
            default:
                break;
            }
        }

    }

    return FillNormalDetail(nDataIndex, nTotalAmount, vecAttr);
}



void CGenbandContactUIHelper::CheckPreciseSearchUI()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    m_pFrameList->SetOnlyTitleLine(false);
    m_pFrameList->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                 NULL);
}

bool CGenbandContactUIHelper::LoadGBPreciseSearchData(int nDataIndex, int nDataCount)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    DIRUI_INFO("LoadGBPreciseSearchData(Index:%d Count:%d)", nDataIndex, nDataCount);

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        DIRUI_ERR("CGenbandContactUIHelper::LoadGBPreciseSearchData() fail when delegate is empty");
        return false;
    }

    InitPreciseSearchTitle();

    m_pFrameList->SetMaxPageItemCount(2);

    if (NULL == m_pSearchLayout)
    {
        DIRUI_ERR("SearchLayout is empty when LoadGBPreciseSearchData");
        return false;
    }

    m_pFrameList->ClearContent();
    m_pFrameList->SetItemAttach();
    m_pFrameList->SetTotalDataCount(2);

    for (int i = 0;
            i < 2 && nDataIndex < 2;
            ++i, ++nDataIndex)
    {
        CDetailListItemBasePtr pItem = m_pSearchLayout->GetDetailListItem(nDataIndex);

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
    }

    SetGABLastSearchData();

    m_pFrameList->update();

    return true;
}



bool CGenbandContactUIHelper::GetPABContactData(ContactGBBookUnit & itemData)
{
    if (m_pDetailLayout == NULL)
    {
        return false;
    }

    m_pDetailLayout->ReturnAllEdit();

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL == pNameItem)
    {
        DIRUI_ERR("GetNameEditItem() is null when GetPABContactData");
        return false;
    }
    itemData.m_strDisplayName = pNameItem->GetValue().trimmed().toUtf8().data();

    CEditListItemPtr pFirstNameItem = m_pDetailLayout->GetFirstNameEditItem();
    if (NULL != pFirstNameItem)
    {
        itemData.m_strFirstName = pFirstNameItem->GetValue().trimmed().toUtf8().data();
    }

    CEditListItemPtr pLastNameItem = m_pDetailLayout->GetLastNameEditItem();
    if (NULL != pLastNameItem)
    {
        itemData.m_strLastName = pLastNameItem->GetValue().trimmed().toUtf8().data();
    }

    CEditListItemPtr pEmailItem = m_pDetailLayout->GetEmailEditItem();
    if (NULL != pEmailItem)
    {
        itemData.m_strEmail = pEmailItem->GetValue().trimmed().toUtf8().data();
    }

    CEditListItemPtr pBusinessNoItem = m_pDetailLayout->GetBusinessNoEditItem();
    if (NULL != pBusinessNoItem)
    {
        stNumber number;
        number.strName = kszGbBookbusinessNumber;
        number.strValue = pBusinessNoItem->GetValue().trimmed().toUtf8().data();
        if (!number.strValue.empty())
        {
            itemData.m_listNumber.push_back(number);
        }
    }

    CEditListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNoEditItem();
    if (NULL != pHomeNoItem)
    {
        stNumber number;
        number.strName = kszGbBookhomeNumber;
        number.strValue = pHomeNoItem->GetValue().trimmed().toUtf8().data();
        if (!number.strValue.empty())
        {
            itemData.m_listNumber.push_back(number);
        }
    }

    CEditListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoEditItem();
    if (NULL != pMobileNoItem)
    {
        stNumber number;
        number.strName = kszGbBookmobile;
        number.strValue = pMobileNoItem->GetValue().trimmed().toUtf8().data();
        if (!number.strValue.empty())
        {
            itemData.m_listNumber.push_back(number);
        }
    }

    CEditListItemPtr pPagerItem = m_pDetailLayout->GetPagerEditItem();
    if (NULL != pPagerItem)
    {
        stNumber number;
        number.strName = kszGbBookpager;
        number.strValue = pPagerItem->GetValue().trimmed().toUtf8().data();
        if (!number.strValue.empty())
        {
            itemData.m_listNumber.push_back(number);
        }
    }

    CEditListItemPtr pFaxItem = m_pDetailLayout->GetFaxEditItem();
    if (NULL != pFaxItem)
    {
        stNumber number;
        number.strName = kszGbBookFax;
        number.strValue = pFaxItem->GetValue().trimmed().toUtf8().data();
        if (!number.strValue.empty())
        {
            itemData.m_listNumber.push_back(number);
        }
    }

    CEditListItemPtr pConfUrlItem = m_pDetailLayout->GetConfUrlEditItem();
    if (NULL != pConfUrlItem)
    {
        stNumber number;
        number.strName = kszGbconferenceURL;
        number.strValue = pConfUrlItem->GetValue().trimmed().toUtf8().data();
        if (!number.strValue.empty())
        {
            itemData.m_listNumber.push_back(number);
        }
    }

    CEditListItemPtr pPriContactNoItem = m_pDetailLayout->GetPriContactEditItem();
    if (NULL != pPriContactNoItem)
    {
        stNumber number;
        number.strName = kszGbBookprimaryContact;
        number.strValue = pPriContactNoItem->GetValue().trimmed().toUtf8().data();
        if (!number.strValue.empty())
        {
            itemData.m_listNumber.push_back(number);
        }
    }

    int nGroupId;
    CComboBoxListItemPtr pGroupItem = m_pDetailLayout->GetGroupComboBoxItem();
    if (NULL != pGroupItem)
    {
        nGroupId = pGroupItem->GetItemData();
    }
    else
    {
        nGroupId = kszGbNAGroupId;
    }

    itemData.m_nParentId = nGroupId;

    return true;
}

GBBOOK_OPERATION_TYPE CGenbandContactUIHelper::GetGABPreciseSearchType()
{
    if (NULL == m_pSearchLayout)
    {
        return PAB_GAB_NONE;
    }

    CComboBoxListItemPtr pSearchTypeItem = m_pSearchLayout->GetSearchTypeItem();

    if (pSearchTypeItem != NULL)
    {
        int iType = pSearchTypeItem->GetItemData();
        return (GBBOOK_OPERATION_TYPE)iType;
    }

    return PAB_GAB_NONE;
}

bool CGenbandContactUIHelper::GetGABPreciseSearchData(yl::string & strKey)
{
    if (NULL == m_pSearchLayout)
    {
        return false;
    }

    m_pSearchLayout->ReturnAllEdit();

    CEditListItemPtr pKeyItem = m_pSearchLayout->GetSearchKeyItem();

    if (NULL != pKeyItem)
    {
        strKey = pKeyItem->GetValue().toUtf8().data();
    }

    return true;
}

void CGenbandContactUIHelper::SetGABLastSearchData()
{
    if (m_pSearchLayout == NULL)
    {
        return;
    }
    CComboBoxListItemPtr pSearchTypeItem = m_pSearchLayout->GetSearchTypeItem();
    if (pSearchTypeItem != NULL)
    {
        int iType = (int)GAB_GetCurrentGabOptType();
        pSearchTypeItem->SetItemData(iType, 0);
    }

    CEditListItemPtr pKeyItem = m_pSearchLayout->GetSearchKeyItem();
    if (NULL != pKeyItem)
    {
        yl::string strKey = GAB_GetCurentGabKey();
        pKeyItem->SetValue(strKey.c_str());
    }
}

void CGenbandContactUIHelper::InitCopyLayout()
{
    if (NULL == m_pDetailLayout)
    {
        return;
    }

    ContactGBBookUnit itemData;

    yl::string strName;
    yl::string strFirstName;
    yl::string strLastName;
    yl::string strEmail;
    yl::string strBusinessNumber;
    yl::string strHomeNumber;
    yl::string strMobileNumber;
    yl::string strPagerNumber;
    yl::string strFaxNumber;
    yl::string strConfUriNumber;
    yl::string strPricontactNumber;

    GetAttributeByName(0, ATTR_NAME, strName);
    GetAttributeByName(0, ATTR_GENBAND_FIRSTNAME, strFirstName);
    GetAttributeByName(0, ATTR_GENBAND_LASTNAME, strLastName);
    GetAttributeByName(0, ATTR_GENBAND_EMAIL, strEmail);
    GetAttributeByName(0, TRID_BUSINESS_NUMBER, strBusinessNumber);
    GetAttributeByName(0, TRID_HOME_NUMBER, strHomeNumber);
    GetAttributeByName(0, TRID_MOBILE_NUMBER, strMobileNumber);
    GetAttributeByName(0, TRID_PAGER, strPagerNumber);
    GetAttributeByName(0, TRID_FAX, strFaxNumber);
    GetAttributeByName(0, TRID_CONFERENCE_URL, strConfUriNumber);
    GetAttributeByName(0, TRID_PRIMARYCONTACT, strPricontactNumber);


    DIRUI_INFO("InitCopyLayout Name:%s PricontactNumber:%s",
               strName.c_str(), strPricontactNumber.c_str());

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();
    if (NULL != pNameItem)
    {
        yl::string strDisplayName = strFirstName;
        if (!strLastName.empty())
        {
            strDisplayName.append(" ");
            strDisplayName.append(strLastName);
        }
        if (strDisplayName.empty())
        {
            strDisplayName = strName;
        }

        pNameItem->SetValue(toQString(strDisplayName, true));
    }

    CEditListItemPtr pFirstNameItem = m_pDetailLayout->GetFirstNameEditItem();
    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(toQString(strFirstName, true));
    }

    CEditListItemPtr pLastNameItem = m_pDetailLayout->GetLastNameEditItem();
    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(toQString(strLastName, true));
    }

    CEditListItemPtr pEmailItem = m_pDetailLayout->GetEmailEditItem();
    if (NULL != pEmailItem)
    {
        pEmailItem->SetValue(toQString(strEmail, true));
    }

    CEditListItemPtr pBussinessNumItem = m_pDetailLayout->GetBusinessNoEditItem();
    if (NULL != pBussinessNumItem)
    {
        pBussinessNumItem->SetValue(toQString(strBusinessNumber, true));
    }

    CEditListItemPtr pHomeNumItem = m_pDetailLayout->GetHomeNoEditItem();
    if (NULL != pHomeNumItem)
    {
        pHomeNumItem->SetValue(toQString(strHomeNumber, true));
    }

    CEditListItemPtr pMobileItem = m_pDetailLayout->GetMobileNoEditItem();
    if (NULL != pMobileItem)
    {
        pMobileItem->SetValue(toQString(strMobileNumber, true));
    }

    CEditListItemPtr pPagerItem = m_pDetailLayout->GetPagerEditItem();
    if (NULL != pPagerItem)
    {
        pPagerItem->SetValue(toQString(strPagerNumber, true));
    }

    CEditListItemPtr pFaxItem = m_pDetailLayout->GetFaxEditItem();
    if (NULL != pFaxItem)
    {
        pFaxItem->SetValue(toQString(strFaxNumber, true));
    }

    CEditListItemPtr pConfURLItem = m_pDetailLayout->GetConfUrlEditItem();
    if (NULL != pConfURLItem)
    {
        pConfURLItem->SetValue(toQString(strConfUriNumber, true));
    }

    CEditListItemPtr pPriContactItem = m_pDetailLayout->GetPriContactEditItem();
    if (NULL != pPriContactItem)
    {
        pPriContactItem->SetValue(toQString(strPricontactNumber, true));
    }


}

void CGenbandContactUIHelper::LoadGenbandAdditionData(int nAction, int nDataIndex, int nDataCount)
{
    if (nAction == DIR_ACTION_GENBAND_ADD_PAB)
    {
        FillGenbandEdit(knInvalidRId, NULL, nDataIndex, nDataCount);
    }
}

bool CGenbandContactUIHelper::GetGenbandGroupData(yl::string strGroupName)
{
    return false;
}

void CGenbandContactUIHelper::SetGABGroupIsSelect()
{
    CGroupDelegatePtr pGroupDelegate = g_DirectoryMgr.GetGroupDelegate();
    CContactGroupDelegate * pContactGroupDelegate = CContactGroupDelegate::GetGroupDelegate(
                pGroupDelegate);

    if (NULL != pContactGroupDelegate)
    {
        pContactGroupDelegate->SetGABGroupIsSelect();
    }
}

bool CGenbandContactUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount)
{
    if (DIR_UI_STATE_BROWSER == nUIState || (DIR_UI_STATE_SETTING == nUIState && IsShowUpateTitile()))
    {
        m_pFrameList->SetTotalDataCount(nTotalCount);
        RefreshUpdateTitle(GetAllContactAmount());
        return true;
    }

    return CDirUIHelper::RefreshListItemAmount(nUIState, nFromIndex, nTotalCount);
}

#endif
