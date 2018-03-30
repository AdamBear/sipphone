#ifndef __MOD_DSSKEY_UI_H__
#define __MOD_DSSKEY_UI_H__

#include "dsskeyui/dsskeyui_common.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "dsskey/include/dsskey_def.h"
#include "dsskey/include/dsskeystructure.h"

#include "edk/include/edk_def.h"
#include "lamp/ledlamp/include/led_data_define.h"

void dsskeyUI_Init();

bool dsskeyUI_RefreshCallback(int dsskeyID, Dsskey_Status dsskeyStatus,
                              const DssKeyLogicData * pLogicData,  void * pExtraData);

void dsskeyUI_UpdateDsskeyLight(int iIndex, DSS_KEY_COLOR_TYPE eColorType, DSS_KEY_FLUSH_TYPE eFlushType);

bool dsskeyUI_DsskeyLightCallback(int iKeyIndex, int iColorType, int iLightStatus);

/************************************************************************/
/* 接口  ： dsskeyUI_OnDsskeyLongPress                                   */
/* 功能  ： 长按DssKey键的响应                                         */
/* 参数说明 ：iDsskeyID DsskeyID值                                        */
/* 返回值 ： 无                                                            */
/************************************************************************/
void dsskeyUI_OnDsskeyLongPress(int iDsskeyID);

/************************************************************************/
/* 接口  ： dsskeyUI_DestoryDssKeyConfigPage                            */
/* 功能  ： 进入DssKey配置界面                                         */
/* 参数说明 ：无                                                      */
/* 返回值 ： 无                                                            */
/************************************************************************/
void dsskeyUI_EnterDssKeyConfigPage(DSSKEY_MODULE_TYPE eType, int iIndex, int iExpIndex = -1);

/************************************************************************/
/* 接口  ： dsskeyUI_EnterSetting                                       */
/* 功能  ： DssKey Setting界面进入 配置界面                              */
/* 参数说明 ：1.iIndex 按键索引                                        */
/* 返回值 ： 无                                                            */
/************************************************************************/
void dsskeyUI_EnterSetting(int iIndex);

//卸载DssKeyUI模块
void dsskeyUI_Uinit();

//打开显示全部LineKey界面
void dsskeyUI_InitLinekeyDlg();

//退出DssKey界面
void dsskeyUI_Exit();

/************************************************************************/
/* 接口  ： dsskeyUI_RegisterDlgClass()                                 */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void dsskeyUI_RegisterDlgClass();

/************************************************************************/
/* 接口  ： dsskeyUI_RefreshDsskey                                       */
/* 功能  ： 刷新某一个Dsskey数据                                         */
/* 参数说明 ：objLogicData 类型：DssKeyData&                             */
/* 返回值 ： 无                                                             */
/************************************************************************/
void dsskeyUI_RefreshDsskey(DssKeyLogicData & objLogicData,
                            DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL);

//解析DssKeyLogicData，生成可被UI使用的DsskeyDisplayData结构
bool dsskeyUI_ParseLogicData(DsskeyDisplayData & objDisplayData,
                             const DssKeyLogicData & sLogicData);

//直接获取DsskeyDisplayData数据
bool dsskeyUI_GetDataForUI(int iIndex, DsskeyDisplayData& objDisplayData, int iEXPIndex = -1);

// 目前仅支持linekey和Programkey的转换
int dsskeyUI_GetDsskeyIDByKeycode(int iKeyCode);


#if IF_FEATURE_T42_LINEKEY_SECONDLINE
void dsskeyUI_ResetLinekeyTimer();
#endif

/************************************************************************/
/* 接口  ： dsskey_GetLinekeyIndex()                                    */
/* 功能  ： 根据按键值返回对应的LinekeyIndex                            */
/* 参数说明 ：                                                          */
/*     1.nKey 按键的键值                                                */
/* 返回值 ：Linekey的下标,键值错误则返回-1                              */
/************************************************************************/
int dsskeyUI_GetLinekeyIndex(int nKey);

// 切换page状态提示开关
int dsskeyUI_SwitchPageTip(BOOL bEnable);


/************************************************************************/
/* 接口  ： dsskeyUI_PresenceUserChange                                  */
/* 功能  ： UC变化刷新账号显示                                           */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ：无                                                         */
/************************************************************************/
void dsskeyUI_PresenceUserChange();

bool dsskeyUI_IsLineKey(int nKey);

//设置由EDK控制的dsskey显示内容
void SetEDKCtrlDsskeyDisplay(DsskeyDisplayData& displayData, const EdkCtrlDsskeyDispData& objEDKDisplayData);

//设置由EDK控制的expkey显示内容
void SetEDKCtrlEXPkeyDisplay(EdkCtrlDsskeyDispData& objEDKDisplayData, yl::string& strLabel, LIGHT_Type& led_status);

void RefreshDsskeyByAccount(int nOldAccount, int nNewAccount);

void dsskeyUI_UpdateEXPEditingText();

void dsskeyUI_RefreshPageKey();

void dsskeyUI_RefreshButton(int iIndex, const DsskeyDisplayData& objDisplayData);

void dsskeyUI_ExitFullDsskeyDlg();

#endif

