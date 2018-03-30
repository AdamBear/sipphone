#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyBlfAction::CDsskeyBlfAction(DssKey_Type dsskeyType)
    : CDsskeyBlfBaseAction(dsskeyType)
{

}


bool CDsskeyBlfAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                  void* pExtraData)
{
    // 先由基类处理
    if (CDsskeyBlfBaseAction::OnKeyPress(dsskeyDataID, logicData, pExtraData))
    {
        return true;
    }

    // 如果监控的账号不为空,则使用Dsskey呼出

    CallOutByDsskey(logicData);

    return true;
}

bool CDsskeyBlfAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    yl::string strSend = "";
    bool bAutoPickup = false;
    int iStatus = GetRemoteStatus(dsskeyID);
    if (NULL != pExtraData)
    {
        iStatus = *((int*)pExtraData);
    }

    if (iStatus == DS_BLF_STATUS_RINGING)
    {
        return GetDPickupCallOutNumber(dsskeyID, strCallOutNumber, logicData.strValue,
                                       logicData.strExtension, logicData.iLineIndex);
    }

    yl::string strNumber = logicData.strValue;

    if (1 == configParser_GetConfigInt(kszBlfCallOutByNumber))
    {
        // 截取sip:11231@192.168.1.199 中的11231，只呼出11231
        yl::string strSIPName;
        yl::string strServerName;
        commonUnits_ParserDialURI(strNumber, strSIPName, strServerName);
        strNumber = strSIPName;
    }

    if (iStatus == DS_BLF_STATUS_PARK)
    {
        yl::string strRetrieveCallParkedCode =
            configParser_GetCfgItemStringValue(kszBLFRetrieveCallPark, logicData.iLineIndex);

        if (!strRetrieveCallParkedCode.empty())
        {
            strSend = strRetrieveCallParkedCode.c_str();
            strSend = strSend + strNumber;
        }
        else
        {
            strSend = strNumber;
            AddIntercomCode(strSend);
        }
    }
    else
    {
        strSend = strNumber;
        AddIntercomCode(strSend);
    }

    strCallOutNumber.swap(strSend);
    return !strCallOutNumber.empty();
}

bool CDsskeyBlfAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
        void* pExtraData)
{
    // 从blf变为其他的类型，需要跟sip发blf改变的消息，sip去取消订阅
    if (newType == DT_BLF)
    {
        return false;
    }

    // 给BLF发送配置改变消息
    etl_NotifyApp(false, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, dsskeyID, 0);

    // 向SIP发送BLF改变消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, dsskey_GetDsskeyIndexByID(dsskeyID));

    return true;
}

bool CDsskeyBlfAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (DSSKEY_ACTION_MAKE_CALL == iActionType)
    {
        CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);

        int iDsskeyID = logicData.iDssKeyID;
        int iStatus = GetRemoteStatus(iDsskeyID);

        if (iStatus == DS_BLF_STATUS_RINGING)
        {
            param.eCallOutType = TCT_PICKUP;

            // BLF状态m_cBlfInfo[i]的最高位表示SIP是否处理自动Call Pickup
            bool bAutoPickup = configParser_GetCfgItemIntValue(kszAccountDialogPickUp, logicData.iLineIndex) == 1;

            if (bAutoPickup && blf_IsSupportAutopPickUpByDsskey(iDsskeyID))
            {
                // 设置呼叫参数
                param.iPickUpBLFIndex = dsskey_GetDsskeyIndexByID(iDsskeyID);
            }

            param.bBindDsskey = false;
        }
        else if (DS_BLF_STATUS_TALKING == iStatus
                 || DS_BLF_STATUS_HOLD == iStatus
                 || DS_BLF_STATUS_PARK == iStatus)
        {
            param.bBindDsskey = false;
        }

        return true;
    }

    return CDsskeyBlfBaseAction::GetCallOutParam(iActionType, proxy, logicData, param);
}

#ifdef IF_FEATURE_GENBAND
CDsskeyBlfGroupAction::CDsskeyBlfGroupAction(DssKey_Type dsskeyType)
    : CDsskeyBlfAction(dsskeyType)
{

}

bool CDsskeyBlfGroupAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType,
        DssKey_Type newType, void* pExtraData)
{
    // 从blf变为其他的类型，需要跟sip发blf改变的消息，sip去取消订阅
    if (newType == DT_BLFGROUP)
    {
        return false;
    }

    // 给BLF发送配置改变消息
    etl_NotifyApp(false, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, dsskeyID, 0);

    // 向SIP发送BLF改变消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, dsskey_GetDsskeyIndexByID(dsskeyID));

    return true;
}

bool CDsskeyBlfGroupAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                       void* pExtraData)
{
    TALK_INFO("CDsskeyBlfGroupAction::OnKeyPress ");
    CallOutByDsskey(logicData);

    return true;
}


bool CDsskeyBlfGroupAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    int iStatus = GetRemoteStatus(dsskeyID);
    if (iStatus != DS_BLF_STATUS_RINGING && iStatus != DS_BLF_STATUS_PARK)
    {
        return false;
    }

    return CDsskeyBlfAction::GetCallOutNumber(dsskeyID, strCallOutNumber, logicData, pExtraData,
            nExtType);
}

bool CDsskeyBlfGroupAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    ROUTINE_STATE eRoutine = proxy.GetInheritsRoutine();
    if (ROUTINE_DIALING == eRoutine
            || ROUTINE_RINGING == eRoutine
            || ROUTINE_TALKING == eRoutine
            || ROUTINE_AOC_FINISH == eRoutine)
    {
        return false;
    }

    return CDsskeyBindAction::ProcessNoBindDsskey(proxy, logicData);
}

#endif
#endif  //IF_FEATURE_DSSKEY
