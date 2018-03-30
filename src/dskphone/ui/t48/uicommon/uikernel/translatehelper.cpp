#include "translatehelper.h"
#include "commonapi/stringhelperapi.h"

static const yl::string strOpenSubPageKey("OpenSubPage");
static const yl::string strLocalContactKey("local_contact_");
static const yl::string strRemotePhoneBookKey("remote_phonebook_");
static const yl::string strBDPhoneBookKey("broadsoft_phonebook_");
static const yl::string strBDCalllogKey("broadsoft_calllog_");
static const yl::string strLDAPKey("ldap");

static const yl::string strOpenDirKey("OpenDirectoryPage");
static const yl::string strOpenCalllogKey("OpenCallLogPage");
static const yl::string strRemoteMenuKey("remote_contact_list.xml");
static const yl::string strBDMenuKey("broadsoft_contact_list.xml");
static const yl::string strLDAPMenuKey("ldpa_contact_list.xml");
static const yl::string strBDCalllogMenuKey("broadsoft_calllog_list.xml");

// 远程联系人的菜单名.
static const char kszMenuDisplayNameRemote[] = "Remote DIR";
// BroadSoft联系人的菜单名.
static const char kszMenuDisplayNameBroadSoft[] = "BroadSoft PhoneBook";
// Ldap联系人的菜单名.
static const char kszMenuDisplayNameLdap[] = "LDAP";
// 远程CallLog的菜单名.
static const char kszMenuNameBSCallLog[] = "Broadsoft CallLog";

bool IsNeedTranslateByAction(TRAN_TEXT_TYPE eTranType,
                             const yl::string & strDisplayName,
                             const yl::string & strClickAction)
{
    bool bResult = true;

    switch (eTranType)
    {
    case TRAN_MENU_LIST:
    case TRAN_MENU_ICON:
        {
            yl::string strAction;
            yl::string strContent;
            if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickAction))
            {
                return bResult;
            }

            if (strAction == strOpenSubPageKey)
            {
                if (strContent.find(strLocalContactKey) != yl::string::npos)
                {
                    yl::string strIndex = strContent.substr(strLocalContactKey.size(),
                                                            strContent.size() - strLocalContactKey.size());
                    if (strIndex != "1" && strIndex != "2")
                    {
                        bResult = false;
                    }
                    else
                    {
                        bResult = true;
                    }
                }
                else if (strContent.find(strRemotePhoneBookKey) != yl::string::npos)
                {
                    bResult = false;
                }
                else if (strContent.find(strBDPhoneBookKey) != yl::string::npos)
                {
                    bResult = false;
                }
                else if (strContent.find(strLDAPKey) != yl::string::npos)
                {
                    bResult = false;
                }
                else if (strContent.find(strBDCalllogKey) != yl::string::npos)
                {
                    bResult = false;
                }
                else
                {
                    bResult = true;
                }
            }
            else if (strAction == strOpenDirKey)
            {
                if (strContent == strRemoteMenuKey && strDisplayName != kszMenuDisplayNameRemote)
                {
                    bResult = false;
                }
                else if (strContent == strBDMenuKey && strDisplayName != kszMenuDisplayNameBroadSoft)
                {
                    bResult = false;
                }
                else if (strContent == strLDAPMenuKey && strDisplayName != kszMenuDisplayNameLdap)
                {
                    bResult = false;
                }
                else
                {
                    // none
                }
            }
            else if (strAction == strOpenCalllogKey)
            {
                if (strContent == strBDCalllogMenuKey && strDisplayName != kszMenuNameBSCallLog)
                {
                    bResult = false;
                }
            }
            else
            {
                // none
            }
        }
        break;
    case TRAN_PAGE_BD_CALLLOG:
        {
            bResult = (strDisplayName == kszMenuNameBSCallLog);
        }
        break;
    case TRAN_PAGE_DIR:
        {
            bResult = (strDisplayName == kszMenuDisplayNameRemote
                       || strDisplayName == kszMenuDisplayNameLdap
                       || strDisplayName == kszMenuDisplayNameBroadSoft);
        }
        break;
    default:
        {
            bResult = true;
        }
        break;
    }

    return bResult;
}
