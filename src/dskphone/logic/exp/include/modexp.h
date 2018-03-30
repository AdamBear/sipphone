#ifndef	 __MOD_EXP_H__
#define  __MOD_EXP_H__
#include "exp_struct.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "explogicdefine.h"

/******************************************************************
** 函数描述:   获取EXP数量
** 参数：      无
** 返回:       exp的数量
******************************************************************/
int exp_GetEXPNum();

/*******************************************************************
** 函数描述:   获取EXP支持的dsskey数量
** 参数：      无
** 返回:       exp的dsskey数量
*******************************************************************/
int exp_GetExpKeyNumByType(DEVICE_TYPE_DEF devType);

/*******************************************************************
** 函数描述:   获取EXP类型
** 参数：      无
** 返回:       返回Exp管理模块的当前EXP类型
*******************************************************************/
DEVICE_TYPE_DEF exp_GetEXPType();

/*******************************************************************
** 函数描述:   获取EXP类型
** 参数：      无
** 返回:       返回Exp 对应dsskey类型
*******************************************************************/
DSSKEY_MODULE_TYPE exp_GetEXPModuleType();

/*******************************************************************
** 函数描述:	   是否EXP类型的按键
** 参数:		   [iDssKeyID] DssKey的ID
** 返回:		   true：是EXP   false: 不是EXP
*******************************************************************/
bool exp_IsEXPKey(int iDssKeyID);

/*******************************************************************
** 函数描述:   获取EXP连接最大个数
** 参数：      无
** 返回:       exp 最大个数
*******************************************************************/
int exp_GetMaxExpNum();

/*******************************************************************
** 函数描述:   exp插入处理
** 参数：      [in] iExpIndex:EXP的序号 eExpType：exp类型
** 返回:       true：处理成功 false：处理失败
*******************************************************************/
bool exp_OnExpInsert(int iExpIndex, DSSKEY_MODULE_TYPE eExpType);

/*******************************************************************
** 函数描述:   exp拔出处理
** 参数：      [in] iExpIndex:EXP的序号 eExpType：exp类型
** 返回:       true：处理成功 false：处理失败
*******************************************************************/
bool exp_OnExpRemove(int iExpIndex, DSSKEY_MODULE_TYPE eExpType);

/*******************************************************************
** 函数描述:   初始化 exp logic manager
** 参数：      eExtType: exp支持类型
** 返回:       true： 初始化成功  false：初始化失败
*******************************************************************/
bool exp_InitExt(DSSKEY_EXT_TYPE eExtType);

/*******************************************************************
** 函数描述:   是否正使用彩屏扩展台
** 参数：      NULL
** 返回:       true： 彩屏  false：非彩屏
*******************************************************************/
bool exp_GetExpColor();

/*******************************************************************
** 函数描述:   设置exp的图标
** 参数：      [in] dsskeyID  [in] icon:图标类型
** 返回:       1：设置成功
*******************************************************************/
int exp_SetExpIcon(int dsskeyID, int icon);

/*******************************************************************
** 函数描述:   设置exp的文本，黑白屏无法单独设置文本
** 参数：      [in] dsskeyID  pStrLabel:文本内容
** 返回:       1：设置成功
*******************************************************************/
int exp_SetExpLabel(int dsskeyID, const char * pStrLabel);

/*******************************************************************
** 函数描述:   设置exp对比度
** 参数：      [in] expIndex：exp序号  constrast:对比度高低
** 返回:       1：设置成功
*******************************************************************/
int exp_SetExpContrast(int expIndex, int constrast);

/*******************************************************************
** 函数描述:   更新 exp
** 参数：      [in] expIndex：exp序号  
** 返回:       1：成功
*******************************************************************/
int exp_RefreshExp(int expIndex);

/*******************************************************************
** 函数描述:   切页
** 参数：      [in] expIndex：exp当前序号
** 返回:       
*******************************************************************/
void exp_Exp39Switch(int iExpIndex);

/*******************************************************************
** 函数描述:   刷某个exp dsskey灯 只能刷扩展台dsskey，不能刷翻页键灯
** 参数：      led_status：灯状态
** 返回:   
*******************************************************************/
int exp_SetExpLEDStatus(int dsskeyId, int led_status);

/*************************************************************************
** 函数描述: 设置彩屏扩展台背景、按键背景（调整dsskey透明度使用）
** 参数：  pStrBackGround必须是全路径，nTransparentLevel 为dsskey背景的透明度
** 返回:
**************************************************************************/
int exp_SetExpBackGround(const char * pStrBackGround, int nTransparentLevel = 1);

/*************************************************************************
** 函数描述: exp进入节电
** 参数：	bEnter： 进入退出
** 返回:
**************************************************************************/
int exp_EnterExpScreenSaver(bool bEnter);

/*************************************************************************
** 函数描述: exp设置背光
** 参数：	expIndex：exp序号 bkLight： 背光等级
** 返回:
**************************************************************************/
int exp_SetExpBKLight(int expIndex, int bkLight);

/*************************************************************************
** 函数描述: exp key是否在当前页
** 参数：	dsskey id
** 返回:		true：在当前页  false：不在当前页
**************************************************************************/
bool exp_IsExpKeyInCurrentPage(int dsskeyID);

/*************************************************************************
** 函数描述: 立刻发送一次exp数据队列
** 参数：	
** 返回:
**************************************************************************/
void exp_FlushExpList();

/***********************************************************************
** 函数描述:	刷翻页键灯
** 参数：   iExpIndex：扩展ID，nKeyIndex：第nKeyIndex个键，led_status灯状态
** 返回:
************************************************************************/
int exp_SetExpLEDStatusByIndex(int iExpIndex, int nKeyIndex, int led_status);

/*************************************************************************
** 函数描述: 设置exp 按键的文本内容
** 参数：	pStrLabel：文本内容
** 返回:
**************************************************************************/
void exp_SetExpEditingText(const char * pStrLabel);

/*************************************************************************
** 函数描述: 设置全部扩展台的背光灯亮度
** 参数：	nLightLevel是0-100；nIndex：exp序号，nExpIndex为-1表示亮全部扩展台
** 返回:
**************************************************************************/
void exp_EXPBackLightUpdate(int nLightLevel, int nIndex = -1);

/*************************************************************************
** 函数描述: 根据话机类型来初始化exp
** 参数：	 NA
** 返回:      NA
**************************************************************************/
void exp_InitExtFollowPhoneType();

/*************************************************************************
** 函数描述: Dump exp 模块信息
** 参数：	strInfo：Dump信息
** 返回:     NA
**************************************************************************/
void exp_DumpExpModelInfo(yl::string& strDumpInfo);

void exp_SetExpKeyEdit(bool bEdit, int nDsskeyID = -1);
//彩屏测试模式使用
void exp_ExpFillWithColor(const EXP_FILL_COLOR & objExpColor, int nIndex = 0);
void exp_ExpFillWithPicture(const char * pStrPicture, int nIndex = 0);
void exp_ExpDrawText(const char * pStrText, int nIndex = 0);
void exp_ExpShowLogo(int nIndex = 0);

//设置扩展台dsskey图标、标签、灯状态、显示状态
//icon图标编号、pStrLabel显示标签、led_status灯状态。
//displayFlag默认值为1表示左边按键标签左对齐，右边按键标签右对齐。
//DT_EXP_SWITCH类型dsskey显示方式需特殊处理，displayFlag设置为EXP_MSGFL_REVERSE | EXP_MSGFL_CENTER居中反选显示
int exp_SetExpKeyDisp(int dsskeyID, int icon, const char * pStrLabel, int led_status,
	int displayFlag = 1, int nDsskeyBgType = -1, const char * pStrIcon = NULL, int nStatus = 0,
	int nAuxiliaryStatus = 0);

// todo
//初始化EHS模块
bool dsskey_InitEHSData();
//to do
//释放exp资源
bool exp_UnInitExp();

#endif // !__MOD_EXP_H__
