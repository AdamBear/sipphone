#include "dialplan_inc.h"

static LRESULT OnMSGReceive(msgObject& msg)
{
    switch (msg.message)
    {
    case TM_TIMER:
        {
            return _DialPlanManager.OnTimer(msg.wParam);
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            switch (msg.wParam)
            {
            case ST_DIALPLAN:
                {
                    _DialRuleController.LoadDialRule(DR_PLAN);
                }
                break;
            case ST_DIALNOW:
                {
                    _DialRuleController.LoadDialRule(DR_NOW);
                }
                break;
            case ST_AREA:
                {
                    _DialRuleController.LoadDialRule(DR_AREA);
                }
                break;
            case ST_BLOCKOUT:
                {
                    _DialRuleController.LoadDialRule(DR_BLOCKOUT);
                }
                break;
            default:
                break;
            }
        }
        break;
     case DSK_MSG_MAKE_CALL:
         {
             bool bRet = false;
 
             do
             {
                 LPCSTR pszTarget = (LPCSTR)msg.GetExtraData();//获得通话目标
                 if (textEMPTY(pszTarget))
                 {
                     break;
                 }
 
                 TALK_WARN("Make call [%s] w[%d] l[%d]", pszTarget, msg.wParam, msg.lParam);
 
                 int iAccountID = msg.wParam;
                 // 分解收到数据得到呼出号码和服务器
                 CallOut_Param tCallParam;
                 bool bExistServer = true;
                 yl::string strSIPName;
                 yl::string strServerName;
                 commonUnits_ParserDialURI(pszTarget, strSIPName, strServerName);
 
                 if (strServerName.empty())
                 {
                     bExistServer = false;
                 }
                 else
                 {
                     tCallParam.tDialPlanMask.RemoveDialRuleFlag();
                 }
 
                 bool bExeMatch = true;
 
                 switch (msg.lParam)
                 {
                 case DSK_MAKE_CALL_MASK_HISTORY_MISSED:
                 case DSK_MAKE_CALL_MASK_HISTORY_RECEIVED:
                     bExeMatch = DigitMap_IsConfigEnable(kszDigitMapInHistoryDial, iAccountID);
                     break;
                 case DSK_MAKE_CALL_MASK_HISTORY_DIALED:
                 case DSK_MAKE_CALL_MASK_HISTORY_FORWARDED:
                     bExeMatch = false;
                     break;
                 case DSK_MAKE_CALL_MASK_DIRECTORY:
                     bExeMatch = DigitMap_IsConfigEnable(kszDigitMapInDirDial, iAccountID);
                     break;
                 case DSK_MAKE_CALL_MASK_MANUAL:
                     bExeMatch = DigitMap_IsConfigEnable(kszDigitMapInPressSend, iAccountID);
                     break;
                 default:
                     bExeMatch = false;
                     break;
                 }
 
                 if (!bExeMatch)
                 {
                     tCallParam.tDialPlanMask.RemoveDigitMapFlag();
                 }
 
                 //  不是紧急号码先先让键盘锁模块判断能否呼出
                 if (!Emergency_IsEmergency(strSIPName.c_str())
                         && keyGuard_ProcessMsg(msg))
                 {
                     break;
                 }
 
                 // 如果未获得用户许可,则不处理
                 if (!accessCtr_Process(msg))
                 {
                     break;
                 }
 
                 if (!acc_IsAccountIDValid(iAccountID))
                 {
                     // 此处需要用string类型
                     yl::string strTmp = strServerName;
                     iAccountID = acc_GetUsableAccount(strTmp);
                 }
 
                 if (!acc_IsAccountAvailable(iAccountID))
                 {
                     iAccountID = acc_GetUsableAccount();
                 }
 
                 // 如果查找到的账号服务器与呼出时指定的服务器不同，则要保留呼出时的服务器地址
                 // http://192.168.1.99/Bug.php?BugID=17494
                 yl::string strTargetName = strSIPName;
                 if (bExistServer
                         && !acc_IsDirectIPAccount(iAccountID)
                         && (acc_GetServerName(iAccountID) != strServerName))
                 {
                     strTargetName += "@";
                     strTargetName += strServerName;
                 }
 
                 if (talklogic_CallOut(strTargetName, "", iAccountID, &tCallParam))
                 {
                     bRet = true;
                 }
             }
             while (0);
 
             msg.ReplyMessage(bRet);
         }
         break;
    default:
        return FALSE;
    }

    //TALK_DBG("msg [%d][%#x] [%d][%d] [%s]", msg.message, msg.message, msg.wParam, msg.lParam, (LPCSTR)msg.GetExtraData());
    return TRUE;
}

void dialplan_Init()
{
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnMSGReceive);
    SingleMsgReg(TM_TIMER, OnMSGReceive);
    SingleMsgReg(DSK_MSG_MAKE_CALL, OnMSGReceive);
    DigitMap_Init();
    Emergency_Init();
    _DialRuleController.LoadDialRule();
}

void dialplan_UnInit()
{
    _DialRuleController.UnLoadDialRule();
}

void dialplan_GetDialNowData(DialNowListData& listDialNowData)
{
    _DialRuleController.GetDialNowData(listDialNowData);
}

bool dialplan_GetAreaCodeNumber(const yl::string& strText, yl::string& strAreaCode, int iLineId)
{
    return _DialRuleController.GetAreaCodeNumber(strText, strAreaCode, iLineId);
}

static bool GetReplaceNumber(yl::string& strText, int iLineID, const CDialPlanMask& mask)
{
    if (mask.Empty())
    {
        return false;
    }

    if (Emergency_IsEmergency(strText.c_str()))
    {
        return false;
    }

    yl::string strOld = strText;

    if (mask.IsAllowDigitMapReplace())
    {
        // 替换号码
        if (DigitMap_GetFmtUri(strText, iLineID))
        {
            TALK_INFO("replace digitmap [%d][%s]->[%s]", iLineID, strOld.c_str(), strText.c_str());
            return true;
        }
    }

    if (mask.IsAllowDialRuleReplace())
    {
        if (_DialRuleController.GetReplaceNumber(strText, strText,
                configParser_GetConfigInt(kszEnableReplaceAll) != 0, iLineID))
        {
            TALK_INFO("replace dialplan [%d][%s]->[%s]", iLineID, strOld.c_str(), strText.c_str());
            return true;
        }
        else
        {
#if !IF_BUG_27045
            if (_DialRuleController.GetAreaCodeNumber(strText, strText, iLineID))
            {
                TALK_INFO("replace areacode [%d][%s]->[%s]", iLineID, strOld.c_str(), strText.c_str());
                return true;
                // 普通电话必须判断区号
            }
#endif
        }
    }

    return false;
}

bool dialplan_IsBlockCallout(const yl::string& strText, int iLineID,
                             CallOut_Param* pCallParam/* = NULL*/)
{
    const CDialPlanMask& mask = pCallParam ? pCallParam->tDialPlanMask : CDialPlanMask();

    if (mask.Empty())
    {
        return false;
    }

    if (Emergency_IsEmergency(strText.c_str()))
    {
        return false;
    }

    // DigitMap优先级高
    if (mask.IsAllowDigitMapReplace())
    {
        if (DigitMap_IsInBlacklist(strText, iLineID))
        {
            TALK_DBG("block by digitmap [%s][%d]", strText.c_str(), iLineID);
            return true;
        }
    }

    if (mask.IsAllowDialRuleReplace())
    {
        if (_DialRuleController.IsBlockOutNumber(strText, iLineID))
        {
            TALK_DBG("block by blacklist [%s][%d]", strText.c_str(), iLineID);
            return true;
        }
    }

    return false;
}

bool dialplan_GetCalloutParam(int iSessionID, int iLineID, int iMask, CallOut_Param& callparam)
{
    if (iMask == COT_REDIAL)
    {
        callparam.tDialPlanMask.ClearDialRuleFalg();
    }

    bool bExeMatch = false;

    if (DigitMap_IsEnable(iLineID) && iLineID >= 0)
    {
        switch (iMask)
        {
        case COT_SEND:
        case COT_DIGITMAP:
        case COT_AUTODIAL:
            bExeMatch = talklogic_GetSessionStateBySessionID(iSessionID) == SESSION_PREDIAL ?
                        DigitMap_IsConfigEnable(kszDigitMapInDial,
                                                iLineID) : DigitMap_IsConfigEnable(kszDigitMapInPressSend, iLineID);
            break;
        case COT_REDIAL:
        case COT_HOTLINE:
        case COT_AUTHCALL:
        case COT_DIALNOW:
            bExeMatch = false;
            break;
        case COT_DSSKEY:
        case COT_PASSIVITY:
        default:
            bExeMatch = DigitMap_IsConfigEnable(kszDigitMapInDial, iLineID);
            break;
        }
    }

    if (!bExeMatch)
    {
        callparam.tDialPlanMask.RemoveDigitMapFlag();
    }

    TALK_INFO("GetCalloutParam mask=[%#x] match=[%d]", iMask, bExeMatch);
    return bExeMatch;
}

bool dialplan_GetReplaceNumber(yl::string& strText, int iLineID,
                               CallOut_Param* pCallParam /*= NULL*/)
{
    bool bRet = false;
    CDialPlanMask mask;

    if (pCallParam != NULL)
    {
        if (pCallParam->eCallOutType == TCT_CALL_CENTER)
        {
            return false;
        }

        mask = pCallParam->tDialPlanMask;
        // 先去除非法字符
        bRet |= RemoveInvalidChar(strText);
    }

    return GetReplaceNumber(strText, iLineID, mask) || bRet;
}

bool dialplan_GetReplaceNumberEx(yl::string& strText, int iLineID, DialPlanCallMask eMask)
{
    CDialPlanMask tDialPlanRepMsk;

    switch (eMask)
    {
    case DPCM_FWD:
        {
            // 去掉fwd号码中非法字符
            RemoveInvalidChar(strText);

            if (!DigitMap_IsConfigEnable(kszDigitMapInAutoFwd, iLineID))
            {
                tDialPlanRepMsk.RemoveDigitMapFlag();
            }
        }
        break;
    case DPCM_TRANSFER:
        {
            if (!DigitMap_IsConfigEnable(kszDigitMapInDial, iLineID))
            {
                tDialPlanRepMsk.RemoveDigitMapFlag();
            }
        }
        break;
    default:
        return false;
    }

    return GetReplaceNumber(strText, iLineID, tDialPlanRepMsk);
}

bool dialplan_GetChangeAccount(int& iLineID, LPCSTR lpNumber, CallOut_Param* pCallParam/* = NULL*/)
{
    if (lpNumber == NULL)
    {
        return false;
    }

    /* 携带呼出标志的号码不需要更改账号 */
    if (pCallParam != NULL && pCallParam->eCallOutType > TCT_NORMAL)
    {
        return false;
    }

    const CDialPlanMask& mask = pCallParam ? pCallParam->tDialPlanMask : CDialPlanMask();
    // 在SCA占线之前匹配否则可能跳过该流程，呼出前必须带匹配标志同时没有设置特殊呼出标志
    if (mask.IsAllowDigitMapReplace())
    {
        // 获取号码
        yl::string strNum, strServer;
        commonUnits_ParserDialURI(lpNumber, strNum, strServer);
        int iAccountID = iLineID;

        if (DigitMap_GetLineID(strNum, iAccountID) && acc_IsAccountAvailable(iAccountID))
        {
            iLineID = iAccountID;
            return true;
        }
    }

    return false;
}

/*
bool dialplan_SyncMatch(LPCSTR lpNumber, int iLineID, const CDialPlanMask& mask)
{
    if (mask.Empty())
    {
        return false;
    }

    if (Emergency_IsEmergency(lpNumber))
    {
        return false;
    }

    // DigitMap优先级高
    if (mask.IsAllowDigitMapReplace())
    {
        if (DigitMap_GetTimer(lpNumber, iLineID) <= 1)
        {
            return true;
        }
    }

    if (mask.IsAllowDialRuleReplace())
    {
        if (_DialRuleController.IsDialNowNumber(lpNumber, iLineID))
        {
            return true;
        }
    }

    return false;
}*/
