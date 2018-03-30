// Setting接口文件.

#ifndef MOD_SETTING_UI_LOGIC_H_
#define MOD_SETTING_UI_LOGIC_H_

#include <ylstring.h>
#include <settinguilogic/include/settingdefine.h>
#include "settinguilogic/include/common.h"
#include "broadsoft/callcenter/include/callcenter_data_define.h"

#ifdef __cplusplus
#define SETTING_EXPORT_C extern "C"
#else
#define SETTING_EXPORT_C
#endif

enum DATE_FORMAT_REQUEST
{
    DATE_FORMAT_REQUEST_CALLLOG,
    DATE_FORMAT_REQUEST_DATEWIDGET,
};

// 获取当前的日期格式, 日期格式为DateFormatType枚举类型.
// 如果获取成功则返回当前设置的日期格式, 否则返回DATE_FORMAT_TYPE_NONE.
int SettingUILogic_GetCurDateFormatType();
// 获取当前时间格式, 时间格式为TimeFormatType枚举类型.
// 如果获取成功则返回当前设置的时间格式, 否则返回TIME_FORMAT_TYPE_NONE.
int SettingUILogic_GetCurTimeFormatType();

// 将time_t类型的时间转换为字符串, 字符串格式由当前的日期和时间类型决定.
// | tTime | 是整数型的时间.
// | strSep | 是日期和时间之间的分隔符, 默认为一个空格.
// 如果转换成功则返回转换后的字符串, 否则返回空.
yl::string SettingUILogic_Time2String(time_t tTime, bool bAddTime = true, const yl::string & strSep = " ");

// 将time_t类型的时间转换为字符串, 字符串格式由当前的日期和时间类型决定.
// | tTime | 是整数型的时间.
// | strSep | 是日期和时间之间的分隔符, 默认为一个空格.
// 如果转换成功则返回转换后的字符串, 否则返回空.
yl::string SettingUILogic_Time2NewFormatString(time_t tTime, bool bAddTime = true,
        const yl::string & strSep = " ");

// 将time_t类型的时间转换为字符串, 字符串格式由当前的日期和时间类型决定.
// | tTime | 是整数型的时间.
// | strSep | 是日期和时间之间的分隔符, 默认为一个空格.
// 如果转换成功则返回转换后的字符串, 否则返回空.
//yl::string SettingUILogic_CallLogTime2String(time_t tTime, const yl::string& strSep = " ");

// 新需求:历史记录显示月日及时间
yl::string SettingUILogic_CallLogTime2SimpleString(time_t tTime);

// Setting模块的初始化.
void SettingUILogic_Init();

// 获取日期格式
yl::string SettingUILogic_GetDateFormat(tm * tmLocal, bool bShowComma = true);

void SettingUILogic_GetDateDetails(yl::string & strDate, UINT & uTick);

// 获取时间格式
yl::string GetTimeFormat(tm * tmLocal, bool bAddSecond = false);

// 处理定时器
bool SettingUILogic_OnTimer(int iTimerID) DECLARE_ATTR;

// 获取所有未读信息数.
// 如果获取成功则返回所有未读信息数, 否则返回0.
int SettingUILogic_GetTotalUnReadMessage();

// 设置对比度
void SettingUILogic_Contrast(int iLevel, bool bEXP = false, int iEXPIndex = 0);

/************************************************************************/
// 功能: Idle界面 按SMS键进入对应的SMS界面
/************************************************************************/
bool SettingUILogic_ConnectVoiceMail(int iAccountID = -1);

bool SettingUILogic_ViewVoiceMail(int iAccountID = -1);

// 获取时长,nDuration单位为ms
void SettingUILogic_Duration2String(int nDuration, yl::string & strDuration);

// 获取IME类型
int SettingUILogic_GetIMEType(const char * pszIME);

// 获取时分秒. 秒为-1，则不获取对应字符串
yl::string SettingUILogic_GetHourMinuteSecond(int nHour, int nMin, int nSec = -1, bool bShowMask = true);

// zero 相关接口
/************************************************************************/
/*  接口:SettingUILogic_EnterZero()                                                   */
/*  接口说明: 进入Zero的入口函数                                     */
/*  参数说明: 无参数                                                 */
/*  返回值:  无                                                         */
/************************************************************************/
void SettingUILogic_EnterZero(bool bZeroLanuched = false, bool bBootEnter = false);

/************************************************************************/
// 功能: 进入RPS界面
/************************************************************************/
bool SettingUILogic_EnterZeroRPS();

/************************************************************************/
// 功能: 网络检测界面
/************************************************************************/
bool SettingUILogic_CheckingNetwork();

/************************************************************************/
/*  接口:SettingUILogic_ExitZero()                                                    */
/*  接口说明: 退出Zero                                                    */
/*  参数说明: 无参数                                                 */
/*  返回值:  无                                                         */
/************************************************************************/
void SettingUILogic_ExitZero();

/************************************************************************/
/*  接口:SettingUILogic_EnterZeroNextStage()                                          */
/*  接口说明: 进入到下一个状态                                          */
/*  参数说明: strCurrentPageAction:当前界面的action                        */
/*            strSoftkeyAction: 当前softkey操作的类型                    */
/*  返回值:  无                                                         */
/************************************************************************/
void SettingUILogic_EnterZeroNextStage(const yl::string& strCurrentPageAction, const yl::string& strSoftkeyAction);

/************************************************************************/
/*  接口:SettingUILogic_SetRedirectData()                                         */
/*  接口说明: 设置是否是由UI启动的，把UI的值传入                     */
/*  参数说明: strUserName传入的用户名 strPassword传入的密码          */
/*  返回值:  无                                                         */
/************************************************************************/
void SettingUILogic_SetRedirectData(const yl::string strUserName, const yl::string strPassword);

/************************************************************************/
/*  接口:SettingUILogic_SetRedirectData()                                */
/*  接口说明: 设置是否是由UI启动的，把UI的值传入                             */
/*  参数说明: strUserName传入的用户名 strPassword传入的密码                  */
/*  返回值:  无                                                          */
/************************************************************************/
void SettingUILogic_AsyncAutop();

#if IF_SUPPORT_WALLPAPER
/************************************************************************/
/*  接口:SettingUILogic_GetWallpaperPath()                                         */
/*  接口说明: 获取壁纸路径                     */
/*  参数说明:           */
/*  返回值:  壁纸路径                                                        */
/************************************************************************/
const yl::string SettingUILogic_GetWallpaperPath(WALLPAPER_Type_E eWallType);
#endif

bool SettingUILogic_GetACDStatusString(ACD_STATUS eAcdStatus, yl::string& strAcdStatus);


#endif  // MOD_SETTING_UI_LOGIC_H_
