#ifndef __MOD_UI_MANAGER__
#define __MOD_UI_MANAGER__

#include "uimanager_inc.h"

class CBaseMainWnd;

/************************************************************************/
/* 接口  ： UIManager_Init()                                            */
/* 功能  ： 初始化UIManager                                             */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_Init();

/************************************************************************/
/* 接口  ： UIManager_SetMainWnd()                                      */
/* 功能  ： 设置主窗口指针                                              */
/* 参数说明 ：                                                          */
/*     [in]pMainWnd:主窗口指针                                         */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_SetMainWnd(CBaseMainWnd * pMainWnd);

/************************************************************************/
/* 接口  ： UIManager_GetMainWnd()                                      */
/* 功能  ： 获取主窗口指针                                              */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ： 主窗口指针                                                 */
/************************************************************************/
CBaseMainWnd * UIManager_GetMainWnd();

/************************************************************************/
/* 接口  ： UIManager_ShowMainWnd()                                     */
/* 功能  ： 主窗口显示(只在main函数中调用)                              */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_ShowMainWnd();

/************************************************************************/
/* 接口  ： UIManager_CreatePrivateDlg()                                */
/* 功能  ： 根据窗口类名与窗口名称创建小窗口，                          */
/*          若窗口名称为空，则将其赋为类名                              */
/* 参数说明 ：                                                          */
/*     [in]strDlgClassName:小窗口类名                                   */
/*     [in]strDlgObjectName:小窗口名称                                  */
/* 返回值 ： 小窗口指针                                                 */
/************************************************************************/
CBaseDialog * UIManager_CreatePrivateDlg(const yl::string & strDlgClassName,
        const yl::string & strDlgObjectName = "", bool bPush = true);

/************************************************************************/
/* 接口  ： UIManager_GetPrivateDlg()                                   */
/* 功能  ： 根据窗口名获取小窗口                                        */
/* 参数说明 ：                                                          */
/*     [in]strDlgObjectName:小窗口名称                                  */
/* 返回值 ： 小窗口指针                                                 */
/************************************************************************/
CBaseDialog * UIManager_GetPrivateDlg(const yl::string & strDlgObjectName);

/************************************************************************/
/* 接口  ： UIManager_PushWnd()                                          */
/* 功能  ： 将窗口入栈                                                  */
/*                                                                      */
/* 参数说明 ：CBaseDialog 窗口指针                                      */
/* 返回值 ： 小窗口指针                                                 */
/************************************************************************/
// 该API慎用！UIManager_CreatePrivateDlg已经有入栈操作
void UIManager_PushWnd(CBaseDialog * pDlg);

/************************************************************************/
/* 接口  ： UIManager_PutPrivateDlg()                                   */
/* 功能  ： 将传入的小窗口还回到窗口缓存                                */
/* 参数说明 ：                                                          */
/*     [in]pDlg:小窗口指针                                              */
/* 返回值 ：无                                                          */
/************************************************************************/
void UIManager_PutPrivateDlg(CBaseDialog * pDlg);

/************************************************************************/
/* 接口  ： UIManager_PutPrivateDlg()                                   */
/* 功能  ： 按传入的小窗口名称将小窗口还回到窗口缓存                    */
/* 参数说明 ：                                                          */
/*     [in]strDlgObjectName:小窗口名称                                  */
/* 返回值 ：无                                                          */
/************************************************************************/
void UIManager_PutPrivateDlg(const yl::string & strDlgObjectName);

/************************************************************************/
/* 接口  ： UIManager_GetTopWindow()                                    */
/* 功能  ： 获取顶层窗口                                                */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ： 顶层的窗口指针                                             */
/************************************************************************/
CBaseDialog * UIManager_GetTopWindow();

/************************************************************************/
/* 接口  ： UIManager_SetTopWindow()                                    */
/* 功能  ： 设置顶层窗口                                                */
/* 参数说明 ：                                                          */
/*     [in]CBaseDialog:要设置为顶层的窗口指针                           */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_SetTopWindow(CBaseDialog * pWnd);

/************************************************************************/
/* 接口  ： UIManager_ReturnToIdle()                                    */
/* 功能  ： 返回Idle界面，清除窗口堆栈中所有非Idle的模板                */
/* 参数说明 ：                                                          */
/*     [enum]eReturnIdle发起回退idle的原因                               */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_ReturnToIdle(RETURN_IDLE_TYPE eReturnIdle);
void UIManager_ReturnToIdle(bool bClickToIdle);
void UIManager_ReturnToIdle();

/************************************************************************/
/* 接口  ： UIManager_ExitDirAndCalllogWnd()                            */
/* 功能  ： 清除堆栈中所有的联系人和历史记录模块                       */
/* 参数说明 ：                                                          */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_ExitDirAndCalllogWnd();

/************************************************************************/
/* 接口  ： UIManager_UpdateWnd()                                       */
/* 功能  ： 更新窗口，传入的窗口必须是当前顶层窗口                      */
/* 参数说明 ：                                                          */
/*     [in]strDlgObjectName:要更新的窗口名                              */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_UpdateWnd(const yl::string & strDlgObjectName);

/************************************************************************/
/* 接口  ： UIManager_UpdateWnd()                                       */
/* 功能  ： 更新窗口，传入的窗口必须是当前顶层窗口                      */
/* 参数说明 ：                                                          */
/*     [in]pDlg:要更新的窗口                                          */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_UpdateWnd(CBaseDialog * pDlg);

/************************************************************************/
/* 接口  ： UIManager_UpdateTopWnd()                                    */
/* 功能  ： 更新顶层窗口                                               */
/* 参数说明 ：  无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void UIManager_UpdateTopWnd();

/************************************************************************/
/* 接口  ： UIManager_RegisterTopWndChangeCallBack()                    */
/* 功能  ： 注册顶层窗口改变时候的回调                                  */
/* 参数说明 ：ON_TOPWND_CHANGE_PROC                                     */
/*            顶层窗口改变的回调函数                                 */
/************************************************************************/
void UIManager_RegisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack);

/************************************************************************/
/* 接口  ： UIManager_UnregisterTopWndChangeCallBack()                  */
/* 功能  ： 注销顶层窗口改变时候的回调                                  */
/* 参数说明 ：ON_TOPWND_CHANGE_PROC                                     */
/*            顶层窗口改变的回调函数                                 */
/************************************************************************/
void UIManager_UnregisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack);

/************************************************************************/
/* 接口  ： UIManager_IsWndExist()                                      */
/* 功能  ： 是否存在当前的界面类型                                        */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ： true:该界面被打开, false:该界面未被打开。不保证在顶层      */
/************************************************************************/
bool UIManager_IsWndExist(EWindowType eType);

/************************************************************************/
/* 接口  ： UIManager_IsInIdle()                                        */
/* 功能  ： 判断当前是否处于idle界面                                   */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ： true:处于idle, false:非idle界面                            */
/************************************************************************/
bool UIManager_IsInIdle();

/************************************************************************/
/* 接口  ： UIManager_IsInTalk()                                        */
/* 功能  ： 判断当前是否处于talk界面                                   */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ： true:处于talk, false:非talk界面                            */
/************************************************************************/
bool UIManager_IsInTalk();

/************************************************************************/
/* 接口  ： UIManager_IsInCallManager()                                 */
/* 功能  ： 判断当前是否处于CallManager界面                                */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ： true:处于CallManager, false:非CallManager界面              */
/************************************************************************/
bool UIManager_IsInCallManager();

/************************************************************************/
/* 接口  ： UIManager_ExitCallManager()                                 */
/* 功能  ： 栈中是否有CallManager界面                                 */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ： true:存在, false:不存在                                   */
/************************************************************************/
bool UIManager_ExistCallManager();

/************************************************************************/
/* 接口  ： UIManager_IsInXMLBrowser()                                 */
/* 功能  ： 判断当前是否处于XMLBrowser界面                             */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ： true:处于XMLBrowser, false:非XMLBrowser界面              */
/************************************************************************/
bool UIManager_IsInXMLBrowser();

void UIManager_ChangeTransparency();

void UIManager_InstallTransChangeSignalSlot(const QObject * receiver, const char * method);

void UIManager_UnInstallTransChangeSignalSlot(const QObject * receiver, const char * method);

#endif // __MOD_UI_MANAGER__
