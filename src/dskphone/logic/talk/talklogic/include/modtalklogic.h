#ifndef __MOD_TALKLOGIC_H__
#define __MOD_TALKLOGIC_H__

#include "talk/talklogic/include/talklogic_common.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talkhelp/include/modtalkhelp.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "talk/talkadapter/include/modtalkadapter.h"
#include "talk/talklogic/include/sessionproxy.h"


/*******************************************************************
** 初始化Talk模块
********************************************************************/
void talklogic_Core_Init();

/*******************************************************************
** 退出Talk模块
********************************************************************/
void talklogic_Uninit();

/*******************************************************************
** 进入拨号界面
** 参数：strNumber：呼出号码 iAccountId：拨号所使用的帐号ID
         bSwitchChannel：是否切换声音通道 eCallOutType：呼出方式
** 返回值：是否成功进入拨号
********************************************************************/
int talklogic_EnterDialUI(const yl::string& strNumber, int iAccountId = AUTO_SELECT_LINE_ID,
                          bool bSwitchChannel = true, TALK_CALLOUT_TYPE eCallOutType = TCT_NORMAL);

/*******************************************************************
** 进入预拨号界面
** 参数：iDigitKey：第一个按键 iAccountId：拨号所使用的帐号ID
********************************************************************/
int talklogic_EnterPredialUI(int iDigitKey, int iAccountId = AUTO_SELECT_LINE_ID);

/*******************************************************************
** 呼出号码
** 参数：strNumber：呼出号码 strName：呼出号码对应的显示名 iAccountId：拨号所使用的帐号ID
         pCallParam：呼出参数
** 返回值：是否呼出成功
********************************************************************/
bool talklogic_CallOut(const yl::string& strNumber, const yl::string& strName,
                       int iAccountId = AUTO_SELECT_LINE_ID, CallOut_Param* pCallParam = NULL, bool bNeedPrefix = false);

bool talklogic_CallOut(const yl::string& strNumber, const yl::string& strName, int iAccountId,
                       int nCallId);

/*******************************************************************
** 用GPickup呼出号码
** 返回值：是否呼出成功
********************************************************************/

bool talklogic_CallOutByGPickup();

/*******************************************************************
** 函数描述:    根据SCA线程呼出
** 参数           [in] iAccountID: SCA账号
                [in] iBlaIndex: 分线线路
                [in] strCullPullCode: 抢回话路特征码
** 返回值:       true: 呼出成功, false: 呼出失败
********************************************************************/
bool talklogic_CallOutBySCALine(int iAccountID, int iBlaIndex,
                                const yl::string& strCallPullCode = "", bool bSilentBarginIn = false);

/*******************************************************************
** 函数描述:    根据SCA线程呼出
** 参数           [in] iAccountID: SCA账号
                [in] iBlaIndex: 分线线路
                [in] strCullPullCode: 抢回话路特征码
** 返回值:       true: 呼出成功, false: 呼出失败
********************************************************************/
TALK_CALLOUT_RESULT talklogic_CallOutBySCALine(int iAccountID, int iBlaIndex, int& iSessionID,
        const yl::string& strCallPullCode = "", bool bSilentBarginIn = false);

/*******************************************************************
** 函数描述:   是否存在话路
** 参数:       [in] bIngoreAutoRedialSession：是否忽略autoredial session
** 返回值 ： 存在通话返回true，不存在返回false
********************************************************************/
bool talklogic_SessionExist(bool bIngoreAutoRedialSession = false);

/*******************************************************************
** 函数描述:   获取话路数
** 参数:       无
** 返回值 ：   话路数
********************************************************************/
int talklogic_GetSessionNum(bool bExceptHide = true);

/*******************************************************************
** 函数描述:    根据指定账号进入拨号界面
** 参数:      [in] iAccountID: 账号ID
** 返回值:       无
********************************************************************/
int talklogic_EnterDialUIByAccount(int iAccountID);

/*******************************************************************
** 函数描述:    回拨给最后呼入的号码
** 参数:      无
** 返回值:       无
********************************************************************/
void talklogic_CallReturn();

/*******************************************************************
** 函数描述:    所有话路都在响铃
** 参数:        无
** 返回:        true: 是,false:不是
********************************************************************/
bool talklogic_IsAllSessionRinging();

/*******************************************************************
** 函数描述:    根据callid获取对方的号码和话路所用账号
** 参数:      [in] iCallID：对应话路的CallID
**              [in] strNumber（输出参数）：话路中对方的号码
**              [in] iAccount（输出参数）：话路所用的账号
** 返回值:       true:成功, false:失败
********************************************************************/
bool talklogic_GetOriginRemoteInfoByCallID(int iCallID, yl::string& strDisplayName,
        yl::string& strNumber, yl::string& strRemoteServer);

/*******************************************************************
** 函数描述:    根据callid获取对方的信息
** 参数:      [in] iCallID：对应话路的CallID
**              [out] sipRemoteUserinfo（输出参数）：话路中对方的信息
** 返回值:       true:成功, false:失败
********************************************************************/
bool talklogic_GetRemoteInfoByCallID(int iCallID, SipUserInfo& sipRemoteUserinfo);

/*******************************************************************
** 函数描述:    获取第一路来电线路类型
** 参数:        [out] eCallLineType 第一路来电的线路类型
** 返回:        true=存在来电，false=当前没来电
********************************************************************/
bool talklogic_GetIncomingCallLineType(CALL_LINE_TYPE& eCallLineType);

/*******************************************************************
** 函数描述:    获取通话是否被MUTE
** 参数:        无
** 返回:        true: 被mute,FLASE: 没有被mute
********************************************************************/
bool talklogic_GetMuteStatus(bool bVideo = false);

#if IF_FEATURE_PAGING
/*******************************************************************
** 函数描述:    Paging的呼出
** 参数:       [in] cszDisplay:          显示名
**             [in] cszNum:              号码
**             [in] iDsskey:             绑定的dsskey ID
** 返回:       true，呼出成功; false，失败
********************************************************************/
bool talklogic_CallPagingOut(const char* cszDisplay, const char* cszNum, int iDsskey = -1,
                             const char* cszChannel = "");

/*******************************************************************
** 函数描述:    Paging的呼出
** 参数:       [in] iCallID:          CallID
**             [in] bStart:           是否打开或关闭
** 返回:       true，成功; false，失败
********************************************************************/
bool talklogic_PagingTalkSwitch(int iCallID, bool bStart);

/*******************************************************************
** 函数描述:    重启等待中的Paging请求
** 参数:        无
** 返回:       true:成功,false:失败
********************************************************************/
bool talklogic_RestartWaitPaging();
#endif

/*******************************************************************
** 函数描述:   获取session数量
**
** 返回:       话路数
********************************************************************/
int talklogic_GetAmountOfSession(bool bExceptHide = true);

/*******************************************************************
** 函数描述:   根据SessionID获取Session State
**             [in]iSessionID
** 返回:       session id对应的Session state
********************************************************************/
SESSION_STATE talklogic_GetSessionStateBySessionID(int iSessionID);

/*******************************************************************
** 函数描述:   获取所有SessionID
**             bool bExceptHide 是否排除隐藏话路
**             vecSessionID输出参数，返回SessionID
** 返回:
********************************************************************/
bool talklogic_GetAllSessionID(YLVector<int>& vecSessionID, bool bExceptHide = true);

/*******************************************************************
** 函数描述:   获取允许作为TransferTo Target的话路(被当前焦点话路tran)
** 返回:       话路数
********************************************************************/
int talklogic_GetNumberOfAllowTranTo();

/*******************************************************************
** 函数描述:    Hold通话 通过CallId
** 参数:      int iCallId
** 返回值:       无
********************************************************************/
bool talklogic_HoldSession(int iCallID);

/*******************************************************************
** 函数描述:    unHold通话，通过CallId,并将该路切换为焦点
** 参数:      int iCallId
** 返回值:       成功与否
********************************************************************/
bool talklogic_UnHoldSession(int iCallID);

/*******************************************************************
** 函数描述:    接起通话通过CallId
** 参数:      int iCallId
** 返回值:       成功与否
********************************************************************/
bool talklogic_AnswerCall(int iCallID);

/*******************************************************************
** 函数描述:    拒绝通话通过CallId
** 参数:      int iCallId
** 返回值:   成功与否
********************************************************************/
bool talklogic_RefuseCall(int iCallID, REFUSE_TYPE eRefuseType = RT_NORMAL);

/*******************************************************************
** 函数描述:    盲转通话通过CallId
** 参数:      int iCallId,pNumber 目标号码
** 返回值:       成功与否
********************************************************************/
bool talklogic_BlindTransfer(int iCallID, const char* pNumber);

//咨询转，新起一路之后按下transfer
bool talklogic_Transfer(int iHeldId, int iTansId);

// 转移来电方
bool talklogic_ForwardIncomingCall(int iCallID, const yl::string& strNumber);

/*******************************************************************
** 函数描述:    是否全部话路hold
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsAllSubSessionHold(int iHostSessionID);

/*******************************************************************
** 函数描述:    是否存在正在通话
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsTalkingSessionExist();

/*******************************************************************
** 函数描述:    是否本地会议通话
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsLocalConfSessionExist();

/*******************************************************************
** 函数描述:    是否存在通话
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsSessionExist(ROUTINE_STATE eRoutine, int iExceptID = INVALID_SESSION_ID);


/*******************************************************************
** 函数描述:    是否存在通话
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsTalkSessionExist();

/*******************************************************************
** 函数描述:    是否存在来电
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsRingSessionExist();

/*******************************************************************
** 函数描述:    是否存在拨号界面(包括预拨号)
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsDialSessionExist();

/*******************************************************************
** 函数描述:    是否存在预拨号界面
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsPreDialSessionExist();

/*******************************************************************
** 函数描述:    是否存在去电
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsConnectingSessionExist();

/*******************************************************************
** 函数描述:    处理SIP消息
** 参数:      msg:待处理的线程消息
** 返回:      是否成功处理
********************************************************************/
bool talklogic_OnSIPMsg(msgObject& msg);

/*******************************************************************
** 函数描述:    通过iCallID切换焦点通话
** 参数:        iActiveCallId : 被激活通话   iHeldCallId : 被Hold通话
** 返回:        true: 成功
********************************************************************/
bool talklogic_AlternateCall(int iActiveCallId, int iHeldCallId);

/*******************************************************************
** 函数描述:    挂断激活通话，取回被hold通话
** 参数:        iEndCallId : 被挂断通话   iRetrievedCallId : 取回被Hold通话
** 返回:        true: 成功
********************************************************************/
bool talklogic_ReconnectCall(int iEndCallId, int iRetrievedCallId);

/*******************************************************************
** 函数描述:    创建会议
** 参数:        iActiveCallId : 被激活通话   iHeldCallId : 被Hold通话
** 返回:        true: 成功
********************************************************************/
bool talklogic_ConferenceCall(int iActiveCallId, int iHeldCallId = -1);

/*******************************************************************
** 函数描述:    是否在会议通话中
** 参数:        nCallId : 通话话路id
** 返回:        true:
********************************************************************/
bool talklogic_IsLocalConf(int nCallId);
bool talklogic_IsLocalConfBySessionID(int iSessionID, bool bHost = true);

/*******************************************************************
** 函数描述:    创建网络会议会议
** 参数:        iActiveCallId : 被激活通话   iHeldCallId : 被Hold通话 iConfSessionID:会议话路的sesisonID
** 返回:        true: 成功
********************************************************************/
bool talklogic_NetConferenceCall(int iActiveCallId, int iHeldCallId, int& iConfSessionID);

/*******************************************************************
** 函数描述:    获取会议CallInfo
** 参数:        nCallId : 通话话路id   pFirst/pSencond:两路callinfo
** 返回:        true: 成功
********************************************************************/
bool talklogic_GetLocalConfCallInfo(int nCallId, CCallInfo** pFirst, CCallInfo** pSencond);

/*******************************************************************
** 函数描述:    通过callID获取CallInfo
** 参数:        strCallId : 通话话路id
** 返回:        话路信息
********************************************************************/
CCallInfo* talklogic_GetCallInfoByCallId(yl::string& strCallId);

/*******************************************************************
** 函数描述:    通过callID获取CallInfo
** 参数:        nCallId : 通话话路id
** 返回:        话路信息
********************************************************************/
CCallInfo* talklogic_GetCallInfoByCallId(const int nCallId);

/*******************************************************************
** 函数描述:    通过Session获取CallInfo
** 参数:        iSessionId : 通话话路id
** 返回:        话路信息
********************************************************************/
CCallInfo* talklogic_GetCallInfoBySessionID(int iSessionId);

/*******************************************************************
** 函数描述:    获取来电呼叫的账号id
** 参数:        无
** 返回:        呼叫的账号id
********************************************************************/
int talklogic_GetIncomingCallAccountId();

/*******************************************************************
** 函数描述:    获取第一路来电话路的sessionid
** 参数:        无
** 返回:        话路session id
********************************************************************/
int talklogic_GetIncomingCallSessionId();

/*******************************************************************
** 函数描述:    通过sessionid返回callid
** 参数:        iSessionID
** 返回:        callid
********************************************************************/
int talklogic_GetCallIdBySessionID(int iSessionID);

/*******************************************************************
** 函数描述:    通过CallId返回SessionId
** 参数:        CallId
** 返回:        SessionId
********************************************************************/
int talklogic_GetSessionIdByCallID(int iCallID, bool bHostSession = true);


/*******************************************************************
** 函数描述:    重播
** 参数:      无
** 返回值:       无
********************************************************************/
bool talklogic_Redial();

/*******************************************************************
** 函数名:       talklogic_GetFollowSessionId
** 函数描述:    通过HostSession根据eSpType查找其Follow Session
**              [in] iHostSessionId     本地通话的CallID
**              [in] iFollowSessionId   账号
**              [in] eSpType            账号
** 返回:      是否获取成功
*******************************************************************/
bool talklogic_GetFollowSessionId(int iHostSessionId, int& iFollowSessionId,
                                  SPONSOR_TYPE eSpType = SPONSOR_TRAN);

/*******************************************************************
** 函数名:       talklogic_GetSponsorSessionId
** 函数描述:    通过FollowSession根据eSpType查找其Host Session
**              [in] iFollowSessionId     本地通话的CallID
**              [in] iHostSessionId     账号
**              [in] eSpType            账号
** 返回:      是否获取成功
*******************************************************************/
bool talklogic_GetSponsorSessionId(int iFollowSessionId, int& iHostSessionId,
                                   SPONSOR_TYPE eSpType = SPONSOR_TRAN);

#ifdef IF_SUPPORT_LINE_DEVICE
/*******************************************************************
** 函数描述:    进入外接设备通话
** 参数:      无
** 返回值:       true:成功，false:失败
********************************************************************/
bool talklogic_EnterLineDeviceTalk();

/*******************************************************************
** 函数描述:    退出外接设备通话
** 参数:      无
** 返回值:       无
********************************************************************/
void talklogic_ExitLineDeviceTalk();

/*******************************************************************
** 函数描述:    判断是否是外接设备话路
** 参数:      [in]iCallID: 通话call id
** 返回值:       true:成功，false:失败
********************************************************************/
bool talklogic_IsLineDevice(int iCallID);
#endif

/*******************************************************************
** 函数描述:    通过callID获取CallInfo
** 参数:        iCallId : 通话话路id
** 返回:        话路信息
********************************************************************/
CCallInfo* talklogic_FindCallInfoByCallId(int iCallId);

/*******************************************************************
** 函数描述:   获取通话中的话路数量
** 返回:       话路数
********************************************************************/
int talklogic_GetTalkingNumber(bool bVideo = false);

#ifdef IF_BT_SUPPORT_PHONE
/*******************************************************************
** 手机账号呼出
** 参数：
strNumber：呼出号码
** 返回值：是否呼出成功
*******************************************************************/
bool talklogic_CallMobileOut(const yl::string& strNumber, const yl::string& strName = "");

/*******************************************************************
** 进入蓝牙手机拨号界面
** 参数：
strNumber：呼出号码
** 返回值：是否呼出成功
*******************************************************************/
bool talklogic_EnterMobileDialUI(const yl::string& strNumber = "");

/*******************************************************************
** 函数描述:   根据Call ID 判断是否蓝牙通话话路
** 参数：       call id
** 返回:       true:是， false:否
********************************************************************/
bool talklogic_IsBluetoothSessionByCallID(int iCallID);

/*******************************************************************
** 函数描述:   根据Session ID 判断是否蓝牙通话话路
** 参数：       session id
** 返回:       true:是， false:否
********************************************************************/
bool talklogic_IsBluetoothSessionBySessionID(int iSessionID);


/*******************************************************************
** 函数描述:    是否存在蓝牙话路
** 参数：    bStartTalk 是否只包含已处于通话中的Session
** 返回:      true: 存在,false:不存在
********************************************************************/
bool talklogic_IsBtSessionExist();

/*******************************************************************
** 函数描述:    判断是否允许再建立新的蓝牙通话
会议参与方，也算，拨号及finish不算
** 参数:      无
** 返回:      true: 允许新建立通话,false:不允许新建立通话
********************************************************************/
bool talklogic_BeAllowNewBTCall();

#endif

/*******************************************************************
** 函数描述:    获取非HOLD通话的session id
** 参数:      无
** 返回值:   session id
********************************************************************/
int talklogic_UnHoldSessionID();

/*******************************************************************
** 函数描述:    通过sessionid返回callid
** 参数:        iSessionID
** 返回:        callid
********************************************************************/
int talklogic_GetCallIdBySessionID(int iSessionID);

/*******************************************************************
** 函数描述:   获取全局静音
** 参数:
** 返回:       是否全局静音状态
********************************************************************/
bool talklogic_GetPersistentMuteStatus();

/*******************************************************************
** 函数描述:   设置全局静音
** 参数:
** 返回:       设置结果
********************************************************************/
void talklogic_SetPersistentMuteStatus();

// 是否接起来电
bool talklogic_IsRingingSessionAnswered(int iSessionID);

/*******************************************************************
** 函数描述:    获取会议的子SessionID链表
** 参数:        SessionID、存储列表
** 返回:        true:成功 false:失败
********************************************************************/
bool talklogic_GetSubSesseionID(int iSessionID, YLVector<int>& vec);

/*******************************************************************
** 函数描述:    获取会议通话的SessionID
** 参数:        void
** 返回:        SessionID
********************************************************************/
int talklogic_GetLocalConfSessionID();

/*******************************************************************
** 函数描述:    是否包含Paging通话
** 参数:        void
** 返回:        bool
********************************************************************/
bool talklogic_BeHavePageOutCall();

/*******************************************************************
** 函数描述:     查找存在host sesssionID
** 参数:        sub sesssion ID
** 返回:        成功返回 host session ID 失败返回 INVALID_SESSION_ID
********************************************************************/
int talklogic_GetHostSessionID(int iSessionID);

/*******************************************************************
** 函数描述:     是否处于Hold状态
** 参数:        sesssion ID
** 返回:        bool
********************************************************************/
bool talklogic_IsHoldSession(int iSessionID);

/*******************************************************************
** 函数描述:     获取账号的通话数量
** 参数:        账号 ID
** 返回:        通话数量
********************************************************************/
int talklogic_GetCallNumberByAccountID(int iAccountID);

#ifdef IF_FEATURE_EMERGENCY
/*******************************************************************
** 函数描述:    获取来电是否是紧急回呼
** 参数:       第一路来电的号码
** 返回:       true:是紧急回呼 false: 不是紧急回呼
********************************************************************/
bool talklogic_IsEmergencyRecall();
#endif

int talklogic_MaxSessionNum();

void talklogic_HoldAllSession(int iExceptID = INVALID_SESSION_ID, HOLD_TYPE eHold = HT_NORMAL);

CCallInfo* talklogic_TraverseCallInfo(CBaseInfoTraver& vistor, bool bExcexpHide = true);

CSessionProxy talklogic_TraverseSession(CBaseSessionTraver& vistor, bool bExcexpHide = true);

CCallInfo* talklogic_CreateNewRoutine(ROUTINE_STATE eRoutine, int iBasedSession = INVALID_SESSION_ID);

bool talklogic_UIEvent(int iSessionID, DataUI2Logic& action);

CCallInfo* talklogic_ProcessCallOut(const CallOut_Param& param);

CCallInfo* talklogic_CreateSession(const CallOut_Param& param);

bool talklogic_Redirect(int iCallID, LPCSTR lpTarget);

#endif // __MOD_TALKLOGIC_H__
