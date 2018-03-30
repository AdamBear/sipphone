#pragma once

#include "talk/talkadapter/include/talkadapter_data.h"

class CCallInfo;


/*******************************************************************
** 初始化Talk模块
********************************************************************/
void talklogic_Init();

/*******************************************************************
** 函数描述:    设置焦点Session
** 参数:        无
** 返回:        true: 设置成功
********************************************************************/
bool talklogic_SetFocusSessionByID(int iSessionID);


/*******************************************************************
** 函数描述:   根据SessionBlaIndex获取话路的数据
**             [in]iAccount
**             [in]iSessionID
**             [out]dataToUI
** 返回:       话路对应的数据
********************************************************************/
bool talklogic_GetDataByAccountBlaIndex(int iAccount, int iBlaIndex, DataLogic2UI& dataToUI);

/*******************************************************************
** 函数描述:   获取焦点话路的SessionID
**             无
** 返回:       焦点话路的SessionID
********************************************************************/
int talklogic_GetFocusedSessionID();

/*******************************************************************
** 函数描述:   获取焦点话路的Session在list中的序号
**             无
** 返回:       焦点话路的Session在list中的序号
********************************************************************/
int talklogic_GetFocusedSessionIndex();

/*******************************************************************
** 函数描述:   根据SessionID获取话路的数据
**             [in]iSessionID
**             [out]dataToUI
** 返回:       话路对应的数据
********************************************************************/
bool talklogic_GetDataBySessionID(int iSessionID, DataLogic2UI& dataToUI);

/*******************************************************************
** 函数描述:   根据SessionID获取话路的基础数据：供T48使用
**             [in]iSessionID
**             [out]dataToUI
** 返回:       话路对应的数据
********************************************************************/
bool talklogic_GetBaseDataBySessionID(int iSessionID, DataLogic2UI& dataToUI);

/*******************************************************************
** 函数描述:    退出焦点通话
** 参数:      无
** 返回值:       无
********************************************************************/
void talklogic_ExitFocusSession();

/*******************************************************************
** 函数描述:    切换焦点通话
** 参数:      [in] bSwitchDown:          true为往下切换，false为往上切换
** 返回值:       无
********************************************************************/
bool talklogic_SwitchFocusSession(bool bSwitchDown);

/*******************************************************************
** 函数描述:   判断焦点通话是否被hold住
** 返回:       TALK_STATE:TS_TALK,TS_HOLD,TS_HELD
********************************************************************/
TALK_STATE talklogic_GetFocusedSessionTalkState();

/*******************************************************************
** 函数描述:   判断焦点通话是否被隐藏住
** 返回:       hide：True  display ：false
*******************************************************************/
bool talklogic_GetFocusedSessionHideState();

/*******************************************************************
** 函数描述:   获取Aoc的信息
** 参数:       tAocData
iSessionId
** 返回:       是否获取成功
********************************************************************/
bool talklogic_GetFocusedSessionTalkState(DataAoc& tAocData, int iSessionId);

/*******************************************************************
** 函数描述:   获取允许作为TransferTo Target的话路(被当前焦点话路tran)
** 返回:       话路数
********************************************************************/
int talklogic_GetNumberOfAllowTranTo();

/*******************************************************************
** 函数描述:    密码拨号，获得加密后的号码
** 参数:      strCallNum：加密前的号码
** 返回:      加密后的号码
********************************************************************/
yl::string talklogic_HidePSW(yl::string strCallNum);

/*******************************************************************
** 函数描述:    获取当前通话状态
** 参数:        strCallId : 通话话路id   strAccountInfo:账号信息
** 返回:        SESSION_STATE 状态
********************************************************************/
SESSION_STATE talklogic_GetCurrentStatus(yl::string& strCallId, yl::string& strAccountInfo);

/*******************************************************************
** 函数描述:    焦点话路是否为去电话路
** 返回:      true: 是,false:不是
********************************************************************/
bool talklogic_IsFoucsInConnectingSession();

/*******************************************************************
** 函数描述:    焦点话路是否为通话话路
** 返回:      true: 是,false:不是
********************************************************************/
bool talklogic_IsFocusInTalkSession();

/*******************************************************************
** 函数描述:    焦点话路是否为拨号话路,包括拨号和预拨号
** 返回:      true: 是,false:不是
********************************************************************/
bool talklogic_IsFocusInPreDialSession();

/*******************************************************************
** 函数描述:    焦点话路是否为来电话路
** 返回:      true: 是,false:不是
********************************************************************/
bool talklogic_IsFoucsInRingSession();

/*******************************************************************
** 函数描述:    焦点话路是否对应的Routine类型，仅限单路通话
** 返回:      true: 是,false:不是
********************************************************************/
bool talklogic_IsFoucsInSession(ROUTINE_STATE eState);


/*******************************************************************
** 函数描述:   UI给Logic发送UI事件
** 参数:       [in] iSessionID:          session ID
**             [in] eActionType:         TALK_ACTION_TYPE
**             [in] wActionParam:        消息参数
**             [in] lActionParam:        消息参数
**             [in] pExtraData:          扩展数据
** 返回:       true，发送成功；false，发送失败
********************************************************************/
bool talklogic_UIEvent(int iSessionID, int iActionType, WPARAM wActionParam = 0,
                       LPARAM lActionParam = 0, void* pExtraData = NULL);

/*******************************************************************
** 函数描述:    刷新当前话路
** 参数:        无
** 返回:        true:刷新成功， false:刷新失败
********************************************************************/
bool talklogic_UpdateWnd();


/*******************************************************************
** 函数描述:    获取来电号码
** 参数:       [out] strNum: 第一路来电的号码
** 返回:       true:存在来电,strNum中存了来电displayname; false: 当前没来电
********************************************************************/
bool talklogic_GetIncomingCallNum(yl::string& strNum);


/*******************************************************************
** 函数描述:    判断是否允许再建立新的通话，用于T19话机限制两路，
本地会议两路、预拨号、拨号和OutTalk状态不算的情况
** 参数:      bCheckForIncomingCall：true=来电检查（尚未添加当前判断Session），false=去电检查（当前判断Session已添加）
** 返回:      true: 允许新建立通话,false:不允许新建立通话
********************************************************************/
bool talklogic_BeAllowNewCall(LPCSTR lpNum = NULL, bool bCheckForIncomingCall = false);

/*******************************************************************
** 函数描述:    是否允许两个session互转
** 参数:      互转的两个SessionID
** 返回:      true:允许 false:不允许
********************************************************************/
bool talklogic_IsAllowTranTo(int iSessionIDFirst, int iSessionIDSec);

#if IF_FEATURE_START2START_ACD
// 设置焦点通话的 TALK_CALLOUT_TYPE 值，START2START ACD要用到
bool talkLogic_SetCalloutType(TALK_CALLOUT_TYPE eType);
#endif

/*******************************************************************
** 函数描述:   账号是否可以呼出
** 参数:        iAccountID：账号ID
** 返回:       返回是否允许账号是否允许呼出
********************************************************************/
bool talklogic_IsAllowNewCallByAccount(int iAccountID);

// 获取允许加入会议SessionID
bool talklogic_GetAllowToConfSessionID(YLVector<int>& vecSessionID);

//是否可以加入本地会议的话路数超过本地会议最大路数
bool talklogic_IsAllowAddLocalConfNumOverMax();

/*******************************************************************
** 函数描述:    通话录音
** 参数:        nCallId : 要录音的话路id，默认为-1表示录当前话路
** 返回:        找到对应的话路，录音成功
********************************************************************/
bool talklogic_CallRecord(int nCallId = -1);

/*******************************************************************
** 函数描述:    判断Dsskey是否有绑定话路
** 参数:        iDsskey DsskeyID
** 返回:        是否绑定了通话
********************************************************************/
bool talklogic_IsDsskeyBindSession(int iDsskey);

int talklogic_GetBindDsskeyBySession(int iSessionID, bool bLinekey = true);

/*******************************************************************
** 函数描述:   更新Remote Display
** 参数:       iCallID：call id strDisplay：显示名称
** 返回:       更新是否成功
********************************************************************/
bool talklogic_UpdateRemoteDispaly(int iCallID, const yl::string& strDisplay, yl::string strURI = "");

#ifdef IF_BT_SUPPORT_PHONE
/*******************************************************************
** 函数描述:   当前焦点话路是否为蓝牙拨号
** 参数：
** 返回:       true:是， false:否
********************************************************************/
bool talklogic_IsFoucsInBluetoothDialSession();
#endif

/*******************************************************************
** 函数描述:    密码拨号，获得加密后的号码
** 参数:      strCallNum：加密前的号码
** 返回:      加密后的号码
********************************************************************/
yl::string talklogic_HidePSW(yl::string strCallNum);

//根据SessionID获取网络会议member信息
YLList<DataNetConfUserInfo> talklogic_GetNetUserInfo(int iSessionID);
//根据SessionID判断网络会议是否为本地创建
bool talklogic_IsNetConfCreator(int iSessionID);
//根据SessionID获取网络会议title信息
yl::string talklogic_GetNetConfTitle(int iSessionID);


/*******************************************************************
** 函数描述:    获取当前通话状态
** 参数:        strCallId : 通话话路id   strAccountInfo:账号信息
** 返回:        SESSION_STATE 状态
********************************************************************/
SESSION_STATE talklogic_GetCurrentStatus(yl::string& strCallId, yl::string& strAccountInfo);

/*******************************************************************
** 函数描述:    如果允许transfer,盲转当前话路到lpNum
** 参数:      [in] strNumber: transfer目标号码
** 返回值:       无
********************************************************************/
void talklogic_BlindTransfer(const yl::string& strNumber);

/*******************************************************************
** 函数描述:    如果允许transfer,则进行transfer呼出,准备进行咨询转
** 参数:      [in] strNumber: transfer目标号码
** 返回值:       无
********************************************************************/
void talklogic_AttendedTransfer(const yl::string& strNumber);


/*******************************************************************
** 函数描述:    通过iCallID咨询转号码
** 参数:      iCallId：将要咨询转的话路callid,strNumber:咨询转的号码
** 返回值:       咨询转成功与否
********************************************************************/
bool talklogic_AttendedTransfer(int iCallID, const yl::string& strNumber);


/*******************************************************************
** 函数描述:   接起第一路来电
** 参数:       无
** 返回值 ：   true:成功,false:失败
********************************************************************/
bool talklogic_AcceptIncomingCall(int iCallID = -1);

/*******************************************************************
** 函数描述:    填充正在拨号的界面(pretransfer界面)
** 参数:      [in] lpNum：需要填充的号码
** 返回值 ： 无
********************************************************************/
void talklogic_FillCallNum(LPCSTR lpNum, int iAccount = -1);

/*******************************************************************
** 函数描述:   获取通话和彩铃的状态
** 参数:       [out] bTalk：通话状态
**             [out] bColorRingback：彩铃状态
**             [out] bPaging：是否paging状态
** 返回值 ：无
********************************************************************/
void talklogic_GetTalkAndColorRingbackStatus(bool& bTalk, bool& bColorRingback, bool& bPaging);

/*******************************************************************
** 函数描述:    发送DTMF
** 参数:        通话话路id    strKey : 需发送的dtmf
** 返回:        true: 成功
********************************************************************/
bool talklogic_SendDtmf(int nCallId, const yl::string& strKey, bool bPlayVoice = true, int interval = 0);

/************************************************************************/
/* 接口  ： talkLogic_IsSessionInTalking()                              */
/* 功能  ： 判断是否当前处于通话界面                                     */
/* 参数说明 ：                                                          */
/*      无                                                             */
/* 返回值 ：                                                            */
/*      当前处于通话界面，则返回true，否则返回false                        */
/************************************************************************/
bool talkLogic_IsSessionInTalking(int iSessionID = INVALID_SESSION_ID, bool bSkipHoldHeld = false);

/************************************************************************/
/* 接口  ： talkLogic_IsSessionCallIncoming()                            */
/* 功能  ： 判断是否当前处于来电界面                         */
/* 参数说明 ：                                                          */
/*      无                                                             */
/* 返回值 ：                                                            */
/*      当前处于来电响铃界面，则返回true，否则返回false      */
/************************************************************************/
bool talkLogic_IsSessionCallIncoming();

/*******************************************************************
** 函数描述:    获取当前焦点session所用的账号
** 参数:      无
** 返回值:       账号
********************************************************************/
int talklogic_GetSessionAccount(int iSessionID = INVALID_SESSION_ID);

#if IF_FEATURE_PAGING
int talklogic_GetMulticastPageSessionID(int iCallID = -1);

/*******************************************************************
** 函数描述:    判断是否是组播
** 参数:      [in]iCallID: 通话call id
** 返回值:       true:成功，false:失败
********************************************************************/
bool talklogic_IsMulticastPage(int iCallID = -1);

/*******************************************************************
** 函数描述:    获取组播的音量大小
** 参数:
** 返回值:       音量大小
********************************************************************/
int talklogic_GetPagingVolume(int iCallID = -1);
#endif // IF_FEATURE_PAGING

/* 判断是否已达到话路上限 (LocalConf算2路) */
bool talklogic_IsMaxSessionNum(bool bCheckForIncomingCall = false);


/*******************************************************************
** 函数描述:    退出通话 通过CallId
** 参数:      int iCallId
** 返回值:       成功与否
********************************************************************/
bool talklogic_ExitSession(int iCallID);

bool talklogic_ExitSession(ROUTINE_STATE eRoutine);

/*******************************************************************
** 函数描述:    退出所有通话
** 参数:
** 返回值:       void
********************************************************************/
void talklogic_ExitAllSession();


bool talklogic_IsBlaIndexUsing(int iIndex);

int talklogic_GetSessionByBLAIndex(int iAccountID, int iIndex);
