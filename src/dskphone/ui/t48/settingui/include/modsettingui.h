#ifndef MODSETTINGUI_H
#define MODSETTINGUI_H

#include <ylstring.h>
#include "setting/include/common.h"

class CDlgMenu;
class QObject;
class CMsgBoxCallBackAction;

/************************************************************************/
/* 接口  ： SettinUI_RegisterDlgClass()                                 */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void SettingUI_RegisterDlgClass();

/************************************************************************/
/* 接口  ： SettingUI_InitDsskeyImplActions()                           */
/* 功能  ： 注册dsskey处理类                                            */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void SettingUI_InitDsskeyImplActions();

/************************************************************************/
/* 接口  ： SettingUI_CreateMenu                                        */
/* 功能  ： 实现菜单显示                                                */
/* 参数说明 ：strMenuAction 类型：str::string                           */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
//bool SettingUI_CreateMenu(yl::string strMenuAction){return true;}

//void SettingUI_DestroyMenu(){}

/************************************************************************/
/* 接口  ：SettingUI_OpenSetVoiceMailPage()                             */
/* 功能  ： 在返回Idle界面时被调用，实现子窗体被释放时同时释放其管理窗口*/
/* 参数说明 ：无                                                      */
/* 返回值 ： 无                                                            */
/************************************************************************/
void SettingUI_OpenSetVoiceMailPage(int nAccountId);

void SettingUI_OpenViewVoiceMailPage(int nAccountId = 0);

/************************************************************************/
/* 接口  ：SettingUI_VoiceMailCallOut()                                 */
/* 功能  ： 在返回Idle界面时被调用，实现子窗体被释放时同时释放其管理窗口*/
/* 参数说明 ：nAccountId 账号ide                                     */
/* 返回值 ： bool 型 true 正常情况  false --出错异常                   */
/************************************************************************/
bool SettingUI_VoiceMailCallOut(int nAccountId);

//进入status general界面
bool SettingUI_OpenStatus();

//进入TestMode status界面
bool SettingUI_OpenTestModeStatus();
/************************************************************************/
/* 接口  ：SettingUI_ConfirmToReboot()                                  */
/* 功能  ： 重启   */
/* 参数说明 ：无*/
/* 返回值 ：void                 */
/************************************************************************/
void SettingUI_ConfirmToReboot();

/************************************************************************/
/* 接口  ：SettingUI_EnterCommonPage()                                   */
/* 功能  ： 根据页面的名字进入配置界面                                  */
/* 参数说明 ：strPageName [in] 页面的名称，iAccount [in] 页面对应的id,
    iIndex [in] 进入页面时默认选中的行                                   */
/* 返回值 ：void                                                        */
/************************************************************************/
//void SettingUI_EnterCommonPage(yl::string strPageName, int iAccount, int iIndex){}

//进入dsskey
bool SettingUI_OpenDsskeyConfigPage(int iDsskeyIndex);

/************************************************************************/
/* 接口  ：SettingUI_PopPasswordDlg()                                   */
/* 功能  ： 弹出需要输入密码界面，并设置OK,CANCEL处理函数              */
/* 参数说明 ：receiver 接收信号的对象，submitPasswordMember 接收者的处理
    提交密码的函数  cancelMember 接收者的处理取消的函数                   */
/* 返回值 ：void                 */
/************************************************************************/
void SettingUI_PopPasswordDlg(CMsgBoxCallBackAction * pMsgBoxCallBack, int nLineId);

bool SettingUI_EnterPageByType(const int ePageType, int nIndex = 0);

/************************************************************************/
/* 接口   :       SettingUI_OpenPage                                      */
/* 功能 :     打开设置界面                                          */
/* 参数说明:    strSubPageAction提供打开页面的action, nDefaultIndex默认选中项*/
/* 返回值:       失败-NULL 成功-指针                                       */
/************************************************************************/
bool SettingUI_OpenPage(const yl::string & strSubPageAction, int nDefaultIndex = 0);

/************************************************************************/
/* 接口   :       SettingUI_OpenPage                                      */
/* 功能 :     打开设置界面                                          */
/* 参数说明:    pageData提供要打开页面的action和默认选中项等信息       */
/* 返回值:       失败-NULL 成功-指针                                       */
/************************************************************************/
bool SettingUI_OpenPage(const SubMenuData & pageData, const bool bHasLeftMenu = false);

//判断界面或子界面是否存在
bool SettingUI_PageExist(const yl::string & strAction);

/************************************************************************/
/* 接口   :   SettingUI_GetDefaultPageStyle                               */
/* 功能 : 获取settingui打开的页面的默认类型（是否有左侧菜单）            */
/* 参数 : 无     */
/* 返回值:   settingui打开的页面的默认类型                             */
/************************************************************************/
int SettingUI_GetDefaultPageStyle();

/************************************************************************/
/* 接口   :   SettingUI_IsDlgOnTop                                        */
/* 功能 : 判断一个指定名称的设置界面是否在顶层显示(只适用于设置界面)  */
/* 参数 : pDlgName [in]: 要判断的界面的类的名字                        */
/* 返回值:   如果指定类的设置界面正在顶层显示则返回true, 否则返回false    */
/************************************************************************/
bool SettingUI_IsDlgOnTop(const char * pDlgName);

bool SettingUI_EnterSpZeroTouch(int iType);

/************************************************************************/
/* 接口   :       SettingUI_RegisterOpenUICheckFunc                                                                           */
/* 功能 :     实现每个界面打开逻辑的注册                                                                                 */
/* 参数说明:    无                                                                                         */
/* 返回值:       无                                                             */
/************************************************************************/
void SettingUI_RegisterOpenUICheckFunc();

#define SETTINGUI_TEST 0
#if SETTINGUI_TEST
enum TEST_CASE_JUMP_ACTION
{
    JA_JUMP_NEXT,
    JA_JUMP_PRE,
    JA_DO_NOT_JUMP,
};
void Settingui_Test(TEST_CASE_JUMP_ACTION eJumpAction = JA_DO_NOT_JUMP);
#endif

#if IF_FEATURE_GENBAND_E911
bool SettingUI_EnterE911Setting(int nLocationID);

void SettingUI_ProcessIdleEvent();
#endif

#endif // MODSETTINGUI_H
