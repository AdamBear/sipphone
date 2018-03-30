#ifndef _____MOD_SETTINGUI_H__
#define _____MOD_SETTINGUI_H__


#include "ylstl/ylstring.h"
#include "setuidatadefine.h"
#include "xwindow/xWindow.h"

// Setting模块初始化
void SettingUI_Init();

void SettingUI_UnInit();

void SettingUI_SetPageDataPath(const yl::string& strPageDataPath);

bool SettingUI_EnterPage(const yl::string & strPageID, void * pExtraData = NULL,
                         bool bRefresh = true);

// 依次打开给定的界面
// strPageIDList格式："pageId1, focusItemId1; pageId2, focusItemId2; pageId3, focusItemId3"
bool SettingUI_EnterListPage(const yl::string & strPageIDList);

/************************************************************************/
/* 接口  ： SettingUI_InitDsskeyImplActions()                           */
/* 功能  ： 注册dsskey处理类                                            */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void SettingUI_InitDsskeyImplActions();

//话机重启，基于DeviceLib的看门狗重启加上一些清理操作
void SettingUI_Reboot(int nTimeDelay = 1);

//话机重置
void SettingUI_Reset(int nResetType);

//导入配置
void SettingUI_UploadConfig();

//话机升级
void SettingUI_UpdateFirware();

// 根据类型进入指定的界面
bool SettingUI_EnterPageByType(const int ePageType, int nIndex);

// 进入解锁界面
bool SettingUI_EnterUnlockCheck();

// 弹出警示信息
void SettingUI_ShowNoteBox(const yl::string & strContent);

//进入开机引导界面
#ifdef IF_SETTING_WIZARD
void SettingUI_EnterSetUpWizard();

// 功能:是否正在设置向导
bool SettingUI_IsWizarding();

#endif

#if IF_FEATURE_METASWITCH
void SettingUI_MTSWLoginReturnCallBack(bool bLoginSucceed);
#endif

// YLType 可以包括 YLList YL_Vector等迭代器容器类型
bool SettingUI_GetSwitchIndex(int nTotal, int nCurIndex, bool bNext, int& nSwitchIndex);

bool SettingUI_IsTopPage(const yl::string& strPageID);

#endif
