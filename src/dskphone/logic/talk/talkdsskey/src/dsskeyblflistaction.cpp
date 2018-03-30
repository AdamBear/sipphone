#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyBlfListAction::CDsskeyBlfListAction()
    : CDsskeyBlfBaseAction(DT_BLFLIST)
{
}

bool CDsskeyBlfListAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                      void* pExtraData)
{
    if (CDsskeyBlfBaseAction::OnKeyPress(dsskeyDataID, logicData, pExtraData))
    {
        return true;
    }

    if (!logicData.strValue.empty())
    {
        return CallOutByDsskey(logicData);
    }

    return false;
}

bool CDsskeyBlfListAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    yl::string strSend = logicData.strValue;
    // http://10.2.1.199/Bug.php?BugID=9122
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
    else if (DS_BLF_STATUS_TALKING == iStatus
             || DS_BLF_STATUS_HOLD == iStatus)
    {
        yl::string strPickupCode = configParser_GetCfgItemStringValue(kszBLFListBargeInCode,
                                   logicData.iLineIndex);

        // -- 移植T2X
        if (logicData.strExtension.empty())
        {
            strSend = strPickupCode + logicData.strValue;
        }
        else
        {
            strSend = strPickupCode + logicData.strExtension;
        }

        if (strPickupCode.empty())
        {
            AddIntercomCode(strSend);
        }

        DSSKEY_INFO("blf list call out number = %s", strSend.c_str());
    }
    else if (DS_BLF_STATUS_PARK == iStatus)
    {
        yl::string strRetrieveCallParkedCode =
            configParser_GetCfgItemStringValue(kszBLFRetrieveCallPark, logicData.iLineIndex);

        // 先加上特征码（即使特征码为空）
        strSend = strRetrieveCallParkedCode;

        // 修改blflist所有呼出优先Extension
        strSend += logicData.strExtension.empty() ? logicData.strValue : logicData.strExtension;

        if (strRetrieveCallParkedCode.empty())
        {
            AddIntercomCode(strSend);
        }
    }
    else if (!logicData.strExtension.empty())
    {
        strSend = logicData.strExtension;
        AddIntercomCode(strSend);
    }

    strCallOutNumber = strSend;
    return !strCallOutNumber.empty();
}

bool CDsskeyBlfListAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType,
        DssKey_Type newType, void* pExtraData)
{
    if (NULL == pExtraData)
    {
        dsskey_change_info_t tChangeDssKeyInfo;
        tChangeDssKeyInfo.eNewType = newType;
        tChangeDssKeyInfo.eOldType = oldType;
        tChangeDssKeyInfo.iDsskeyID = dsskeyID;
        tChangeDssKeyInfo.iOldLineID = -1;

        //etl_NotifyApp(false, BLF_MESSAGE_BLFLISTDSSKEY_CONFIGCHANGE, dsskeyID, 0);


        etl_NotifyAppEx(FALSE, DSSKEY_RELOAD_FINISHED, 0, 0, sizeof(dsskey_change_info_t),
                        &tChangeDssKeyInfo);
    }

    return false;
}

bool CDsskeyBlfListAction::OnConfigChange(int dsskeyID, void* pExtraData)
{
    if (NULL == pExtraData)
    {
        // 修改账号id后，需要重新排列刷新dsskey
        dsskey_change_info_t tChangeDssKeyInfo;
        tChangeDssKeyInfo.eNewType = DT_BLFLIST;
        tChangeDssKeyInfo.iDsskeyID = dsskeyID;
        etl_NotifyAppEx(FALSE, DSSKEY_RELOAD_FINISHED, 0, 0, sizeof(dsskey_change_info_t),
                        &tChangeDssKeyInfo);
    }

    return true;
}

bool CDsskeyBlfListAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (DSSKEY_ACTION_MAKE_CALL == iActionType)
    {
        CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);

        int iDsskeyID = logicData.iDssKeyID;
        int iStatus = GetRemoteStatus(iDsskeyID);

        if (iStatus == DS_BLF_STATUS_RINGING)
        {
            param.eCallOutType = TCT_PICKUP;
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


#endif  //IF_FEATURE_DSSKEY
