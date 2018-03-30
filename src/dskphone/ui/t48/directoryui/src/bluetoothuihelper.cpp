#include "bluetoothcontactcontroler.h"
#include "bluetoothuihelper.h"
#include "directorymgr.h"
#include "settingdelegate.h"
#include "directorylistaction.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "contactupdatelisttitle.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelist.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"


CBluetoothUIHelper::CBluetoothUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_BLUETOOTH)
{
}

CBluetoothUIHelper::~CBluetoothUIHelper()
{
}

CBluetoothUIHelper * CBluetoothUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_BLUETOOTH != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CBluetoothUIHelper *)pUIHelper.Get();
}

bool CBluetoothUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount)
{
    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        RefreshUpdateTitle(GetAllContactAmount());
        return true;
    }

    return CDirUIHelper::RefreshListItemAmount(nUIState, nFromIndex, nTotalCount);
}

void CBluetoothUIHelper::OnAttachFrameList()
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

bool CBluetoothUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    LocalDirContactRecord * pRecord = CBluetoothContactControler::GetLocalContact(pDetail);

    DIRUI_INFO("CBluetoothUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pRecord, nDataIndex, nDataCount, m_pFrameList);

    if (NULL == pRecord || NULL == m_pFrameList)
    {
        return false;
    }

    if (!m_pFrameList->IsCanReuseItem(CDetailListItem::GetDetailListItemType()))
    {
        m_pFrameList->ClearContent();
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = TRID_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    vecAttr.push_back(stAttr);

    yl::string strNumber;

    if (pRecord->GetNumberByName(kszOfficeNumber, strNumber) && !strNumber.empty())
    {
        stAttr.m_strName = TRID_OFFICE_NUMBER;
        stAttr.m_strValue = strNumber;
        vecAttr.push_back(stAttr);
    }

    if (pRecord->GetNumberByName(kszMobileNumber, strNumber) && !strNumber.empty())
    {
        stAttr.m_strName = TRID_MOBILE_NUMBER;
        stAttr.m_strValue = strNumber;
        vecAttr.push_back(stAttr);
    }

    if (pRecord->GetNumberByName(kszOtherNumber, strNumber) && !strNumber.empty())
    {
        stAttr.m_strName = TRID_OTHER_NUMBER;
        stAttr.m_strValue = strNumber;
        vecAttr.push_back(stAttr);
    }

    int nItemIndex = 0;
    int nTotalCount = vecAttr.size();

    for (; nItemIndex < nTotalCount; ++nItemIndex)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = vecAttr[nItemIndex];
        QString strTitle = LANG_TRANSLATE(stAttr.m_strName.c_str());
        QString strValue = toQString(stAttr.m_strValue);

        RefreshDetailItem(nItemIndex, nItemIndex, nTotalCount, strTitle, strValue);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    return true;
}

bool CBluetoothUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                      SmartPtr & pDetail)
{
    LocalDirContactRecord * pRecord = CBluetoothContactControler::GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        return false;
    }

    yl::string strNumber = "";

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBluetoothContactControler * pBlutoothContact = CBluetoothContactControler::GetControler(pControler);
    if (NULL != pBlutoothContact)
    {
        if (pBlutoothContact->IsSearchState())
        {
            strNumber = pRecord->GetSearchShowNumber(pBlutoothContact->GetSearchKey());
        }
    }

    if (strNumber.empty())
    {
        if (!pRecord->GetNumberByName(kszOfficeNumber, strNumber) || strNumber.empty())
        {
            if (!pRecord->GetNumberByName(kszMobileNumber, strNumber) || strNumber.empty())
            {
                pRecord->GetNumberByName(kszOtherNumber, strNumber);
            }
        }
    }

    yl::string strHead = PIC_CONTACTS_ICON_BLUETOOTH_DEFAULT;

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pRecord->GetDisplayName(),
                              strNumber, strHead);
}

void CBluetoothUIHelper::CheckSettingContactTitle()
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

void CBluetoothUIHelper::InitSettingTitle()
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

    pContactListTitle->SetBtnCount(2);
    pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_TO));
    pContactListTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pContactListTitle);
    }
    else
    {
        pContactListTitle->RefreshTitle();
    }
}

void CBluetoothUIHelper::OnGroupChange()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBluetoothContactControler * pBluetoothContact = CBluetoothContactControler::GetControler(
                pControler);

    if (NULL == pBluetoothContact)
    {
        return;
    }

    if (pBluetoothContact->IsSettingState())
    {
        if (!IsShowSettingTitle())
        {
            CheckSettingContactTitle();
        }

        return;
    }

    if (!IsShowUpateTitile())
    {
        InitUpdateTitle();
    }

    SetUpdateTitleGroupName(LANG_TRANSLATE(TRID_MOBILE_CONTACTS));
}

void CBluetoothUIHelper::InitDetailTitle()
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

    pDetailTitle->SetBtnCount(3);
    pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD));
    pDetailTitle->AddButton(LIST_TITLE_ACTION_ADD_BLACKLIST, LANG_TRANSLATE(TRID_BLACKLIST));

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

int CBluetoothUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        if (LIST_TITLE_ACTION_CONTACT_UPDATE == nTitleAction)
        {
            return DIR_ACTION_BLUETOOTH_UPDATE;
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
