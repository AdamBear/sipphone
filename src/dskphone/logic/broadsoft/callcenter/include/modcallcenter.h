#ifndef __MOD_CALLCENTER_H__
#define __MOD_CALLCENTER_H__

#include "cccommon.h"
#include <ETLLib.hpp>
#include "interfacedefine.h"
#include "ccmessagedefine.h"
#include "broadsoft/callcenter/include/callcenter_data_define.h"
#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/mtswcommondefine.h"
#endif

void CallCenter_Init();
//ACD和Hoteling登陆接口
bool CallCenter_Login(ACDLogin & refData);

//ACD和Hoteling登出接口
bool CallCenter_Logout();

//请求ACD状态改变，单独登陆接口
bool ACD_ReqChangeACDStatus(ACD_ACTION eACDAction);

bool ACD_ReqLogin();

HOTELING_STATUS CallCenter_GetHotelingStatus();

//像服务器发送追踪信令
bool CallCenter_OriginatedTrace(int nCallId = -1);

//向服务器发送Disposition Code
bool CallCenter_SendDispCode(const yl::string & strCode, int nCallId = -1);

//向班长发起3方通话
bool CallCenter_EmergencyEscalation(const yl::string & strNumber, int nCallId = -1);

//ACD 功能是否开启
bool CallCenter_IsACDOn();

//Hoteling功能是否开启
bool CallCenter_IsHotelingOn();

// 是否启用FlexibleSeating功能
bool CallCenter_IsFlexibleSeatingOn(int iAccountId = -1);

// Guest话机是否支持键盘锁
bool CallCenter_EnableGuestLock();

// Guest话机的键盘锁是否开启
bool CallCenter_IsGuestLock();

// Guest话机的键盘锁密码是否匹配
bool CallCenter_IsMatchGuestLockPin(const yl::string & strUnlockPin);

// Guest话机开启/关闭键盘锁
void CallCenter_GuestLock(bool bLock);

// 清除FlexibleSeating的配置
void CallCenter_ClearFlexibleSeating();

//ACD Trace 功能是否开启
bool CallCenter_IsTraceOn();

//DispCode 功能是否开启
bool CallCenter_IsDispCodeOn();

//EmergencyEscalation功能是否开启
bool CallCenter_IsEmergencyEscalationOn();

//该接口只是用于Ui界面如何显示，true打钩，密码账号添冲。
bool Hoteling_IsAutoLogin();

//用于auto登陆时UI显示
void Hoteling_GetUserId(yl::string & strUserId);

//设置初始化状态
bool ACD_SetInitStatus(ACD_STATUS eStatus);

//获取初始登陆状态
ACD_STATUS ACD_GetInitStatus();

//获取ACD当前状态
ACD_STATUS ACD_GetAcdStatus();

//获取UnAvailable Reason List
bool ACD_GetReasonMap(DispCodeList & listData);

//设置Unavailable Reason
void ACD_SetCurReason(const yl::string & strReason);

void ACD_GetDispCodeList(DispCodeList & refList);

void ACD_GetSupervisorList(SupervisorList & refList);

//设置当前Call Center信息
//输入为xml字符串
bool ACD_SetCurCallInfo(yl::string & strXmlInfo, int nCallId);

//返回Call Center信息
const CallCenterInfo & ACD_GetCallInfo();

//返回队列状态
const CallCenterQueueStatus & ACD_GetQueueStatus();

//获取CallInfo显示时间
int CallCenter_GetCallInfoShowTime();

//返回Account iD信息
int ACD_GetAccountId();

//返回Account iD信息
int Hoteling_GetAccountId();

//获取hoteling显示名字
void Hoteling_GetDisplayName(yl::string & strName);

//获取不可用原因code
void ACD_GetUnavailCode(yl::string & strCodeName);

//ACD Call info功能是否开启
bool CallCenter_IsCallInfoOn();

int ACD_GetCallInfoCallId();

//队列状态是否开启
bool CallCenter_IsEnableQueueStatus();
//队列状态灯是否开启
bool CallCenter_IsEnableQueueStatusLightOn();

bool CallCenter_IsUnavailReasonCodeOn();

// 返回ACD类型
ACD_TYPE Get_ACDType();

bool ACD_ProcessAfterLogin();

bool ACD_IsGensACD();

yl::string ACD_GetACDLoginName();

Dsskey_Status ACD_GetDsskeyStatus(ACD_STATUS eAcdStatus);

#if IF_FEATURE_START2START_ACD
// 设置ACD状态，Start2Start不是是通过xmlbowser实现
void ACD_SetAcdStatus(ACD_STATUS eNewStatus);

// 设置acd对应提示的session id
void ACD_SetSessionID(const yl::string & strSessionid);

// 获取Start2Start ACD呼出参数
yl::string CallCenter_GetNextCallOutParam();

// 获取DTMF时间间隔
int CallCenter_GetNextInterval();

void CallCenter_SetAutoRefreshTime(unsigned long iTimer);
#endif //IF_FEATURE_START2START_ACD

#if IF_FEATURE_METASWITCH_ACD
// 下载数据
bool MTSWACD_DownloadData(MtswActionType eDownloadType, const yl::string & strMlhgName = "");

// 获取MLHG列表
bool MTSWACD_GetMLHGList(YLVector<SMLHGData> & vecData);

// 是否有 Premium ACD 类型的acd
bool MTSWACD_BeHasPreminumAcd();

// 获取UnavailReason列表
bool MTSWACD_GetUnavailReasonList(YLList<yl::string> & listUnavailReason);

// 获取当前unavail reason
yl::string MTSWACD_GetCurUnavailReason();

// 获取mlhg组队列信息
const SMLGHQueuesData * MTSWACD_GetCurQueuesData(yl::string strMlhgName = "");

// 设置当前焦点组
bool MTSWACD_SetCurrentMLHG(yl::string strMlhgName);

// 获取当前MLHG组名
yl::string MTSWACD_GetCurrentMLHG();

// 获取MLHG组列表
bool MTSWACD_GetMLHGMemberList(YLVector<SMLHGMemberData> & vecData, yl::string strMlhgName = "");

// 是否允许登录多个组
bool MTSWACD_IsAllowMultipleLogin();

// 获取Disposition Code列表
bool MTSWACD_GetDispCodeList(YLVector<yl::string> & vecData, yl::string strMlhgName);

// 从Wrap-Up设置到其它状态
bool MTSWACD_SetDispCode(yl::string strCode, yl::string strMlhgName);

// 当前组是否强制需要dispcode
// strMlhgName默认值为取当时组
bool MTSWACD_IsDispCodeRequired(yl::string strMlhgName = "");

// 是否所有的组都是登出状态
bool MTSWACD_IsEnableShowState();

// 获取最后一次来电所属的组
yl::string MTSWACD_GetLastAnseredMHLG();

// 是否所有的组都处于登出状态
bool MTSWACD_IsAllMLHGLogout();
#endif // IF_FEATURE_METASWITCH_ACD
#endif

