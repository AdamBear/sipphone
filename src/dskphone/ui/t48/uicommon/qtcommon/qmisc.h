#ifndef QMISC_H
#define QMISC_H

/*
 * qMisc
 * 放置一些每个页面都可能用到的函数或者类
*/

#include <ETLLib/ETLLib.hpp>
#include <QObject>
#include <QString>
#include <QFontMetrics>
#include <ylstring.h>

#define CUTOffTEXT_LENGTH   3

//打印时间内容
void dskPhonePrintTime(const char * lpszFmt, ...);

//判断pChind是否为pRoot的子对象
bool isChildOf(QObject * pChild, QObject * pRoot);

//判断pChind是否为某个控件的子对象
bool isChildOf(const QObject * pChild, LPCSTR lpszClassName);

//根据传入的字体,将字符串strText宽iWidth后面的部分截断并添加...
QString CutOffText(const QString & strText, const QFontMetrics & objFontMetrics, int iWidth);

//根据传入的字体,将字符串strText宽iWidth前面的部分截断
QString CutOffTextExt(const QString & strText, const QFontMetrics & objFontMetrics, int iWidth);

// 将QString转换为ylString
yl::string fromQString(const QString & other);

// 将ylString转换为QString
QString toQString(const yl::string & other, bool bCheckFormat = false);

// 获取时分.
yl::string GetHourMinute(int nHour, int nMin, bool bShowMask = true);

// 获取时分.
yl::string GetHourMinute(time_t tTime, bool bShowMask = true);

// 获取时分.
yl::string GetData(time_t tTime, bool bFullWeekDay = true);

/* 函数名  ：Timet2String()                                       */
/* 功能  ： time_t格式的时间转换成字符串,并且翻译（因此不放在逻辑层）,转换后的时间不包括年，
/* 参数说明 ：tTime [in] 是要装换的时间，strSep [in]是分隔符，
bFullWeekDay [in] 是否显示星期完整名，不显示完整名时显示缩写，如完整名为Monday，缩写为Mon,同时
如果是今天或昨天日期会转换为today或者yesterday，不显示完整名时不会显示today和yesterday
bShowMask [in] 分和秒之间是否要冒号*/
/* 返回值 ：普通的时间，字符串                 */
/************************************************************************/
yl::string Timet2String(time_t tTime, const yl::string & strSep = "", bool bFullWeekDay = true,
                        bool bShowMask = false, bool bOnlyDate = false, bool bShowComma = false,
                        bool bAddZone = false);

// 将time_t类型的时间转换为字符串, 字符串格式由当前的日期和时间类型决定,转换后的时间包括年
// | tTime | 是整数型的时间.
// | strSep | 是日期和时间之间的分隔符, 默认为一个空格.
// 如果转换成功则返回转换后的字符串, 否则返回空.
yl::string Timet2DetailString(time_t tTime, const yl::string & strSep = " ");

yl::string GetDataFormat(tm * tmLocal, bool bShowComma = false);

/*******************************************************************
** 函数描述:   希伯来语 从右到左倒转
** 参数：      strSrcW: 需要倒转的字符串
** 返回:       无
********************************************************************/
void ReverseRTLWord(yl::wstring & strSrcW);

//希伯来语把"1.xxxx:" 或 “xxxx:” 转换成"1.:xxxx"
void ReverseRTLMaskString(QString & strText);

void RTL_GetRealString(QString & strRTL);

#endif //QMISC_H
