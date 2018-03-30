#ifndef _MOD_BW_CALLCONTROL_H_
#define _MOD_BW_CALLCONTROL_H_

#include "bwcommon.h"
/*******************************************************************
** 函数名:       BWCallControl_CheckPhoneNumberValid
** 函数描述:    判断Phone Number格式是否合法
**              [in] szNumber:  要判断的号码
**              [in] nMaxLength:号码允许的最大长度，不同的服务允许的长度不一样。
                     如Anywhere为19，Remoteoffice为30
** 返回:      true：合法   false：非法
*******************************************************************/
bool BWCallControl_CheckPhoneNumberValid(const char * szNumber, int nMaxLength = 30);

/*******************************************************************
** 函数名:       BWCallControl_IsAccountXSIEnable
** 函数描述:    判断某个账号的XSI是否可用
**              [in] nAccountID:    要判断的账号ID
** 返回:      true：可用   false：不可用
*******************************************************************/
bool BWCallControl_IsAccountXSIEnable(int nAccountID);

/*******************************************************************
** 函数名:       BWCallControl_SetAccountXSIPassword
** 函数描述:    设置某个账号的XSI密码
**              [in] nAccountID:    要判断的账号ID
** 返回:      true：可用   false：不可用
*******************************************************************/
bool BWCallControl_SetAccountXSIPassword(const char * szPassword, int nAccountID);

/*******************************************************************
** 函数名:       CLID_SetAnonymousCallState
** 函数描述:    设置匿名呼叫开启/关闭状态
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_SetAnonymousCallState(bool bState, int nLineID);

/*******************************************************************
** 函数名:       CLID_QueryAnonymousCallState
** 函数描述:    获取服务器上的匿名呼叫状态，获取的状态是通过消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_QueryAnonymousCallState(int nLineID);

/*******************************************************************
** 函数名:       CLID_GetAnonymousCallState
** 函数描述:    获取上次从服务器请求的Anonymous Call状态
**              [in] nLineID:   根据账号获取对应信息
** 返回:      Anonymous Call状态
*******************************************************************/
bool CLID_GetAnonymousCallState(int nLineID);

/*******************************************************************
** 函数名:       CLID_SetAnonymousCallRejectonState
** 函数描述:    设置匿名呼叫拒绝开启/关闭状态
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_SetAnonymousCallRejectonState(bool bState, int nLineID);

/*******************************************************************
** 函数名:       CLID_QueryAnonymousCallRejectionState
** 函数描述:    获取服务器上的匿名呼叫拒绝状态，获取的状态是通过消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CLID_QueryAnonymousCallRejectionState(int nLineID);

/*******************************************************************
** 函数名:       CLID_GetAnonymousCallRejectionState
** 函数描述:    获取上次从服务器请求的Anonymous Call Rejection状态
**              [in] nLineID:   根据账号获取对应信息
** 返回:      Anonymous Call Rejection状态
*******************************************************************/
bool CLID_GetAnonymousCallRejectionState(int nLineID);

/*******************************************************************
** 函数名:       CLID_CancelRequest
** 函数描述:    取消任务
** 返回:
*******************************************************************/
void CLID_CancelRequest();

/*******************************************************************
** 函数名:       RO_SetRemoteOffice
** 函数描述:    设置Remote Office
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] szRemoteNumber:    Remote Office Number
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool RO_SetRemoteOffice(bool bState, const char * szRemoteNumber, int nLineID);

/*******************************************************************
** 函数名:       RO_QueryRemoteOffice
** 函数描述:    获取服务器上的Remote Office信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool RO_QueryRemoteOffice(int nLineID);

/*******************************************************************
** 函数名:       RO_GetRemoteOfficeInfo
** 函数描述:    获取上次从服务器请求的Remote office信息
**              [out] roInfo:   Remote Office 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
void RO_GetRemoteOfficeInfo(SRemoteOffice & roInfo, int nLineID);

/*******************************************************************
** 函数名:       RO_CancelRequest
** 函数描述:    取消Remote Office任务
** 返回:
*******************************************************************/
void RO_CancelRequest();

/*******************************************************************
** 函数名:       SRP_SetSimultaneousRing
** 函数描述:    设置Simultaneous Ring Personal
**              [in] simRingInfo:   Simultaneous Ring Personal内容
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool SRP_SetSimultaneousRing(const SSimultaneousRing & simRingInfo);

/*******************************************************************
** 函数名:       SRP_QuerySimultaneousRing
** 函数描述:    获取服务器上的Simultaneous Ring Personal信息，返回的结果是通过发送
                SRP_MESSAGE_GET_RESULT消息通知下载成功，调用SRP_GetSimultaneousRingInfo获取信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool SRP_QuerySimultaneousRing(int nLineID);

/*******************************************************************
** 函数名:       SRP_GetSimultaneousRingInfo
** 函数描述:    获取上次从服务器得到的Simultaneous Ring Personal信息
**              [out] simRingInfo: 返回的结果
**              [in] nLineID:   根据账号获取对应信息, 当nLineID == -1时，获取当前界面所在账号的信息
** 返回:      true：执行成功   false：执行失败
*******************************************************************/
bool SRP_GetSimultaneousRingInfo(SSimultaneousRing & simRingInfo);

/*******************************************************************
** 函数名:       SRP_CancelRequest
** 函数描述:    取消Simultaneous Ring Personal任务
** 返回:
*******************************************************************/
void SRP_CancelRequest();

/*******************************************************************
** 函数名:       SRP_CheckNumberExit
** 函数描述:    检查号码在Simultaneous Ring Personal里是否已经存在
**              [in] szNumber:  要检查的号码
** 返回:      true：已存在   false：不存在
*******************************************************************/
bool SRP_CheckNumberExit(const char * szNumber);

/*******************************************************************
** 函数名:       CFNR_SetForwardNotReachable
** 函数描述:    设置Forward Not Reachable
**              [in] bState:    要设置的状态，true:开启，false:关闭
**              [in] szForwardNumber:   Forward Number
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CFNR_SetForwardNotReachable(bool bState, const char * szForwardNumber, int nLineID);

/*******************************************************************
** 函数名:       CFNR_QueryForwardNotReachable
** 函数描述:    获取服务器上的Forward Not Reachable信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CFNR_QueryForwardNotReachable(int nLineID);

/*******************************************************************
** 函数名:       CFNR_GetForwardNotReachableInfo
** 函数描述:    获取上次从服务器请求的Forward Not Reachable信息
**              [out] cfnrInfo: Forward Not Reachable 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
void CFNR_GetForwardNotReachableInfo(SCallForwardNotReachable & cfnrInfo, int nLineID);

/*******************************************************************
** 函数名:       CFNR_CancelRequest
** 函数描述:    取消Forward Not Reachable任务
** 返回:
*******************************************************************/
void CFNR_CancelRequest();

/*******************************************************************
** 函数名:       MBLT_SetMobility
** 函数描述:    设置mobility
**              [in] mobilityInfo Mobility信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool MBLT_SetMobility(const SMobility & mobilityInfo, int nLineID);

/*******************************************************************
** 函数名:       MBLT_QueryMoblity
** 函数描述:    获取服务器上的Moblity信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool  MBLT_QueryMobility(int nLineID);

/*******************************************************************
** 函数名:       RO_GetMobilityInfo
** 函数描述:    获取上次从服务器请求的Mobility信息
**              [out] mobilityInfo: Mobility 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
void  MBLT_GetMobilityInfo(SMobility & mobilityInfo, int nLineID);

/*******************************************************************
** 函数名:       RO_CancelRequest
** 函数描述:    取消Mobility任务
** 返回:
*******************************************************************/
void MBLT_CancelRequest();

/*******************************************************************
** 函数名:       GNF_QueryGnf
** 函数描述:    获取服务器上的Group Night Forward信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool  GNF_QueryGnf(int nLineID);

/*******************************************************************
** 函数名:       GNF_GetGnfInfo
** 函数描述:    获取上次从服务器请求的Group Night Forward信息
**              [out] gnfInfo:  Group Night Forward 信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
void  GNF_GetGnfInfo(SGroupNightForward & gnfInfo, int nLineID);

/*******************************************************************
** 函数名:       GNF_CancelRequest
** 函数描述:    取消Group Night Forward任务
** 返回:
*******************************************************************/
void GNF_CancelRequest();

/*******************************************************************
** 函数名:       MOH_QueryMusicOnHold
** 函数描述:    获取服务器上的MusicOnHold信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool MOH_QueryMusicOnHold(int nLineID);

/*******************************************************************
** 函数名:       MOH_GetMusicOnHoldState
** 函数描述:    获取上次从服务器请求的MusicOnHold信息
**              [out] bActive:  MusicOnHold 状态信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
bool  MOH_GetMusicOnHoldState(int nLineID);

/*******************************************************************
** 函数名:       MOH_SetMusicOnHoldState
** 函数描述:    设置MusicOnHold
**              [in] bActive MusicOnHold状态信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool MOH_SetMusicOnHoldState(bool bActive, int nLineID);

/*******************************************************************
** 函数名:       MOH_CancelRequest
** 函数描述:    取消MusicOnHold任务
** 返回:  无
*******************************************************************/
void MOH_CancelRequest();

/*******************************************************************
** 函数名:       CW_QueryCallWaiting
** 函数描述:    获取服务器上的CallWaiting信息，返回的结果是通过发送XSI_MESSAGE_REQ_FINISH消息来传送的
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CW_QueryCallWaiting(int nLineID);

/*******************************************************************
** 函数名:       CW_GetCallWaitingState
** 函数描述:    获取上次从服务器请求的CallWaiting信息
**              [out] bActive:  CallWaiting 状态信息
**              [in] nLineID:   根据账号获取对应信息
** 返回:      无
*******************************************************************/
bool  CW_GetCallWaitingState(int nLineID);

/*******************************************************************
** 函数名:       CW_SetCallWaitingState
** 函数描述:    设置CallWaiting
**              [in] bActive CallWaiting状态信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool CW_SetCallWaitingState(bool bActive, int nLineID);

/*******************************************************************
** 函数名:       CW_CancelRequest
** 函数描述:    取消CallWaiting任务
** 返回:  无
*******************************************************************/
void CW_CancelRequest();

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
/*******************************************************************
** 函数名:       FS_QueryForwardSelective
** 函数描述:    查询服务器上的ForwardSelective信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool FS_QueryForwardSelective(int iLineId);

/*******************************************************************
** 函数名:       FS_GetForwardSelectiveInfo
** 函数描述:    获取缓存在本地的ForwardSelective信息
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool FS_GetForwardSelectiveInfo(SForwardSelectiveData & sData, int iLineId);

/*******************************************************************
** 函数名:       FS_QueryForwardSelective
** 函数描述:    保存ForwardSelective信息到服务器
**              [in] nLineID:   账号
** 返回:      true：命令执行成功   false：执行失败
*******************************************************************/
bool FS_SetForwardSelective(const SForwardSelectiveData & sData, int iLineId);
#endif


/*******************************************************************
** 函数名:       SA_QuerySilentAlerting
** 函数描述:        查询SilentAlerting信息
**              [in] nLineID:   账号
** 返回:          true：命令执行成功   false：执行失败
*******************************************************************/
bool SA_QuerySilentAlerting(int iLineId);

/*******************************************************************
** 函数名:       SA_GetSilentAlerting
** 函数描述:        获取SilentAlerting信息
**              [in] nLineID:   账号  [out] bValue: 值
** 返回:          true：命令执行成功   false：执行失败
*******************************************************************/
bool SA_GetSilentAlerting(int iLineId, bool & bValue);

/*******************************************************************
** 函数名:       SA_SetSilentAlerting
** 函数描述:        设置SilentAlerting信息
**              [in] nLineID:   账号  [in] bValue: 值
** 返回:          true：命令执行成功   false：执行失败
*******************************************************************/
bool SA_SetSilentAlerting(int iLineId, bool bValue);

/*******************************************************************
** 函数名:       SA_CancelRequest
** 函数描述:     取消SIlentAlert请求
*******************************************************************/
void SA_CancelRequest();
#endif //_MOD_BW_CALLCONTROL_H_
