#include "bwcallcontrol_inc.h"

#ifdef IF_FEATURE_BROADSOFT_CLID
/*******************************************************************
** 函数名:       CLID_SetAnonymousCallState
** 函数描述:    设置匿名呼叫开启/关闭状态
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_SetAnonymousCallState(bool bState, int nLineID)
{
    return _CLIDProcesser.SetAnonymousCallState(bState, nLineID);
}

/*******************************************************************
** 函数名:       CLID_QueryAnonymousCallState
** 函数描述:    获取服务器上的匿名呼叫状态，获取的状态是通过消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_QueryAnonymousCallState(int nLineID)
{
    return _CLIDProcesser.QueryAnonymousCallState(nLineID);
}

/*******************************************************************
** 函数名:       CLID_GetAnonymousCallState
** 函数描述:    获取上次从服务器请求的Anonymous Call状态
**              [in] nLineID:   根据账号获取对应信息
** 返回:      Anonymous Call状态
*******************************************************************/
bool CLID_GetAnonymousCallState(int nLineID)
{
    return _CLIDProcesser.GetAnonymousCallState(nLineID);
}

/*******************************************************************
** 函数名:       CLID_SetAnonymousCallRejectonState
** 函数描述:    设置匿名呼叫拒绝开启/关闭状态
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_SetAnonymousCallRejectonState(bool bState, int nLineID)
{
    return _CLIDProcesser.SetAnonymousCallRejectionState(bState, nLineID);
}

/*******************************************************************
** 函数名:       CLID_QueryAnonymousCallRejectionState
** 函数描述:    获取服务器上的匿名呼叫拒绝状态，获取的状态是通过消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_QueryAnonymousCallRejectionState(int nLineID)
{
    return _CLIDProcesser.QueryAnonymousCallRejectionState(nLineID);
}

/*******************************************************************
** 函数名:       CLID_GetAnonymousCallRejectionState
** 函数描述:    获取上次从服务器请求的Anonymous Call Rejection状态
**              [in] nLineID:   根据账号获取对应信息
** 返回:      Anonymous Call Rejection状态
*******************************************************************/
bool CLID_GetAnonymousCallRejectionState(int nLineID)
{
    return _CLIDProcesser.GetAnonymousCallRejectionState(nLineID);
}

/*******************************************************************
** 函数名:       CLID_CancelRequest
** 函数描述:    取消任务
** 返回:
*******************************************************************/
void CLID_CancelRequest()
{
    _CLIDProcesser.CancelAllRequest();
}
#else
bool CLID_SetAnonymousCallState(bool bState, int nLineID)
{
    return false;
}

bool CLID_QueryAnonymousCallState(int nLineID)
{
    return false;
}

bool CLID_SetAnonymousCallRejectonState(bool bState, int nLineID)
{
    return false;
}

bool CLID_QueryAnonymousCallRejectionState(int nLineID)
{
    return false;
}

void CLID_CancelRequest()
{
}
#endif // #ifdef IF_FEATURE_BROADSOFT_CLID

#ifdef IF_FEATURE_BROADSOFT_REMOTE_OFFICE
/*******************************************************************
** 函数名:       RO_SetRemoteOffice
** 函数描述:    设置Remote Office
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] szRemoteNumber:    Remote Office Number
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
**  //Modified by yuyt, 2014/8/15
*******************************************************************/
bool RO_SetRemoteOffice(bool bState, const char * szRemoteNumber, int nLineID)
{
    if (NULL == szRemoteNumber)
    {
        return false;
    }
    return g_refRemoteOfficeProcessor.SetRemoteOffice(bState, szRemoteNumber, nLineID);
}

/*******************************************************************
** 函数名:       RO_QueryRemoteOffice
** 函数描述:    获取服务器上的Remote Office信息，返回的结果是通过发送RO_MESSAGE_GET_RESULT消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
**  //Modified by yuyt, 2014/8/15
*******************************************************************/
bool RO_QueryRemoteOffice(int nLineID)
{
    return g_refRemoteOfficeProcessor.QueryRemoteOffice(nLineID);
}

/*******************************************************************
** 函数名:       RO_GetRemoteOfficeInfo
** 函数描述:    获取上次从服务器请求的Remote office信息
**              [out] roInfo:   Remote Office 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
**  //Modified by yuyt, 2014/8/15
*******************************************************************/
void RO_GetRemoteOfficeInfo(SRemoteOffice & roInfo, int nLineID)
{
    g_refRemoteOfficeProcessor.GetRemoteOfficeInfo(roInfo, nLineID);
}

/*******************************************************************
** 函数名:       RO_CancelRequest
** 函数描述:    取消Remote Office任务
** 返回:
**  //Modified by yuyt, 2014/8/15
*******************************************************************/
void RO_CancelRequest()
{
    g_refRemoteOfficeProcessor.CancelAllRequest();
}
#else
bool RO_SetRemoteOffice(bool bState, const char * szRemoteNumber, int nLineID)
{
    return false;
}

bool RO_QueryRemoteOffice(int nLineID)
{

    return false;
}

void RO_GetRemoteOfficeInfo(SRemoteOffice & roInfo, int nLineID)
{
}

void RO_CancelRequest()
{
}
#endif // #ifdef IF_FEATURE_BROADSOFT_REMOTE_OFFICE

#ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT
/*******************************************************************
** 函数名:       SRP_SetSimultaneousRing
** 函数描述:    设置Simultaneous Ring Personal
**              [in] simRingInfo:   Simultaneous Ring Personal内容
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
** Modified by yuyt, 2014/8/18
*******************************************************************/
bool SRP_SetSimultaneousRing(const SSimultaneousRing & simRingInfo)
{
    return g_refSRPProcessor.SetSRP(simRingInfo);
}

/*******************************************************************
** 函数名:       SRP_QuerySimultaneousRing
** 函数描述:    获取服务器上的Simultaneous Ring Personal信息，返回的结果是通过发送
                SRP_MESSAGE_GET_RESULT消息通知下载成功，调用SRP_GetSimultaneousRingInfo获取信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
** Modified by yuyt, 2014/8/18
*******************************************************************/
bool SRP_QuerySimultaneousRing(int nLineID)
{
    return g_refSRPProcessor.QuerySRP(nLineID);
}

/*******************************************************************
** 函数名:       SRP_GetSimultaneousRingInfo
** 函数描述:    获取上次从服务器得到的Simultaneous Ring Personal信息
**              [out] simRingInfo: 返回的结果
**              [in] nLineID:   根据账号获取对应信息
** 返回:      true：执行成功   false：执行失败
** Modified by yuyt, 2014/8/18
*******************************************************************/
bool SRP_GetSimultaneousRingInfo(SSimultaneousRing & simRingInfo)
{
    return g_refSRPProcessor.GetSRPInfo(simRingInfo);
}

/*******************************************************************
** 函数名:       SRP_CancelRequest
** 函数描述:    取消Simultaneous Ring Personal任务
** 返回:
** Modified by yuyt, 2014/8/18
*******************************************************************/
void SRP_CancelRequest()
{
    g_refSRPProcessor.CancelAllRequest();
}

/*******************************************************************
** 函数名:       SRP_CheckNumberExit
** 函数描述:    检查号码在Simultaneous Ring Personal里是否已经存在
**              [in] szNumber:  要检查的号码
** 返回:      true：已存在   false：不存在
** Modified by yuyt, 2014/8/18
*******************************************************************/
bool SRP_CheckNumberExit(const char * szNumber)
{
    return g_refSRPProcessor.CheckNumberExit(szNumber);
}
#else
bool SRP_SetSimultaneousRing(const SSimultaneousRing & simRingInfo)
{
    return false;
}

bool SRP_QuerySimultaneousRing(int nLineID)
{
    return false;
}

bool SRP_GetSimultaneousRingInfo(SSimultaneousRing & simRingInfo)
{
    return false;
}

void SRP_CancelRequest()
{
}

bool SRP_CheckNumberExit(const char * szNumber)
{
    return false;
}
#endif // #ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
/*******************************************************************
** 函数名:       CFNR_SetForwardNotReachable
** 函数描述:    设置Forward Not Reachable
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] szForwardNumber:   Forward Number
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
**  Modified by yuyt! 2014/8/12
*******************************************************************/
bool CFNR_SetForwardNotReachable(bool bState, const char * szForwardNumber, int nLineID)
{
    if (NULL == szForwardNumber)
    {
        return false;
    }
    return g_pCFNRProcessor->SetCFNR(bState, szForwardNumber, nLineID);
}

/*******************************************************************
** 函数名:       CFNR_QueryForwardNotReachable
** 函数描述:    获取服务器上的Forward Not Reachable信息，返回的结果是通过CFNR_GetForwardNotReachableInfo接口获得
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
** Modified by yuyt! 2014/8/12
*******************************************************************/
bool CFNR_QueryForwardNotReachable(int nLineID)
{
    return g_pCFNRProcessor->QueryCFNR(nLineID);
}

/*******************************************************************
** 函数名:       CFNR_GetForwardNotReachableInfo
** 函数描述:    获取上次从服务器请求的Forward Not Reachable信息
**              [out] cfnrInfo: Forward Not Reachable 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
** Modified by yuyt! 2014/8/12
*******************************************************************/
void CFNR_GetForwardNotReachableInfo(SCallForwardNotReachable & cfnrInfo, int nLineID)
{
    g_pCFNRProcessor->GetCFNRInfo(cfnrInfo, nLineID);
}

/*******************************************************************
** 函数名:       CFNR_CancelRequest
** 函数描述:    取消Forward Not Reachable任务
** 返回:
** Modified by yuyt! 2014/8/12
*******************************************************************/
void CFNR_CancelRequest()
{
    g_pCFNRProcessor->CancelAllRequest();
}
#else
bool CFNR_SetForwardNotReachable(bool bState, const char * szForwardNumber, int nLineID)
{
    return false;
}

bool CFNR_QueryForwardNotReachable(int nLineID)
{

    return false;
}

void CFNR_GetForwardNotReachableInfo(SCallForwardNotReachable & cfnrInfo, int nLineID)
{
}

void CFNR_CancelRequest()
{
}
#endif // #ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE

#ifdef IF_FEATURE_BROADSOFT_MOBILITY
/*******************************************************************
** 函数名:       MBLT_SetMobility
** 函数描述:    设置mobility
**              [in] mobilityInfo Mobility信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
** Modified by yuyt! 2014/8/15
*******************************************************************/
bool MBLT_SetMobility(const SMobility & mobilityInfo, int nLineID)
{
    return g_refMBLTProcessor.SetMobility(mobilityInfo, nLineID);
}

/*******************************************************************
** 函数名:       MBLT_QueryMoblity
** 函数描述:    获取服务器上的Moblity信息，返回的结果是通过发送MBLT_MESSAGE_GET_RESULT消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
** Modified by yuyt! 2014/8/15
*******************************************************************/
bool  MBLT_QueryMobility(int nLineID)
{
    return g_refMBLTProcessor.QueryMobility(nLineID);
}

/*******************************************************************
** 函数名:       RO_GetMobilityInfo
** 函数描述:    获取上次从服务器请求的Mobility信息
**              [out] mobilityInfo: Mobility 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
** Modified by yuyt! 2014/8/15
*******************************************************************/
void  MBLT_GetMobilityInfo(SMobility & mobilityInfo, int nLineID)
{
    g_refMBLTProcessor.GetMobilityInfo(mobilityInfo, nLineID);
}

/*******************************************************************
** 函数名:       RO_CancelRequest
** 函数描述:    取消Mobility任务
** 返回:
** Modified by yuyt! 2014/8/15
*******************************************************************/
void MBLT_CancelRequest()
{
    g_refMBLTProcessor.CancelAllRequest();
}
#else
bool MBLT_SetMobility(const SMobility & mobilityInfo, int nLineID)
{
    return false;
}

bool MBLT_QueryMobility(int nLineID)
{

    return false;
}

void MBLT_GetMobilityInfo(SMobility & mobilityInfo, int nLineID)
{
}

void MBLT_CancelRequest()
{
}
#endif // #ifdef IF_FEATURE_BROADSOFT_MOBILITY

#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
/*******************************************************************
** 函数名:       GNF_QueryGnf
** 函数描述:    获取服务器上的Group Night Forward信息，返回的结果是通过发送GNF_MESSAGE_GET_RESULT消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
** Modified by yuyt! 2014/8/13
*******************************************************************/
bool  GNF_QueryGnf(int nLineID)
{
    return g_pGNFProcessor->QueryGNF(nLineID);
}

/*******************************************************************
** 函数名:       GNF_GetGnfInfo
** 函数描述:    获取上次从服务器请求的Group Night Forward信息
**              [out] gnfInfo:  Group Night Forward 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
** Modified by yuyt! 2014/8/13
*******************************************************************/
void  GNF_GetGnfInfo(SGroupNightForward & gnfInfo, int nLineID)
{
    g_pGNFProcessor->GetGNFInfo(gnfInfo, nLineID);
}

/*******************************************************************
** 函数名:       GNF_CancelRequest
** 函数描述:    取消Group Night Forward任务
** 返回:
** Modified by yuyt! 2014/8/13
*******************************************************************/
void GNF_CancelRequest()
{
    g_pGNFProcessor->CancelAllRequest();
}
#else
bool GNF_QueryGnf(int nLineID)
{
    return false;
}

void GNF_GetGnfInfo(SGroupNightForward & gnfInfo, int nLineID)
{
}

void GNF_CancelRequest()
{
}
#endif

#ifdef IF_FEATURE_BROADSOFT_MUSIC_ON_HOLD

/*******************************************************************
** 函数名:       MOH_QueryMusicOnHold
** 函数描述:    获取服务器上的MusicOnHold信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool MOH_QueryMusicOnHold(int nLineID)
{
    return g_refMOHProcessor.QueryMOH(nLineID);
}

/*******************************************************************
** 函数名:       MOH_GetMusicOnHoldInfo
** 函数描述:    获取上次从服务器请求的MusicOnHold信息
**              [out] bActive:  MusicOnHold 状态信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
bool  MOH_GetMusicOnHoldState(int nLineID)
{
    return g_refMOHProcessor.GetMOHState(nLineID);
}

/*******************************************************************
** 函数名:       MOH_SetMusicOnHold
** 函数描述:    设置MusicOnHold
**              [in] bActive MusicOnHold状态信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool MOH_SetMusicOnHoldState(bool bActive, int nLineID)
{
    return g_refMOHProcessor.SetMOHState(bActive, nLineID);
}

/*******************************************************************
** 函数名:       MOH_CancelRequest
** 函数描述:    取消MusicOnHold任务
** 返回:  无
*******************************************************************/
void MOH_CancelRequest()
{
    g_refMOHProcessor.CancelAllRequest();
}

#else // IF_FEATURE_BROADSOFT_MUSIC_ONHOLD

bool MOH_QueryMusicOnHold(int nLineID)
{
    return false;
}

bool  MOH_GetMusicOnHoldState(int nLineID)
{
    return false;
}

bool MOH_SetMusicOnHold(bool bActive, int nLineID)
{
    return false;
}

void MOH_CancelRequest()
{
}

#endif // IF_FEATURE_BROADSOFT_MUSIC_ONHOLD

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
/*******************************************************************
** 函数名:       CW_QueryCallWaiting
** 函数描述:    获取服务器上的CallWaiting信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CW_QueryCallWaiting(int nLineID)
{
    return g_refCallWatingProcessor.QueryCallWating(nLineID);
}

/*******************************************************************
** 函数名:       CW_GetCallWaitingState
** 函数描述:    获取上次从服务器请求的CallWaiting信息
**              [out] bActive:  CallWaiting 状态信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
bool  CW_GetCallWaitingState(int nLineID)
{
    return g_refCallWatingProcessor.GetCallWatingState(nLineID);
}

/*******************************************************************
** 函数名:       CW_SetCallWaitingState
** 函数描述:    设置CallWaiting
**              [in] bActive CallWaiting状态信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CW_SetCallWaitingState(bool bActive, int nLineID)
{
    return g_refCallWatingProcessor.SetCallWatingState(bActive, nLineID);
}

/*******************************************************************
** 函数名:       CW_CancelRequest
** 函数描述:    取消CallWaiting任务
** 返回:  无
*******************************************************************/
void CW_CancelRequest()
{
    g_refCallWatingProcessor.CancelAllRequest();
}

#else //IF_FEATURE_BROADSOFT_CALL_WAITING
bool CW_QueryCallWaiting(int nLineID)
{
    return false;
}

bool  CW_GetCallWaitingState(int nLineID)
{
    return false;
}

bool CW_SetCallWaitingState(bool bActive, int nLineID)
{
    return false;
}

void CW_CancelRequest()
{
}
#endif //IF_FEATURE_BROADSOFT_CALL_WAITING

//判断所填的Phone Number是否合法
bool BWCallControl_CheckPhoneNumberValid(const char * szNumber, int nMaxLength /*= 30*/)
{
    if (NULL == szNumber)
    {
        return false;
    }

    int nLength = strlen(szNumber);
    if (nLength < 1
            || nLength > nMaxLength)
    {
        return false;
    }

    //An E.164 number used for outgoing calls must contain only characters +, - and 0-9.
    //Only the leading character may be a +.
    for (int i = 0; i < nLength; i++)
    {
        if (szNumber[i] == '+')
        {
            if (i > 0)
            {
                return false;
            }
        }
        else if (szNumber[i] != '-'
                 && (szNumber[i] < '0' || szNumber[i] > '9')
                 && szNumber[i] != ' ')
        {
            return false;
        }
    }

    return true;
}

/*******************************************************************
** 函数名:       BWCallControl_IsAccountXSIEnable
** 函数描述:    判断某个账号的XSI是否可用
**              [in] nAccountID:    要判断的账号ID
** 返回:      true：可用   false：不可用
*******************************************************************/
bool BWCallControl_IsAccountXSIEnable(int nAccountID)
{
    if (nAccountID < 0)
    {
        return false;
    }

    //获取UserID
    yl::string strUserID = configParser_GetCfgItemStringValue(kszAccountXSIUserID, nAccountID);

    //获取服务器网址
    yl::string strHostAddr = configParser_GetCfgItemStringValue(kszAccountXSIHostAddr, nAccountID);

    CALLCONTROL_INFO("IsAccountXSIEnable(nAccountID = %d): strUserID = %s, strHostAddr = %s",
                     nAccountID, strUserID.c_str(),
                     strHostAddr.c_str());

    //获取账号名
    yl::string strRegisterName = configParser_GetCfgItemStringValue(kszAccountRegisterName, nAccountID);

    //获取服务器类型
    yl::string  strServerType = configParser_GetCfgItemStringValue(kszAccountXSIServerType, nAccountID);

    //是否是新鉴权机制
    bool bIsAllSipAuth = configParser_GetConfigInt(kszAllowSipAuth) == 1;

    if (!bIsAllSipAuth
            && (strUserID.empty()
                || strHostAddr.empty()
                || strServerType.empty()))
    {
        return false;
    }
    else if (bIsAllSipAuth
             && (strUserID.empty()
                 || strHostAddr.empty()
                 || strServerType.empty()
                 || strRegisterName.empty()))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool BWCallControl_SetAccountXSIPassword(const char * szPassword, int nAccountID)
{
    if (nAccountID < 0
            || NULL == szPassword)
    {
        return false;
    }

    bool bIsAllowSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);
    if (bIsAllowSipAuth)
    {
        configParser_SetCfgItemStringValue(kszAccountPassword, nAccountID, szPassword, CONFIG_LEVEL_PHONE);

        //发送消息，账号配置改变
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nAccountID);
    }
    else
    {
        configParser_SetCfgItemStringValue(kszAccountXSIPassword, nAccountID, szPassword,
                                           CONFIG_LEVEL_PHONE);
    }

    if (nAccountID == 0)
    {
        CBaseHelper::SetXSINotif(true);
    }
    else
    {
        //非第一个账号不需通知外部模块
        CBaseHelper::SetXsiAvailable(true, nAccountID);
    }

    return true;
}

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
bool FS_QueryForwardSelective(int iLineId)
{
    return _CForwardSelectiveProcesser.QueryForwardSelective(iLineId);
}

bool FS_GetForwardSelectiveInfo(SForwardSelectiveData & sData, int iLineId)
{
    return _CForwardSelectiveProcesser.GetForwardSelective(sData, iLineId);
}

bool FS_SetForwardSelective(const SForwardSelectiveData & sData, int iLineId)
{
    return _CForwardSelectiveProcesser.SetForwardSelective(sData, iLineId);
}
#endif

bool SA_QuerySilentAlerting(int iLineId)
{
    return g_pSilentAlertingProcessor.Query(iLineId);
}

bool SA_GetSilentAlerting(int iLineId, bool & bValue)
{
    return g_pSilentAlertingProcessor.GetValue(iLineId, bValue);
}

bool SA_SetSilentAlerting(int iLineId, bool bValue)
{
    return g_pSilentAlertingProcessor.Save(iLineId, bValue);
}

void SA_CancelRequest()
{
    g_pSilentAlertingProcessor.CancelAllRequest();
}
