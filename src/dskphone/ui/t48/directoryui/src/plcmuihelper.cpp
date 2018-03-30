#include "plcmcontactcontroler.h"
#include "plcmuihelper.h"
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

#if IF_BUG_PLCMDIR
CPlcmUIHelper::CPlcmUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_PLCM)
{
}

CPlcmUIHelper::~CPlcmUIHelper()
{
}

CPlcmUIHelper * CPlcmUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_PLCM != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CPlcmUIHelper *)pUIHelper.Get();
}

bool CPlcmUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount)
{
    if (DIR_UI_STATE_BROWSER == nUIState || (DIR_UI_STATE_SETTING == nUIState && IsShowUpateTitile()))
    {
        RefreshUpdateTitle(GetAllContactAmount());
        return true;
    }

    return CDirUIHelper::RefreshListItemAmount(nUIState, nFromIndex, nTotalCount);
}

void CPlcmUIHelper::OnAttachFrameList()
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
        InitUpdateTitle();
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

bool CPlcmUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    ContactRemoteUnit * pContact = CPlcmContactControler::GetPlcmContact(pDetail);

    DIRUI_INFO("CPlcmUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
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

    for (; nItemIndex < nMaxPageTotalCount && nItemIndex < nSize; ++nItemIndex)
    {
        QString strTitle;

        if (0 == nItemIndex)
        {
            strTitle = LANG_TRANSLATE(TRID_NAME);
        }
        else if (2 == nSize)
        {
            strTitle = strNumberTitle;
        }
        else
        {
            strTitle = strNumberTitle + QString("%1").arg(nItemIndex);
        }

        QString strValue = toQString(vecValue[nItemIndex], true);

        RefreshDetailItem(nItemIndex, nItemIndex, nSize, strTitle, strValue);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

bool CPlcmUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                 SmartPtr & pDetail)
{
    ContactRemoteUnit * pContact = CPlcmContactControler::GetPlcmContact(pDetail);

    if (NULL != pContact)
    {
        if (0 == nDataIndex)
        {
            CheckSettingContactTitle();
        }

        return FillPlcmContact(nId, nItemIndex, nDataIndex, nDataCount, pContact);
    }

    ContactRemoteMenu * pGroup = CPlcmContactControler::GetPlcmGroup(pDetail);

    if (NULL != pGroup)
    {
        DIRUI_INFO("RefreshGroupMenuItem(Id:%d ItemIndex:%d DataIndex:%d)", nId, nItemIndex, nDataIndex);

        QString strGroupName = toQString(pGroup->GetDisplayName(), true);

        return RefreshGroupMenuItem(nId, nItemIndex, nDataIndex, nDataCount, strGroupName);
    }

    DIRUI_ERR("CPlcmUIHelper::FillListItem(Id:%d ItemIndex:%d DataIndex:%d DataCount:%d Detail:%p) fail",
              nId, nItemIndex, nDataIndex, nDataCount, pDetail.Get());
    return false;
}

void CPlcmUIHelper::CheckSettingContactTitle()
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

bool CPlcmUIHelper::FillPlcmContact(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                    ContactRemoteUnit * pContact)
{
    DIRUI_INFO("FillPlcmContact(Id:%d ItemIndex:%d DataIndex:%d TotalCount:%d MaxItemCount:%d)",
               nId, nItemIndex, nDataIndex, m_pFrameList->GetTotalDataCount(),
               m_pFrameList->GetMaxPageItemCount());

    if (NULL == pContact)
    {
        return false;
    }

    yl::string strShowNumber = "";

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CPlcmContactControler * pPlcmContactControll = CPlcmContactControler::GetControler(pControler);
    if (NULL == pPlcmContactControll)
    {
        if (pPlcmContactControll->IsSearchState())
        {
            strShowNumber = pContact->GetSearchShowNumber(pPlcmContactControll->GetSearchKey());
        }
    }

    if (strShowNumber.empty())
    {
        int nSize = pContact->m_listNumber.size();
        yl::string strShowNumber = "";

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

void CPlcmUIHelper::InitSettingTitle()
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

void CPlcmUIHelper::OnGroupChange()
{
    RefreshTitle();
}

void CPlcmUIHelper::RefreshTitle()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CPlcmContactControler * pPlcmContact = CPlcmContactControler::GetControler(pControler);

    if (NULL == pPlcmContact)
    {
        return;
    }

    if (!IsShowUpateTitile())
    {
        InitUpdateTitle();
    }

    if (!IsShowUpateTitile())
    {
        if (!pPlcmContact->IsSettingState() || !IsShowSettingTitle())
        {
            return;
        }

        InitUpdateTitle();
    }

    int nGroupId = pPlcmContact->GetGroupId();

    if (kszRootId == nGroupId)
    {
        SetUpdateTitleGroupName(LANG_TRANSLATE(TRID_PLCMBOOK));
    }
    else
    {
        yl::string strGroupName;

        if (!pPlcmContact->GetGroupTitle(nGroupId, strGroupName))
        {
            DIRUI_ERR("CPlcmContactControler::GetGroupTitle(%d) fail in OnGroupChange", nGroupId);
        }

        if (strGroupName.empty())
        {
            SetUpdateTitleGroupName(LANG_TRANSLATE(TRID_PLCMBOOK));
        }
        else
        {
            SetUpdateTitleGroupName(toQString(strGroupName, true));
        }
    }
}

void CPlcmUIHelper::InitDetailTitle()
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


int CPlcmUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        if (LIST_TITLE_ACTION_CONTACT_UPDATE == nTitleAction)
        {
            return DIR_ACTION_PLCM_UPDATE;
        }

        return DIR_ACTION_NONE;
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

void CPlcmUIHelper::RefreshSearchTitle(int nTotalCount)
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CDirectoryControlerPtr pControler = pDelegate->GetControler();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    if (NULL == m_pFrameList)
    {
        return;
    }

    if (0 == nTotalCount)
    {
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
    }
    else
    {

        CFrameSearchTitlePtr pTitle = (CFrameSearchTitlePtr)m_pFrameList->GetFrameListTitle();

        if (NULL != pTitle && (!CFrameSearchTitle::IsSearchTitle(pTitle)))
        {
            pTitle = NULL;
            m_pFrameList->SetFrameListTitle(NULL);
            m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
        }

        bool bAdd = false;

        if (NULL == pTitle)
        {
            bAdd = true;
            pTitle = new CFrameSearchTitle;
        }

        if (NULL != pTitle)
        {
            pTitle->SetSearchAmount(nTotalCount);
            if (bAdd)
            {
                m_pFrameList->SetFrameListTitle(pTitle);
                m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
            }
            else
            {
                pTitle->RefreshTitle();
            }
        }
    }
}

#endif
