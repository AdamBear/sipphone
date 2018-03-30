#include "directorycommon.h"
#include "ldapcontactcontroler.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "diruiadapter.h"
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
#include "ildapuihelper.h"
#endif
#include "commonapi/stringhelperapi.h"
#include "configiddefine/configiddefine.h"
#include "configparser/modconfigparser.h"


typedef YLList<LdapContactData> LIST_LDAP_CONTACT_DATA;

CLDAPContactControler::CLDAPContactControler()
    : CDirectoryControler(DIRECTORY_TYPE_LDAP)
{
}

CLDAPContactControler::~CLDAPContactControler()
{
}

CLDAPContactControler * CLDAPContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_LDAP != pControler->GetType())
    {
        return NULL;
    }

    return (CLDAPContactControler *)pControler.Get();
}

bool CLDAPContactControler::IsEmptyKeySearch() const
{
    return true;
}

bool CLDAPContactControler::PreSearchContact()
{
    const yl::string & strSearchKey = GetSearchKey();

    DIRUI_INFO("LDAP_ReqSearch(%s)", strSearchKey.c_str());

    m_strLastSearchKey = strSearchKey;

    // 通过搜索消息返回
    if (LDAP_ReqSearch(strSearchKey))
    {
        if (!m_pUIHelper.IsEmpty())
        {
            m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                   DIR_UI_REFRESH_PARAM_ALL);
        }

        return false;
    }

    return true;
}

#if DIR_UI_FEATURE_LOAD_DATA_LIST
bool CLDAPContactControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    return false;
}

bool CLDAPContactControler::LoadListData(int nDataIndex, int nDataCount)
{
    ILDAPUIHelper * pLDAPUIHelper = g_DirUIAdapter.GetLDAPUIHelper(m_pUIHelper);

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

    return pLDAPUIHelper->FillLDAPContactList(listData);
}
#endif

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
                DIRUI_ERR("GetDetailByIndex(%d) fail in CLDAPContactControler::OnAction", nId);
                return false;
            }

            m_nDetailStatus = GetState();

#ifdef DIR_SUPPORT_SWITCH_DETAIL
            SetCurIndex(nDataIndex);
#endif

            UpdateDetailData(ldapContact);

            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_DETAIL, nAction, nId);
        }
        break;
    case DIR_ACTION_COMMON_SEND:
        {
            DialContact(nId);
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS:
        {
            OnCopyContact(nId, DIR_ACTION_LOCAL_COPY_TO_CONTACTS, kszAllContactId);
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST:
        {
            OnCopyContact(nId, DIR_ACTION_LOCAL_COPY_TO_BLACKLIST, kszBlackListGroupId);
        }
        break;
#if IF_FEATURE_34162
    case DIR_ACTION_LDAP_CHG_PWD:
        {
            LDAP_ReSetLdapOption();

            g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_LDAP_CHG_PWD, false);

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
    }

    return true;
}

bool CLDAPContactControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    LdapContactData ldapContact;

    if (!GetDetailByIndex(nContactId, ldapContact))
    {
        return false;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;
    bool bDisplayNumber = (0 == configParser_GetConfigInt(kszLdapNumberDisplayMode));//号码属性显示是否为Number
    int nNumberCount = ldapContact.m_listNumber.size();
    for (int i = 0; i < nNumberCount; ++i)
    {
        stAttr.m_strName = bDisplayNumber ? _UILOGIC_LANG_TRAN(TRID_NUMBER) : ldapContact.GetNameByIndex(i);
        if (bDisplayNumber && nNumberCount > 1)
        {
            stAttr.m_strName = commonAPI_FormatString("%s%d", stAttr.m_strName.c_str(), i + 1);
        }
        stAttr.m_strValue = ldapContact.GetNumberByIndex(i);
        lsAttr.push_back(stAttr);
    }

    g_DirUIAdapter.Dial(nAccountId, ldapContact.m_strDisplayName, lsAttr, GetDialAction());

    return true;
}

bool CLDAPContactControler::GetContactShowName(yl::string & strName)
{
    VEC_DETAIL_DATA::iterator iter = m_vecDetail.begin();

    for (; iter != m_vecDetail.end(); ++iter)
    {
        ST_DETAIL_DATA & stDetail = *iter;

        if (DETAIL_DATA_TYPE_NONE == stDetail.nType)
        {
            strName = stDetail.stAttr.m_strValue;

            return true;
        }
    }

    return false;
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
        UpdateDetailData(*iter);
        SetCurIndex(nIndex);
        return true;
    }

    return false;
}
#endif

int CLDAPContactControler::GetBrowseItemAmount()
{
    return DIR_INVALID_AMOUNT;
}

int CLDAPContactControler::GetSearchItemAmount()
{
    LdapListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 1;

    LDAP_GetContactList(&listData, GetSearchKey());

    return listData.m_nTotalCount;
}

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
            g_DirUIAdapter.ExitAllUI(m_nType);
        }

        return TRUE;
    }
    else if (LDAP_SRARCH_RESULT == refMessage.message)
    {
        char * pszSearchKey = (char *)refMessage.GetExtraData();

        if (NULL != pszSearchKey && (0 == strcmp(pszSearchKey, m_strLastSearchKey.c_str())))
        {
            if (!m_pUIHelper.IsEmpty())
            {
                m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                       DIR_UI_REFRESH_PARAM_ALL);
            }
        }

#if IF_FEATURE_34162
        if (0 != refMessage.lParam)
        {
            yl::string strError;

            LDAP_GetErrorStringByCode(refMessage.lParam, strError);

            bool bAuthError = LDAP_IsAuthError(refMessage.lParam);

            g_DirUIAdapter.ShowMsgBox(strError.c_str(), NULL,
                                      bAuthError ? DIR_ACTION_LDAP_ENTER_CHG_PWD : DIR_ACTION_NONE);
        }
#endif

        return TRUE;
    }

    return FALSE;
}

int CLDAPContactControler::GetContactAmount()
{
    LdapListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 1;

    LDAP_GetContactList(&listData, GetSearchKey());

    return listData.m_nTotalCount;
}

bool CLDAPContactControler::GetDetailByIndex(int nIndex, LdapContactData & ldapContact)
{
    LdapListData listData;

    listData.m_nIndexFrom = nIndex;
    listData.m_nCountToGet = 1;

    LDAP_GetContactList(&listData, m_strLastSearchKey);

    LIST_LDAP_CONTACT_DATA::iterator iter = listData.m_listData.begin();

    if (iter != listData.m_listData.end())
    {
        ldapContact = *iter;
        return true;
    }

    return false;
}

void CLDAPContactControler::OnCopyContact(int nIndex, int nAction, int nGroupId)
{
    LdapContactData ldapContact;

    if (!GetDetailByIndex(nIndex, ldapContact))
    {
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = DIR_ATTR_NAME;
    stAttr.m_strValue = ldapContact.m_strDisplayName;
    lsAttr.push_back(stAttr);

    for (int nNumIndex = 0; nNumIndex < ldapContact.m_listNumber.size(); ++nNumIndex)
    {
        stAttr.m_strName = DIR_ATTR_NUMBER;
        stAttr.m_strValue = ldapContact.GetNumberByIndex(nNumIndex);
        lsAttr.push_back(stAttr);
    }

    SetAttributeList(m_nType, lsAttr);

    g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION, nAction, nGroupId);
}

void CLDAPContactControler::UpdateDetailData(LdapContactData & ldapContact)
{
    m_vecDetail.clear();

    ST_DETAIL_DATA stDetail;

    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NAME);

    if (ldapContact.m_strDisplayName.empty())
    {
        stDetail.stAttr.m_strValue = _UILOGIC_LANG_TRAN(TRID_NAME_UNKNOWN);
    }
    else
    {
        stDetail.stAttr.m_strValue = ldapContact.m_strDisplayName;
    }

    m_vecDetail.push_back(stDetail);

    bool bDisplayNumber = (0 == configParser_GetConfigInt(kszLdapNumberDisplayMode));//号码属性显示是否为Number
    int nNumberCount = ldapContact.m_listNumber.size();
    for (int i = 0; i < nNumberCount; ++i)
    {
        stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
        stDetail.stAttr.m_strName = bDisplayNumber ? _UILOGIC_LANG_TRAN(TRID_NUMBER) : ldapContact.GetNameByIndex(i);
        if (bDisplayNumber && nNumberCount > 1)
        {
            stDetail.stAttr.m_strName = commonAPI_FormatString("%s%d", stDetail.stAttr.m_strName.c_str(), i + 1);
        }
        stDetail.stAttr.m_strValue = ldapContact.GetNumberByIndex(i);
        m_vecDetail.push_back(stDetail);
    }
}

bool CLDAPContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
#if IF_FEATURE_34162
    case DIR_ACTION_LDAP_ENTER_CHG_PWD:
        {
            g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_LDAP, DIR_UI_STATE_LDAP_CHG_PWD, DIR_ACTION_LDAP_CHG_PWD,
                                   knInvalidRId);
            return true;
        }
        break;
#endif
    default:
        break;
    }

    return false;
}
