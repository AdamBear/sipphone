#ifndef __MOD_IDLE_SCREEN_H__
#define __MOD_IDLE_SCREEN_H__

#include <ylvector.h>
#include "idlescreen_def.h"

typedef void(*StatusNotifyFunction)(const YLList<IdleStatusItem> &);
typedef void(*PopupBoxActionFunction)(const int iReason);
typedef void(*StatusChangeFunction)(STATUS_ACTION eAction);

/*******************************************************************
** 函数描述:   idle处理的初始化
** 参数:     无
** 返回:       void
********************************************************************/
void idleScreen_Init(bool bIsXwin = true);

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
/*******************************************************************
** 函数描述:   注册状态
** 参数:     nStatus：状态枚举
               ePopupStatus： 弹出设置
               nLoc：状态固定位置(优先级) 0-最低优先级，无固定位置，后到先显示  1...n 值越低，位置越靠前
                     如果未非0值，则不同的状态nLoc的值不能相同
** 返回:       注册状态
********************************************************************/
bool idleScreen_RegisterStatus(PhoneState nStatus, PopupWindowType ePopupStatus, int nLoc = 0);


/*******************************************************************
** 函数描述:   修改状态注册的Popup类型
** 参数:     nStatus:状态枚举
               ePopupType：新的Popup类型
** 返回:       是否成功
********************************************************************/
bool idleScreen_SetPopupType(int nStatus, PopupWindowType ePopupType);

/*******************************************************************
** 函数描述:   获取状态列表
** 参数:     无
** 返回:       返回UI需要显示的状态
********************************************************************/
YLList<IdleStatusItem> idleScreen_GetStatusList(int iCount = -1);

/*******************************************************************
** 函数描述:   获取第一个文字提示状态
** 参数:     无
** 返回:       Notify提示的状态列表
********************************************************************/
IdleStatusItem idleScreen_GetTopStatusNotifyType();

/*******************************************************************
** 函数描述:   获取第一个弹出提示状态
** 参数:     无
** 返回:       Popup提示的状态列表
********************************************************************/
IdleStatusItem idleScreen_GetTopStatusPopType();

/*******************************************************************
** 函数描述:   获取需要Popup的状态id列表
** 参数:     无
** 返回:       Popup提示的状态列表
********************************************************************/
YLVector<int> idleScreen_GetStatusToPopup();

/*******************************************************************
** 函数描述:   设置回调函数和返回给UI的状态个数
** 参数:     需要返回的状态个数
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_SetNotifyFunByCount(StatusNotifyFunction func, int nStatusCount);


/*******************************************************************
** 函数描述:   设置状态的Popup状态
** 参数:     nStatus状态id
** 返回:       设置是否成功
** SP：        该接口修改目前状态的PopupType值，
               需要修改状态新增时的PopupType，请使用idleScreen_SetPopupType
********************************************************************/
bool idleScreen_SetPopupStatus(int nStatus, int nPopupStatus);

/*******************************************************************
** 函数描述:   设置清除状态的Popup状态
** 参数:     nStatus状态id
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_ClearPopupFlag(int nStatus);

/*******************************************************************
** 函数描述:   获取状态的Popup状态
** 参数:     nStatus状态id
** 返回:       设置是否成功
********************************************************************/
int idleScreen_GetPopupStatus(int nStatus);

/*******************************************************************
** 函数描述:   注册Idle弹窗的按键响应函数
** 参数:       [in] ePopupType:  IDLE_POPUPBOX_TYPE
               [in] fnCallback: PopupBoxActionFunction
**
** 返回:       0: 注册成功 1：注册失败
********************************************************************/
bool idleScreen_RegisterPopupBoxAction(IDLE_POPUPBOX_TYPE ePopupType,
                                       PopupBoxActionFunction fnCallback);

/*******************************************************************
** 函数描述:   Idle界面弹窗
** 参数:       [in] iPopupBoxType:  IDLE_POPUPBOX_TYPE
**
** 返回:       0: 成功弹出；-1: 失败、弹窗将延迟到可以弹出
********************************************************************/
IDLE_POPUPBOX_ERROR idleScreen_PopupMsgBox(int iPopupBoxType = 0);

/*******************************************************************
** 函数描述:   关闭Idle界面的指定弹窗
** 参数:       [in] iPopupBoxType:  IDLE_POPUPBOX_TYPE
**
** 返回:        0: 成功关闭；-1: 失败;
********************************************************************/
int idleScreen_CloseMsgBox(int iPopupBoxType);

/*******************************************************************
** 函数描述:    弹出可用的框
** 参数:      无
** 返回:        TRUE: 成功弹出；FALSE: 无、或目前不能弹出;
********************************************************************/
BOOL idleScreen_PopupAvaliableMsgBox();

/*******************************************************************
** 函数描述:   idleui触发器退出
** 参数:      无
** 返回:        TRUE: 成功处理；FALSE: 未处理;
********************************************************************/
BOOL idleScreen_OnTriggerExit(int iReason, int iTriggerType = -1);

/*******************************************************************
** 函数描述:   获取栈顶的弹出窗口（不一定正在弹出）
** 参数:      无
** 返回:        窗口类型
********************************************************************/
IDLE_POPUPBOX_TYPE idleScreen_GetTopPopupBox();

/*******************************************************************
** 函数描述:   获取正在弹出的窗口，为窗口栈中从上往下第一个弹出成功的窗口
** 参数:      无
** 返回:        窗口类型
********************************************************************/
IDLE_POPUPBOX_TYPE idleScreen_GetPopupingBox();

#endif // IF_NOTIFICATION_CENTER_ON_IDLE

/*******************************************************************
** 函数描述:   注册状态
** 参数:     nStatus：状态枚举
               eNotifyType： 通知类型（为StateNotifyType 枚举值）
               iPriority:0,1,2,3,....n 优先级依次变高
** 返回:       注册状态
********************************************************************/
bool idleScreen_RegisterStatus(PhoneState nStatus, StateNotifyType eNotifyType, int iPriority = 0);

/*******************************************************************
** 函数描述:   修改状态通知类型
** 参数:      nStatus:状态枚举
                eNotifyType： 通知类型（为StateNotifyType 枚举值）
                iPriority:   eNotifyType类型对应的优先级
** 返回:       是否成功
********************************************************************/
bool idleScreen_SetNotifyType(int nStatus, StateNotifyType eNotifyType, int iPriority);

/*******************************************************************
** 函数描述:   获取状态列表
** 参数:     eNotifyType： 通知类型（为StateNotifyType 枚举值）
               iCount    获取个数
** 返回:       返回UI需要显示的状态
********************************************************************/
YLList<IdleStatusItem> idleScreen_GetStatusList(StateNotifyType eNotifyType);

/*******************************************************************
** 函数描述:   获取状态列表顶层状态
** 参数:      eNotifyType： 通知类型（为StateNotifyType 枚举值）
** 返回:       IdleStatusItem类型对象
********************************************************************/
IdleStatusItem idleScreen_GetTopStatus(StateNotifyType eNotifyType);

/*******************************************************************
** 函数描述:   设置回调函数和返回给UI的状态个数
** 参数:
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_SetNotifyFunction(StatusNotifyFunction func, StateNotifyType eNotifyType);

/*******************************************************************
** 函数描述:   修改状态提示信息
** 参数:     nStatus状态id，strHint状态Notify文本, uNotifyType 通知类型
** 返回:       是否成功
********************************************************************/
bool idleScreen_ChangeHint(int nStatus, const yl::string strHint = "",
                           unsigned int uNotifyType = SNT_ICON);

/*******************************************************************
** 函数描述:   设置清除通知类型标识
** 参数:     nStatus状态id
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_ClearNotifyTypeFlag(int nStatus, unsigned int iNotifyType);

/*******************************************************************
** 函数描述:   设置清除注册状态的类型
** 参数:      nStatus状态id
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_ClearRegStatusTypeFlag(int nStatus, unsigned int iNotifyType);
/*******************************************************************
** 函数描述:   获取状态的通知类型
** 参数:     nStatus状态id
** 返回:       通知类型
********************************************************************/
unsigned int idleScreen_GetNotifyType(int nStatus);

/*******************************************************************
** 函数描述:   设置一个状态，状态如果没有则添加
** 参数:     nStatus状态id，strHint状态Notify文本
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_SetStatus(int nStatus, const yl::string strHint = "");

/*******************************************************************
** 函数描述:   删除掉设置的状态
** 参数:     nStatus状态id
**         bNeedNotify 是否需要通知UI  刷新
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_DropStatus(int nStatus, bool bNeedNotify = true);

/*******************************************************************
** 函数描述:   控制状态模块是否通知UI
** 参数:     bNotified true-通知 false-不通知
** 返回:       设置是否成功
********************************************************************/
bool idleScreen_TurnStatusNotify(bool bNotified = true);

/*******************************************************************
** 函数描述:   判断是否已经接入网络
** 参数:     NONE
** 返回:       网络状态
********************************************************************/
bool idleScreen_IsNetworkAvailable();

/*******************************************************************
** 函数描述:   获取网络状态
** 参数:     bRefresh 是否到系统层重新读取状态
** 返回:       网络状态
********************************************************************/
NETWORK_STATUS idleScreen_GetNetworkStatus(bool bRefresh = false);

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////



/*******************************************************************
** 函数描述:   idleui更新自定义提示
** 参数:     listCusNotify [in]  提示列表
** 返回:        TRUE: 成功处理；FALSE: 未处理;
********************************************************************/
BOOL idleScreen_UpdateCustomNotify(typeCusNotifyList & listCusNotify);

/*******************************************************************
** 函数描述:   idleui获取自定义提示
** 参数:
** 返回:       返回提示列表
********************************************************************/
typeCusNotifyList * idleScreen_GetCustomNotify();

/*******************************************************************
** 函数描述:   idleui获取自定义提示是否是Queue Status
** 参数:
** 返回:       返回提示列表是否是Queue Status
********************************************************************/
bool idleScreen_GetQueueStatusCusNotify();

/*******************************************************************
** 函数描述:   设置idleui获取自定义提示是否是Queue Status
** 参数:     [bool] bQueueStatus: true:是Queue Status提示列表
** 返回:       无
********************************************************************/
void idleScreen_SetQueueStatusCusNotify(bool bQueueStatus);

/*******************************************************************
** 函数描述:   设置idleui获取自定义提示是否是Queue Status
** 参数:     [bool] bQueueStatus: true:是Queue Status提示列表
** 返回:       无
********************************************************************/
bool idleScreen_SetStatusChangeCallback(int nStatus, StatusChangeFunction funcCallback);

#ifdef IF_FEATURE_PSTN
void idleScreen_GetCurrAddPstnId(YLVector<int> & vecID);

void idleScreen_ClearPstnIdList();
#endif //IF_FEATURE_PSTN

#endif
