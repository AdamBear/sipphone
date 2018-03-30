/************************************************************************
 * FileName  : modDsskeyUI.h (header file)
 * Purpose   :
 * Date      : 2012/09/13 15:09:42
 ************************************************************************/

#ifndef _MODDSSKEYUI_H_
#define _MODDSSKEYUI_H_

#include "dsskeyui_common.h"
//#include "dsskey/include/dsskeystructure.h"
//!!!!!, 确认所有这些接口的用处及模块实现，待Logic整合后
#include "dsskeybtndisplaydelegate.h"

#include "edk/include/edk_def.h"

//初始化DssKeyUI模块
void dsskeyUI_Init();

//卸载DssKeyUI模块
void dsskeyUI_Uinit();

//打开显示全部DssKey界面
void dsskeyUI_OpenFullDsskeyDlg();

/************************************************************************/
/* 接口  ： dsskeyUI_RefreshDsskey                                       */
/* 功能  ： 刷新某一个Dsskey数据                                         */
/* 参数说明 ：objLogicData 类型：DssKeyData&                             */
/* 返回值 ： 无                                                             */
/************************************************************************/
void dsskeyUI_RefreshDsskey(const DssKeyLogicData & objLogicData,
                            DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL);

//解析DssKeyLogicData，生成可被UI使用的DsskeyDisplayData结构
bool dsskeyUI_ParseLogicData(DsskeyDisplayData & objDisplayData,
                             const DssKeyLogicData & sLogicData);

//直接获取DsskeyDisplayData数据
bool dsskeyUI_GetDataForUI(int iIndex, DsskeyDisplayData & objDisplayData, int iEXPIndex = -1);

//销毁DssKey窗体
bool dsskeyUI_DestroyDsskeyDlg();

// 目前仅支持linekey和Programkey的转换
int dsskeyUI_GetDsskeyIDByKeycode(int iKeyCode);

// 目前仅支持linekey数据的获取
bool dsskeyUI_GetDsskeyDataByKeycode(int iKeyCode, DssKeyLogicData & LogicData);

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

bool dsskeyUI_RefreshCallback(int dsskeyID, Dsskey_Status dsskeyStatus,
                              const DssKeyLogicData * pLogicData,  void * pExtraData);

void dsskeyUI_UpdateDsskeyLight(int iIndex, DSS_KEY_COLOR_TYPE eColorType,
                                DSS_KEY_FLASH_TYPE eFlashType);

bool dsskeyUI_DsskeyLightCallback(int iKeyIndex, int iColorType, int iLightStatus);

/************************************************************************/
/* 接口  ： dsskey_PopMessageBox                                     */
/* 功能  ： 根据不同类型的Dsskey弹出不同的提示框                        */
/* 参数说明 ：                                                          */
/*     1.nKey Dsskey按键的类型  2. str 需要显示的部分字符串             */
/* 返回值 ：无                                                         */
/************************************************************************/
void dsskeyUI_RefreshMultDsskey(int iBegin, int iEnd,
                                DSSKEY_MODULE_TYPE eModule = DMT_LINEKEY_MODULE);

/************************************************************************/
/* 接口  ： dsskeyUI_SetCustomBtnDisplay                             */
/* 功能  ： 设置指定Dsskey的自定义DsskeyBtn显示                            */
/* 参数说明 ：                                                          */
/*     1.iDsskeyID Dsskey按键的ID  2. str 需要显示的部分字符串           */
/* 返回值 ：无                                                         */
/************************************************************************/
bool dsskeyUI_SetCustomBtnDisplay(int iDsskeyID, DsskeyBtnDisplayDelegete * pDelegate);

/************************************************************************/
/* 接口  ： dsskeyUI_DebindCustomBtnDisplay                              */
/* 功能  ： 解除指定Dsskey的自定义DsskeyBtn显示                            */
/* 参数说明 ：                                                          */
/*     1.iDsskeyID Dsskey按键的ID  2. str 需要显示的部分字符串           */
/* 返回值 ：无                                                         */
/************************************************************************/
void dsskeyUI_DebindCustomBtnDisplay(int iDsskeyID);

/************************************************************************/
/* 接口  ： dsskeyUI_CtrlDsskeyLight                                    */
/* 功能  ： 刷新某一个Dsskey底部颜色相当于灯的控制                      */
/* 参数说明 ：objLogicData 类型：DssKeyData&                            */
/* 返回值 ： 无                                                            */
/************************************************************************/
void dsskeyUI_CtrlDsskeyLight(const DsskeyDisplayData & objLogicData);

bool dsskeyUI_IsLineKey(int nKey);

//设置由EDK控制的expkey显示内容
void SetEDKCtrlEXPkeyDisplay(EdkCtrlDsskeyDispData & objEDKDisplayData, yl::string & strLabel,
                             LIGHT_Type & led_status);

void RefreshDsskeyByAccount(int nOldAccount, int nNewAccount);

int dsskeyUI_GetColorType(const LIGHT_Type eLightType);
int dsskeyUI_GetFlushType(const LIGHT_Type eLightType);

#endif
