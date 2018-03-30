#if IF_FEATURE_METASWITCH_DIRECTORY
#include <QtGui>
#include "mtswcontactuihelper.h"
#include "mtswcontactcontroler.h"
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
#include "uicommon.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include <resource/modresource.h>

namespace
{
#define MTSW_LIST_ITEM_COUNT 5
}


CMTSWContactUIHelper::CMTSWContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_METASWICTH)
    , m_pDetailLayout(NULL)
    , m_bDetailEditing(false)
{
}

CMTSWContactUIHelper::~CMTSWContactUIHelper()
{
}

CMTSWContactUIHelper * CMTSWContactUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_METASWICTH != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CMTSWContactUIHelper *)pUIHelper.Get();
}

void CMTSWContactUIHelper::GetMetaSwitchDisplayName(int nGroupId, const yl::string & strGroupName,
        QString & strDisplayName)
{
    //T48有All Contact 组 其他机型没有显示All Contact组
    if (kszAllContactId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_NETWORK_CONTACTS);
        return;
    }

    if (kszMtswAllContact == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_ALL_CONTACTS);
        return;
    }

    if (!strGroupName.empty())
    {
        strDisplayName = toQString(strGroupName, true);
        return;
    }

    if (kszContactList == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_MTSW_GROUP_NAME_CONTACTLIST);
    }
    else if (kszExtention == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_MTSW_GROUP_NAME_EXTENTION);
    }
    else if (kszMLHG == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_MTSW_GROUP_NAME_MLHG);
    }
    else if (kszMtswAllContact == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_ALL_CONTACTS);
    }
}

void CMTSWContactUIHelper::SetAdditionLayout(CMTSWDetailLayoutPtr pLayout)
{
    DIRUI_INFO("CMTSWContactUIHelper::SetAdditionLayout(%p)", pLayout);

    m_pDetailLayout = pLayout;

    if (NULL == m_pDetailLayout)
    {
        return;
    }

    if (m_pDetailLayout->IsCopyLayout())
    {
        InitCopyLayout();
    }
}

void CMTSWContactUIHelper::InitCopyLayout()
{
    if (NULL == m_pDetailLayout)
    {
        return;
    }

    yl::string strFirstName =  "";
    yl::string strLastName = "";
    yl::string strNumber1;
    yl::string strNumber2;

    GetAttributeByName(0, ATTR_MTSW_FIRST_NAME, strFirstName);
    GetAttributeByName(0, ATTR_MTSW_LAST_NAME, strLastName);
    GetAttributeByName(0, ATTR_MTSW_HOMENUMBER, strNumber1);
    GetAttributeByName(1, ATTR_MTSW_WORKNUMBER, strNumber2);

    DIRUI_INFO("InitCopyLayout FirstName:%s LastName:%s Number1:%s Number2:%s",
               strFirstName.c_str(), strLastName.c_str(), strNumber1.c_str(), strNumber2.c_str());

    CEditListItemPtr pFirstNameItem = m_pDetailLayout->GetFirstNameItem();

    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(toQString(strFirstName, true));
    }

    CEditListItemPtr pLastNameItem = m_pDetailLayout->GetLastNameItem();

    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(toQString(strLastName, true));
    }

    // 本地联系人复制界面赋值
    CComboBoxListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNumberComboBoxItem();
    CComboBoxListItemPtr pWorkNoItem = m_pDetailLayout->GetWorkNumberComboBoxItem();
    CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileComboBoxItem();
    //  CComboBoxListItemPtr pFaxNoItem = m_pDetailLayout->GetFaxNumberComboBoxItem();
    //  CComboBoxListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNumberComboBoxItem();


    if (NULL == pHomeNoItem || NULL == pWorkNoItem || NULL == pMobileNoItem)
    {
        return;
    }

    if (strNumber1.empty() && strNumber2.empty())
    {
        pHomeNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                              QVariant::fromValue<yl::string>(yl::string("")));
        pWorkNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                              QVariant::fromValue<yl::string>(yl::string("")));
        pMobileNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                                QVariant::fromValue<yl::string>(yl::string("")));
        //      pFaxNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
        //          QVariant::fromValue<yl::string>(yl::string("")));
        //      pOtherNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
        //          QVariant::fromValue<yl::string>(yl::string("")));
        return;
    }

    pHomeNoItem->ClearValue();

    if (strNumber1.empty())
    {
        pHomeNoItem->AddValue(toQString(strNumber2, true), QVariant::fromValue<yl::string>(strNumber2));
    }
    else
    {
        pHomeNoItem->AddValue(toQString(strNumber1, true), QVariant::fromValue<yl::string>(strNumber1));
    }

    pHomeNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                          QVariant::fromValue<yl::string>(yl::string("")));

    pHomeNoItem->SetCurrentIndex(0);

    pWorkNoItem->ClearValue();

    pWorkNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                          QVariant::fromValue<yl::string>(yl::string("")));

    if (strNumber2.empty())
    {
        pWorkNoItem->AddValue(toQString(strNumber1, true), QVariant::fromValue<yl::string>(strNumber1));
    }
    else
    {
        pWorkNoItem->AddValue(toQString(strNumber2, true), QVariant::fromValue<yl::string>(strNumber2));
    }

    pWorkNoItem->SetCurrentIndex(0);

    pMobileNoItem->ClearValue();

    pMobileNoItem->AddValue(LANG_TRANSLATE(TRID_OLD_NUMBER),
                            QVariant::fromValue<yl::string>(yl::string("")));

    if (strNumber1.empty())
    {
        pMobileNoItem->AddValue(toQString(strNumber2, true), QVariant::fromValue<yl::string>(strNumber2));
    }
    else
    {
        pMobileNoItem->AddValue(toQString(strNumber1, true), QVariant::fromValue<yl::string>(strNumber1));
    }

    pMobileNoItem->SetCurrentIndex(0);
}



void CMTSWContactUIHelper::SetDetailLayout(CMTSWDetailLayoutPtr pDetailLayout)
{
    m_pDetailLayout = pDetailLayout;

    if (NULL == m_pDetailLayout)
    {
        return;
    }
}

void CMTSWContactUIHelper::SetInputFocus(const char * pszAttr)
{
    if (NULL == pszAttr)
    {
        return;
    }

    if (0 == strcmp(pszAttr, ATTR_NAME))
    {
        CEditListItem * pNameItem = NULL;

        if (NULL != m_pDetailLayout)
        {
            pNameItem = m_pDetailLayout->GetFirstNameItem();
        }

        if (NULL != pNameItem)
        {
            setCurrentPageForItem(pNameItem);
            pNameItem->FocusItem(true);
        }
        return;
    }

    if (0 == strcmp(pszAttr, ATTR_MTSW_HOMENUMBER))
    {
        CEditListItem * pNumberItem = NULL;

        if (NULL != m_pDetailLayout)
        {
            pNumberItem = m_pDetailLayout->GetHomeNumberItem();
        }

        if (NULL != pNumberItem)
        {
            setCurrentPageForItem(pNumberItem);
            pNumberItem->FocusItem(true);
        }
    }
    else if (0 == strcmp(pszAttr, ATTR_MTSW_WORKNUMBER))
    {
        CEditListItem * pNumberItem = NULL;

        if (NULL != m_pDetailLayout)
        {
            pNumberItem = m_pDetailLayout->GetWorkNumberItem();
        }

        if (NULL != pNumberItem)
        {
            setCurrentPageForItem(pNumberItem);
            pNumberItem->FocusItem(true);
        }
    }
    else if (0 == strcmp(pszAttr, ATTR_MTSW_MOBILENUMBER))
    {
        CEditListItem * pNumberItem = NULL;

        if (NULL != m_pDetailLayout)
        {
            pNumberItem = m_pDetailLayout->GetMobileItem();
        }

        if (NULL != pNumberItem)
        {
            setCurrentPageForItem(pNumberItem);
            pNumberItem->FocusItem(true);
        }
    }
    else if (0 == strcmp(pszAttr, ATTR_MTSW_FAXNUMBER))
    {
        CEditListItem * pNumberItem = NULL;

        if (NULL != m_pDetailLayout)
        {
            pNumberItem = m_pDetailLayout->GetFaxNumberItem();
        }

        if (NULL != pNumberItem)
        {
            setCurrentPageForItem(pNumberItem);
            pNumberItem->FocusItem(true);
        }
    }
    else  if (0 == strcmp(pszAttr, ATTR_MTSW_OTHERNUMBER))
    {
        CEditListItem * pNumberItem = NULL;

        if (NULL != m_pDetailLayout)
        {
            pNumberItem = m_pDetailLayout->GetOtherNumberItem();
        }

        if (NULL != pNumberItem)
        {
            setCurrentPageForItem(pNumberItem);
            pNumberItem->FocusItem(true);
        }
    }
}

void CMTSWContactUIHelper::OnDetailCtrlFocus(bool bFocus)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact || !pMTSWContact->IsDetailState())
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

void CMTSWContactUIHelper::InitSettingTitle()
{
    DIRUI_INFO("CMTSWContactUIHelper::InitSettingTitle");

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
#if IF_FEATURE_GBBOOK
    pContactListTitle->SetCheckable(true);
#endif

    int nGroupId = pControler->GetGroupId();

    if (Dir_GetFirstChildTypeByGroupId(nGroupId, MTSW_DIR) == DIR_INHERIT_CONTACT
            && (nGroupId == kszContactList || nGroupId == kszMtswAllContact))
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
    else if (nGroupId == kszExtention || nGroupId == kszMLHG)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_TO));
    }


    //
    DIRUI_INFO("bAdd:%d", bAdd);
    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pContactListTitle);
    }
    else
    {
        pContactListTitle->RefreshTitle();
    }
}

void CMTSWContactUIHelper::InitDetailTitle()
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
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact)
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

    int nGroupId = pMTSWContact->GetGroupId();
    int nContactId = pMTSWContact->GetContactId();

    if (pMTSWContact->IsContactList(nContactId))
    {
        pDetailTitle->SetBtnCount(2);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        if ((nGroupId == kszContactList
                && DIR_INHERIT_CONTACT == Dir_GetFirstChildTypeByGroupId(kszContactList, MTSW_DIR))
                || nGroupId == kszMtswAllContact
                || nGroupId == kszAllContactId)
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
        }
    }
    else
    {
        pDetailTitle->SetBtnCount(2);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_TO_PHONEBOOK));
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

int CMTSWContactUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        if (LIST_TITLE_ACTION_CONTACT_UPDATE == nTitleAction)
        {
            return DIR_ACTION_MTSW_UPDATE;
        }
        else if (LIST_TITLE_ACTION_CONTACT_SORT == nTitleAction)
        {
            return DIR_ACTION_MTSW_SORT;
        }

        return DIR_ACTION_NONE;
    }

    switch (nTitleAction)
    {
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_MTSW_BATCH_DEL_CONTACT;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_MTSW_DEL_CONTACTLIST;
            }
        }
        break;
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_MTSW_BATCH_MOVE_TO_CONTACT;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_MTSW_ADD_TO_LOCAL;
            }
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

void CMTSWContactUIHelper::CheckSettingTitle()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

    if (NULL == pSettingDelegate)
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact)
    {
        return;
    }

    InitSettingTitle();


    yl::string strGroupName;

    int nGroupId = pMTSWContact->GetGroupId();

    pMTSWContact->GetGroupName(nGroupId, strGroupName);

    int nTotalCount = GetAllContactAmount();

    if (nGroupId == kszContactList)
    {
//      DIR_INHERIT_TYPE eType = Dir_GetFirstChildTypeByGroupId(nGroupId, MTSW_DIR);
//      if (eType == DIR_INHERIT_GROUP)
//      {
//          strGroupName = "Contact groups";
//      }

        ContactListData pData;
        Dir_GetContactListByGroupId(&pData, nGroupId, MTSW_DIR);
        nTotalCount = pData.m_listId.size();
    }

    SetUpdateTitleGroupName(toQString(strGroupName, true));

    RefreshUpdateTitle(nTotalCount);
}

bool CMTSWContactUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount)
{
    if (DIR_UI_STATE_BROWSER == nUIState || (DIR_UI_STATE_SETTING == nUIState && IsShowUpateTitile()))
    {
        m_pFrameList->SetTotalDataCount(nTotalCount);
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

        if (NULL != pMTSWContact
                && pMTSWContact->GetGroupId() == kszContactList)
        {
            OnGroupChange();

            if (Dir_GetFirstChildTypeByGroupId(kszContactList, MTSW_DIR) == DIR_INHERIT_GROUP)
            {
                RefreshUpdateTitle(nTotalCount);
                return true;
            }
        }

        RefreshUpdateTitle(GetAllContactAmount());
        return true;
    }

    return CDirUIHelper::RefreshListItemAmount(nUIState, nFromIndex, nTotalCount);
}

void CMTSWContactUIHelper::OnAttachFrameList()
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

void CMTSWContactUIHelper::OnGroupChange()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact)
    {
        return;
    }

    int nGroupId = pMTSWContact->GetGroupId();

    DIRUI_INFO("CMTSWContactUIHelper::OnGroupChange() GroupId:%d Setting:%d",
               nGroupId, (pMTSWContact->IsSettingState()));

    if (pMTSWContact->IsSettingState())
    {
        CheckSettingTitle();
        return;
    }
    else if (!IsShowUpateTitile())
    {
        return;
    }
    else
    {
        InitUpdateTitle();
    }

    yl::string strGroupName;

    if (kszAllContactId != nGroupId && !pMTSWContact->GetGroupName(nGroupId, strGroupName))
    {
        DIRUI_ERR("CMTSWContactControler::GetGroupName(%d) fail in OnGroupChange", nGroupId);
    }

    QString strDisplayName;

    GetMetaSwitchDisplayName(nGroupId, strGroupName, strDisplayName);

//  if (nGroupId == kszContactList)
//  {
//      DIR_INHERIT_TYPE eType = Dir_GetFirstChildTypeByGroupId(nGroupId, MTSW_DIR);
//      if (eType == DIR_INHERIT_GROUP)
//      {
//          strDisplayName = LANG_TRANSLATE("Contact groups");
//      }
//  }

    SetUpdateTitleGroupName(strDisplayName, true);
}

bool CMTSWContactUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CMTSWContactUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pDetail.Get(), nDataIndex, nDataCount, m_pFrameList);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    switch (pDetail->GetType())
    {
    case DIR_TYPE_METASWITCH_ITEM_CONTACTLIST:
        {
            return FillContactDetail(nId, (ContactMetaSwitchContactList *)pDetail.Get(), nDataIndex,
                                     nDataCount);
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_EXTENTION:
        {
            return FillExtentionDetail((ContactMetaSwitchExtention *)pDetail.Get());
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_MLHG:
        {
            return FillMLHGDetail((ContactMetaSwitchMLHG *)pDetail.Get());
        }
        break;
    default:
        {
            DIRUI_ERR("CMTSWContactUIHelper::FillDetail fail, Type=%d", pDetail->GetType());
        }
        break;
    }
    return false;
}

bool CMTSWContactUIHelper::FillNormalDetail(int nDataIndex, int nTotalCount,
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

        if (TRID_MTSW_HOME_NUMBER == stAttr.m_strName)
        {
            strData = ATTR_MTSW_HOMENUMBER;
        }
        else if (TRID_MTSW_WORK_NUMBER == stAttr.m_strName)
        {
            strData = ATTR_MTSW_WORKNUMBER;
        }
        else if (TRID_MTSW_MOBILE_NUMBER == stAttr.m_strName)
        {
            strData = ATTR_MTSW_MOBILENUMBER;
        }
        else if (TRID_MTSW_FAX_NUMBER == stAttr.m_strName)
        {
            strData = ATTR_MTSW_FAXNUMBER;
        }
        else if (TRID_OTHER_NUMBER == stAttr.m_strName)
        {
            strData = ATTR_MTSW_OTHERNUMBER;
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

bool CMTSWContactUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                        SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    int nDetailType = pDetail->GetType();

    if (DIR_TYPE_GROUP == nDetailType || DIR_TYPE_METASWITCH_MENU == nDetailType)
    {
        QString strDisplayName;

        GetMetaSwitchDisplayName(nId, pDetail->GetDisplayName(), strDisplayName);

        return RefreshGroupMenuItem(nId, nItemIndex, nDataIndex, nDataCount, strDisplayName);
    }

    switch (nDetailType)
    {
    case DIR_TYPE_METASWITCH_ITEM_CONTACTLIST:
        {
            return FillContactListItem(nId, nItemIndex, nDataIndex, nDataCount,
                                       (ContactMetaSwitchContactList *)pDetail.Get());
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_EXTENTION:
        {
            return FillExtentionItem(nId, nItemIndex, nDataIndex, nDataCount,
                                     (ContactMetaSwitchExtention *)pDetail.Get());
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_MLHG:
        {
            return FillMLHGItem(nId, nItemIndex, nDataIndex, nDataCount,
                                (ContactMetaSwitchMLHG *)pDetail.Get());
        }
        break;
    default:
        break;
    }

    DIRUI_ERR("CMTSWContactUIHelper::FillListItem(Id:%d ItemIndex:%d DataIndex:%d DataCount:%d Detail:%p) fail",
              nId, nItemIndex, nDataIndex, nDataCount, pDetail.Get());
    return false;
}

bool CMTSWContactUIHelper::FillContactListItem(int nId, int nItemIndex, int nDataIndex,
        int nDataCount, ContactMetaSwitchContactList * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum = "";

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMtswContact = CMTSWContactControler::GetControler(pControler);
    if (NULL != pMtswContact)
    {
        if (pMtswContact->IsSearchState())
        {
            strNum = pContact->GetSearchShowNumber(pMtswContact->GetSearchKey());
        }
    }

    if (strNum.empty())
    {
        if (!pContact->GetNumberByName(kszMtswHomeNumber, strNum) || strNum.empty())
        {
            if (!pContact->GetNumberByName(kszMtswWorkNumber, strNum) || strNum.empty())
            {
                if (!pContact->GetNumberByName(kszMtswWorkNumber, strNum) || strNum.empty())
                {
                    if (!pContact->GetNumberByName(kszMtswMobile, strNum) || strNum.empty())
                    {
                        if (!pContact->GetNumberByName(kszMtswFax, strNum) || strNum.empty())
                        {
                            pContact->GetNumberByName(kszMtswOtherNumber, strNum);
                        }
                    }
                }
            }
        }
    }

    yl::string strHead;
    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

bool CMTSWContactUIHelper::FillExtentionItem(int nId, int nItemIndex, int nDataIndex,
        int nDataCount, ContactMetaSwitchExtention * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum;

    if (!pContact->GetNumberByName(kszMtswExtention, strNum) || strNum.empty())
    {
        pContact->GetNumberByName(kszMtswDirectoryNumber, strNum);
    }

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

bool CMTSWContactUIHelper::FillMLHGItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                        ContactMetaSwitchMLHG * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum;


    pContact->GetNumberByName(kszMtswDirectoryNumber, strNum);

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

bool CMTSWContactUIHelper::FillContactDetail(int nId, ContactMetaSwitchContactList * pDetail,
        int nDataIndex, int nDataCount)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    DIRUI_INFO(" CMTSWContactUIHelper::FillContactDetail(Index:%d Count:%d)", nDataIndex, nDataCount);

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        DIRUI_ERR("CMTSWContactUIHelper::FillContactDetail fail when delegate is empty");
        return false;
    }

    if (pDetail != NULL)
    {
        m_pFrameList->SetMaxPageItemCount(MTSW_LIST_ITEM_COUNT + 1);
    }
    else
    {
        m_pFrameList->SetMaxPageItemCount(MTSW_LIST_ITEM_COUNT + 2);
    }


    if (NULL == m_pDetailLayout)
    {
        DIRUI_ERR("m_pDetailLayout is empty");
        return false;
    }

    DIRUI_INFO("CMTSWContactUIHelper::FillContactDetail TotalDataCount  [%d]",
               m_pFrameList->GetTotalDataCount());
    m_pFrameList->ClearContent();
    m_pFrameList->SetItemAttach();
    m_pFrameList->SetTotalDataCount(MTSW_DETAIL_ITEM_INDEX_MAX);
    DIRUI_INFO("CMTSWContactUIHelper::FillContactDetail TotalDataCount  [%d]",
               m_pFrameList->GetTotalDataCount());

    if (pDetail != NULL)
    {
        m_pDetailLayout->UpdateContact(nId, pDetail);
    }

    for (int i = 0; i < nDataCount && nDataIndex < MTSW_DETAIL_ITEM_INDEX_MAX; ++i, ++nDataIndex)
    {
        CDetailListItemBasePtr pItem = m_pDetailLayout->GetDetailListItem(nDataIndex);

        if (NULL == pItem)
        {
            DIRUI_INFO("NULL == pItem");
            continue;
        }

        if (0 == i)
        {
            pItem->SetDataIndex(INVALID_ITEM_INDEX);
        }

        pItem->SetAction(pDelegate.Get());
        pItem->SetDataIndex(nDataIndex);
        m_pFrameList->AddItem(pItem);

        DIRUI_INFO("AddItem(%p) ItemIndex:%d DataIndex=%d", pItem, i, nDataIndex);
    }

    if (pDetail == NULL)
    {
        m_pFrameList->FocusItemByIndex(0);
    }

    m_pFrameList->update();

    return true;
}

bool CMTSWContactUIHelper::FillExtentionDetail(ContactMetaSwitchExtention * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = TRID_NAME;
    stAttr.m_strValue = pContact->GetDisplayName();
    vecAttr.push_back(stAttr);

    stAttr.m_strValue = "";
    stAttr.m_strName = TRID_MTSW_NUMBER;
    pContact->GetNumberByName(kszMtswDirectoryNumber, stAttr.m_strValue);
    vecAttr.push_back(stAttr);

    stAttr.m_strName = TRID_DEPARTMENT;
    stAttr.m_strValue = pContact->m_strDepartment;
    vecAttr.push_back(stAttr);

    stAttr.m_strValue = "";
    stAttr.m_strName = TRID_MTSW_EXTENTION_NUMBER;
    pContact->GetNumberByName(kszMtswExtention, stAttr.m_strValue);
    vecAttr.push_back(stAttr);

    return FillNormalDetail(0, 4, vecAttr);
}

bool CMTSWContactUIHelper::FillMLHGDetail(ContactMetaSwitchMLHG * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = TRID_NAME;
    stAttr.m_strValue = pContact->GetDisplayName();
    vecAttr.push_back(stAttr);

    stAttr.m_strValue = "";
    stAttr.m_strName = TRID_MTSW_NUMBER;
    pContact->GetNumberByName(kszMtswDirectoryNumber, stAttr.m_strValue);
    vecAttr.push_back(stAttr);

    stAttr.m_strName = TRID_DEPARTMENT;
    stAttr.m_strValue = pContact->m_strDepartment;
    vecAttr.push_back(stAttr);

    return FillNormalDetail(0, 3, vecAttr);
}

bool CMTSWContactUIHelper::GetMTSWContactListData(ContactMetaSwitchContactList & itemData)
{
    if (m_pDetailLayout == NULL)
    {
        return false;
    }

    CEditListItemPtr pFirstNameItem = m_pDetailLayout->GetFirstNameItem();
    if (pFirstNameItem != NULL)
    {
        pFirstNameItem->ReturnAllInput();
        itemData.m_strFirstName = pFirstNameItem->GetValue().trimmed().toUtf8().data();
    }

    CEditListItemPtr pLastNameItem = m_pDetailLayout->GetLastNameItem();
    if (pLastNameItem != NULL)
    {
        pLastNameItem->ReturnAllInput();
        itemData.m_strLastName = pLastNameItem->GetValue().trimmed().toUtf8().data();
    }

    itemData.m_strDisplayName = itemData.m_strFirstName;
    if (!itemData.m_strFirstName.empty() && !itemData.m_strLastName.empty())
    {
        itemData.m_strDisplayName.append(",");
    }
    itemData.m_strDisplayName.append(itemData.m_strLastName);


    CEditListItemPtr pNickNameItem = m_pDetailLayout->GetNickNameItem();
    if (pNickNameItem != NULL)
    {
        pNickNameItem->ReturnAllInput();
        itemData.m_strNickName = pNickNameItem->GetValue().trimmed().toUtf8().data();
    }

    CEditListItemPtr pJobItem = m_pDetailLayout->GetJobTitleItem();
    if (pJobItem != NULL)
    {
        pJobItem->ReturnAllInput();
        itemData.m_strJobTitle = pJobItem->GetValue().trimmed().toUtf8().data();
    }

    CEditListItemPtr pOrgItem = m_pDetailLayout->GetOrgItem();
    if (pOrgItem != NULL)
    {
        pOrgItem->ReturnAllInput();
        itemData.m_strOrganisation = pOrgItem->GetValue().trimmed().toUtf8().data();
    }

    if (m_pDetailLayout->IsCopyLayout())
    {
        CComboBoxListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNumberComboBoxItem();
        if (NULL != pHomeNoItem)
        {
            itemData.SetNumber(kszMtswHomeNumber, pHomeNoItem->GetItemStringData());
        }

        CComboBoxListItemPtr pWorkNoItem = m_pDetailLayout->GetWorkNumberComboBoxItem();
        if (NULL != pWorkNoItem)
        {
            itemData.SetNumber(kszMtswWorkNumber, pWorkNoItem->GetItemStringData());
        }

        CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileComboBoxItem();
        if (NULL != pMobileNoItem)
        {
            itemData.SetNumber(kszMtswMobile, pMobileNoItem->GetItemStringData());
        }
    }
    else
    {
        CEditListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNumberItem();
        if (NULL != pHomeNoItem)
        {
            itemData.SetNumber(kszMtswHomeNumber, pHomeNoItem->GetValue().trimmed().toUtf8().data());
        }

        CEditListItemPtr pWorkNoItem = m_pDetailLayout->GetWorkNumberItem();
        if (NULL != pWorkNoItem)
        {
            itemData.SetNumber(kszMtswWorkNumber, pWorkNoItem->GetValue().trimmed().toUtf8().data());
        }

        CEditListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileItem();
        if (NULL != pMobileNoItem)
        {
            itemData.SetNumber(kszMtswMobile, pMobileNoItem->GetValue().trimmed().toUtf8().data());
        }
    }

    CEditListItemPtr pFaxNoItem = m_pDetailLayout->GetFaxNumberItem();
    if (NULL != pFaxNoItem)
    {
        itemData.SetNumber(kszMtswFax, pFaxNoItem->GetValue().trimmed().toUtf8().data());
    }

    CEditListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNumberItem();
    if (NULL != pOtherNoItem)
    {
        itemData.SetNumber(kszMtswOtherNumber, pOtherNoItem->GetValue().trimmed().toUtf8().data());
    }



    return true;
}

void CMTSWContactUIHelper::LoadMTSWAdditionData(int nAction, int nDataIndex, int nDataCount)
{
    if (nAction == DIR_ACTION_MTSW_ADD_CONTACTLIST)
    {
        FillContactDetail(knInvalidRId, NULL, nDataIndex, nDataCount);
    }
}

void CMTSWContactUIHelper::InitUpdateTitle()
{
    DIRUI_INFO("CMTSWContactUIHelper::InitUpdateTitle()");

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
    CContactUpdateListTitlePtr pUpdateTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactUpdateListTitle::IsContactUpdateListTitle(pTitle))
    {
        bAdd = false;
        pUpdateTitle = (CContactUpdateListTitlePtr)pTitle;
    }
    else
    {
        pUpdateTitle = new CContactUpdateListTitle;
        if (NULL == pUpdateTitle)
        {
            return;
        }
    }

    bool bShowSortIcon = false;
    bool bShowUpdateIcon = CDirectoryControler::IsDirCanEidt(DIR_MOD_TYPE_NONE);
    CDirectoryControlerPtr pControler = pDelegate->GetControler();

    if (!pControler.IsEmpty())
    {
        int nGroupId = pControler->GetGroupId();

        if (Dir_GetFirstChildTypeByGroupId(nGroupId, MTSW_DIR) == DIR_INHERIT_CONTACT
                && (nGroupId == kszContactList || nGroupId == kszMtswAllContact))
        {
            bShowSortIcon = true;
        }

        if ((nGroupId == kszContactList
                || nGroupId == kszMtswAllContact
                || nGroupId == kszExtention
                || nGroupId == kszMLHG
                || nGroupId == kszAllContactId)
                && CDirectoryControler::IsDirCanEidt(DIR_MOD_TYPE_NONE))
        {
            bShowUpdateIcon = true;
        }
        else
        {
            bShowUpdateIcon = false;
        }
    }

    pUpdateTitle->SetAction(pDelegate.Get());
    pUpdateTitle->SetUpdatable(bShowUpdateIcon, bShowSortIcon);

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pUpdateTitle);
    }
    else
    {
        pUpdateTitle->RefreshTitle();
    }
}

#endif //IF_FEATURE_METASWITCH_DIRECTORY
