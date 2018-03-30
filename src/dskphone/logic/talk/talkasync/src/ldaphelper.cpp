#include "talkasync_inc.h"

////////////////////////////////////CHelperLDAPInTalk///////////////////////////////////
IMPLEMENT_GETINSTANCE(CHelperLDAPInTalk)

CHelperLDAPInTalk::CHelperLDAPInTalk()
{
}

CHelperLDAPInTalk::~CHelperLDAPInTalk()
{
}

// 设置搜索的CallIInfo
void CHelperLDAPInTalk::SetSearchCallInfo(LDAPSearchInfo* pSearchInfo)
{
    if (pSearchInfo == NULL)
    {
        return;
    }

    LST_INFO_IT it = yl::find(m_listLDAPInfo.begin(), m_listLDAPInfo.end(), pSearchInfo);
    if (it != m_listLDAPInfo.end())
    {
        delete *it;
        m_listLDAPInfo.erase(it);
    }

    m_listLDAPInfo.push_back(pSearchInfo);
}

bool CHelperLDAPInTalk::OnLdapCallback(const yl::string& strKey)
{
    bool bSpecialHandle = (configParser_GetConfigInt(kszLDAPSpecialSearch) == 1);
    bool bRet = false;

    LST_LDAP_INFO::iterator it = m_listLDAPInfo.begin();
    while (it != m_listLDAPInfo.end())
    {
        LDAPSearchInfo* pSearchInfo = *it;
        if (strKey == pSearchInfo->strSearchNum)
        {
            int iCallID = pSearchInfo->iCallID;

            //http://192.168.1.99/Bug.php?BugID=23529
            // 如果对应的CallInfo已不存在则不处理
            bool bSearchAgain = false;
            LdapContactData itemData;

            bool bHandled = LDAP_GetContactInfoByKey(strKey, itemData);

            if (bHandled && !itemData.m_strDisplayName.empty())
            {
                CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallID);
                if (pCallInfo != NULL)
                {
                    pCallInfo->m_tRemoteInfo.sDisplayName = itemData.m_strDisplayName;
                }
            }

            // 需要判断LDAP是否已经被talk处理
            if (bSpecialHandle && !pSearchInfo->m_bIsSecend
                    && !bHandled && !pSearchInfo->strSearchNum.empty())
            {
                if (pSearchInfo->strSearchNum[0] == '+')
                {
                    pSearchInfo->strSearchNum.replace(0, 1, "00");
                    bSearchAgain = LDAP_ReqSearch(pSearchInfo->strSearchNum);
                }
                else if (strncmp(pSearchInfo->strSearchNum.c_str(), "00", 2) == 0)
                {
                    pSearchInfo->strSearchNum.replace(0, 2, "+");
                    bSearchAgain = LDAP_ReqSearch(pSearchInfo->strSearchNum);
                }
            }

            bRet |= bHandled;

            if (bSearchAgain)
            {
                pSearchInfo->m_bIsSecend = true;
                ++it;
            }
            else
            {
                TALK_DBG("OnLdapCallback call[%d] second[%d] key[%s]",
                         pSearchInfo->iCallID, pSearchInfo->m_bIsSecend, pSearchInfo->strSearchNum.c_str());
                // 删除对应的信息
                delete pSearchInfo;
                it = m_listLDAPInfo.erase(it);
            }
        }
        else
        {
            ++it;
        }
    }

    if (bRet)
    {
        talklogic_UpdateWnd();
    }

    return bRet;
}

void CHelperLDAPInTalk::ClearByCallId(int iCallID)
{
    LST_INFO_IT it = m_listLDAPInfo.begin();
    for (; it != m_listLDAPInfo.end();)
    {
        LDAPSearchInfo* pSearchInfo = *it;

        if (pSearchInfo->iCallID == iCallID)
        {
            TALK_DBG("ClearByCallId call[%d] second[%d] key[%s]",
                     iCallID, pSearchInfo->m_bIsSecend, pSearchInfo->strSearchNum.c_str());

            delete *it;
            it = m_listLDAPInfo.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
