#ifndef __MOD_ACTION_URL_H__
#define __MOD_ACTION_URL_H__
#include "action_def.h"

/*******************************************************************
** 函数描述:   初始化ActionURL功能
** 参数：      无
** 返回:       无
********************************************************************/
void actURL_ActionURLInit();

#if !IF_FEATURE_EVENT_MODULE
/*******************************************************************
** 函数描述:   发送action url事件
** 参数：      [in] eEventType: 发送的事件类型
** 返回:       无
********************************************************************/
void actURL_SendActionURL(ActionUrlType eEventType);

/*******************************************************************
** 函数描述:   发送action url事件, 有额外变量
** 参数：      [in] eEventType: 发送的事件
**             [in] objValue: 发送事件时附带的信息
** 返回:       无
********************************************************************/
void actURL_SendActionURL(ActionUrlType eEventType, const ActionUrlData & objValue);
#endif

#ifdef IF_SUPPORT_UME
void actURL_SendPhoneInfo();
#endif

/////////////////////////Action URI/////////////////////////////////////

/*******************************************************************
** 函数描述:   初始化ActionURI功能
** 参数：      无
** 返回:       无
********************************************************************/
void actURI_Init();

#if IF_BUG_23996
/*******************************************************************
** 函数描述:   判断当前按键是否为ActionUri操控的key
** 参数：      无
** 返回:       true/false
********************************************************************/
BOOL actURI_IsActionUriKey();

/*******************************************************************
** 函数描述:   设置当前按键是否为ActionUri操控的key
** 参数：      [in] : bIsActionUriKey
** 返回:       无
********************************************************************/
void actURI_SetActionUriKey(BOOL bIsActionUriKey);
#endif

#if IF_BUG_27560
/*******************************************************************
** 函数描述:   判断是否隐藏通话信息
** 参数：      无
** 返回:       true/false
********************************************************************/
bool actURI_IsHideTalkInfo();

/*******************************************************************
** 函数描述:   设置是否隐藏通话信息
** 参数：      [in] : bHideTalkInfo
** 返回:       无
********************************************************************/
void actURI_SetHideTalkInfo(bool bHideTalkInfo);
#endif

/*******************************************************************
** 函数描述:     判断是否需要询问用户是否能控制话机
** 参数：
** 返回:          TRUE表示需要获取用户许可,FALSE表示不需要
********************************************************************/
BOOL accessCtr_Process(msgObject & objMsg);

/*******************************************************************
** 函数描述:     判断是否需要询问用户是否能控制话机
** 参数：
** 返回:          TRUE表示需要获取用户许可,FALSE表示不需要
********************************************************************/
BOOL accessCtr_NeedRequest();

/*******************************************************************
** 函数描述:     清除延迟到IDLE弹出提示的标志
** 参数：
** 返回:
********************************************************************/
void accessCtr_PopSuccess();
/*******************************************************************
** 函数描述:     设置对话机的控制权
** 参数：        [in] bAllowCtr: 为TRUE则可以通过网页控制话机和呼出,否则禁止
** 返回:          无
********************************************************************/
void accessCtr_SetPermissions(BOOL bAllowCtr);

#endif // __MOD_ACTION_URL_H__
