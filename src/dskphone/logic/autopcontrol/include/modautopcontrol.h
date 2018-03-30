#ifndef _MOD_AUTOPCONTROL_H_
#define _MOD_AUTOPCONTROL_H_
#include "autop_common.h"
#include "ylstl/ylstring.h"
#include "ETLLib/ETLLib.hpp"

//初始化
bool Autop_Init();

//Autop 流程开始
bool Autop_Start(AUTOP_UPDATE_TYPE eAutopType);

// 设置Zero 启动状态
void Autop_StartByZero(bool bStartByZero);

// 获取是否Zero 启动方式
bool Autop_IsRunByZero();

//设置 autop now
void Autop_SetAutopNow(bool bAutopNow, int iValue = -1);

//获取autop Now 状态
bool Autop_IsAutopNow();

// 获取当前autop 更新模式
AUTOP_UPDATE_TYPE Autop_GetUpdateType();

//开始RPS 连接流程
bool Autop_StartRPSConnect();

//开始Zero 连接流程
bool Autop_StartZeroConnect();

//开始autop code 流程
bool AutopStartCodeConnect(const yl::string & strName = "", const yl::string & strPwd = "");

//通用入口
bool Autop_StartConnect(AUTOP_UPDATE_TYPE eAutopType, const yl::string & strName,
                        const yl::string & strPwd);

//停止本次更新
bool Autop_Exit();

/**
 * @brief autop 流程是否开始.
 *
 * @author Song
 * @date 2017/12/14
 *
 * @param bSync (Optional) true:直接向autop查询.false:获取GUI上层缓存，由autop消息决定状态
 *
 * @return True if it succeeds, false if it fails.
 */

bool Autop_IsUpdating(bool bSync = false);

//RPS 功能是否开启
bool Autop_IsRpsEnable();

//是否需要显示更新中
bool Autop_IsNeedShowUpdating();

//获取autop 上电启动状态
POWER_ON_STATUS Autop_GetAutopStatus();

//等待网络
bool Autop_WaitingForNetwork();

//autop code 处理中
bool Autop_CodeProcessing();

//获取当前鉴权用户名
yl::string Autop_GetCurrentUserName();

//获取当前鉴权密码
yl::string Autop_GetCurrentPassword();

//输入鉴权信息
bool Autop_SetAuthInfo(const yl::string & strUserName, const yl::string & strPasswd,
                       bool bSaveConfig = true);

//////////////////////////////////////////////////////////////////////////////////////////////
// 是否时strNum 某个Code前缀
// 返回 0->部分匹配 1->全部匹配 -1->不匹配
int Autop_MatchAutoPCodePrefix(const yl::string & strNum, char cKey = 0);


/*******************************************************************
** 函数描述:   获取当前匹配上的号码
** 参数：      无
** 返回:        无
********************************************************************/
yl::string Autop_AutopCodeGetCurrentName();

#if IF_BUG_30885
bool Autop_StartOption66Connect();

bool Autop_StartDMSConnect();
#endif //IF_BUG_30885

#if IF_FEATURE_METASWITCH
bool Autop_StartHttpDigestTConnect();
#endif

//是否需要执行RPS
bool Autop_IsLaunchRPS();

/**
 * @brief 发送消息给autoServer.
 *
 * @author Song
 * @date 2017/9/6
 *
 * @param iSize Zero-based index of the size.
 *
 * @param bSync  True to synchronise.
 * @param uMesg  The message.
 * @param wParam The wParam field of the message.
 * @param lParam The lParam field of the message.
 *
 * @return True if it succeeds, false if it fails.
 */

bool Autop_NotifyAutoServer(bool bSync, UINT uMesg, WPARAM wParam, LPARAM lParam);

/**
 * @brief 发送消息给autoServer.
 *
 * @author Song
 * @date 2017/9/6
 *
 * @param bSync      True to synchronise.
 * @param uMesg      The message.
 * @param wParam     The wParam field of the message.
 * @param lParam     The lParam field of the message.
 * @param iSize      Zero-based index of the size.
 * @param pExtraData Information describing the extra.
 *
 * @return True if it succeeds, false if it fails.
 */

bool Autop_NotifyAutoServerEx(bool bSync, UINT uMesg, WPARAM wParam, LPARAM lParam, int iSize,
                              LPCVOID pExtraData);

#endif //_MOD_AUTOPCONTROL_H_
