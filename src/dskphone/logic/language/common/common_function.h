#pragma once

#include "config.h"
#include "yllist.h"
#include "ETLLib.hpp"

//是否为空白字符
bool IsWhiteSpace(int ch);

struct mapItem
{
    int                 iKey;
    LPCSTR const        strName;
};

// 两组操作函数
int StrToKey(const yl::string & str, const mapItem mapArr[], int iSize);

LPCSTR KeyToStr(int iKey, const mapItem mapArr[], int iSize);

#define Str2KeyEx(sstr, mapArr)  (StrToKey(sstr, mapArr, sizeof(mapArr)/sizeof(mapItem)))
#define Key2StrEx(ikey, mapArr)  (KeyToStr(ikey, mapArr, sizeof(mapArr)/sizeof(mapItem)))

void ReverseString(WCHAR * wchStart, int iLength);

void YLMiniBidiAlgorithm(yl::wstring & strW);

//跳过空白字符，返回第一个正常字符的字符串指针
LPCSTR SkipWhiteSpace(LPCSTR lpszText);

//根据lpszToken将lpszText截取，将所截取到的第一段字符串赋值给strText
int PeekPhraseByCharTokens(LPCSTR & lpszText, yl::string & strText, LPCSTR lpszToken);

LPCSTR PeekContentFromQuotaString(LPCSTR lpszParamText, yl::string & strContent);

//字符串排序
void SortStringList(YLList<yl::string> & liststring);

// 获取星期 短格式
LPCSTR GetWeeday(int nDay);
// 获取星期 长格式
LPCSTR GetWeedayFull(int nDay);
// 获取月份 短格式
LPCSTR GetMonth(int month);
// 获取月份 长格式
LPCSTR GetMonthFull(int month);


