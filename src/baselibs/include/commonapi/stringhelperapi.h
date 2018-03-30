#ifndef _STRING_COMMON_API_HEADER_
#define _STRING_COMMON_API_HEADER_

#include <ylstl/yllist.h>
#include <ETLLib/ETLLib.hpp>

//#include <string>

#include <ylstl/ylstring.h>
#include "common_api_enumtypes.h"
/*******************************************************************
** 函数描述:   增加http头
** 参数：      [in] strURL: http头信息
** 返回:       无
********************************************************************/
void commonAPI_AddHttpHead(yl::string & strURL);


/*******************************************************************
** 函数描述:   判断字符串是否为空
** 参数：      字符指针
** 返回:       TRUE:空, FALSE:不为空
********************************************************************/
BOOL commonAPI_IsEmpty(LPCSTR lpszInput);

/*******************************************************************
** 函数描述:     将DHCP的Option字段转换成URL
** 参数：        [in] str option字段信息
                [in] buf 转换结果
                [in] size buf长度
** 返回:      -1 失败, >0:成功(url len)
********************************************************************/
int commonAPI_GetIdFromStrAction(const yl::string & strAction);

// Trim string from input string.
// For example.
// Trim "####112 112 113 " to "112 112 113 " when strTrim is "#" and eTrimType is TRIM_LEFT.
// | strInput | [in] is the input string.
// | strTrim | [in] is the trim string.
// | eTrimType | [in] is the trimmed type, such as TRIM_LEFT, TRIM_RIGHT, OR TRIM_ALL.
// Return the string trimmed.
yl::string commonAPI_TrimString(const yl::string & strInput,
                                const yl::string & strTrim, TrimType eTrimType = TRIM_ALL);

// skip white char(\r,\n,\t, )
// return string without whitechar
yl::string commonAPI_SkipWhiteChar(const yl::string & strInput, TrimType eTrimType = TRIM_ALL);

// 将字符串全转换为大写或小写形式. 何种形式, 取决于参数.
// | strInput | [in] 是待转换的字符串.
// | eType | [in] 为转换方式.
// 如果转换成功则返回转换后的字符串, 否则返回原字符串.
yl::string commonAPI_StringConvert(const yl::string & strInput,
                                   StringConvertType eType);

//是否前置匹配函数，isCaseSensitive参数可指定是否区分大小写
bool commonAPI_StringMatch(const yl::string & str1, const yl::string & str2, bool isCaseSensitive,
                           StringMatchType eMatchtype = CONTAIN_MATCH);


// 获取起始index.
// | pnIndexFrom | [out] 为起始index.
// | pnIndexTo | [out] 为终止index.
// | nIndexFrom | 为起始index.
// | nCountToGet | 为需获取的数量.
// | nMaxIndexTo | 为终止index可以到的最大值.
// 如果获取成功则返回true, 否则返回false.
bool commonAPI_GetFromAndToIndex(int * pnIndexFrom,
                                 int * pnIndexTo,
                                 int nIndexFrom,
                                 int nCountToGet,
                                 int nMaxIndexTo);


/*******************************************************************
** 函数描述:     字符串是否由全数字组成
** 参数：        [in] strKey 待判断的字符串
** 返回:      -1 失败, >0:成功(url len)
********************************************************************/
bool commonAPI_IsDigital(const yl::string & strKey);


/*******************************************************************
** 函数描述:     将DHCP的Option字段转换成URL
** 参数：        [in] str option字段信息
                [in] buf 转换结果
                [in] size buf长度
** 返回:      -1 失败, >0:成功(url len)
********************************************************************/
int commonAPI_GetWebServerUrl(const char * str, char * buf, int size);

/*******************************************************************
** 函数描述:    在字符串的指定位置插入字串
** 参数：        [in/out] strSrc 待插入字串的字符串
                [in] strToAppend 插入的字符串
                [in] insertPos 插入位置
** 返回:      插入结果
********************************************************************/
bool commonAPI_appendSubString(yl::string & strSrc, const yl::string & strToAppend, int insertPos);

/*******************************************************************
** 函数描述:     格式化字符串，返回格式后的字符串
** 参数：        [in] szFormat 格式串
** 返回:      -1 失败, >0:成功(url len)
********************************************************************/
yl::string commonAPI_FormatString(const char * szFormat, ...);

/*******************************************************************
** 函数描述:    将目标字符串中的指定字串替换为指定的字符串
** 参数：        [in] strSrc 目标字符串
                [in] subStrToReplace：待替换的字串
                [in] strToInsert 待插入的字符串
** 返回:      操作结果
********************************************************************/
yl::string commonAPI_replaceSubStringByTag(const yl::string & strSrc,
        const yl::string & subStrToReplace, const yl::string & strToInsert);

/*******************************************************************
** 函数描述:    解析指定字符串，并根据分割符输出字串集合
** 参数：        [in] strInput 待解析的字符串
                [in] Separator 分割符号
                [out] OutPutList 字串集合
** 返回:      -1 失败, >0:成功(url len)
********************************************************************/
bool commonAPI_splitStringBySeparator(const yl::string & strInput, const char Separator,
                                      YLList<yl::string> & OutPutList);

/*******************************************************************
** 函数描述:    解析指定字符串，并根据分割符输出字串集合
** 参数：        [in] strInput 待解析的字符串
[in] Separator 分割符号
[out] OutPutList 字串集合
** 返回:      -1 失败, >0:成功(url len)
********************************************************************/
bool commonAPI_splitStringBySeparator(const yl::string & strInput, const yl::string & strSeparator,
                                      YLList<yl::string> & OutPutList);

/*******************************************************************
** 函数描述:    解析字符串中的Action信息
** 参数：        [in] strClickAction 待解析的字符串
                [out] pstrAction action信息
                [out] pstrContent action内容
                如actionName(actionContent) 输出pstrAction:actionName pstrContent:actionContent
** 返回:      -1 失败, >0:成功(url len)
********************************************************************/
bool commonAPI_ParseActionInfo(yl::string * pstrAction, yl::string * pstrContent,
                               const yl::string & strClickAction);

/*******************************************************************
** 函数描述:   将time转化为制定格式的时间字符串
** 参数：      time
                formatType TM_TYPE_DEFAULT:日期+时间
                TM_TYPE_DATE 只有日期
                TM_TYPE_TIME 只有时间
** 返回:       指定格式的字符串
********************************************************************/
yl::string commonAPI_convertFormatTime(SYSTEMTIME time, TIME_FORMAT_TYPE formatType);

/*******************************************************************
** 函数描述:   将time转化为制定格式的时间字符串
** 参数：      time
               format：格式字符串
               年:%Y 月:%m 日:%d 时:%H(24小时制) %I(12小时制) 分:%M 秒:%S

               例如：commonAPI_convertFormatTime(time, "year:%Y month:%m day:%d");
               结果：year:2014 month:03 day:26
** 返回:       指定格式的字符串
********************************************************************/
yl::string commonAPI_convertFormatTime(SYSTEMTIME time, const yl::string & format);

/*******************************************************************
** 函数描述:    目标字符串是否为IPV6格式
** 参数：        [in] lpszIpAddress 待操作字符串
** 返回:      正确与否
********************************************************************/
bool commonAPI_IsValidIPV6(LPCSTR lpszIpAddress);

/*******************************************************************
** 函数描述:    目标字符串是否为IPV4格式
** 参数：        [in] lpszIpAddress 待操作字符串
** 返回:      正确与否
********************************************************************/
bool commonAPI_IsValidIPV4(LPCSTR lpszIpAddress);
//
///*******************************************************************
//** 函数描述:     将IPv4的地址转化为字符串
//** 参数：        [in] 待转换的地址
//** 返回:      转换完成的字符串
//********************************************************************/
//yl::string commonAPI_Addr2String(const in_addr & addr);
//
///*******************************************************************
//** 函数描述:     将IPv6的地址转化为字符串
//** 参数：        [in] 待转换的地址
//** 返回:      转换完成的字符串
//********************************************************************/
//yl::string commonAPI_Addr2String(const in6_addr & addr);
//
///*******************************************************************
//** 函数描述:     将IPv4格式的字符串转化为in_addr
//** 参数：        [in] pStr 待转换得字符串
//** 返回:      in_addr
//********************************************************************/
//in_addr commonAPI_IPV4String2Addr(const char * pStr);
//
///*******************************************************************
//** 函数描述:     将IPv6格式的字符串转化为in_addr
//** 参数：        [in] pStr 待转换得字符串
//** 返回:      in_addr
//********************************************************************/
//in6_addr commonAPI_IPV6String2Addr(const char * pStr);

/*******************************************************************
** 函数描述:     判断指定字符串是否为有效IP地址，包括IPv4和IPv6
** 参数：        [in] lpszIpAddress
** 返回:      true 有效IP地址, false:无效IP地址
********************************************************************/
BOOL commonAPI_IsValidIP(LPCSTR lpszIpAddress);


// 将字符串转换为T9按键数字字符串.
// | strInput | 是输入的字符串.
// 如果转换成功则返回按键数字字符串, 否则返回空.
yl::string commonAPI_transT9String(const yl::string & strInput);
#endif
