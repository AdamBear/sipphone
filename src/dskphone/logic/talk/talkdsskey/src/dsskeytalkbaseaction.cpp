#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"
#include "talk/dialplan/include/moddialplan.h"
#include "talk/talklogic/src/bttalkmanager.h"

bool CDssTalkBaseAciton::CallOutByDsskey(const DssKeyLogicData& logicData)
{
    CSessionStateProxy proxy;

    /* 非绑定类型处理 */
    if (!proxy)
    {
        return talklogic_EnterDialUI(logicData.strValue) != INVALID_SESSION_ID;
    }

    if (!proxy.IsTopestHostFocused())
    {
        return false;
    }

    return ProcessNoBindDsskey(proxy, logicData);
}

bool CDssTalkBaseAciton::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    bool bTransferDirect = proxy.IsInherits(ROUTINE_PRETRAN);

    if (!bTransferDirect && proxy.IsInherits(ROUTINE_TALKING))
    {
        if (OnActionSend(proxy, GetNumber(logicData)))
        {
            return true;
        }

        bTransferDirect = true;
    }

    return bTransferDirect && SendTalkEvent(proxy, logicData, DSSKEY_ACTION_TRAN_DIRECT);
}

bool CDssTalkBaseAciton::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    param.iSponsorDsskeyID = logicData.iDssKeyID;
    param.bBindDsskey = IsBindType();
    param.strNumber = logicData.strValue;

    int iAccountID = logicData.iLineIndex;
    if (!param.bBindDsskey && iAccountID == INVALID_ACCOUNTID)
    {
        iAccountID = acc_GetUsableAccount();
    }

    param.iAccountID = iAccountID;

    if (DSSKEY_ACTION_TRAN_DIRECT == iActionType)
    {
        if (proxy.IsInherits(ROUTINE_PRETRAN))
        {
            GetLigaturesText(proxy, logicData, param);
        }

        param.eTransferType = (DSSKEY_TRANS_TYPE)configParser_GetConfigInt(kszTransferModeViaDsskey);
    }

    return true;
}

bool CDssTalkBaseAciton::IsDTMFRelpalceTransfer(int iAccountID)
{
    return configParser_GetConfigInt(kszEnableDTMFReplaceTran) != 0;
}

bool CDssTalkBaseAciton::SendTalkEvent(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, int iActionType)
{
    /* 构造呼出参数 */
    CallOut_Param param;
    GetCallOutParam(iActionType, proxy, logicData, param);

    switch (iActionType)
    {
    case DSSKEY_ACTION_TRAN_DIRECT:
        {
            return TransferDirectly(proxy, logicData, param);
        }
        break;
    case DSSKEY_ACTION_CONF_TO:
        {
            return ConfTo(proxy, logicData, param);
        }
        break;
    default:
        break;
    }

    return false;
}

bool CDssTalkBaseAciton::EnterDialUI(const DssKeyLogicData& logicData)
{
    if (logicData.iDssKeyID == INVALID_DSSKEY_ID)
    {
        return false;
    }

    CallOut_Param calloutParam;
    CSessionStateProxy proxy;

    GetCallOutParam(DSSKEY_ACTION_MAKE_CALL, proxy, logicData, calloutParam);

    return EnterDialUI(logicData, calloutParam);
}

bool CDssTalkBaseAciton::EnterDialUI(const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (!IsBindType())
    {
        // 判断账号是否可以呼出
        if (!_DsskeyBindManager.IsAllowNewCallByAccount(param.iAccountID))
        {
            return false;
        }
    }
    else if (!_DsskeyBindManager.IsAllowNewCallByLineKey(logicData.iDssKeyID))
    {
        return false;
    }

    param.eRoutineType = ROUTINE_DIALING;
    return talklogic_CreateSession(param);
}

bool CDssTalkBaseAciton::ProcessCallOut(const DssKeyLogicData& logicData)
{
    int iDsskey = logicData.iDssKeyID;
    if (iDsskey == INVALID_DSSKEY_ID)
    {
        return false;
    }

    if (!_DsskeyBindManager.IsAllowNewCallByLineKey(iDsskey))
    {
        return false;
    }

    DssKey_Type eDskType = logicData.eDKType;

    // intercom 呼出时如果value为空，直接进拨号
    if (eDskType == DT_INTERCOM && logicData.strValue.empty())
    {
        return EnterDialUI(logicData);
    }

    CallOut_Param calloutParam;
    CSessionStateProxy proxy;

    GetCallOutParam(DSSKEY_ACTION_CALLOUT, proxy, logicData, calloutParam);

    if (calloutParam.strNumber.empty())
    {
        yl::string strCallOutNum = calloutParam.strNumber;
        dsskey_GetCallOutNumber(iDsskey, strCallOutNum);
        calloutParam.strNumber = strCallOutNum;

        if (strCallOutNum.empty())
        {
            return false;
        }
    }

    return talklogic_ProcessCallOut(calloutParam);
}

bool CDssTalkBaseAciton::ProcessDial(const DssKeyLogicData& logicData)
{
    yl::string strCallOutNum;
    dsskey_GetCallOutNumber(logicData.iDssKeyID, strCallOutNum);
    if (strCallOutNum.empty())
    {
        return EnterDialUI(logicData);
    }

    return ProcessCallOut(logicData);
}

bool CDssTalkBaseAciton::DialByCurrent(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    yl::string strNumber = pCallInfo->GetDialText();
    // 退出该界面
    proxy.ExitSession();

#ifdef IF_BT_SUPPORT_PHONE
    if (logicData.eDKType == DT_BTLINE)
    {
        return _BTMobileManager.EnterDialUI(strNumber);
    }
#endif

    CallOut_Param calloutParam;
    GetCallOutParam(DSSKEY_ACTION_MAKE_CALL, CSessionStateProxy(), logicData, calloutParam);
    calloutParam.bForceToCallOut = true;
    calloutParam.strNumber = strNumber;

    return EnterDialUI(logicData, calloutParam);
}

void CDssTalkBaseAciton::GetLigaturesText(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    param.strNumber = proxy.GetDialText();
    yl::string strText;
    GetCallOutNumber(logicData.iDssKeyID, strText, logicData, NULL);
    param.strNumber += strText;
}

bool CDssTalkBaseAciton::TransferDirectly(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (proxy.IsInherits(ROUTINE_DIALING))
    {
        DataDial2Logic DialData;
        DialData.strNumber = param.strNumber;

        return talklogic_UIEvent(proxy, TALK_ACTION_SEND, COT_DSSKEY, param.eTransferType, (LPVOID)&DialData);
    }
    else if (proxy.IsInherits(ROUTINE_TALKING))
    {
        int iAccountID = proxy.GetAccountID();
        yl::string& strNumber = param.strNumber;

        CCallInfo* pCallInfo = proxy.GetCallInfo();
        if (pCallInfo != NULL && !strNumber.empty())
        {
            pCallInfo->SetDialedNumber(strNumber);
            /* park业务先处理 */
            if (talklogic_UIEvent(proxy, TALK_ACTION_SEND))
            {
                return true;
            }
        }

        if (DTT_BLIND_TRANSFER == param.eTransferType
                && IsDTMFRelpalceTransfer(iAccountID))
        {
            yl::string strTransferDTMF = IsBLF()
                                         ? configParser_GetCfgItemStringValue(kszAccountBLFDTMFCode, iAccountID)
                                         : configParser_GetConfigString(kszTransferDTMF);

            strTransferDTMF += strNumber;
            return proxy.SendDTMF(strTransferDTMF);
        }
        // 配置不支持Trans或者允许Trans
        else if (param.eTransferType == DTT_NEW_CALL
                 || proxy.IsAllowTran())
        {
            if (param.strNumber.empty())
            {
                return false;
            }

            // 如果已经在转移中，则完成转移
            if (proxy.IsInPreTrans())
            {
                //转接
                return talklogic_UIEvent(proxy, TALK_ACTION_TRAN);
            }

            if (param.eTransferType != DTT_NEW_CALL)
            {
                proxy.HoldSession(true);
            }

            dialplan_GetCalloutParam(proxy, iAccountID, COT_DSSKEY, param);

            // 根据类型进行处理
            switch (param.eTransferType)
            {
            case DTT_BLIND_TRANSFER://直接忙转
                {
                    // 盲转
                    return proxy.BlindTransfer(param.strNumber);
                }
            case DTT_CALL_OUT://咨询或半咨询
                {
                    return proxy.AttendedTransfer(param, false);
                }
            case DTT_NEW_CALL: // 按功能呼出
                {
                    // 呼出
                    if (param.bBindDsskey)
                    {
                        // 如果是要绑定的DSSKey，则通过DSSKey的绑定呼出
                        return ProcessCallOut(logicData);
                    }
                    else
                    {
                        // 直接新起话路呼出
                        return talklogic_ProcessCallOut(param);
                    }
                }
#if IF_BUG_30961
            case DTT_FON_CALL_OUT:
                {
                    return proxy.AttendedTransfer(param, true);
                }
#endif
            default:
                break;
            }
        }
    }

    return false;
}

bool CDssTalkBaseAciton::ConfTo(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    const yl::string& strNumber = param.strNumber;
    if (strNumber.empty())
    {
        return false;
    }

    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    param.iAccountID = pCallInfo->GetAccountID();

    if (proxy.IsLocalConf())
    {
        // 如果只有当前一路
        if (!proxy.IsAllowCreateLocalConf())
        {
            return false;
        }

        param.bConfOnePress = (configParser_GetConfigInt(kszConfWithOnePress) == 1);
        CSessionProxy callout = talklogic_ProcessCallOut(param);

        if (!callout)
        {
            return false;
        }

        callout.SetConfSponsor(proxy.GetSubByIndex(0));

        if (param.bConfOnePress)
        {
            return callout.EstablishLocalConf();
        }
    }
    else if (proxy.IsInherits(ROUTINE_TALKING))
    {
        if (talklogic_IsLocalConfSessionExist()
                || !proxy.IsAllowAddToConf())
        {
            return false;
        }

        param.bConfOnePress = (configParser_GetConfigInt(kszConfWithOnePress) == 1)
                              && (pCallInfo->GetConfType() != CT_BROADSOFT);

        CSessionProxy callout = talklogic_ProcessCallOut(param);

        if (!callout)
        {
            return false;
        }

        callout.SetConfSponsor(proxy);

        if (param.bConfOnePress)
        {
            return callout.EstablishLocalConf();
        }
    }
    else if (proxy.IsInherits(ROUTINE_DIALING))
    {
        // Conf to拨号界面
        if (!proxy.IsInPreConf())
        {
            return false;
        }

        pCallInfo->m_bConferenceDirect = true;
        return talklogic_ProcessCallOut(param);
    }

    return false;
}

bool CDssTalkBaseAciton::OnActionSend(const CSessionStateProxy& proxy, const yl::string& strNumber)
{
    if (!proxy || strNumber.empty())
    {
        return false;
    }

    DataDial2Logic DialData;
    DialData.strNumber = strNumber;
    return talklogic_UIEvent(proxy, TALK_ACTION_SEND, COT_DSSKEY, 0, (LPVOID)&DialData);
}

CDssTalkBaseAciton* CDssTalkBaseAciton::GetImplAction(DssKey_Type eType)
{
    IDsskeyImplAction* pImpAction = IDsskeyImplAction::GetImplAction(eType);
    if (pImpAction == NULL
            || !pImpAction->IsRelateTalk())
    {
        return NULL;
    }

    return (CDssTalkBaseAciton*)(pImpAction);
}

#endif
