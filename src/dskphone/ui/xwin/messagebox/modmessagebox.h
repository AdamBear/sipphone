#ifndef _MOD_MESSAGEBOX_H
#define _MOD_MESSAGEBOX_H

#include "messageboxbase.h"
#include "messageboxmanager.h"


// 弹出框时间
#ifndef SHORT_TIME
#define SHORT_TIME           1500
#endif

#ifndef LONGER_TIME
#define LONGER_TIME          2000
#endif

#ifndef AUTO_EXIT_TIME
#define AUTO_EXIT_TIME          60000
#endif

#define DEFAULT_INPUT_LENGTH        20

void MessageBox_RegisterMsgBox(MESSAGEBOX_TYPE eMsgBoxType, FUN_CREATE_MSGBOX pFunCreateMsgBox);

void MessageBox_ShowCommonMsgBox(const MSGBOX_INFO & stMsgInfo);

/***********************************************************/
// 功能: 弹出提示框
// 注意：传入MessageBox指针后，该MessageBox就被托管，外界无需释放
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  pMessageBox: 弹出框的指针
//                  iID: 用来区分同一个回调类，同时存在多个同种类型的弹出框的时候使用
// 返回值:             弹出框处理的结果
/***********************************************************/
void MessageBox_ShowMessageBox(CMsgBoxCallBackAction * pAction,
                               CMessageBoxBase * pMessageBox);

/***********************************************************/
// 功能: 获取顶层弹出框
//返回值: 顶层弹出框指针
/***********************************************************/
CMessageBoxBase * MessageBox_GetTopMessageBox();

/**********************************************************/
// 功能: 删除指定类型的弹出框
// 参数说明: 无
//          eResult : 弹出框消失方式
//          iType : 要删除的弹框类型
// 返回值: 成功清除（至少清除了一个弹框）返回true，否则返回false
/**********************************************************/
bool MessageBox_RemoveMessageBoxType(MESSAGEBOX_TYPE nType,
                                     MESSAGEBOX_RESULT eResult = MessageBox_Cancel);

/**********************************************************/
// 功能: 删除指定的弹出框
// 参数说明: 无
//          eResult : 弹出框消失方式
//          pAction : 弹出框结束后的回调指针
//          iID ： 表示弹出框的ID，为-1表示删除Action对应的所有弹出框
// 返回值: 成功清除返回true，否则返回false
/**********************************************************/
bool MessageBox_RemoveMessageBox(CMsgBoxCallBackAction * pAction,
                                 MESSAGEBOX_RESULT eResult = MessageBox_Cancel,
                                 int iID = -1);

/***********************************************************/
// 功能: 以指定方式清除当前顶层弹出框
// 参数说明:
//                  nResultType : 弹出框消失方式
//返回值: 成功清除返回true，否则返回false
/***********************************************************/
bool MessageBox_RemoveTopMessageBox(MESSAGEBOX_RESULT eResult = MessageBox_Cancel);

/***********************************************************/
// 功能: 以指定方式清除当前所有弹出框
// 参数说明:
//                  nResultType : 弹出框消失方式
//返回值: 成功清除返回true，否则返回false
/***********************************************************/
bool MessageBox_RemoveAllMessageBox(MESSAGEBOX_RESULT eResult = MessageBox_Cancel);

/**********************************************************/
// 功能 : 获取当前弹出框总数
// 参数说明 : 无
// 返回值 : 当前存在的弹出框总数
/**********************************************************/
int MessageBox_GetMessageBoxCount();

void MessageBox_ConnettShowMessageSlot(xSlotHandler* pHandler, slotMethod pMethod);

void MessageBox_DisconnectShowMessageSlot(xSlotHandler* pHandler, slotMethod pMethod);

#endif // _MOD_MESSAGEBOX_H_
