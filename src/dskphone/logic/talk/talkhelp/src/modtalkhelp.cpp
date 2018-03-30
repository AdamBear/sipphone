#include "talkhelp_inc.h"

static LRESULT OnMSGReceive(msgObject& msg)
{
    switch (msg.message)
    {
    case ACCOUNT_STATUS_CHANGED:
        {
            if (!acc_IsDirectIPAccount(msg.wParam))
            {
                //Line 注册上,发送DND特征码
                _CallWaiting.SendCallWaitingCode(msg.wParam);

                //http://bugfree.yealink.com/Bug.php?BugID=135363
                _SecurityClassification.OnAccountStatusChange(msg.wParam);
            }
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
#ifdef IF_FEATURE_TALK_ERROR_OPT
            if (msg.wParam == ST_ERROR_INFO)
            {
                _TalkErrorInfoManager.Init();
            }
            else
#endif
                if (msg.wParam == ST_CALLWAIT)
                {
                    _CallWaiting.UpdateAndSendCode();
                }
        }
        break;
    case SIP_SUB_UPDATE_FEATURE_KEY_STATUS:
        {
            LPCSTR pExtraData = (LPCSTR)msg.GetExtraData();
            if (NULL == pExtraData)
            {
                break;
            }

            // 解析数据
            _SecurityClassification.ParseXML(msg.wParam, pExtraData);
        }
        break;
    case SIP_CALL_INFORMATION_UPDATE:
        {
            sip_call_infomation_header_t* pInfo = (sip_call_infomation_header_t*)msg.GetExtraData();
            if (NULL == pInfo || SIP_CALL_INFORMATION_SECURITY_CLASS != pInfo->type)
            {
                break;
            }

            // 获取当前话路的安全等级
            yl::string strLevel = _SecurityClassification.ParseTalkSecurityLevel(pInfo->payload);

            int iSessId = talklogic_GetSessionIdByCallID(msg.lParam, false);

            TALK_DBG("callid[%d] session[%d] level[%s]", msg.lParam, iSessId, strLevel.c_str());
            // 记录通话的等级
            _SecurityClassification.AddTalkSecurityLevel(iSessId, strLevel);

            TALK_INFO("SecurityClassification, TalkChange iAccount[%d], iSessId[%d], iCallId[%d], SecurityLev[%s]",
                      msg.wParam, iSessId, msg.lParam, strLevel.c_str());

            // 话路更新界面
            talklogic_UpdateWnd();
        }
        break;
    case SIP_MSG_UPDATE_DATE:
        {
            return Comm_UpdateSysTime(msg);
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

void talkhelp_Init()
{
#ifdef IF_FEATURE_TALK_ERROR_OPT
    _TalkErrorInfoManager.Init();
#endif
    _SecurityClassification.Init();
    _CallWaiting.UpdateAndSendCode();

    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnMSGReceive);
    SingleMsgReg(ACCOUNT_STATUS_CHANGED, OnMSGReceive);

    SingleMsgReg(SIP_SUB_UPDATE_FEATURE_KEY_STATUS, OnMSGReceive);
    SingleMsgReg(SIP_CALL_INFORMATION_UPDATE, OnMSGReceive);
    SingleMsgReg(SIP_MSG_UPDATE_DATE, OnMSGReceive);
}

void talkhelp_UnInit()
{
}

bool talkhelp_GetContractRing(yl::string& strPath, LPCSTR lpNumber)
{
    if (lpNumber == NULL)
    {
        return false;
    }

    strPath = LocalDir_GetRingByNumber(lpNumber);
    TALK_DBG("GetContractRing ring[%s] path[%s]", lpNumber, strPath.c_str());
    return pathFileExist(strPath.c_str());
}

bool talkhelp_GetDefaultRing(yl::string& strPath)
{
    yl::string strRing = configParser_GetConfigString(kszRingtype);
    strPath = commonUnit_SearchRingPath(strRing);
    TALK_DBG("GetDefaultRing ring[%s] path[%s]", strRing.c_str(), strPath.c_str());
    return pathFileExist(strPath.c_str());
}

bool talkhelp_GetCallWaitingToneType(TONE_TYPE& eToneType, CallInUserInfo* pCallData)
{
    int iToneType = 1;
    if (pCallData != NULL)
    {
        // 取出指定的信号音序号
        if (!pCallData->strRingLocal.empty())
        {
            if (1 != sscanf(pCallData->strRingLocal.c_str(), "ringtone-%d", &iToneType))
            {
                sscanf(pCallData->strRingLocal.c_str(), "Bellcore-dr%d", &iToneType);
            }
        }
    }

    switch (iToneType)
    {
    case 1:
        eToneType = (configParser_GetConfigInt(kszAlertInfo) == 0) ? TONE_CALLWAIT : TONE_CALLWAIT1;
        break;
    case 2:
        eToneType = TONE_CALLWAIT2;
        break;
    case 3:
        eToneType = TONE_CALLWAIT3;
        break;
    case 4:
        eToneType = TONE_CALLWAIT4;
        break;
    default:
        eToneType = TONE_CALLWAIT;
        break;
    }

    return true;
}

TONE_TYPE talkhelp_GetDialTone(int iAccountID)
{
    if (INVALID_ACCOUNTID == iAccountID)
    {
        return TONE_DIAL;
    }

#ifdef IF_SUPPORT_BLUETOOTH
    if (Bluetooth_GetBtAccountID() == iAccountID)
    {
        return TONE_DIAL;
    }
#endif

    switch (configParser_GetCfgItemIntValue(kszAccountDialTone, iAccountID))
    {
    case CN_STANDARD_DIAL_TONE:
        return TONE_DIAL;
    case CN_SPECIAL_CONDITION_TONE:
        return TONE_DIAL2;
    case CN_MESSAGE_WAITING_TONE:
        return TONE_DIAL3;
    default:
        return TONE_DIAL;
    }
}

LPCSTR talkhelp_GetErrorInfo(int iType)
{
#ifdef IF_FEATURE_TALK_ERROR_OPT
    return _TalkErrorInfoManager.GetErrorInfo(iType);
#else
    return NULL;
#endif
}

bool talkhelp_IsCallWaitingEnable()
{
    return _CallWaiting.IsEnableCallWait();
}

bool talklogic_IsShowSecurity(int iAccountID)
{
    return _SecurityClassification.IsShowSecurity(iAccountID);
}

// 获取账号对应的安全等级
yl::string talklogic_GetAccountSecurityLevel(int iAccount)
{
    yl::string strTmp;
    _SecurityClassification.GetAccountSecurityLevel(strTmp, iAccount);
    return strTmp;
}

// 获取账号的覆盖安全等级：通话中设置的等级
yl::string talklogic_GetAccountOverrideLevel(int iAccount)
{
    yl::string strTmp;
    _SecurityClassification.GetAccountOverrideLevel(strTmp, iAccount);
    return strTmp;
}

// 获取账号可设置的安全等级列表
bool talklogic_GetAccountAllowSecurityLevels(YLList<yl::string>& lstAllowLevels, int iAccount)
{
    return _SecurityClassification.GetAllowSecurityLevels(lstAllowLevels, iAccount);
}

// 修改当前账号的安全等级
bool talklogic_ModifyAccountOverrideLevel(int iAccount, const yl::string& strLevel)
{
    return _SecurityClassification.ModifyAccountOverrideLevel(iAccount, strLevel);
}

// 获取通话的安全等级
yl::string talklogic_GetTalkSecurityLevel(int iSessId)
{
    return _SecurityClassification.GetTalkSecurityLevel(iSessId);
}

// 获取本地会议中安全等级最小的账号
int talklogic_GetAccountLeastLevFromLocalConf()
{
    yl::string strLevel;
    int iConfSessionID = talklogic_GetLocalConfSessionID();
    return _SecurityClassification.GetLeastLevelInfoFromLocalConf(iConfSessionID, strLevel);
}
