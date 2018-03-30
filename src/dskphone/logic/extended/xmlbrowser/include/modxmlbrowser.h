#ifndef __MOD_XMLBROWSER_H__
#define __MOD_XMLBROWSER_H__

#include "logicanduicommon.h"
#include "lamp/ledlamp/include/led_data_define.h"

/************************************************************************/
/* 接口  ： xmlbrowser_Init()                                            */
/* 功能  ： XmlBrowser初始化                                              */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void xmlbrowser_Init();

/************************************************************************/
/* 接口  ： XMLBrowser_Enter()                                           */
/* 功能  ： 进入XMLBrowser接口                                           */
/* 参数说明 ：                                                         */
/*     [in]1.lpszURL：XMLBrowser URL                                       */
/*     [in]2.bExitCurrent：是否退出当前的xmlbrowser                     */
/* 返回值 ： 无                                                            */
/************************************************************************/
void xmlbrowser_Enter(const char * lpszURL, bool bExitCurrent = true, int nDsskeyId = -1,
                      bool bShowTips = true);

/************************************************************************/
/* 接口  ： XMLBrowser_Exit()                                           */
/* 功能  ： 退出XMLBrowser接口                                           */
/* 参数说明 ：                                                         */
/* 返回值 ： 无                                                            */
/************************************************************************/
void xmlbrowser_Exit();

/************************************************************************/
/* 接口  ： xmlbrowser_GetData()                                     */
/* 功能  ： 获取当前数据及界面类型                                    */
/* 参数说明 ：                                                         */
/*     [in]1.nElementID：element的标识                                      */
/* 返回值 ： 空类型指针                                                    */
/************************************************************************/
bool xmlbrowser_GetData(XMLLogicToUIData & refToUIData);

/************************************************************************/
/* 接口  ： xmlbrowser_OnTimer()                             */
/* 功能  ： 定时器处理                                                  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 是否处理                                                   */
/************************************************************************/
BOOL xmlbrowser_OnTimer(UINT uTimer);

/************************************************************************/
/* 接口  ： xmlbrowser_OnAction()                                        */
/* 功能  ： 对UI事件的处理                                                 */
/* 参数说明 ：                                                         */
/*     [in]1.eAction：相应的action                                      */
/*     [in]2.nIndex：                                      */
/*     [in]3.pData：附带的其他数据                                      */
/* 返回值 ： 是否处理                                                       */
/************************************************************************/
bool xmlbrowser_OnAction(int nElementID, XB_ACTION_TYPE eAction, void * pData = NULL);

/************************************************************************/
/* 接口  ： xmlbrowser_CleanAllData()                                    */
/* 功能  ： 清除所有数据                                                   */
/* 参数说明 ：无                                                          */
/* 返回值 ：无                                                         */
/************************************************************************/
void xmlbrowser_CleanAllData();

/************************************************************************/
/* 接口  ： xmlbrowser_HideXmlBrowser()                                  */
/* 功能  ： 隐藏模块                                                         */
/* 参数说明 ：                                                         */
/*     [in]1.bHide：是否隐藏Xmlbrowser                                   */
/* 返回值 ：无                                                         */
/************************************************************************/
void xmlbrowser_HideXmlBrowser(bool bHide);

/************************************************************************/
/* 接口  ： xmlbrowser_CancelDownload()                                  */
/* 功能  ： 取消下载                                                         */
/* 参数说明 ：                                                         */
/*     [in]1.bCancel：是否取消下载                                   */
/* 返回值 ：无                                                         */
/************************************************************************/
void xmlbrowser_CancelDownload();

bool xmlbrowser_IsHaveXBElement();

#if IF_BUG_30385
void xmlbrowser_RequestAutopXml();
#endif

// 解析XML文件
bool xmlbrowser_ParseFile(const yl::string & strFilename);

// 强制取消xmlbrowser弹出框
void xmlbrowser_EnableTips(bool bEnabled = true);

void xmlbrowser_CloseLoading();

// 释放xml browser中的status
void xmlbrowser_ReleaseStatus(const yl::string & strSessionId);

#if IF_FEATURE_XMLBROWSER_HOTELING
bool xmlbrowser_HotelingRequest(bool bLogin);

bool xmlbrowser_IsHotelingLogin();
#endif

#if IF_FEATURE_XMLB_SCREENSAVER
/************************************************************************/
/* 接口  ： xmlbrowser_ScreenSaverEnter()                               */
/* 功能  ： xmlbrowser模式屏保人口                                         */
/* 参数说明 ：无                                                      */
/* 返回值 ：无                                                         */
/************************************************************************/
void xmlbrowser_ScreenSaverEnter(const char * lpszURL);

/************************************************************************/
/* 接口  ： xmlbrowser_GetSSData()                                       */
/* 功能  ： 获取xmlbrowser模式屏保数据                                   */
/* 参数说明 ：无                                                      */
/* 返回值 ：无                                                         */
/************************************************************************/
bool xmlbrowser_GetSSData(XMLLogicToUIData & objToUIData);

/************************************************************************/
/* 接口  ： xmlbrowser_GetSSData()                                       */
/* 功能  ： 删除xmlbrowser模式屏保缓存数据                             */
/* 参数说明 ：无                                                      */
/* 返回值 ：无                                                         */
/************************************************************************/
void xmlbrowser_DeleteSSData();

#endif //#if IF_FEATURE_XMLB_SCREENSAVER

#endif // __MOD_XMLBROWSER_H__
