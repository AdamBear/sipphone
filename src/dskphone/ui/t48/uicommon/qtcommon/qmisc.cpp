#include "qmisc.h"
#include "dsklog/log.h"
#include <QRegExp>


#include <sys/time.h>


#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "devicelib/networkserver.h"
#include <ylstl/ylstringalgorithm.h>
#include "config.h"
#include "language/input/include/modInput.h"
#include "language/common/common_function.h"
#include "setting/src/dateandtimecontroller.h"
#include "commonapi/stringhelperapi.h"
#include "setting/include/modsetting.h"
#include <winimp/winimp.h>

// 最大的buffer长度.
#define knMaxBufferLen 260

enum CHAR_RTL_MASK_TYPE
{
    HB_INVALID = 0,
    HB_WITH_INDEX,
    HB_NO_INDEX,
    HB_INDEX_TAG,  // 1. more...
    HB_ONLY_TAG,   // updateing...
};


void dskPhonePrintTime(const char * lpszFmt, ...)
{
    struct timeval tData;
    gettimeofday(&tData, NULL);
    ulong ulCostTimes = tData.tv_sec * 1000 + tData.tv_usec / 1000;
    va_list argList;
    va_start(argList, lpszFmt);
    fprintf(stdout, "[Time:(%.8u)] ", ulCostTimes);
    vfprintf(stdout, lpszFmt, argList);
    fprintf(stdout, "\n");
    va_end(argList);
}

//判断pChind是否为pRoot的子对象
bool isChildOf(QObject * pChild, QObject * pRoot)
{
    QObject * pParent = pChild;
    while (pParent != NULL)
    {
        if (pParent == pRoot)
        {
            return true;
        }
        pParent = pParent->parent();
    }
    return false;
}

//判断pChind是否为某个控件的子对象
bool isChildOf(const QObject * pChild, LPCSTR lpszClassName)
{
    // 入口参数判断
    if (NULL == pChild)
    {
        return false;
    }

    const QObject * pParent = pChild;
    while (NULL != pParent)
    {
        if (pParent->inherits(lpszClassName))
        {
            return true;
        }
        pParent = pParent->parent();
    }

    return false;
}

// 截断过长的Text
// iWidth为Text显示的宽度
QString CutOffText(const QString & strText, const QFontMetrics & objFontMetrics, int iWidth)
{
    int iTextWidth = objFontMetrics.width(strText);

    // 如下情况返回原字符
    // 字符串为空
    // 显示宽度大于文字长度
    if (strText.isEmpty()
            || iWidth >= iTextWidth)
    {
        return strText;
    }

    // 显示宽度小于末端符号返回原字符
    QString strEndSymbol("...");
    iWidth -= objFontMetrics.width(strEndSymbol);
    if (iWidth <= 0)
    {
        return strText;
    }

    // 二分法确定截断位置的范围
    int iMinIndex = 0;
    int iMaxIndex = strText.size() - 1;
    int iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    int iInterval = 2;

    // 循环确定截断的范围,范围大小为0~2*iInterval
    while (iMaxIndex > iMidIndex + iInterval)
    {
        if (iWidth >= objFontMetrics.width(strText.left(iMidIndex)))
        {
            iMinIndex = iMidIndex;
        }
        else
        {
            iMaxIndex = iMidIndex - 1;
        }

        iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    }

    // 逆向遍历找出第一个符合的位置
    while (iMinIndex <= iMaxIndex)
    {
        if (iWidth > objFontMetrics.width(strText.left(iMaxIndex)))
        {
            break;
        }

        --iMaxIndex;
    }

#ifdef IF_FEATURE_ARABIC
    yl::wstring strSrcW = yl::to_utf16(fromQString(strText));
    if (Lang_IsRTL(strSrcW))
    {
        static QRegExp exp_index_dot("\\d+[.]{1}.*");
        if (exp_index_dot.exactMatch(strText))
        {
            // 处理 1. ABCDEFG  截断，处理之后为1. ...ABC
            int iPos = strSrcW.find_first_of('.');
            if (iPos != yl::wstring::npos && iPos + 1 < strSrcW.length())
            {
                yl::wstring strPrefix = strSrcW.substr(0, iPos + 1);

                int iFrom = iPos;
                while ((strSrcW[iFrom] == ' ' || strSrcW[iFrom] == ':' || strSrcW[iFrom] == '.')
                        && iFrom < strSrcW.length())
                {
                    ++iFrom;
                }

                if (iFrom < strSrcW.length())
                {
                    yl::wstring strSuffer = strSrcW.substr(iFrom);

                    QString strPre = toQString(yl::to_utf8(strPrefix)) + " ";
                    if (iWidth - objFontMetrics.width(strPre) <= 0)
                    {
                        return strText;
                    }

                    return strPre + CutOffText(toQString(yl::to_utf8(strSuffer)), objFontMetrics,
                                               iWidth - objFontMetrics.width(strPre));
                }

                return strText;
            }
        }

        // ABCDEFG => ...ABC
        return strEndSymbol + strText.left(iMaxIndex);
    }
#endif

    return strText.left(iMaxIndex) + strEndSymbol;
}

// 截断过长的Text, 显示后半段且不显示...
// iWidth为Text显示的宽度
QString CutOffTextExt(const QString & strText, const QFontMetrics & objFontMetrics, int iWidth)
{
    int iTextWidth = objFontMetrics.width(strText);

    // 如下情况返回原字符
    // 字符串为空
    // 显示宽度大于文字长度
    if (strText.isEmpty()
            || iWidth >= iTextWidth)
    {
        return strText;
    }

    // 二分法确定截断位置的范围
    int iMinIndex = 0;
    int iMaxIndex = strText.size() - 1;
    int iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    int iInterval = 2;

    // 循环确定截断的范围,范围大小为0~2*iInterval
    while (iMaxIndex > iMidIndex + iInterval)
    {
        if (iWidth >= objFontMetrics.width(strText.right(iMidIndex)))
        {
            iMinIndex = iMidIndex;
        }
        else
        {
            iMaxIndex = iMidIndex - 1;
        }

        iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    }

    // 逆向遍历找出第一个符合的位置
    while (iMinIndex <= iMaxIndex)
    {
        if (iWidth > objFontMetrics.width(strText.right(iMaxIndex)))
        {
            break;
        }

        --iMaxIndex;
    }

    return strText.right(iMaxIndex);
}

static QString g_strTranTestText = "";

void InitTranTestText()
{
    static bool bInited = false;
    if (bInited)
    {
        return;
    }
    //计算能容纳多少个'a'
    for (int i = 0; i < 16; i++)
    {
        g_strTranTestText.append("aaaaaaaaaaaaaaaa");
    }
    bInited = true;
}

int CountMaxCharInCtrl(const QFontMetrics & objFontMetrics, int iTextAreaWidth)
{
    InitTranTestText();
    //计算能容纳多少个'a'
    QString strText = g_strTranTestText;

    // 显示宽度小于末端符号返回原字符
    if (iTextAreaWidth <= 0)
    {
        return 0;
    }

    int iCount = 1;
    // 循环确定截断的范围
    while (iTextAreaWidth >= objFontMetrics.width(strText.left(iCount)))
    {
        ++iCount;
    }
    iCount--;

    return iCount;
}

QString toQString(const yl::string & other, bool bCheckFormat/* = false*/)
{
    QString strTemp = QString::fromUtf8(other.c_str());

    if (bCheckFormat)
    {
        yl::string ylstring = fromQString(strTemp);

        if (ylstring != other)
        {
            // 根据不同的编码类型转换编码
            return QString::fromLatin1(other.c_str());
        }
    }

    return strTemp;
}

yl::string fromQString(const QString & other)
{
    yl::string strTemp = other.toUtf8().data();
    return  strTemp;
}

// 获取时分.
yl::string GetHourMinute(int nHour, int nMin, bool bShowMask/*=true*/)
{
    yl::string strAPM;  // 上下午标识, 可能为空.
    int nHourGet = nHour;

    // 根据setting的格式, 返回时分.
    char szBuffer[knMaxBufferLen + 1] = {'\0'};
    int iTimeFormat = Setting_GetCurTimeFormatType();
    switch (iTimeFormat)
    {
    case TIME_FORMAT_12HOUR:
        if (nHour > 12)
        {
            //下午
            nHourGet = nHour - 12;
            strAPM = TRID_PM;
        }
        else if (nHour < 12 && nHour > 0)
        {
            //上午
            nHourGet = nHour;
            strAPM = TRID_AM;
        }
        else if (nHour == 12)
        {
            //中午12点
            nHourGet = 12;
            strAPM = TRID_PM;
        }
        else if (nHour == 0)
        {
            //半夜12点
            nHourGet = 12;
            strAPM = TRID_AM;
        }
        break;

    default:
        nHourGet = nHour;
        break;
    }

    if (strAPM.empty())
    {
        sprintf(szBuffer, bShowMask ? "%02d:%02d" : "%02d %02d", nHourGet, nMin);
    }
    else
    {
        sprintf(szBuffer, bShowMask ? "%02d:%02d %s" : "%02d %02d %s", nHourGet, nMin, strAPM.c_str());
    }

    szBuffer[knMaxBufferLen] = '\0';
    return szBuffer;
}

yl::string GetHourMinute(time_t tTime, bool bShowMask/* = true*/)
{
    tm * tmLocal = localtime(&tTime);
    if (tmLocal != NULL)
    {
        return GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min, bShowMask);
    }

    return yl::string();
}

// 获取时分.
yl::string GetData(time_t tTime, bool bFullWeekDay/* = true*/)
{
    // 最大的buffer长度.
    static const long klSecPerDay = 24 * 60 * 60;

    time_t tTemp = tTime;
    tm * ptmTransit = localtime(&tTemp);
    if (ptmTransit == NULL)
    {
        return yl::string();
    }

    if (!bFullWeekDay)
    {
        return GetDataFormat(ptmTransit);
    }

    yl::string strRet = "";
    //用临时变量解决localtime()函数的缓存区问题
    tm objtmTransit = *ptmTransit;
    tm * tmLocal = &objtmTransit;

    time_t tCurrent = netGetLocalTime();
    time_t tTodayZero = (tCurrent / klSecPerDay) * klSecPerDay;  // 今天0点的时间.
    time_t tTomorrowZero = tTodayZero + klSecPerDay;  // 明天0点的时间.
    time_t tSixDayBeforeZero = tTodayZero - klSecPerDay * 7;  // 7天前的0点时间.

    if (tTime >= tTodayZero
            && tTime < tTomorrowZero)
    {
        strRet = LANG_TRANSLATE(TRID_TODAY).toUtf8().data();
    }
    else if (tTime >= tTodayZero - klSecPerDay
             && tTime < tTodayZero)
    {
        strRet = LANG_TRANSLATE(TRID_YESTERDAY).toUtf8().data();
    }
    else if (tTime >= tSixDayBeforeZero
             && tTime < tTodayZero - klSecPerDay)
    {
        strRet = LANG_TRANSLATE(GetWeedayFull(tmLocal->tm_wday)).toUtf8().data();
    }
    else
    {
        // 其他情况都显示年月日.
        strRet = GetDataFormat(tmLocal);
    }

    return strRet;
}

yl::string Timet2String(time_t tTime, const yl::string & strSep /*= " "*/,
                        bool bFullWeekDay/* = true*/, bool bShowMask/* = false*/, bool bOnlyDate/* = false*/,
                        bool bShowComma /*= false*/, bool bAddZone /*= false*/)
{
    // 最大的buffer长度.
    static const long klSecPerDay = 24 * 60 * 60;
    // 日期分隔.
    static const char kszSepStr[] = " ";

    time_t tLocaltime = tTime;
    // 是否需要添加时区计算今天明天
    if (bAddZone)
    {
        sys_Time_t timer;
        sys_get_systime(&timer);
        tLocaltime += timer.sys_tm.tm_gmtoff;
    }

    // localtime_r线程安全
    time_t tTemp = tTime;
    tm objtmTransit;
    localtime_r(&tTemp, &objtmTransit);
    tm * tmLocal = &objtmTransit;

    // 获取当前时间. // http://10.2.1.199/Bug.php?BugID=46338
    time_t tCurrent = netGetLocalTime();
    time_t tTodayZero = (tCurrent / klSecPerDay) * klSecPerDay;  // 今天0点的时间.
    time_t tTomorrowZero = tTodayZero + klSecPerDay;  // 明天0点的时间.
    time_t tSixDayBeforeZero = tTodayZero - klSecPerDay * 7;  // 7天前的0点时间.

    yl::string strRet;

    //判断是否要显示today和yesterday
    if (bFullWeekDay)
    {
        if (tLocaltime >= tTodayZero
                && tLocaltime < tTomorrowZero)
        {
            // 如果是今天, 则显示为Today.
            strRet = LANG_TRANSLATE(TRID_TODAY).toUtf8().data();
            strRet.append(kszSepStr);
            strRet.append(GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min));
        }
        else if (tLocaltime >= tTodayZero - klSecPerDay
                 && tLocaltime < tTodayZero)
        {
            // 如果是昨天
            strRet = LANG_TRANSLATE(TRID_YESTERDAY).toUtf8().data();
            strRet.append(kszSepStr);
            strRet.append(GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min));
        }
        else if (tLocaltime >= tSixDayBeforeZero
                 && tLocaltime < tTodayZero - klSecPerDay)
        {
            // 如果是7天前的0点到昨天23:59之间, 则直接显示星期.
            strRet = LANG_TRANSLATE(GetWeedayFull(tmLocal->tm_wday)).toUtf8().data();
            strRet.append(kszSepStr);
            strRet.append(GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min));
        }
        else
        {
            // 其他情况都显示年月日.
            strRet = GetDataFormat(tmLocal, bShowComma);
        }
    }
    else
    {
        if (bOnlyDate)
        {
            strRet.append(GetDataFormat(tmLocal, bShowComma));
        }
        else
        {
            DateAndTimeFormatData eDateFmt;
            if (g_pDateAndTimeController->GetDateAndTimeFormat(&eDateFmt) && eDateFmt.m_bDateShowFirst)
            {
                // 日期显示在前面
                strRet = GetDataFormat(tmLocal, bShowComma);
                strRet.append(kszSepStr);
                strRet.append(GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min, bShowMask));
            }
            else
            {
                strRet = GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min, bShowMask);
                strRet.append(kszSepStr);
                strRet.append(GetDataFormat(tmLocal, bShowComma));
            }
        }
    }

    return strRet;
}

yl::string Timet2DetailString(time_t tTime, const yl::string & strSep /*= " "*/)
{
    yl::string strDT;
    // 获取日期格式.
    tm * tmLocal = localtime(&tTime);
    if (tmLocal != NULL)
    {
        strDT.append(GetDataFormat(tmLocal));

        // 获取时间.
        char szBuffer[knMaxBufferLen + 1] = {'\0'};
        int nTFType = Setting_GetCurTimeFormatType();
        switch (nTFType)
        {
        //在时间之后要添加AM/PM
        case TIME_FORMAT_12HOUR:
            //下午
            if (tmLocal->tm_hour > 12)
            {
                sprintf(szBuffer, "%02d:%02d:%02d PM",
                        tmLocal->tm_hour - 12, tmLocal->tm_min, tmLocal->tm_sec);
            }
            //上午
            else if (tmLocal->tm_hour < 12
                     && tmLocal->tm_hour > 0)
            {
                sprintf(szBuffer, "%02d:%02d:%02d AM",
                        tmLocal->tm_hour, tmLocal->tm_min, tmLocal->tm_sec);
            }
            else if (tmLocal->tm_hour == 12)
            {
                //中午12点
                sprintf(szBuffer, "12:%02d:%02d PM",
                        tmLocal->tm_min, tmLocal->tm_sec);
            }
            else if (tmLocal->tm_hour == 0)
            {
                //半夜12点
                sprintf(szBuffer, "12:%02d:%02d AM",
                        tmLocal->tm_min, tmLocal->tm_sec);
            }
            break;

        case TIME_FORMAT_24HOUR:
            sprintf(szBuffer, "%02d:%02d:%02d", tmLocal->tm_hour,
                    tmLocal->tm_min, tmLocal->tm_sec);
            break;

        default:
            break;
        }

        szBuffer[knMaxBufferLen] = '\0';
        strDT.append(strSep);
        strDT.append(szBuffer);
    }

    return strDT;
}

static bool GetCustomDateFmt(tm * tmLocal, yl::string & strDate, bool bChineseStyleDateFmt,
                             DateAndTimeFormatData & eFmtDate)
{
    if (tmLocal == NULL)
    {
        return false;
    }

    yl::string & strDiyFmt = eFmtDate.m_strDiyDateFmt;

    if (strDiyFmt.length() >= 2)
    {
        // 强制大写，以符合当前显示
        strDiyFmt.trim_both();
        strDiyFmt.to_upper();

        LPCSTR lpDateItem = "YMDW";
        LPCSTR lpNone = "";
        strDate.clear();
        // 中文是否显示后缀 如： 2015年10月11日  年月日是后缀
        LPCSTR lpSplit = "-/\\:.";  // ,; 两个字符和空格一样，不作为数字显示
        bool bAutoPostfix = bChineseStyleDateFmt && strDiyFmt.find_first_of(lpSplit) == yl::string::npos;
        int iFmtLenth = strDiyFmt.length();

        for (int i = 0; i < iFmtLenth;)
        {
            bool bVariable = (strchr(lpDateItem, strDiyFmt[i]) != NULL);
            int iCount = 1;

            if (bVariable)
            {
                // 往后预测
                int iBaseIndex = i + 1;
                while (iBaseIndex < iFmtLenth && strDiyFmt[iBaseIndex] == strDiyFmt[i])
                {
                    ++iBaseIndex;
                }

                iCount = iBaseIndex - i;
                yl::string strTmp;

                switch (strDiyFmt[i])
                {
                case 'Y': // Year
                {
                    yl::string strYear;
                    if (bAutoPostfix)
                    {
                        strYear = LANG_TRANSLATE(TRID_YEAR).toUtf8().data();
                    }

                    if (iCount <= 2)
                    {
                        strTmp = commonAPI_FormatString("%02d%s", (tmLocal->tm_year + 1900) % 100, strYear.c_str());
                    }
                    else
                    {
                        strTmp = commonAPI_FormatString("%04d%s", (tmLocal->tm_year + 1900), strYear.c_str());
                    }
                }
                break;
                case 'M': // Month
                {
                    if (iCount <= 2)
                    {
                        strTmp = commonAPI_FormatString("%02d", tmLocal->tm_mon + 1);
                    }
                    else if (iCount == 3)
                    {
                        strTmp = commonAPI_FormatString("%s",
                                                        LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data());
                    }
                    else
                    {
                        strTmp = commonAPI_FormatString("%s",
                                                        LANG_TRANSLATE(GetMonthFull(tmLocal->tm_mon + 1)).toUtf8().data());
                    }
                }
                break;
                case 'D': // Day
                {
                    yl::string strDay;
                    if (bAutoPostfix)
                    {
                        strDay = LANG_TRANSLATE(TRID_CHINESE_DAY_OF_MONTH).toUtf8().data();
                    }

                    strTmp = commonAPI_FormatString(bChineseStyleDateFmt ? "%d%s" : "%02d%s", tmLocal->tm_mday,
                                                    strDay.c_str());
                }
                break;
                case 'W': // Week
                {
                    yl::string strWeek = iCount <= 2 ? GetWeeday(tmLocal->tm_wday) : GetWeedayFull(tmLocal->tm_wday);
                    strTmp = LANG_TRANSLATE(strWeek.c_str()).toUtf8().data();
                }
                break;
                default:
                    break;
                }

                if (!strTmp.empty())
                {
                    strDate += strTmp;
                }
            }
            else
            {
                strDate += strDiyFmt[i];
            }

            i += iCount;
        }

        return !strDate.empty();
    }

    return false;
}

yl::string GetDataFormat(tm * tmLocal, bool bShowComma/* = false*/)
{
    /*time_t tTemp = netGetLocalTime();
    tm* tmLocal = localtime(&tTemp);*/

    LPCSTR lpDefault = "XXXX-XX-XX XX:XX:XX";
    if (tmLocal == NULL)
    {
        return lpDefault;
    }

    DateAndTimeFormatData eDateFmt;
    if (!g_pDateAndTimeController->GetDateAndTimeFormat(&eDateFmt))
    {
        return yl::string();
    }

    bool bChineseStyleDateFmt = Lang_IsChineseStyleDate();
    yl::string strCurrentLang = configParser_GetConfigString(kszCurrentLang);
    bool bShouldShowComma = (bShowComma && "English" == strCurrentLang && !bChineseStyleDateFmt);

    yl::string strDate;

    if (GetCustomDateFmt(tmLocal, strDate, bChineseStyleDateFmt, eDateFmt))
    {
        return strDate;
    }

    yl::string strYear, strDay;
    if (bChineseStyleDateFmt)
    {
        strYear = LANG_TRANSLATE(TRID_YEAR).toUtf8().data();
        strDay = LANG_TRANSLATE(TRID_CHINESE_DAY_OF_MONTH).toUtf8().data();
    }

    switch (eDateFmt.m_eDateFormat)
    {
    case DATE_FORMAT_WWW_MMM_DD:  // WWW MMM DD.
    {
        const char * pszDataFormat = bShouldShowComma ? "%s, %s %02d%s" : "%s %s %02d%s";
        return commonAPI_FormatString(pszDataFormat,
                                      LANG_TRANSLATE(GetWeeday(tmLocal->tm_wday)).toUtf8().data(),
                                      LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data(), tmLocal->tm_mday, strDay.c_str());
    }
    case DATE_FORMAT_DD_MMM_YY:  // DD-MM-YY.
    {
        return commonAPI_FormatString("%02d%s-%s-%02d%s", tmLocal->tm_mday, strDay.c_str(),
                                      LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data(), (tmLocal->tm_year + 1900) % 100,
                                      strYear.c_str());
    }
    break;
    case DATE_FORMAT_YYYY_MM_DD:  // // YYYY-MM-DD
    {
        return commonAPI_FormatString("%d-%02d-%02d", tmLocal->tm_year + 1900,
                                      tmLocal->tm_mon + 1, tmLocal->tm_mday);
    }
    case DATE_FORMAT_DD_MM_YYYY:  // DD/MM/YYYY.
    {
        return commonAPI_FormatString("%02d/%02d/%d", tmLocal->tm_mday,
                                      tmLocal->tm_mon + 1, tmLocal->tm_year + 1900);
    }
    case DATE_FORMAT_MM_DD_YY:  // MM/DD/YY.
    {
        return commonAPI_FormatString("%02d/%02d/%02d", tmLocal->tm_mon + 1,
                                      tmLocal->tm_mday, (tmLocal->tm_year + 1900) % 100);
    }
    case DATE_FORMAT_DD_MMM_YYYY:  // DD MMM YYYY.
    {
        const char * pszDataFormat = bShouldShowComma ? "%02d%s %s, %d%s" : "%02d%s %s %d%s";
        return commonAPI_FormatString(pszDataFormat, tmLocal->tm_mday, strDay.c_str(),
                                      LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data(), tmLocal->tm_year + 1900,
                                      strYear.c_str());
    }
    case DATE_FORMAT_WWW_DD_MMM:  // WWW DD MMM.
    {
        const char * pszDataFormat = bShouldShowComma ? "%s, %02d%s %s" : "%s %02d%s %s";
        return commonAPI_FormatString(pszDataFormat,
                                      LANG_TRANSLATE(GetWeeday(tmLocal->tm_wday)).toUtf8().data(),
                                      tmLocal->tm_mday, strDay.c_str(), LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data());
    }
    case DATE_FORMAT_MM_DD_WWW_SPACE:  // MM DD WWW.
    {
        return commonAPI_FormatString("%s%d%s %s",
                                      LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data(),
                                      tmLocal->tm_mday, strDay.c_str(), LANG_TRANSLATE(GetWeedayFull(tmLocal->tm_wday)).toUtf8().data());
    }
    case DATE_FORMAT_MM_DD_WW_SPACE: // MM DD WW
    {
        return commonAPI_FormatString("%s%d%s %s",
                                      LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data(),
                                      tmLocal->tm_mday, strDay.c_str(), LANG_TRANSLATE(GetWeeday(tmLocal->tm_wday)).toUtf8().data());
    }
    case DATE_FORMAT_YY_MM_DD_HYPHEN:  // YY-MM-DD
    {
        return commonAPI_FormatString("%02d-%02d-%02d", (tmLocal->tm_year + 1900) % 100,
                                      tmLocal->tm_mon + 1, tmLocal->tm_mday);
    }
    case DATE_FORMAT_YY_MM_DD_VIRGULE:  // YY/MM/DD
    {
        return commonAPI_FormatString("%02d/%02d/%02d", (tmLocal->tm_year + 1900) % 100,
                                      tmLocal->tm_mon + 1, tmLocal->tm_mday);
    }
    case DATE_FORMAT_YYYY_MM_DD_VIRGULE:  // YYYY/MM/DD
    {
        return commonAPI_FormatString("%02d/%02d/%02d", tmLocal->tm_year + 1900,
                                      tmLocal->tm_mon + 1, tmLocal->tm_mday);
    }
    case DATE_FORMAT_YYYY_MM_DD_SPACE:  // YYYY MM DD
    {
        return commonAPI_FormatString("%d%s%s%d%s", tmLocal->tm_year + 1900, strYear.c_str(),
                                      LANG_TRANSLATE(GetMonth(tmLocal->tm_mon + 1)).toUtf8().data(), tmLocal->tm_mday, strDay.c_str());
    }
    default:
        return lpDefault;
    }

    return lpDefault;
}

int GetRTLMaskType(QString & strText)
{
    QRegExp expr("\\d+[.]{1}.*:$");
    QRegExp exprless(".*:$");
    if (expr.exactMatch(strText))
    {
        return HB_WITH_INDEX;
    }
    else if (exprless.exactMatch(strText))
    {
        return HB_NO_INDEX;
    }

    /* 该段代码用于将客户提供的翻译进行纠错处理，如客户提供类似 "More..."="עוד..." （按照客户习惯，省略号应该放在左边）
    将其转化成"...עוד"，虽然后者更加符合客户语言书写习惯，但是翻译应该尊重客户的选择。
    客户提供什么样的翻译，话机就应该显示成什么样子，话机不做过多处理，避免机型间的差异 */

#if 0 // def IF_FEATURE_ARABIC
    static QRegExp exp_dot_tag("\\d+[.]{1}.*\\.\\.\\.$");
    static QRegExp exp_tag("*\\.\\.\\.$");
    if (exp_dot_tag.exactMatch(strText))
    {
        return HB_INDEX_TAG;
    }
    else if (exp_tag.exactMatch(strText))
    {
        return HB_ONLY_TAG;
    }
#endif

    return HB_INVALID;
}

void ReverseRTLMaskString(QString & strText)
{
    int iFormat = GetRTLMaskType(strText);

    if (iFormat == 0)
    {
        return;
    }

    yl::wstring strSrcW = yl::to_utf16(fromQString(strText).c_str());
    if (!Lang_IsRTL(strSrcW))
    {
        return;
    }

    if (iFormat > 0)
    {
        //希伯来语把"1.xxxx:" 或 “xxxx:” 转换成"1.:xxxx"
        int iLen = strSrcW.length();
        if (strSrcW[iLen - 1] == ':')
        {
            if (iFormat == HB_WITH_INDEX)
            {
                int iPos = 0;
                for (; iPos < iLen - 1; iPos++)
                {
                    if (strSrcW[iPos] == '.')
                    {
                        for (int i = iLen - 1; i > iPos + 1; i--)
                        {
                            strSrcW[i] = strSrcW[i - 1];
                        }
#ifdef IF_FEATURE_ARABIC
                        // 1. xxxx: 应该转成 1. :xxxx 而不是 1.: xxxx(注意空格位置)
                        if (strSrcW[iPos + 1] == L' ' && iPos + 2 < iLen)
                        {
                            strSrcW[iPos + 2] = ':';
                            strSrcW[iPos + 1] = ' ';
                        }
                        else
#endif
                        {
                            strSrcW[iPos + 1] = ':';
                        }
                        break;
                    }
                }
            }
            else if (iFormat == HB_NO_INDEX)
            {
                for (int i = iLen - 1; i > 0; i--)
                {
                    strSrcW[i] = strSrcW[i - 1];
                }
                strSrcW[0] = ':';
            }
        }
#if 0 //def IF_FEATURE_ARABIC                       
        else if (iLen > 5 && iFormat == HB_INDEX_TAG)
        {
            // 4. more... => 4. ...more
            int iPos = strSrcW.find_first_of(L'.');
            if (iPos != yl::wstring::npos && iPos <= iLen - 4)
            {
                yl::wstring strPrefix = strSrcW.substr(0, iPos + 1);

                bool bSikpNext = strSrcW[iPos + 1] == L' ' && iPos <= iLen - 5;
                yl::wstring strSuffer = strSrcW.substr(iPos + (bSikpNext ? 2 : 1));

                // 为了区分，添加一个空格，防止四个连续的"."
                strSrcW = strPrefix + L" " + yl::wstring(L"...") + strSuffer.substr(0, strSuffer.length() - 3);
            }
        }
        else if (iLen > 3 && iFormat == HB_ONLY_TAG)
        {
            // updateing... => ...updateing
            strSrcW = yl::wstring(L"...") + strSrcW.substr(0, iLen - 3);
        }
#endif
    }

    strText = toQString(yl::to_utf8(strSrcW));
}

// 希伯来语 从右到左倒转
void ReverseRTLWord(yl::wstring & strSrcW)
{
#ifdef IF_FEATURE_ARABIC
    return;
#else
    if (!Lang_IsRTL(strSrcW))
    {
        return;
    }

    //如果字符串的前几位是表示序号，如 1. 2. ，则序号不翻转
    int i = 0;
    BOOL bHeadDigitChar = FALSE;
    BOOL bFindIt = FALSE;
    while (strSrcW[i] != L'\0')
    {
        if (!bHeadDigitChar)
        {
            if (iswdigit(strSrcW[i]))
            {
                bHeadDigitChar = TRUE;
            }
            else if (strSrcW[i] != L' ')
            {
                // 如果起始字符不是数字也不是空格，则退出
                break;
            }
        }
        else //如果已经有数字开头
        {
            if (!iswdigit(strSrcW[i]) && strSrcW[i] != L'.')
            {
                break;
            }
            else if (strSrcW[i] == L'.')
            {
                bFindIt = TRUE;
                break;
            }
        }

        i++;
    }

    yl::wstring strPrefixW;
    yl::wstring strContentW;

    if (bFindIt)
    {
        if (strSrcW[i + 1] == L' ')
        {
            i++;
        }

        strPrefixW = strSrcW.substr(0, i + 1);
        strContentW = strSrcW.substr(i + 1, strSrcW.length() - i - 1);
    }
    else
    {
        strPrefixW;
        strContentW = strSrcW;
    }

    //////////////////
    int iLength = strContentW.length();
    ReverseString((WCHAR *)strContentW.c_str(), iLength);

    int iStart = -1;
    for (int i = 0; i < iLength; i++)
    {
        // 空格不参与倒置如 “希1 希2 空格 a b”
        // 第一次全部Reverse之后为“b a 空格 希2 希1”
        // 再对非希伯来语倒置一次 "a b 空格 希2 希1"(此处的空格不能和a/b一起倒置)
        if (Lang_IsRTL(strContentW[i]) || iswspace(strContentW[i]))
        {
            if (iStart != -1)
            {
                //连续的LTR字符串结束，则按开始位置到结束位置的长度进行倒置
                ReverseString((WCHAR *)(strContentW.c_str() + iStart), i - iStart);
                iStart = -1;
            }
        }
        else
        {
            if (iStart == -1)
            {
                //记录LTR字符串开始的位置
                iStart = i;
            }
            else if (i == iLength - 1)
            {
                //如果已到字符串结尾，且最后一个字符是LTR，且前面有连续的LTR字符
                ReverseString((WCHAR *)(strContentW.c_str() + iStart), iLength - iStart);
                iStart = -1;
            }
        }
    }


    iStart = -1;
    for (int i = 0; i < iLength; i++)
    {
        if (!Lang_IsRTL(strContentW[i]))
        {
            if (iStart != -1)
            {
                //连续的RTL字符串结束，则按开始位置到结束位置的长度进行倒置
                ReverseString((WCHAR *)(strContentW.c_str() + iStart), i - iStart);
                iStart = -1;
            }
        }
        else
        {
            if (iStart == -1)
            {
                //记录LTR字符串开始的位置
                iStart = i;
            }
            else if (i == iLength - 1)
            {
                //如果已到字符串结尾，且最后一个字符是RTL，且前面有连续的RTL字符
                ReverseString((WCHAR *)(strContentW.c_str() + iStart), iLength - iStart);
                iStart = -1;
            }
        }
    }

    strSrcW = strPrefixW + strContentW;
#endif
}

void RTL_GetRealString(QString & strRTL)
{
#ifdef IF_FEATURE_ARABIC
    ReverseRTLMaskString(strRTL);
#else
    if (!Lang_IsRTL(strRTL.toUtf8().data()))
    {
        return;
    }

    //因为希伯来语是逆序，所以对时间字符串进行特殊处理
    //http://192.168.1.99/Bug.php?BugID=19801
    bool bNeedReverse = true;
    int nSubIndex = 0;

    //判断是否是时间字符串
    //字符串最后两个字符是AM或PM就是时间字符串，且为12小时制。
    if (strRTL.mid(6, 2) == "PM" || strRTL.mid(6, 2) == "AM")
    {
        nSubIndex = 8;
    }
    else if (strRTL.left(5).indexOf(":") == (strRTL.left(5).length() - 3)
             || strRTL.left(5).indexOf(" ") == (strRTL.left(5).length() - 3))
    {
        //对字符串后五个字符进行判断，如果这五个字符串符合xx:xx 或者xx xx， 为24小时制的时间字符串
        bool bOk = false;

        strRTL.left(2).toInt(&bOk);

        if (bOk)
        {
            strRTL.mid(4, 2).toInt(&bOk);

            if (bOk)
            {
                nSubIndex = 5;
            }
        }
    }

    if (0 != nSubIndex)
    {
        // 时间格式，不需要手动转换，转换之后反而是错误的  http://192.168.1.99/Bug.php?BugID=27776
        QString first = strRTL.mid(0, nSubIndex + 1);
        QString second = strRTL.mid(nSubIndex + 1);
        yl::wstring chwText = yl::to_utf16(fromQString(second).c_str());

        ReverseRTLWord(chwText);
        second = toQString(yl::to_utf8(chwText));
        strRTL = first + second;
        bNeedReverse = false;
    }

    if (bNeedReverse)
    {
        ReverseRTLMaskString(strRTL);
    }
#endif
}

///////////////////////////////////////

