#ifndef _DSSKEY_COMMON_FUCS_H_
#define _DSSKEY_COMMON_FUCS_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>
#include <QtGui>

/*******************************************************************
** 函数描述:   初始化Dsskey类型字符串数据
** 参数：      无
** 返回:       无
********************************************************************/
void DsskeyTypeString_Init();

/*******************************************************************
** 函数描述:   获取Dsskey类型字符串数据
** 参数：      Dsskey类型
** 返回:       Dsskey类型字符串
********************************************************************/
yl::string GetDsskeyTypeString(int type);

/*******************************************************************
** 函数描述:   希伯来语 从右到左倒转
** 参数：      strSrcW: 需要倒转的字符串
** 返回:       无
********************************************************************/
//void EXPReverseRTLWord(yl::wstring& strSrcW);

/*******************************************************************
** 函数描述:   翻译Dsskey的Label
** 参数：      lpszText 需要翻译的字符串 bIsExpKey 是否扩展台
** 返回:       无
********************************************************************/
QString TranslateDsskeyLabel(const char * lpszText, const bool bIsExpKey);

#define LANG_TRANSLATE_DSSKEY_LABEL(lpszText, bIsExpKey)    (TranslateDsskeyLabel((lpszText), (bIsExpKey)))

#endif //_DSSKEY_COMMON_FUCS_H_
