#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"


CDsskeyIntercomAction::CDsskeyIntercomAction()
    : CDsskeyBlfBaseAction(DT_INTERCOM)
{

}

CDsskeyIntercomAction::~CDsskeyIntercomAction()
{

}


bool CDsskeyIntercomAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                       void* pExtraData)
{
    CSessionStateProxy focus;
    if (!focus && logicData.strValue.empty())
    {
        // 没有通话存在时
        return EnterDialUI(logicData);
    }
    else
    {
        // 使用Dsskey呼出
        return CallOutByDsskey(logicData);
    }
}

bool CDsskeyIntercomAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    // http://10.2.1.199/Bug.php?BugID=87822
    if (GetRemoteStatus(dsskeyID) == DS_BLF_STATUS_RINGING
            && 1 == configParser_GetConfigInt(kszIntercomLedEnable))
    {
        return GetDPickupCallOutNumber(dsskeyID, strCallOutNumber, logicData.strValue,
                                       logicData.strExtension, logicData.iLineIndex);
    }

    strCallOutNumber += logicData.strValue;
    AddIntercomCode(strCallOutNumber);

    return !strCallOutNumber.empty();
}

bool CDsskeyIntercomAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType,
        DssKey_Type newType, void* pExtraData)
{
    if (newType == DT_INTERCOM)
    {
        return false;
    }

    // 给BLF发送配置改变消息
    etl_NotifyApp(false, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, dsskeyID, 0);

    // 向SIP发送BLF改变消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, dsskey_GetDsskeyIndexByID(dsskeyID));

    return true;
}

bool CDsskeyIntercomAction::OnConfigChange(int dsskeyID,  void* pExtraData)
{
    // 给BLF模块发送配置改变消息
    etl_NotifyApp(false, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, dsskeyID, 0);

    // 向SIP发送BLF改变消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, dsskey_GetDsskeyIndexByID(dsskeyID));

    return true;
}

#if IF_FEATRUE_PTT
bool CDsskeyIntercomAction::OnKeyRelease(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
    if (dsskey_IsExistLongPress() && dsskey_RemoveLongPress(dsskeyDataID))
    {
        return ExitSessionByDsskey(dsskeyDataID);
    }
    return false;
}
#endif

#endif  //IF_FEATURE_DSSKEY
