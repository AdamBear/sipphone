#ifndef _MOD_TALKUILOGIC_H
#define _MOD_TALKUILOGIC_H
#include <ylstring.h>
#include "talkuilogicmanager.h"

/************************************************************************/
/* 接口  ： talkui_Init()                                                */
/* 功能  ： talkUI模块初始化                                                */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_Init();

/************************************************************************/
/* 接口  ： talkui_Uninit()                                              */
/* 功能  ： talkUI模块注销                                               */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_Uninit();

/************************************************************************/
/* 接口  ： talkui_UpdateWnd(void* pData)                                */
/* 功能  ： 根据Logic层所提供的数据进行界面更新                               */
/* 参数说明 ：[in] void* pData:Logic层所提供的数据，需为DataLogic2UI     */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_UpdateWnd(void * pData);

/*******************************************************************
** 函数描述:    session退出
** 参数:        [in] iSessionID: session id
** 返回:        无
********************************************************************/
void talkuilogic_SessionExit(int iSessionID);

/*******************************************************************
** 函数描述:    根据通话Id清空UI对应的数据指针
** 参数:        [in] iSessionID: session id
** 返回:        无
********************************************************************/
void talkuilogic_ClearSessionData(int iSessionID);

/************************************************************************/
/* 接口  ： talkui_UpdateErrorInfo(void* pData)                          */
/* 功能  ： 根据Logic层所提供的数据进行错误信息更新，若有错误信息，         */
/*         则直接显示，无论处于何通话界面，若处于错误界面，                 */
/*         传过来的参数为空，则关闭错误提示界面                               */
/* 参数说明 ：[in] void* pData:Logic层所提供的数据，需为Error_Info           */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_UpdateErrorInfo(void * pData);


/************************************************************************/
/* 接口  ： talkui_GetTalkTimeBySessionId()                              */
/* 功能  ： 根据通话Id获得通话时间数据                                   */
/* 参数说明 ：[in] int nSessionId:所需获取的Session对应的Id                */
/* 返回值 ： 对应的Session通话时间的秒数                              */
/************************************************************************/
int talkuilogic_GetTalkTimeBySessionId(int nSessionId);


/************************************************************************/
/* 接口  ： talkui_GetInputContent()                                     */
/* 功能  ：  将当前所有UI(Qt)事件执行完毕，将所有输入完成 */
/* 参数说明 ：无                                                          */
/* 返回值 ： 输入框内容，yl::string类型                                 */
/************************************************************************/
yl::string talkuilogic_GetInputContent();
bool talkuilogic_GetInputContent(yl::string & strNum, yl::string & strDisplayname);


/************************************************************************/
/* 接口  ： talkui_UpdateCurrentWnd()                                        */
/* 功能  ：  刷新当前窗口  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_UpdateCurrentWnd();


/************************************************************************/
/* 接口  ： talkuilogic_RegisterOnTalkCallBack(ON_TALK_PROC funCallBack)                          */
/* 功能  ： 注册通话回调函数                               */
/* 参数说明 ：[in] ON_TALK_PROC funCallBack：回调函数的函数指针*/
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_RegisterOnTalkCallBack(ON_TALK_PROC funCallBack);


/************************************************************************/
/* 接口  ： talkuilogic_UnregisterOnTalkCallBack(ON_TALK_PROC funCallBack)                          */
/* 功能  ： 消除注册的通话回调函数                            */
/* 参数说明 ：[in] ON_TALK_PROC funCallBack：回调函数的函数指针*/
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_UnregisterOnTalkCallBack(ON_TALK_PROC funCallBack);

#endif // MODTALKUI_H
