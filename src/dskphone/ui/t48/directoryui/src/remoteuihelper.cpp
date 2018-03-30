#include "remotecontactcontroler.h"
#include "remoteuihelper.h"
#include "directorymgr.h"
#include "settingdelegate.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "contactupdatelisttitle.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelist.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"

#include "baseui/framesearchtitle.h"
#include "remotelisttitle.h"


CRemoteUIHelper::CRemoteUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_REMOTE)
{
}

CRemoteUIHelper::~CRemoteUIHelper()
{
}

CRemoteUIHelper * CRemoteUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_REMOTE != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CRemoteUIHelper *)pUIHelper.Get();
}

bool CRemoteUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount)
{
    if (DIR_UI_STATE_BROWSER == nUIState || (DIR_UI_STATE_SETTING == nUIState && IsShowUpateTitile()))
    {
        RefreshUpdateTitle(GetAllContactAmount());
        return true;
    }

    return CDirUIHelper::RefreshListItemAmount(nUIState, nFromIndex, nTotalCount);
}

void CRemoteUIHelper::OnAttachFrameList()
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

    if (DIR_UI_STATE_BROWSER == nState)
    {
        InitUpdateTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_SETTING == nState)
    {
        InitUpdateTitle(false);
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

bool CRemoteUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    ContactRemoteUnit * pContact = CRemoteContactControler::GetRemoteContact(pDetail);

    DIRUI_INFO("CRemoteUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pContact, nDataIndex, nDataCount, m_pFrameList);

    if (NULL == pContact || NULL == m_pFrameList)
    {
        return false;
    }

    if (!m_pFrameList->IsCanReuseItem(CDetailListItem::GetDetailListItemType()))
    {
        m_pFrameList->ClearContent();
    }

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    QString strNumberTitle = LANG_TRANSLATE(TRID_NUMBER);
    YLVector<yl::string> vecValue;

    vecValue.push_back(pContact->GetDisplayName());

    int nSize = pContact->m_listNumber.size();
    int i = 0;

    for (; i < nSize; ++i)
    {
        const yl::string & strNumber = pContact->GetNumberByIndex(i);

        if (!strNumber.empty())
        {
            vecValue.push_back(strNumber);
        }
    }

    if (1 == vecValue.size())
    {
        vecValue.push_back(yl::string(""));
    }

    int nItemIndex = 0;

    nSize = vecValue.size();

    for (i = 0; i < nSize && nItemIndex < nDataCount; ++i)
    {
        if (i < nDataIndex)
        {
            continue;
        }

        QString strTitle;

        if (0 == i)
        {
            strTitle = LANG_TRANSLATE(TRID_NAME);
        }
        else if (2 == nSize)
        {
            strTitle = strNumberTitle;
        }
        else
        {
            strTitle = strNumberTitle + QString("%1").arg(i);
        }

        QString strValue = toQString(vecValue[i], true);

        RefreshDetailItem(nItemIndex, i, nSize, strTitle, strValue);
        ++nItemIndex;
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

bool CRemoteUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                   SmartPtr & pDetail)
{
    ContactRemoteUnit * pContact = CRemoteContactControler::GetRemoteContact(pDetail);

    if (NULL != pContact)
    {
        if (0 == nDataIndex)
        {
            CheckSettingContactTitle();
        }

        return FillRemoteContact(nId, nItemIndex, nDataIndex, nDataCount, pContact);
    }

    ContactRemoteMenu * pGroup = CRemoteContactControler::GetRemoteGroup(pDetail);

    if (NULL != pGroup)
    {
        DIRUI_INFO("RefreshGroupMenuItem(Id:%d ItemIndex:%d DataIndex:%d)", nId, nItemIndex, nDataIndex);

        QString strGroupName = toQString(pGroup->GetDisplayName(), true);

        return RefreshGroupMenuItem(nId, nItemIndex, nDataIndex, nDataCount, strGroupName);
    }

    DIRUI_ERR("CRemoteUIHelper::FillListItem(Id:%d ItemIndex:%d DataIndex:%d DataCount:%d Detail:%p) fail",
              nId, nItemIndex, nDataIndex, nDataCount, pDetail.Get());
    return false;
}

void CRemoteUIHelper::CheckSettingContactTitle()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

    if (NULL == pSettingDelegate)
    {
        return;
    }

    InitSettingTitle();

    int nTotalCount = GetAllContactAmount();

    pSettingDelegate->SetTotalCount(nTotalCount);

    RefreshSettingTitle(nTotalCount, pSettingDelegate->GetSelectCount());
}

bool CRemoteUIHelper::FillRemoteContact(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                        ContactRemoteUnit * pContact)
{
    DIRUI_INFO("FillRemoteContact(Id:%d ItemIndex:%d DataIndex:%d TotalCount:%d MaxItemCount:%d)",
               nId, nItemIndex, nDataIndex, m_pFrameList->GetTotalDataCount(),
               m_pFrameList->GetMaxPageItemCount());

    if (NULL == pContact)
    {
        return false;
    }

    yl::string strShowNumber = "";

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CRemoteContactControler * pRemoteContactControll = CRemoteContactControler::GetControler(pControler);
    if (NULL != pRemoteContactControll)
    {
        if (pRemoteContactControll->IsSearchState())
        {
            strShowNumber = pContact->GetSearchShowNumber(pRemoteContactControll->GetSearchKey());
        }
    }

    if (strShowNumber.empty())
    {
        int nSize = pContact->m_listNumber.size();

        for (int i = 0; i < nSize; ++i)
        {
            const yl::string & strNumber = pContact->GetNumberByIndex(i);

            if (!strNumber.empty())
            {
                strShowNumber = strNumber;
                break;
            }
        }
    }

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strShowNumber, strHead);
}

void CRemoteUIHelper::InitSettingTitle()
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
#if 1//zzl
    int iBtnCount = 0;
    bool bShowAddContact = false;
    bool bShowAddBlackList = false;

    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, 0))
    {
        bShowAddContact = true;
        iBtnCount++;
    }
    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        bShowAddBlackList = true;
        iBtnCount++;
    }

    pContactListTitle->SetBtnCount(iBtnCount);
    if (bShowAddContact)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_TO));
    }
    if (bShowAddBlackList)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
    }
#else
    pContactListTitle->SetBtnCount(2);
    pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_TO));
    pContactListTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
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

void CRemoteUIHelper::OnGroupChange()
{
    RefreshTitle();
}

void CRemoteUIHelper::RefreshTitle()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CRemoteContactControler * pRemoteContact = CRemoteContactControler::GetControler(pControler);

    if (NULL == pRemoteContact)
    {
        return;
    }

    if (pRemoteContact->IsSettingState()
            && pRemoteContact->IsHadContactInGroup(pRemoteContact->GetGroupId()))
    {
        CheckSettingContactTitle();
        return;
    }

    if (!IsShowUpateTitile())
    {
        if (!pRemoteContact->IsSettingState() || !IsShowSettingTitle())
        {
            return;
        }

        InitUpdateTitle();
    }

    int nGroupId = pRemoteContact->GetGroupId();

    if (kszRootId == nGroupId)
    {
        const yl::string & strDisplayName = RemoteDir_GetRemoteDisplayName();

        if (strDisplayName.empty())
        {
            SetUpdateTitleGroupName(LANG_TRANSLATE(TRID_REMOTE_PHONEBOOK));
        }
        else
        {
            SetUpdateTitleGroupName(toQString(strDisplayName, true));
        }
    }
    else
    {
        yl::string strGroupName;

        if (!pRemoteContact->GetGroupTitle(nGroupId, strGroupName))
        {
            DIRUI_ERR("CRemoteContactControler::GetGroupTitle(%d) fail in OnGroupChange", nGroupId);
        }

        SetUpdateTitleGroupName(toQString(strGroupName, true));
    }
}

void CRemoteUIHelper::InitDetailTitle()
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
#if 1//zzl
    int iBtnCount = 1;
    bool bShowAddContact = false;
    bool bShowAddBlackList = false;

    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, 0))
    {
        bShowAddContact = true;
        iBtnCount++;
    }
    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        bShowAddBlackList = true;
        iBtnCount++;
    }

    pDetailTitle->SetBtnCount(iBtnCount);
    pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    if (bShowAddContact)
    {
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD));
    }
    if (bShowAddBlackList)
    {
        pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
    }
#else
    pDetailTitle->SetBtnCount(3);
    pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD));
    pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));
#endif
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


int CRemoteUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (LIST_TITLE_ACTION_CONTACT_UPDATE == nTitleAction &&
            (DIR_UI_STATE_BROWSER == nUIState || DIR_UI_STATE_SETTING == nUIState))
    {
        return DIR_ACTION_REMOTE_UPDATE;
    }

    if (LIST_TITLE_ACTION_MOVE_CONTACT == nTitleAction)
    {
        if (DIR_UI_STATE_DETAIL == nUIState)
        {
            return DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS;
        }
        else if (DIR_UI_STATE_SETTING == nUIState)
        {
            return DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT;
        }
    }

    if (LIST_TITLE_ACTION_ADD_BLACKLIST == nTitleAction)
    {
        if (DIR_UI_STATE_DETAIL == nUIState)
        {
            return DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST;
        }
        else if (DIR_UI_STATE_SETTING == nUIState)
        {
            return DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST;
        }
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}
