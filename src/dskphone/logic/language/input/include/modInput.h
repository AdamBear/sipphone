#pragma once

#include "config.h"
#include <ETLLib.hpp>
#include <ylstring.h>


// 模块初始化
void Ime_Lang_Init();

// 重新加载翻译、语言 输入法
void Lang_ReLoadLang();

// 小写=>大写
UINT Ime_Lower2Upper(UINT uCode);
// 大写=>小写
UINT Ime_Upper2Lower(UINT uCode);

// 是否中文样式的日期格式
bool Lang_IsChineseStyleDate();

// 是否是RTL的一组判断接口
bool Lang_IsRTL(const yl::string & strValue);
bool Lang_IsRTL(const yl::wstring & strValue);
bool Lang_IsRTL(WCHAR wch);

// 翻译预处理 的一组接口
yl::wstring Lang_Format(const yl::string & strValue);
yl::wstring Lang_Format(LPCWSTR lpwszText);
void Lang_Format(yl::wstring & strW);


int Ime_GetCustomImeCount();

// 以下接口用于兼容客户特制，满足当前版本的客户特制，在新版本中可以去掉以下接口
#ifdef IF_FEATURE_KOREANIME
bool Ime_IsKorean(int iIndex);
#endif

bool Lang_IsJapanese();