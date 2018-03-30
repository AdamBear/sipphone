#include "ldapuihelper.h"
#include "directorymgr.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "ldapcontactcontroler.h"
#include "contactdetailtitle.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelist.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#if IF_BUG_31829
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#endif

#include "baseui/editlistitem.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "inputmethod/inputmethod.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#endif


CLDAPUIHelper::CLDAPUIHelper()
    : CDirUIHelper(DIRECTORY_TYPE_LDAP)
    , m_bShowSearchTitle(true)
{
}

CLDAPUIHelper::~CLDAPUIHelper()
{
}

CLDAPUIHelper * CLDAPUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_LDAP != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CLDAPUIHelper *)pUIHelper.Get();
}

bool CLDAPUIHelper::FillLDAPContactList(LdapListData & listData)
{
    if (NULL == m_pFrameList)
    {
        DIRUI_ERR("FillLDAPContactList when m_pFrameList is null");
        return false;
    }

#if IF_BUG_31829
    if (configParser_GetConfigInt(kszLDAPHintType) == 0)
    {
        RefreshListItemAmount(DIR_UI_STATE_SEARCH, listData.m_nIndexFrom, m_bShowSearchTitle ? listData.m_nTotalCountByName : 0);
    }
    else
#endif
    {
        RefreshListItemAmount(DIR_UI_STATE_SEARCH, listData.m_nIndexFrom, m_bShowSearchTitle ? listData.m_nTotalCount : 0);
    }
    m_bShowSearchTitle = true;

    if (0 == listData.m_listData.size())
    {
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

        if (!pDelegate.IsEmpty())
        {
            pDelegate->SetPageDataIndex(0);
        }

        m_pFrameList->ClearContent(true);
        return true;
    }

    LIST_LDAP_CONTACT_DATA::iterator iter = listData.m_listData.begin();
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();
    int nItemIndex = 0;
    yl::string strHead = "";

    for (; iter != listData.m_listData.end() && nItemIndex < nMaxPageItemCount; ++iter, ++nItemIndex)
    {
        int nDataIndex = listData.m_nIndexFrom + nItemIndex;
        LdapContactData & info = *iter;
        yl::string strName = info.m_strDisplayName;

        if (strName.empty())
        {
            QString strNameQ = LANG_TRANSLATE(TRID_NAME_UNKNOWN);

            strName = strNameQ.toUtf8().data();
        }

        yl::string strShowNumber = "";

        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CLDAPContactControler * pLdapContactControll = CLDAPContactControler::GetControler(pControler);
        if (NULL != pLdapContactControll)
        {
            if (pLdapContactControll->IsSearchState())
            {
                strShowNumber = info.GetSearchShowNumber(pLdapContactControll->GetSearchKey());
            }
        }

        if (strShowNumber.empty())
        {
            strShowNumber = (*info.m_listNumber.begin()).strValue;
        }

        RefreshContactItem(nItemIndex, nDataIndex, listData.m_nTotalCount, nItemIndex,
                           strName, strShowNumber, strHead);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

bool CLDAPUIHelper::FillLDAPContact(LdapContactData & info, int nDataIndex, int nDataCount)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    int nItemIndex = 0;
    int nTmpDataIndex = nDataIndex;
    int nTotalCount = info.m_listNumber.size() + 1;
    //number的属性名称显示为Number
    bool bDisplayNumber = configParser_GetConfigInt(kszLdapNumberDisplayMode) == 0;
    bool bResetNumer = (info.m_listNumber.size() > 1 && bDisplayNumber);

    for (; nItemIndex < nDataCount && nTmpDataIndex < nTotalCount; nItemIndex++, nTmpDataIndex++)
    {
        QString strTitle = "";
        QString strValue = "";

        if (0 == nDataIndex && 0 == nItemIndex)
        {
            //处理话机名字
            strTitle = LANG_TRANSLATE(TRID_NAME);
            strValue = toQString(info.m_strDisplayName, true);
        }
        else
        {
            //处理numbers
            strTitle = LANG_TRANSLATE(TRID_NUMBER);
            if (bResetNumer)
            {
                strTitle = strTitle + toQString(fmtString("%d", nItemIndex + nDataIndex));
            }
            else if (!bDisplayNumber)
            {
                //显示服务器推送属性名称则不翻译
                strTitle = toQString(info.GetNameByIndex(nItemIndex + nDataIndex - 1));
            }

            strValue = toQString(info.GetNumberByIndex(nItemIndex + nDataIndex - 1), true);
        }

        RefreshDetailItem(nItemIndex, (nDataIndex + nItemIndex), nTotalCount, strTitle,
                          strValue);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

void CLDAPUIHelper::OnAttachFrameList()
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

    if (DIR_UI_STATE_DETAIL == nState)
    {
        InitDetailTitle();
        return;
    }
    else if (DIR_UI_STATE_SEARCH == nState)
    {
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    CDirUIHelper::OnAttachFrameList();
}

void CLDAPUIHelper::InitDetailTitle()
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

    pDetailTitle->SetBtnCount(2);
    pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD));

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

int CLDAPUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (LIST_TITLE_ACTION_MOVE_CONTACT == nTitleAction)
    {
        return DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}
