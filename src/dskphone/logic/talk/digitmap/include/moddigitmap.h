#ifndef __MOD_DIGIT_MAP_H__
#define __MOD_DIGIT_MAP_H__

#include "config.h"

/*******************************************************************
** 模块初始化
** 参数：void
** 返回值：void
********************************************************************/
void DigitMap_Init();

/*******************************************************************
** 依据当前号码返回操作类型
** 参数：strNumber：欲呼出号码
** 返回值：是否是黑名单号码
********************************************************************/
bool DigitMap_IsInBlacklist(const yl::string & strNumber, int iAccount = -1);

/*******************************************************************
** 依据当前号码返回操作类型
** 参数：strNumber：欲呼出号码
** 返回值：是否是呼出号码
********************************************************************/
bool DigitMap_IsDelayCallout(const yl::string & strNumber, int iAccount = -1);

/*******************************************************************
** 获取格式化&替换后的URI
** 参数：strNumber：欲呼出号码
** 返回值：sip uri 是否改变
********************************************************************/
bool DigitMap_GetFmtUri(yl::string & strNumber, int iAccount = -1);

/*******************************************************************
** 获取账号ID
** 参数：strNumber：欲呼出号码
** 返回值：账号ID
********************************************************************/
bool DigitMap_GetLineID(const yl::string & strNumber, int & iAccount);

/*******************************************************************
** 获取延时呼出的延迟时间
** 参数：strNumber：欲呼出号码
** 返回值：延迟时间（0表示：立即呼出）
********************************************************************/
UINT DigitMap_GetTimer(const yl::string & strNumber, int iAccount = -1);

/*******************************************************************
** 是否立即播放拨号音
** 参数：strNumber：欲呼出号码
** 返回值：延迟时间（0表示：立即呼出）
********************************************************************/
bool DigitMap_PlayDialTone(const yl::string & strNumber, int iAccount = -1);

/*******************************************************************
** 获取开关状态
** 参数：iAccount：账号ID
** 返回值：true/false true开关开启
********************************************************************/
bool DigitMap_IsEnable(int iAccount = -1);

/*******************************************************************
** 获取预拨号开关状态
** 参数：iAccount：账号ID
** 返回值：true/false true开关开启
********************************************************************/
bool DigitMap_IsConfigEnable(LPCSTR lpConfigName, int iAccount = -1);

/*******************************************************************
** 转换来电号码
** 参数：号码
** 返回值：true/false
********************************************************************/
bool DigitMap_GetFmtIncomingNumber(yl::string& strNumber);

#endif // __MOD_DIGIT_MAP_H__

