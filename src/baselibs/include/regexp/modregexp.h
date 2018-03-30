#ifndef __MOD_REGEXP_h__
#define __MOD_REGEXP_h__

#include <stdint.h>
// 正则表达式句柄
typedef intptr_t HREGEXP;

//////////////////////////////////////////////////////////////////////////
// C interface
#ifdef  __cplusplus
extern "C" {
#endif
/************************************************************************
** 功能: 是否完成匹配正则规则
** 输入: szRegExp(正则表达式), szMatchStr(要求匹配的字符串)
** 返回: 0/-1 (OK/FAIL)
************************************************************************/
int regExp_IsMatchedExact(const char * szRegExp, const char * szMatchStr);

/************************************************************************
** 功能: 执行正则表达式替换功能
** 输入: szRegExp(正则表达式), szMatchStr(要求匹配的字符串), szRepPattern(替换的模式)
** 输出: szReplacedBuf(替换后的字符串)
** 返回: 0/-1/-2 (成功/不匹配/保存返回的字符BUF长度不够)
************************************************************************/
int regExp_Replace(const char * szRegExp, const char * szMatchStr, const char * szRepPattern,
                   char * szReplacedBuf, int iBufLen);

/************************************************************************
** 功能: 是否完成匹配正则规则，返回第一次匹配的位置信息
** 输入: szRegExp(正则表达式), szMatchStr(要求匹配的字符串) iStartPos开始匹配的位置，iEndPos结束匹配的位置
** 返回: 0/-1 (OK/FAIL) 返回值为0的情况下iStartPos、iEndPos才有实际意义，否则没有实际意义
************************************************************************/
int regExp_IsMatchedExactEx(const char * szRegExp, const char * szMatchStr, int & iStartPos,
                            int & iEndPos);

/************************************************************************
** 功能: 创建并编译正则表达式
** 输入: cszRegExp(正则表达式)
** 返回: 正则表达式句柄，用完后需要释放
************************************************************************/
HREGEXP regExp_Alloc(const char * cszRegExp);

/************************************************************************
** 功能: 释放正则表达式句柄
** 输入: hRegExp(正则表达式句柄)
************************************************************************/
void regExp_Release(HREGEXP & hRegExp);

/************************************************************************
** 功能: 执行正则表达式替换功能
** 输入: hRegExp(正则表达式句柄), szMatchStr(要求匹配的字符串), szRepPattern(替换的模式)
** 输出: szReplacedBuf(替换后的字符串缓冲区), iBufLen(字符串缓冲区大小), iTimes(替换次数, -1表示全部替换)
** 返回: 0/-1/-2/-3 (成功/不匹配/保存返回的字符BUF长度不够/句柄无效)
************************************************************************/
int regExp_ReplaceEx(HREGEXP hRegExp, const char * szMatchStr, const char * szRepPattern,
                     char * szReplacedBuf, int iBufLen, int iTimes = -1);

#ifdef  __cplusplus
};
#endif

#endif //__MOD_REGEXP_h__
