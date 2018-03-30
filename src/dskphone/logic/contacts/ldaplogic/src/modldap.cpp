#include "modldap.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include <devicelib/phonedevice.h>
#include "ldap/yealink_ldap.h"
#include "translateiddefine.h"
#include "ldapmanager.h"

void LDAP_Init()
{
    g_refLdapManager.Init();
}

yl::string LDAP_GetName()
{
    // 获取LDAP名.
    return g_refLdapManager.GetName();
}

bool LDAP_ReqSearch(const yl::string & strKey)
{
    return g_refLdapManager.ReqSearch(strKey);
}

bool LDAP_GetContactList(LdapListData * pData, const yl::string & strKey)
{
    return g_refLdapManager.GetContactList(pData, strKey);
}

bool LDAP_GetContactInfoByKey(const yl::string & strKey, LdapContactData & stData)
{
    return g_refLdapManager.GetContactInfoByKey(strKey, stData);
}

bool LDAP_IsEnable()
{
    if (configParser_GetConfigInt(kszLDAPSwitch) == 1)
    {
        return true;
    }
    return false;
}

#if IF_FEATURE_34162
bool LDAP_IsAuthError(int nErrorCode)
{
    return (YEALINK_LDAP_INVALID_CREDENTIALS == nErrorCode
            || YEALINK_LDAP_INVALID_DN_SYNTAX == nErrorCode
            || YEALINK_LDAP_X_PROXY_AUTHZ_FAILURE == nErrorCode
            || YEALINK_LDAP_UNWILLING_TO_PERFORM == nErrorCode);
}

bool LDAP_GetErrorStringByCode(int nErrorCode, yl::string & strErrorTip)
{

    if (LDAP_IsAuthError(nErrorCode))
    {
        strErrorTip = TRID_AUTHORIZED_INCORRECT_RETRY;
        return true;
    }

    char cErrorTip[ERROR_BUF_MAX_LEN] = {0};
    switch (nErrorCode)
    {
    case YEALINK_LDAP_OPERATIONS_ERROR:
    case YEALINK_LDAP_PROTOCOL_ERROR:
    case YEALINK_LDAP_TIMELIMIT_EXCEEDED:
    case YEALINK_LDAP_ADMINLIMIT_EXCEEDED:
    case YEALINK_LDAP_BUSY:
    case YEALINK_LDAP_UNAVAILABLE:
        {
            int nRet = yealink_ldap_err2string(nErrorCode, cErrorTip, ERROR_BUF_MAX_LEN);
            if (0 == nRet)
            {
                strErrorTip = cErrorTip;
            }
            else
            {
                strErrorTip = TRID_LDAP_ERROR_TIPS_FAILED;
            }
        }
        break;
    default:
        strErrorTip = TRID_LDAP_SERVER_IS_UNAVAILABLE;
        break;
    }
    return true;
}

void LDAP_ReSetLdapOption()
{
    g_refLdapManager.ReSetLdapOption();
}
#endif

//获取LDAP联系人ID列表数据
bool LDAP_GetContactSuperSearchList(YLVector<SuperDirData> & output, const yl::string & strKey)
{
    return g_refLdapManager.GetContactSuperSearchList(output, strKey);
}
