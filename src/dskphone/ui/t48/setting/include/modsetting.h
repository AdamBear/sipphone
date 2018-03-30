// Setting接口文件.

#ifndef SETTING_MODSETTING_H_
#define SETTING_MODSETTING_H_

#include <ylstring.h>
#include "ETLLib/ETLLib.hpp"
#include "setting/include/common.h"
#include "setting/setting_data_define.h"
#include "feature/include/featurecommon.h"

#ifdef __cplusplus
#define SETTING_EXPORT_C extern "C"
#else
#define SETTING_EXPORT_C
#endif

struct VoiceMailInfoListData;
struct AccountTextMsgListData;

enum ESettingDefaultIconType
{
    eSDIT_UnreadIncomMsgDeftIcon,// 未读接收信息的图片名.
    eSDIT_ReadIncomMsgDeftIcon, // 已读接收信息的图片名.
    eSDIT_DraftMsgDeftIcon,         // 草稿信息的图片名.
    eSDIT_SendingMsgDeftIcon,       // 发送中信息的图片名.
    eSDIT_SentMsgDeftIcon               // 发送完毕信息的图片名.
};

//根据实际的用户名获取用户的显示名
yl::string Setting_GetUserDisplayName(yl::string strUsername);

// 获取当前的日期格式, 日期格式为DateFormatType枚举类型.
// 如果获取成功则返回当前设置的日期格式, 否则返回DATE_FORMAT_TYPE_NONE.
SETTING_EXPORT_C int Setting_GetCurDateFormatType();
// 获取当前时间格式, 时间格式为TimeFormatType枚举类型.
// 如果获取成功则返回当前设置的时间格式, 否则返回TIME_FORMAT_TYPE_NONE.
SETTING_EXPORT_C int Setting_GetCurTimeFormatType();

// 将time_t类型的时间转换为字符串, 字符串格式由当前的日期和时间类型决定.
// | tTime | 是整数型的时间.
// | strSep | 是日期和时间之间的分隔符, 默认为一个空格.
// 如果转换成功则返回转换后的字符串, 否则返回空.
//yl::string Setting_Time2String(time_t tTime, const yl::string& strSep = " ");

// 将time_t类型的时间转换为字符串, 字符串格式由当前的日期和时间类型决定.
// | tTime | 是整数型的时间.
// | strSep | 是日期和时间之间的分隔符, 默认为一个空格.
// 如果转换成功则返回转换后的字符串, 否则返回空.
//yl::string Setting_Time2String(time_t tTime, bool bAddTime = true, const yl::string& strSep = " ");

// 新需求:历史记录显示月日及时间
yl::string Setting_CallLogTime2SimpleString(time_t tTime);

// 注册消息, 注册的消息为网页改变了网络配置时的处理.
SETTING_EXPORT_C void Setting_RegisterMessage();

// 取消setting 模块的消息注册.
SETTING_EXPORT_C void Setting_UnRegisterMessage();

// Setting模块的初始化.
void Setting_Init();
// Setting模块的清除.
void Setting_Final();

// 获取当前墙纸(全路径).
// 如果获取成功则返回当前墙纸(全路径), 否则返回空字符串.
yl::string Setting_GetCurrentWallpaper();

// 获取当前主题(全路径).
// 如果获取成功则返回当前主题(全路径), 否则返回空字符串.
yl::string Setting_GetCurrentTheme();

// 获取当前铃声(全路径).
// 如果获取成功则返回当前铃声(全路径), 否则返回空字符串.
yl::string Setting_GetCurrentRing();

// 获取所有账号当前有的Voice Mail列表.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
SETTING_EXPORT_C bool Setting_GetVoiceMailList(VoiceMailInfoListData * pData);

// 点击某个Voice Mail的处理
// | nIndex | 是某个下标, 从0开始.
void Setting_ClickVoiceMailInfo(int nIndex);

// 获取所有账号当前有的Text Message列表.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
SETTING_EXPORT_C bool Setting_GetTextMsgList(AccountTextMsgListData * pData);


// 获取所有未读信息数.
// 如果获取成功则返回所有未读信息数, 否则返回0.
int Setting_GetTotalUnReadMessage();

// 根据账号id获取账号当前的未读信息数.
// | nAccountId | 是账号的用户名.
// 如果获取成功则返回当前数量, 否则返回0.
int Setting_GetAccountUnRead(int nAccountId);

// Message按键处理
bool Setting_OnMsgKeyPress();

//设置Setting中用到的默认图标
void Setting_SetDefaultIcon(ESettingDefaultIconType eIconType, const yl::string & strIconFile);

//获取Setting中用到的默认图标
yl::string Setting_GetDefaultIcon(ESettingDefaultIconType eIconType);

//获取当前对比度
int Setting_GetCurrentContrastLevel();

//获取当前背光亮度
int Setting_GetCurrentBacklightLevel();

//话机重启，基于DeviceLib的看门狗重启加上一些清理操作
void Setting_Reboot(int iTimeDelay = 1);

// 注册重启前提示的回调函数
void Setting_RegisterRebootTipCallBack(FunRebootTipCallBack fun);

bool Setting_ConnectVoiceMail(int iAccountID = -1);

#endif  // SETTING_MODSETTING_H_
