#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyBlfBaseAction::CDsskeyBlfBaseAction(DssKey_Type dsskeyType)
    : CDsskeyBindAction(dsskeyType)
{

}

bool CDsskeyBlfBaseAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                      void* pExtraData)
{
    if (ProcessBLFCustomAction(dsskeyDataID, logicData))
    {
        return true;
    }

    CSessionStateProxy focus;
    if (!focus)
    {
        return false;
    }

    if (ProcessBLF(focus, logicData))
    {
        return true;
    }
    //http://bugfree.yealink.com/Bug.php?BugID=120208
    //如果是在conf拨号界面按blf/blflist/speeddial则将value输入
    else if (focus.GetRoutineState() == ROUTINE_DIALING)
    {
        DIAL_UI_TYPE uiType = focus.GetDialUIType();
        if (uiType == DIAL_UI_CONFERENCE
#if IF_BUG_33235
                || uiType == DIAL_UI_INTERCOM
#endif
           )
        {
            CCallInfo* pCallInfo = focus.GetCallInfo();
            if (pCallInfo == NULL)
            {
                return false;
            }

            yl::string strDialNum = pCallInfo->GetDialText();
            strDialNum += logicData.strValue;
            pCallInfo->SetDialedNumber(strDialNum);
            return true;
        }
    }

    return false;
}

int CDsskeyBlfBaseAction::GetRemoteStatus(int iDsskey)
{
    return dsskey_GetDsskeyStatus(iDsskey);
}

bool CDsskeyBlfBaseAction::OnConfigChange(int dsskeyID, void* pExtraData)
{
    if (dsskey_GetDsskeyType(dsskeyID) != DT_BLFLIST)
    {
        etl_NotifyApp(false, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, dsskeyID, 0);
    }

    // 向SIP发送BLF改变消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, dsskey_GetDsskeyIndexByID(dsskeyID));

    return true;
}

bool CDsskeyBlfBaseAction::ProcessBLFCustomAction(int iDsskey, const DssKeyLogicData& logicData)
{
    Dsskey_Status eDssStatus = dsskey_GetDsskeyStatus(iDsskey);

    int iSessionID = talklogic_GetFocusedSessionID();
    int iCallID = talklogic_GetCallIdBySessionID(iSessionID);
    SESSION_STATE eSessState = talklogic_GetSessionStateBySessionID(iSessionID);

    eBLFOperateType eAction = blf_GetCustomOperationType(eDssStatus, eSessState);

    if (OPERATE_BLF_TYPE_NONE == eAction)
    {
        return false;
    }
    else if (OPERATE_BLF_TYPE_EDK == eAction)
    {
        yl::string strValue = blf_GetCustomOperationValue(eDssStatus, eSessState);
        EDK_DoAction(strValue, iDsskey, BLF_TRIGGER);
        return true;
    }

    Dsskey_Status eTargetStatus = DS_BLF_STATUS_IDLE;
    if (OPERATE_BLF_TYPE_PICKUP == eAction)
    {
        eTargetStatus = (DS_BLF_STATUS_RINGING == eDssStatus) ? eDssStatus : eTargetStatus;
    }
    else if (OPERATE_BLF_TYPE_BARGEIN == eAction)
    {
        eTargetStatus = (DS_BLF_STATUS_TALKING == eDssStatus) ? eDssStatus : eTargetStatus;
    }

    bool bCallOut = true;
    yl::string strNumber;
    dsskey_GetCallOutNumber(iDsskey, strNumber, (void*)&eTargetStatus);

    if (OPERATE_BLF_TYPE_BTRANSFER == eAction)
    {
        bCallOut = !talklogic_BlindTransfer(iCallID, strNumber.c_str());
        bCallOut = bCallOut && !talklogic_ForwardIncomingCall(iCallID, strNumber);
    }
    else if (OPERATE_BLF_TYPE_CTRANSFER == eAction)
    {
        int iFollowSessionId = -1, iHostSessionId = -1;
        if (talklogic_GetFollowSessionId(iSessionID, iFollowSessionId, SPONSOR_TRAN))
        {
            iHostSessionId = iSessionID;
        }
        else if (talklogic_GetSponsorSessionId(iSessionID, iHostSessionId, SPONSOR_TRAN))
        {
            iFollowSessionId = iSessionID;
        }

        if (iFollowSessionId != -1 && iHostSessionId != -1)
        {
            CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionID(iFollowSessionId);
            if (NULL != pCallInfo && pCallInfo->m_tRemoteInfo.sSIPName == strNumber)
            {
                bCallOut = !talklogic_Transfer(talklogic_GetCallIdBySessionID(iHostSessionId),
                                               talklogic_GetCallIdBySessionID(iFollowSessionId));
            }
        }
        else
        {
            bCallOut = !talklogic_AttendedTransfer(iCallID, strNumber);
        }

        bCallOut = bCallOut && !talklogic_ForwardIncomingCall(iCallID, strNumber);
    }

    // 呼出
    if (bCallOut)
    {
        CallOut_Param param;
        param.strNumber = strNumber;
        param.iAccountID = logicData.iLineIndex;
        param.bConfOnePress = SESSION_RINGING == eSessState;
        talklogic_ProcessCallOut(param);
    }

    DSSKEY_INFO("ProcessBLFCustomAction, eDssStatus[%d], eSessState[%d], eAction[%d], strNumber[%s], bCallOut[%d]",
                eDssStatus, eSessState, eAction, strNumber.c_str(), bCallOut);

    return true;
}

bool CDsskeyBlfBaseAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_TALKING))
    {
#if IF_BUG_30886
        if (GetRemoteStatus(logicData.iDssKeyID) == DS_BLF_STATUS_RINGING)
        {
            return ProcessCallOut(logicData);
        }
#endif
        if (OnActionSend(proxy, GetNumber(logicData)))
        {
            return true;
        }

        return SendTalkEvent(proxy, logicData, DSSKEY_ACTION_TRAN_DIRECT);
    }
    else if (proxy.IsInherits(ROUTINE_RETRIEVE_CALLPARK))
    {
        return OnActionSend(proxy, GetNumber(logicData));
    }

    return CDsskeyBindAction::ProcessNoBindDsskey(proxy, logicData);
}

bool CDsskeyBlfBaseAction::ProcessBLF(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    yl::string& strNumber = pCallInfo->m_tRemoteInfo.sSIPName;

    if (proxy.IsInherits(SESSION_RINGBACK)
            || proxy.IsInherits(SESSION_TALKING))
    {
        if (strNumber != logicData.strValue
                && strNumber != logicData.strExtension)
        {
            return false;
        }

        DSSKEY_TRANS_TYPE eDsskeyTranType = (DSSKEY_TRANS_TYPE)configParser_GetConfigInt(
                                                kszTransferModeViaDsskey);
        // 当BLF转接设置为咨询转时，再按下BLF则转接
        if (eDsskeyTranType != DTT_CALL_OUT
#if IF_BUG_30961
                && eDsskeyTranType != DTT_FON_CALL_OUT
#endif
           )
        {
            return false;
        }

        // 当BLF转接设置为咨询转时，再按下BLF则转接
        bool bAllowTrans = true;
        if (proxy.IsInherits(SESSION_TALKING))
        {
            bAllowTrans = proxy.IsInPreTrans();
        }

        return bAllowTrans && talklogic_UIEvent(proxy, TALK_ACTION_TRAN);
    }
    else if (proxy.IsInherits(SESSION_RINGING))
    {
        int iAccount = proxy.GetAccountID();
        if (iAccount != logicData.iLineIndex
                || !acc_IsAccountIDValid(iAccount))
        {
            return false;
        }

        const yl::string& strFWDNumber = pCallInfo->GetDialText();
        // 当前处在Forward界面且前缀不为空时
        if (proxy.IsPreForword() && !strFWDNumber.empty())
        {
            return false;
        }

        int iDsskeyID = logicData.iDssKeyID;
        if (!_DsskeyBindManager.GetSessionByDsskey(iDsskeyID))
        {
            yl::string strSIPName;
            yl::string strServerName;
            const yl::string& strMonitor = GetNumber(logicData);
            commonUnits_ParserDialURI(strMonitor, strSIPName, strServerName);

            if (strSIPName.empty())
            {
                return false;
            }

            // BLF监控方来电, 按BLF可以接起
            if (strNumber == strSIPName || strNumber == strMonitor)
            {
                _DsskeyBindManager.AddBinder(proxy, iDsskeyID, false);
                return talklogic_UIEvent(proxy, TALK_ACTION_ANSWER);
            }

            if (proxy.IsPreForword()
                    || 0 == configParser_GetConfigInt(kszForbidBLFFWDIncoming))
            {
                yl::string strFwdTarget = strSIPName;
                if (!strServerName.empty())
                {
                    strFwdTarget = "sip:";
                    strFwdTarget += strSIPName + "@" + strServerName;
                }

                return OnActionSend(proxy, strFwdTarget);
            }
        }

        return false;
    }

    return false;
}

bool CDsskeyBlfBaseAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);

    switch (iActionType)
    {
    case DSSKEY_ACTION_MAKE_CALL:
        {
            if (IsIntercomMode())
            {
                param.bIntercom = true;
                param.eCallOutType = TCT_INTERCOM;
                param.strNumber += GetNumber(logicData);
                AddIntercomCode(param.strNumber);
            }
        }
        break;
    case DSSKEY_ACTION_CALLOUT:
        {
            if (proxy.IsInherits(ROUTINE_TALKING) && IsIntercomMode())
            {
                int iDsskey = _DsskeyBindManager.GetDsskeyBySession(proxy, false);
                if (iDsskey != INVALID_ACCOUNTID)
                {
                    DssKey_Type eType = DT_INTERCOM;
                    if (dsskey_GetDsskeyType(iDsskey) == eType)
                    {
                        GetCallOutNumber(logicData.iDssKeyID, param.strNumber, logicData, NULL, 0);

                        // 从发起者获取数据供组装呼出参数
                        DssKeyLogicData dsskeyData;
                        dsskey_GetData(iDsskey, dsskeyData);

                        if (GetDsskeyType() == eType)
                        {
                            GetCallOutParam(DSSKEY_ACTION_MAKE_CALL, proxy, dsskeyData, param);
                        }
                        else
                        {
                            CCallInfo* pCallInfo = proxy.GetCallInfo();
                            if (pCallInfo != NULL)
                            {
                                param.strNumber = pCallInfo->GetDialText() + param.strNumber;
                            }

                            // intercom 发起的呼出需要从发起者获取呼出参数
                            CDssTalkBaseAciton* pAction = GetImplAction(eType);
                            if (pAction != NULL)
                            {
                                pAction->GetCallOutParam(DSSKEY_ACTION_MAKE_CALL, proxy, dsskeyData, param);
                            }
                        }

                        break;
                    }
                }
            }

            GetLigaturesText(proxy, logicData, param);
        }
        break;
    }

    return true;
}

bool CDsskeyBlfBaseAction::AddIntercomCode(yl::string& strNumber)
{
    // intercom mode
    if (IsIntercomMode())
    {
        if (!strNumber.empty() && configParser_GetConfigInt(kszIntercomMode) != 0)
        {
            strNumber = configParser_GetConfigString(kszIntercomFeatureAccessCode) + strNumber;
        }

        return true;
    }

    return false;
}

bool CDsskeyBlfBaseAction::IsDTMFRelpalceTransfer(int iAccountID)
{
    return (acc_IsAccountIDValid(iAccountID)
            && configParser_GetCfgItemIntValue(kszAccountEnableBLFSendDTMF, iAccountID) != 0)
           || CDssTalkBaseAciton::IsDTMFRelpalceTransfer(iAccountID);
}

bool CDsskeyBlfBaseAction::BeforeUpdateUI(int dsskeyDataID, DsskeyExpRefresh dsskeyRefresh/* = DSS_KEY_REFRESH_ALL*/)
{
    return !blf_RefreshDsskey(dsskeyDataID, dsskeyRefresh);
}

bool CDsskeyBlfBaseAction::IsIntercomMode()
{
    return configParser_GetConfigInt(kszBLFIntercomMode) == 1;
}

#endif  //IF_FEATURE_DSSKEY
