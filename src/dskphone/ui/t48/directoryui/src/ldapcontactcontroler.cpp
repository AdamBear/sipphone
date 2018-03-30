#include "directorycommon.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "ldapuihelper.h"
#endif
#include "ldapcontactcontroler.h"
#include "translateiddefine.h"


CLDAPContactControler::CLDAPContactControler()
    : CDirectoryControler(DIRECTORY_TYPE_LDAP)
{
}

CLDAPContactControler::~CLDAPContactControler()
{
}

CLDAPUIHelper * CLDAPContactControler::GetLDAPUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CLDAPUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

CLDAPContactControler * CLDAPContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_LDAP != pControler->GetType())
    {
        return NULL;
    }

    return (CLDAPContactControler *)pControler.Get();
}

bool CLDAPContactControler::PreSearchContact()
{
    m_ItemInfo.m_strDisplayName = "";
    m_ItemInfo.m_listNumber.clear();
    m_listItem.clear();


    const yl::string & strSearchKey = GetSearchKey();

    DIRUI_INFO("LDAP_ReqSearch(%s)", strSearchKey.c_str());

    // 通过搜索消息返回
    if (LDAP_ReqSearch(strSearchKey))
    {
        CLDAPUIHelper * pLDAPUIHelper = GetLDAPUIHelper();

        if (NULL == pLDAPUIHelper)
        {
            return false;
        }
        pLDAPUIHelper->SetShowSearchTitle(false);
        CDirUIHelper::RefreshUI(true, true);
        return false;
    }

    return true;
}

bool CLDAPContactControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    return false;
}

bool CLDAPContactControler::LoadListData(int nDataIndex, int nDataCount)
{
    CLDAPUIHelper * pLDAPUIHelper = GetLDAPUIHelper();

    if (NULL == pLDAPUIHelper)
    {
        return false;
    }

    const yl::string & strSearchKey = GetSearchKey();
    LdapListData listData;

    listData.m_nIndexFrom = nDataIndex;
    listData.m_nCountToGet = nDataCount;

    LDAP_GetContactList(&listData, strSearchKey);

#ifdef DIR_SUPPORT_SWITCH_DETAIL
    SetTotalCount(listData.m_nTotalCount);
#endif

    bool bResult = pLDAPUIHelper->FillLDAPContactList(listData);

    m_listItem.clear();

    LIST_LDAP_CONTACT_DATA::iterator iter = listData.m_listData.begin();

    for (; iter != listData.m_listData.end(); iter++)
    {
        LdapContactData & ldapData = *iter;

        m_listItem.push_back(ldapData);
    }

    return bResult;
}

bool CLDAPContactControler::LoadDetailData(int nDataIndex, int nDataCount)
{
    CLDAPUIHelper * pLDAPUIHelper = GetLDAPUIHelper();

    if (NULL == pLDAPUIHelper)
    {
        return false;
    }

    return pLDAPUIHelper->FillLDAPContact(m_ItemInfo, nDataIndex, nDataCount);
}

bool CLDAPContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CLDAPContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_COMMON_DETAIL:
        {
            LdapContactData ldapContact;

            if (!GetDetailByIndex(nId, ldapContact))
            {
                DIRUI_ERR("GetDetailByIndex(%d) fail in CLDAPContactControler::OnAction Count:%d",
                          nId, m_listItem.size());
                return false;
            }

#ifdef DIR_SUPPORT_SWITCH_DETAIL
            SetCurIndex(nDataIndex);
            m_strLastSearchKey = GetSearchKey();
#endif

            m_ItemInfo = ldapContact;
            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_DETAIL, nAction, nId);
        }
        break;
    case DIR_ACTION_COMMON_SELECT:
        {
            DialContact(nId);
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS:
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST:
        {
            OnCopyContact(nAction, nId);
        }
        break;
#if IF_FEATURE_34162
    case DIR_ACTION_LDAP_CHG_PWD:
        {
            PreSearchContact();
        }
        break;
#endif
    default:
        {
            DIRUI_ERR("Action:%d is not be Processed in OnAction", nAction);
            return false;
        }
        break;
        break;
    }

    return true;
}

bool CLDAPContactControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    LdapContactData ldapContact;

    if (!GetDetailByIndex(nContactId, ldapContact))
    {
#ifdef DIR_SUPPORT_SWITCH_DETAIL
        if (nContactId != s_nCurIndex)
        {
            return false;
        }

        ldapContact = m_ItemInfo;
#else
        return false;
#endif
    }

    LIST_ATTRIBUTE lsNumber;
    m_ItemInfo = ldapContact;

    GetNumberAttr(lsNumber, ldapContact);

    CDirUIHelper::Dial(nAccountId, ldapContact.m_strDisplayName, lsNumber, this, NULL);

    return true;
}

bool CLDAPContactControler::GetNumberAttr(LIST_ATTRIBUTE & lsAttr, LdapContactData ldapContact)
{
    for (int nNumIndex = 0; nNumIndex < ldapContact.m_listNumber.size(); nNumIndex++)
    {
        ST_CONTACT_ATTRIBUTE stAttr;
        stAttr.m_strName = ldapContact.GetNameByIndex(nNumIndex);
        stAttr.m_strValue = ldapContact.GetNumberByIndex(nNumIndex);
        lsAttr.push_back(stAttr);
    }
    return true;
}

bool CLDAPContactControler::GetContactName(yl::string & strName)
{
    strName = m_ItemInfo.m_strDisplayName;

    return true;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
void CLDAPContactControler::GetDetailInfo(int & nId, int & nIndex, int & nCount) const
{
    nIndex = GetCurIndex();
    nId = nIndex;
    nCount = GetTotalCount();
}

bool CLDAPContactControler::GetContactImage(int nId, yl::string & strImage) const
{
    strImage = "";
    return true;
}

bool CLDAPContactControler::SwitchContact(int & nId, int & nIndex)
{
    LdapListData listData;

    listData.m_nIndexFrom = nIndex;
    listData.m_nCountToGet = 1;

    LDAP_GetContactList(&listData, m_strLastSearchKey);

    LIST_LDAP_CONTACT_DATA::iterator iter = listData.m_listData.begin();

    if (iter != listData.m_listData.end())
    {
        m_ItemInfo = *iter;
        SetCurIndex(nIndex);
        return true;
    }

    return false;
}
#endif

LRESULT CLDAPContactControler::OnMessage(msgObject & refMessage)
{
    if (refMessage.message < LDAP_MESSAGE_BEGIN || refMessage.message > LDAP_MESSAGE_END)
    {
        return FALSE;
    }

    DIRUI_INFO("CLDAPContactControler::OnMessage(msg:%d wp:%d lp:%d)",
               refMessage.message, refMessage.wParam, refMessage.lParam);

    if (LDAP_STATUS_CHANGED == refMessage.message)
    {
        if (0 == refMessage.lParam)
        {
            CDirUIHelper::ExitAllUI(m_nType);
        }

        return TRUE;
    }
    else if (LDAP_SRARCH_RESULT == refMessage.message)
    {
        char * pszSearchKey = (char *)refMessage.GetExtraData();

        if (NULL != pszSearchKey && (0 == strcmp(pszSearchKey, GetSearchKey().c_str())))
        {
            CDirUIHelper::RefreshUI(true, true);
        }

#if IF_FEATURE_34162
        if (refMessage.lParam != 0)
        {
            yl::string strError;
            LDAP_GetErrorStringByCode(refMessage.lParam, strError);
            bool bAuthError = LDAP_IsAuthError(refMessage.lParam);
            CDirUIHelper::ShowMsgBox(strError.c_str(), NULL,
                                     bAuthError ? DIR_ACTION_LDAP_ENTER_CHG_PWD : DIR_ACTION_NONE);
        }
#endif

        return TRUE;
    }

    return FALSE;
}

bool CLDAPContactControler::GetDetailByIndex(int nIndex, LdapContactData & ldapContact)
{
    LIST_LDAP_CONTACT_DATA::iterator iter = m_listItem.begin();

    for (int i = 0; iter != m_listItem.end(); iter++, ++i)
    {
        if (i == nIndex)
        {
            ldapContact = *iter;
            return true;
        }
    }

    return false;
}

void CLDAPContactControler::OnCopyContact(int nAction, int nIndex)
{
    LdapContactData ldapContact;

    if (!GetDetailByIndex(nIndex, ldapContact))
    {
#ifdef DIR_SUPPORT_SWITCH_DETAIL
        if (nIndex != s_nCurIndex)
        {
            return;
        }

        ldapContact = m_ItemInfo;
#else
        return;
#endif
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = ldapContact.m_strDisplayName;
    lsAttr.push_back(stAttr);

    for (int nNumIndex = 0; nNumIndex < ldapContact.m_listNumber.size(); nNumIndex++)
    {
        stAttr.m_strName = ATTR_NUMBER;
        stAttr.m_strValue = ldapContact.GetNumberByIndex(nNumIndex);
        lsAttr.push_back(stAttr);
    }

    CDirUIHelper::SetAttributeList(m_nType, lsAttr);

    CDirUIHelper::EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION, nAction, knInvalidRId);
}

#if IF_FEATURE_34162
bool CLDAPContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
    case DIR_ACTION_LDAP_ENTER_CHG_PWD:
        {
            CDirUIHelper::EnterUI(DIRECTORY_TYPE_LDAP, DIR_UI_STATE_LDAP_CHG_PWD, DIR_ACTION_LDAP_CHG_PWD,
                                  knInvalidRId);
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}
#endif
