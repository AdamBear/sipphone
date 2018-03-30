#include "talkasync_inc.h"

using namespace NS_TA;

#define URL_HTTP_HEADER "http://"
#define URL_HTTPS_HEADER "https://"

#define URL_HEADER_MATCH(url, header) (strncmp(url, header, strlen(header)) == 0)

static LRESULT OnMSGReceive(msgObject& msg)
{
    switch (msg.message)
    {
    case LDAP_SRARCH_RESULT:
        {
            LPCSTR lpKey = (LPCSTR)msg.GetExtraData();
            if (lpKey != NULL)
            {
                TALK_DBG("LDAP_SRARCH_RESULT key[%s]", lpKey);
                return _HelperLDAPInTalk.OnLdapCallback(lpKey);
            }
        }
        break;
    case SIP_CALL_INFORMATION_UPDATE:
        {
            sip_call_infomation_header_t* pInfo = (sip_call_infomation_header_t*)msg.GetExtraData();
            if (pInfo == NULL)
            {
                return FALSE;
            }

            TALK_DBG("SIP_CALL_INFORMATION_UPDATE type[%d] data[%s] len[%d]",
                     pInfo->type, pInfo->payload, pInfo->payload_len);

            if (SIP_CALL_INFORMATION_IMAGE_URI == pInfo->type)
            {
                CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);
                if (NULL == pCallInfo)
                {
                    return FALSE;
                }

                yl::string strUrl = pInfo->payload;
                trim_both(strUrl, " \r\n");

                TALK_INFO("CallPic strUrl = %s", strUrl.c_str());

                pCallInfo->SetCallerImageURL(strUrl);
                return talkasync_CreateUpdateCallerImage(pCallInfo->GetCallID(), strUrl);
            }
            else if (SIP_CALL_INFORMATION_INFO == pInfo->type)
            {
                // http://192.168.1.99/Bug.php?BugID=19246
                CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);
                if (pCallInfo != NULL)
                {
                    char buf[256] = { 0 };
                    sscanf(pInfo->payload, "%[^\r\n]", buf);
#if IF_BUG_31481
                    pCallInfo->m_strCallTextInfo = buf;
                    trim_both(pCallInfo->m_strCallTextInfo);
                    TALK_INFO("pCallInfo->m_strCallTextInfo[%s]", pCallInfo->m_strCallTextInfo.c_str());
#else

                    pCallInfo->m_tRemoteInfo.sDisplayName = buf;
                    trim_both(pCallInfo->m_tRemoteInfo.sDisplayName);
                    TALK_INFO("pCallInfo->m_tRemoteInfo.sDisplayName[%s]",
                              pCallInfo->m_tRemoteInfo.sDisplayName.c_str());
#endif
                    talklogic_UpdateWnd();
                }
            }
            else if (SIP_CALL_INFORMATION_REFERRED_BY == pInfo->type)
            {
                CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);

                if (NULL == pCallInfo)
                {
                    return FALSE;
                }

                sip_user_info_t* pUserInfo = (sip_user_info_t*)pInfo->payload;

                if (pUserInfo != NULL)
                {
                    TALK_INFO("Referred By[%s]", pUserInfo->user_name);
                    pCallInfo->SetRelationInfo(CR_NONE, pUserInfo->user_name);
                }
            }
        }
        break;
    default:
        break;
    }

    return FALSE;
}


void talkasync_Init()
{
    _ASyncHelper;
    _HelperLDAPInTalk;

    SingleMsgReg(LDAP_SRARCH_RESULT, OnMSGReceive);
    SingleMsgReg(SIP_CALL_INFORMATION_UPDATE, OnMSGReceive);
}

void talkasync_UnInit()
{

}

bool talkasync_CreateUpdateCallerImage(int iCallID, const yl::string& strUrl)
{
    if (strUrl.empty())
    {
        return false;
    }

    if (URL_HEADER_MATCH(strUrl.c_str(), URL_HTTP_HEADER)
            || URL_HEADER_MATCH(strUrl.c_str(), URL_HTTPS_HEADER))
    {
        yl::string strType = strUrl;
        int index = strType.rfind(".");
        if (index != yl::string::npos)
        {
            strType = strType.substr(index);
            if (strcasecmp(strType.c_str(), ".png") != 0
                    && strcasecmp(strType.c_str(), ".jpg") != 0
                    && strcasecmp(strType.c_str(), ".jpeg") != 0
                    && strcasecmp(strType.c_str(), ".BMP") != 0
                    && strcasecmp(strType.c_str(), ".GIF") != 0)
            {
                strType.clear();
            }
        }
        else
        {
            strType.clear();
        }

        yl::string strPicTempPath = commonAPI_FormatString(TEMP_PATH "SIPPic%d%s", iCallID,
                                    strType.c_str());
        CSipPicAction* pAction = new CSipPicAction(strPicTempPath, strUrl);
        if (pAction != NULL)
        {
            pAction->SetRead(true);
            pAction->SetSync(false);  // 异步.
            _ASyncHelper.SetCallID(pAction, iCallID);
            pAction->SetCallback(&_ASyncHelper);

            return TaskAction_ExecAction(pAction);
        }
    }

    return false;
}

bool talkasync_CreateGetRingFile(int iCallID, const yl::string& strUrl)
{
    if (strUrl.empty())
    {
        return false;
    }

    // 开始下载
    CRingFileAction* pAction = TaskAction_CreateRingFileAction(strUrl.c_str());
    if (pAction != NULL)
    {
        pAction->SetSync(false);
        pAction->SetRead(true);
        _ASyncHelper.SetCallID(pAction, iCallID);
        pAction->SetCallback(&_ASyncHelper);

        return TaskAction_ExecAction(pAction);
    }
    return false;
}

bool talkasync_CreateLdapSearch(CCallInfo* pCallInfo)
{
    // 判断开关是否开启, 如果不开启, 则直接返回.
    if (pCallInfo == NULL || !LDAP_IsEnable())
    {
        return false;
    }

    const yl::string& strKey = pCallInfo->m_tRemoteInfo.sSIPName;

    if (LDAP_ReqSearch(strKey))
    {
        LDAPSearchInfo* pInfo = new LDAPSearchInfo;
        pInfo->strSearchNum = strKey;
        pInfo->iCallID = pCallInfo->GetCallID();
        TALK_DBG("CreateLdapSearch call[%d] second[%d] key[%s]",
                 pInfo->iCallID, pInfo->m_bIsSecend, pInfo->strSearchNum.c_str());
        _HelperLDAPInTalk.SetSearchCallInfo(pInfo);
        return true;
    }

    return false;
}

#if IF_BUG_38979
bool talkasync_GetCotactNameByNumber(int iCallID, const yl::string& strNumber)
{
    if (strNumber.empty())
    {
        return false;
    }

    // 开始匹配
    CContactMatchAction* pAction = new CContactMatchAction();
    if (pAction != NULL)
    {
        pAction->SetSync(false);
        pAction->SetMatchNum(strNumber);
        _ASyncHelper.SetCallID(pAction, iCallID);
        pAction->SetCallback(&_ASyncHelper);

        return TaskAction_ExecAction(pAction);
    }

    return false;
}
#endif

void talkasync_ReleaseLdapSearch(CCallInfo* pCallInfo)
{
    if (pCallInfo == NULL)
    {
        return;
    }

    _HelperLDAPInTalk.ClearByCallId(pCallInfo->GetCallID());
}

