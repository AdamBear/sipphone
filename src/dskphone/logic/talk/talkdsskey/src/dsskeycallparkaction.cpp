#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"
#include "broadsoft/callpark/include/modcallpark.h"

CDsskeyCallparkAction::CDsskeyCallparkAction()
    : CDsskeyBindAction(DT_CALLPARK)
{
}

bool CDsskeyCallparkAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                       void* pExtraData)
{
    bool bHandled = false;
    yl::string strNumber;
    GetCallOutNumber(dsskeyDataID, strNumber, logicData, pExtraData);

    if (configParser_GetConfigInt(kszCallParkMode) != 2 || strNumber.empty())
    {
        //Park模式不为Transfer或者strNumber为空才调用以下接口
        //strNumber为空是为了走进入park预拨号界面
        bHandled = EnterCallParkUI(strNumber, logicData);
    }
    else if (HavingParkCall(dsskeyDataID, logicData)
             || configParser_GetConfigInt(kszForceCallParkDirectCallOut) != 0)
    {
        if (!strNumber.empty())
        {
            bHandled = CallOutByDsskey(logicData);
        }
    }
    else if (IsCallParkUsable(dsskeyDataID, logicData))
    {
        bHandled = true;
        SetCallPark(dsskeyDataID, logicData);
    }
    else if (CanFetchPark(dsskeyDataID, logicData))
    {
        if (!strNumber.empty())
        {
            int iStatus = GetRemoteStatus(dsskeyDataID);
            if (iStatus == DS_BLF_STATUS_RINGING
                    || iStatus == DS_BLF_STATUS_TALKING)
            {
                if (!ParkInTalking())
                {
                    return true;
                }
            }

            bHandled = CallOutByDsskey(logicData);
        }
    }

    return bHandled;
}

bool CDsskeyCallparkAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType,
        DssKey_Type newType, void* pExtraData)
{
    if (newType == DT_CALLPARK)
    {
        return false;
    }

    // 给BLF发送配置改变消息
    etl_NotifyApp(false, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, dsskeyID, 0);

    // 向SIP发送BLF改变消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, dsskey_GetDsskeyIndexByID(dsskeyID));

    return true;
}

bool CDsskeyCallparkAction::EnterCallParkUI(const yl::string& strNumber, const DssKeyLogicData& logicData)
{
    CSessionStateProxy proxy;
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (proxy.IsInherits(ROUTINE_TALKING))
    {
        yl::string strValue;
        if (configParser_GetConfigString(kszCallParkCode).length() < strNumber.length())
        {
            strValue = strNumber.substr(configParser_GetConfigString(kszCallParkCode).length());
        }

        if (!strValue.empty()
                || (configParser_GetConfigInt(kszCallParkMode) != 0
                    && configParser_GetConfigString(kszCallParkCode) != ""
                    && configParser_GetConfigInt(kszFeatureDirectSendEnable) == 1))
        {
            callPark_CallPark(pCallInfo->GetCallID(), strNumber, pCallInfo->GetAccountID());
            return true;
        }
        else
        {
            CListenInfo* pInfoData = modtklsn_get_privdata(proxy);
            if (pInfoData)
            {
                pInfoData->SetPark(true);
            }
            return true;
        }
    }

    if (strNumber.empty())
    {
        return false;
    }

    // 如果不处于通话中，则直接呼出
    if (configParser_GetConfigInt(kszCallParkMode) == 1)
    {
        return talklogic_CallOut(strNumber, "");
    }

    return false;

}

bool CDsskeyCallparkAction::HavingParkCall(int iDsskey, const DssKeyLogicData& logicData)
{
    return blf_IsSupportAutopPickUpByDsskey(iDsskey);
}

// call park是否可用
bool CDsskeyCallparkAction::IsCallParkUsable(int iDsskey, const DssKeyLogicData& logicData)
{
    CSessionStateProxy focus;
    yl::string strParkCode = configParser_GetConfigString(kszCallParkCode);

    // CallPark按键实现Park当前通话(Talk状态)或来电(通话的账号可以是非设置的Line)
    if ((!logicData.strValue.empty() || !strParkCode.empty())
            && focus)
    {
        int iAccountID = focus.GetAccountID();

        if (focus.IsInherits(ROUTINE_TALKING))
        {
            // http://192.168.1.99/Bug.php?BugID=20568
            // 修改轨道上有通话时，在通话中仍然tran到SP0的问题
            int iStatue = GetRemoteStatus(iDsskey);
            if (iStatue == DS_BLF_STATUS_PARK
                    || iStatue == DS_BLF_STATUS_TALKING)
            {
                return false;
            }

            // 判断当前通话账号是否对应DssKey
            bool bIsTalkingAccount = false;
            if (iAccountID == logicData.iLineIndex)
            {
                bIsTalkingAccount = true;
            }
            else
            {
                //对比两个Line的RegisterName和ServerName
                int iTalkingAccount = iAccountID;
                bIsTalkingAccount = (acc_GetFullName(iTalkingAccount) == acc_GetFullName(logicData.iLineIndex));
            }

            bool bRestictLine = configParser_GetConfigInt(kszParkLineRestrictionEnable) == 1;
            if (SST_START2START == acc_GetServerType(iAccountID)
                    || !bRestictLine)
            {
                //当前通话账号为Star2Star服务器则linekey上的park不管配置那个账号都当做当前账号来用
                bIsTalkingAccount = true;
            }

            // 通话中，盲转到设置的callpark号码
            if (bIsTalkingAccount
                    && focus.GetTalkState() == TS_TALK)
            {
                return true;
            }
        }
        else
        {
            // 响铃时，fwd到设置的callpark号码
            if (iAccountID == logicData.iLineIndex
                    && focus.IsInherits(ROUTINE_RINGING))
            {
                return true;
            }
        }
    }

    return false;
}

void CDsskeyCallparkAction::SetCallPark(int iDsskey, const DssKeyLogicData& logicData)
{
    CSessionStateProxy focus;
    if (!focus)
    {
        DSSKEY_INFO("[DSSKEY:%d] error in SetCallPark", iDsskey);
        return;
    }

    yl::string strParkCode = configParser_GetConfigString(kszCallParkCode);

    if (!logicData.strValue.empty() || !strParkCode.empty())
    {
        if (focus.IsInherits(ROUTINE_TALKING))
        {
            // 通话中，盲转到设置的callpark号码
            if (focus.GetTalkState() == TS_TALK)
            {
                BlindTransferForCallpark(focus, logicData.strValue);
            }
        }
        else
        {
#if IF_BUG_20588
            CallOutByDsskey(logicData);
#else
            // 响铃时，fwd到设置的callpark号码
            if (focus.IsInherits(ROUTINE_RINGING))
            {
                focus.Forward(logicData.strValue);
            }
#endif
        }
    }
}

void CDsskeyCallparkAction::BlindTransferForCallpark(const CSessionStateProxy& proxy, const yl::string& strNumber)
{
    yl::string strNumberTmp = strNumber.empty() ? configParser_GetConfigString(kszCallParkCode) : strNumber;
    proxy.TransferForCallpark(strNumberTmp);
}

bool CDsskeyCallparkAction::CanFetchPark(int iDsskey, const DssKeyLogicData& logicData)
{
    yl::string strParkCode = configParser_GetConfigString(kszCallParkCode);
    if (logicData.strValue.empty() && strParkCode.empty())
    {
        return false;
    }

    if (!talklogic_SessionExist())
    {
        // 没有通话时，可以取回
        return true;
    }

    int iStatus = GetRemoteStatus(iDsskey);
    if (iStatus == DS_BLF_STATUS_IDLE
            || iStatus == DS_BLF_STATUS_RINGING
            || iStatus == DS_BLF_STATUS_TALKING
            || iStatus == DS_BLF_STATUS_UNKOWN)
    {
        CSessionStateProxy focus;
        if (focus && focus.GetAccountID() == logicData.iLineIndex)
        {
            ROUTINE_STATE eRoutine = focus.GetRoutineState();
            if (eRoutine == ROUTINE_TALKING
                    || eRoutine == ROUTINE_DIALING
                    || eRoutine == ROUTINE_PREDIAL)
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }

    return false;
}

bool CDsskeyCallparkAction::ParkInTalking()
{
    CSessionStateProxy focus;
    if (!focus)
    {
        return true;
    }

    if (focus.GetRoutineState() == ROUTINE_TALKING
            && focus.GetTalkState() == TS_TALK)
    {
        focus.ShowInfo(ETI_ALREADY_PARKED_TO_THIS_SLOT);
        return false;
    }

    return true;
}

int CDsskeyCallparkAction::GetRemoteStatus(int iDsskey)
{
    return dsskey_GetDsskeyStatus(iDsskey);
}

bool CDsskeyCallparkAction::OnConfigChange(int dsskeyID, void* pExtraData)
{
    // 给BLF模块发送配置改变消息
    etl_NotifyApp(false, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, dsskeyID, 0);

    // 向SIP发送BLF改变消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, dsskey_GetDsskeyIndexByID(dsskeyID));

    return true;
}

bool CDsskeyCallparkAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    // XSI模式不需要code
    if (configParser_GetConfigInt(kszCallParkMode) == 0)
    {
        strCallOutNumber = logicData.strValue;
    }
    else
    {
        strCallOutNumber = configParser_GetConfigString(kszCallParkCode) + logicData.strValue;
    }

    return true;
}

bool CDsskeyCallparkAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);

    if (DSSKEY_ACTION_MAKE_CALL == iActionType)
    {
        bool bIsAutoPickup = false;
        int iDsskeyID = logicData.iDssKeyID;
        int iStatus = GetRemoteStatus(iDsskeyID);

        if (iStatus == DS_BLF_STATUS_RINGING
                || iStatus == DS_BLF_STATUS_TALKING)
        {
            // 判断auto_pickup标识位
            if (blf_IsSupportAutopPickUpByDsskey(iDsskeyID))
            {
                bIsAutoPickup = configParser_GetCfgItemIntValue(kszAccountDialogPickUp,
                                logicData.iLineIndex) == 1;
            }
        }

        if (bIsAutoPickup)
        {
            // 设置呼叫参数
            param.eCallOutType = TCT_PICKUP;
            param.iPickUpBLFIndex = dsskey_GetDsskeyIndexByID(iDsskeyID);
            param.bBindDsskey = false;
        }
    }

    return true;
}

#endif  //IF_FEATURE_DSSKEY
