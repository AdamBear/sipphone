#include "baseuicommon.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "regexp/modregexp.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "devicelib/networkserver.h"
#include "ylstl/ylstringalgorithm.h"
#include "dsklog/log.h"
#include "settinguilogic/include/common.h"
#include "xwindow/xWindowManager.h"
#include "language/common/common_function.h"
#include "language/input/include/modInput.h"
#include "settinguilogic/include/modsetuilogic.h"


const char * BASE_UI_LAYOUT_FILE_OPTION_MENU = "optionmenu.xml";
const char * BASE_UI_LAYOUT_FILE_DETAIL_LIST_ITEM = "detailitem.xml";
const char * BASE_UI_LAYOUT_FILE_EDIT_LIST_ITEM = "edititem.xml";
const char* BASE_UI_LAYOUT_FILE_TWO_LINE_EDIT_LIST_ITEM = "twolineedititem.xml";
const char * BASE_UI_LAYOUT_FILE_COMBOBOX_LIST_ITEM = "comboboxitem.xml";
const char * BASE_UI_LAYOUT_FILE_MENU_LIST_ITEM = "menuitem.xml";
const char * BASE_UI_LAYOUT_FILE_RADIO_LIST_ITEM = "radioitem.xml";
const char * BASE_UI_LAYOUT_FILE_SELECT_LIST_ITEM = "selectitem.xml";
const char * BASE_UI_LAYOUT_FILE_TEXTSTATUS_LIST_ITEM = "textstatusitem.xml";

const char * BASE_UI_ID_TEXT_TITLE = "txtTitle";
const char * BASE_UI_ID_TEXT_VALUE = "txtValue";
const char * BASE_UI_ID_EDIT_VALUE = "editValue";
const char * BASE_UI_ID_COMBO_VALUE = "comboValue";
const char * BASE_UI_ID_TEXT_MENU = "txtMenu";
const char * BASE_UI_ID_ICON_RADIO = "iconRadio";
const char * BASE_UI_ID_ICON_CHECK = "iconCheck";
const char * BASE_UI_ID_LIST_VIEW = "listView";
const char* BASE_UI_ID_BTN_RADIO = "btnRadio";

const char * BASE_UI_ATTR_TIME_SIZE = "android:timeSize";
const char * BASE_UI_ATTR_DATE_SIZE = "android:dateSize";
const char * BASE_UI_ATTR_LINE_SPACING = "android:lineSpacing";
const char * BASE_UI_ATTR_DATE_TIME_FORMAT = "android:dateTimeFormat";  // 时间日期显示格式
const char * BASE_UI_ATTR_DATE_FORMAT = "android:dateFormat";               // 日期显示格式
const char * BASE_UI_ATTR_TIME_FORMAT =
    "android:timeFormat";               // 时间日期显示格式
const char * BASE_UI_ATTR_FORMAT = "android:format";

// 时间日期显示格式
const char * BASE_UI_ENUM_DATE_TIME_FORMAT_TIME_DATE = "timeDate";              // Time Date
const char * BASE_UI_ENUM_DATE_TIME_FORMAT_TWO_LINE =
    "towLine";                // 两行，第一行时间，第二行日期，字体不一样
const char * BASE_UI_ENUM_DATE_TIME_FORMAT_LEFT_DATE_MID_TIME =
    "dateTime";     // Date      Time       (right)
const char* BASE_UI_ENUM_DATE_TIME_FORMAT_TIME = "time";     // Time
// 日期显示格式
const char * BASE_UI_ENUM_DATE_FORMAT_WWW_MMM_DD = "WWW MMM DD";    // WWW MMM DD
const char * BASE_UI_ENUM_DATE_FORMAT_DD_MMM_YY = "DD-MM-YY";           // DD-MM-YY
const char * BASE_UI_ENUM_DATE_FORMAT_YYYY_MM_DD = "YYYY-MM-DD";    // YYYY-MM-DD
const char * BASE_UI_ENUM_DATE_FORMAT_DD_MM_YYYY = "DD/MM/YYYY";    // DD/MM/YYYY
const char * BASE_UI_ENUM_DATE_FORMAT_MM_DD_YY = "MM/DD/YY";        // MM/DD/YY
const char * BASE_UI_ENUM_DATE_FORMAT_DD_MMM_YYYY = "DD MM YYYY";       // DD MM YYYY
const char * BASE_UI_ENUM_DATE_FORMAT_WWW_DD_MMM = "WWW DD MMM";    // WWW DD MMM
const char * BASE_UI_ENUM_DATE_FORMAT_T20_DD_MM_YY = "DD/MM/YY";    //显示格式： DD/MM/YY
const char * BASE_UI_ENUM_DATE_FORMAT_T20_YY_MM_DD = "YY/MM/DD";    //显示格式： YY/MM/DD
// 时间显示格式
const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM_APM = "HH:MM A/PM";     // HH:MM A/PM
const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM = "HH:MM";              // HH:MM
const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM_SS = "HH:MM:SS";        // HH:MM:SS
const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM_SMALL_SS = "HH:MM ss";  // HH:MM ss

// 最大的buffer长度.
#define knMaxBufferLen 260

#define SUN     TRID_SUNDAY_SHORT
#define MON     TRID_MONDAY_SHORT
#define TUE     TRID_TUESDAY_SHORT
#define WED     TRID_WEDNESDAY_SHORT
#define THU     TRID_TURSDAY_SHORT
#define FRI     TRID_FRIDAY_SHORT
#define SAT     TRID_SATURDAY_SHORT

#define SUNFULL TRID_SUNDAY
#define MONFULL TRID_MONDAY
#define TUEFULL TRID_TUESDAY
#define WEDFULL TRID_WEDNESDAY
#define THUFULL TRID_TURSDAY
#define FRIFULL TRID_FRIDAY
#define SATFULL TRID_SATURDAY

#define JAN     TRID_JANUARY_SHORT
#define FEB     TRID_FEBRUARY_SHORT
#define MAR     TRID_MARCH_SHORT
#define APR     TRID_APRIL_SHORT
#define MAY     TRID_MAY
#define JUN     TRID_JUNE_SHORT
#define JUL     TRID_JULY_SHORT
#define AUG     TRID_AUGUST_SHORT
#define SEP     TRID_SEPTEMBER_SHORT
#define OCT     TRID_OCTOBER_SHORT
#define NOV     TRID_NOVEBER_SHORT
#define DEC     TRID_DECEMBER_SHORT

enum HEBREW_MASK_TYPE
{
    HB_INVALID = 0,
    HB_WITH_INDEX,
    HB_NO_INDEX
};

static yl::string s_week[] =
{
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

static yl::string s_month[] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};

#define SUB_PATH_T19        "t19/"
#define SUB_PATH_T21        "t21/"
#define SUB_PATH_T23        "t23/"
#define SUB_PATH_T27        "t27/"
#define SUB_PATH_T40        "t40/"
#define SUB_PATH_T42        "t42/"
#define SUB_PATH_BLACK      "black/"
#define SUB_PATH_CP920      "cp920/"

#define SUB_PATH_PAGE      "page/"

#define SUB_PATH_T46        "t46/"
#define SUB_PATH_T29        "t29/"
#define SUB_PATH_T52        "t52/"
#define SUB_PATH_T54        "t54/"
#define SUB_PATH_COLOR      "t4x/"

void dskPhonePrintTime(const char * lpszFmt, ...)
{
#ifndef _WIN32
    struct timeval tData;
    gettimeofday(&tData, NULL);
    ulong ulCostTimes = tData.tv_sec * 1000 + tData.tv_usec / 1000;
    va_list argList;
    va_start(argList, lpszFmt);
    fprintf(stdout, "[Time:(%.8u)] ", ulCostTimes);
    vfprintf(stdout, lpszFmt, argList);
    fprintf(stdout, "\n");
    va_end(argList);
#else
    va_list argList;
    va_start(argList, lpszFmt);
    vfprintf(stdout, lpszFmt, argList);
    fprintf(stdout, "\n");
    va_end(argList);
#endif
}

static yl::string g_strTranTestText = "";

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

int CountMaxCharInCtrl(const xFont & objFontMetrics, int iTextAreaWidth)
{
    InitTranTestText();
    //计算能容纳多少个'a'
    yl::string strText = g_strTranTestText;

    // 显示宽度小于末端符号返回原字符
    if (iTextAreaWidth <= 0)
    {
        return 0;
    }

    int iCount = 1;
    // 循环确定截断的范围
    while (iTextAreaWidth >= objFontMetrics.textBound(strText.substr(0, iCount)).cx)
    {
        ++iCount;
    }
    iCount--;

    return iCount;
}

yl::string Timet2String(time_t tTime, const yl::string & strSep /*= " "*/,
                        bool bFullWeekDay/* = true*/, bool bShowMask/* = false*/,
                        bool bShowComma/* = true*/)
{
    // 最大的buffer长度.
    static const long klSecPerDay = 24 * 60 * 60;
    // 日期分隔.
    static const char kszSepStr[] = " ";

    time_t tTemp = tTime;
    tm * ptmTransit = localtime(&tTemp);
    if (ptmTransit == NULL)
    {
        printf("SettingUILogic_CallLogTime2String tmLocal == NULL\n");
        return "XXXX-XX-XX XX:XX:XX";
    }
    tm objtmTransit = *ptmTransit;
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
        if (tTime >= tTodayZero
                && tTime < tTomorrowZero)
        {
            // 如果是今天, 则显示为Today.
            strRet = _LANG(TRID_TODAY);
            strRet.append(kszSepStr);
            strRet.append(SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min));
        }
        else if (tTime >= tTodayZero - klSecPerDay
                 && tTime < tTodayZero)
        {
            // 如果是昨天
            strRet = _LANG(TRID_YESTERDAY);
            strRet.append(kszSepStr);
            strRet.append(SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min));
        }
        else if (tTime >= tSixDayBeforeZero
                 && tTime < tTodayZero - klSecPerDay)
        {
            // 如果是7天前的0点到昨天23:59之间, 则直接显示星期.
            strRet = _LANG(GetWeedayFull(tmLocal->tm_wday));
            strRet.append(kszSepStr);
            strRet.append(SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min));
        }
        else
        {
            // 其他情况都显示年月日.
            strRet = SettingUILogic_GetDateFormat(tmLocal, bShowComma);
        }
    }
    else
    {
        strRet = SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min, -1, bShowMask);
        strRet.append(kszSepStr);
        strRet.append(SettingUILogic_GetDateFormat(tmLocal, bShowComma));
    }

    return strRet;
}

yl::string Timet2DetailString(time_t tTime, const yl::string & strSep /*= " "*/, bool bShowComma/* = true*/)
{
    yl::string strDT;
    // 获取日期格式.
    tm * tmLocal = localtime(&tTime);
    if (tmLocal != NULL)
    {
        strDT.append(SettingUILogic_GetDateFormat(tmLocal, bShowComma));

        // 获取时间.
        char szBuffer[knMaxBufferLen + 1] = {'\0'};
        int nTFType = configParser_GetConfigInt(kszTimeFormatType);
        switch (nTFType)
        {
        //在时间之后要添加AM/PM
        case TIME_FORMAT_12HOUR:
            if (tmLocal->tm_hour > 12)                              //下午
            {
                sprintf(szBuffer, "%02d:%02d:%02d PM", tmLocal->tm_hour - 12, tmLocal->tm_min, tmLocal->tm_sec);
            }
            else if (tmLocal->tm_hour < 12 && tmLocal->tm_hour > 0) //上午
            {
                sprintf(szBuffer, "%02d:%02d:%02d AM", tmLocal->tm_hour, tmLocal->tm_min, tmLocal->tm_sec);
            }
            else if (12 == tmLocal->tm_hour)                        //中午12点
            {
                sprintf(szBuffer, "12:%02d:%02d PM", tmLocal->tm_min, tmLocal->tm_sec);
            }
            else if (0 == tmLocal->tm_hour)                         //半夜12点
            {
                sprintf(szBuffer, "12:%02d:%02d AM", tmLocal->tm_min, tmLocal->tm_sec);
            }
            break;

        case TIME_FORMAT_24HOUR:
            {
                sprintf(szBuffer, "%02d:%02d:%02d", tmLocal->tm_hour, tmLocal->tm_min, tmLocal->tm_sec);
            }
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

yl::string ClockTime2String(yl::string & strHour, yl::string & strMin, yl::string & strSec)
{
    time_t tTemp = netGetLocalTime();
    tm * ptmTransit = localtime(&tTemp);
    if (NULL == ptmTransit)
    {
        BASEUI_WARN("[xDateTimeView::Time2String] tmLocal == NULL");
        return "XXXX-XX-XX XX:XX:XX";
    }
    tm objtmTransit = *ptmTransit;
    tm * tmLocal = &objtmTransit;
    if (NULL == tmLocal)
    {
        return "";
    }

    yl::string strTime("");
    yl::string strAPM;  // 上下午标识, 可能为空.
    int nHour = tmLocal->tm_hour;
    int nHourGet = nHour;
    int nMin = tmLocal->tm_min;
    int nSecond = tmLocal->tm_sec;

    // 根据setting的格式, 返回时分.
    char szBuffer[knMaxBufferLen + 1] = {'\0'};
    int nTFType = configParser_GetConfigInt(kszTimeFormatType);
    switch (nTFType)
    {
    case TIME_FORMAT_12HOUR:
        if (nHour > 12)                     //下午
        {
            nHourGet = nHour - 12;
            strAPM = _LANG(TRID_PM);
        }
        else if (nHour < 12 && nHour > 0)   //上午
        {
            nHourGet = nHour;
            strAPM = _LANG(TRID_AM);
        }
        else if (nHour == 12)               //中午12点
        {
            nHourGet = 12;
            strAPM = _LANG(TRID_PM);
        }
        else if (nHour == 0)                //半夜12点
        {
            nHourGet = 12;
            strAPM = _LANG(TRID_AM);
        }
        break;
    default:
        nHourGet = nHour;
        break;
    }

    sprintf(szBuffer, "%02d", nHourGet);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    strHour = szBuffer;
    strTime += strHour;
    strTime += ':';

    sprintf(szBuffer, "%02d", nMin);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    strMin = szBuffer;
    strTime += strMin;

    strSec = strAPM;
    if (strAPM.empty())
    {
        sprintf(szBuffer, "%02d", nSecond);
        szBuffer[sizeof(szBuffer) - 1] = '\0';

        strSec = szBuffer;
        strTime += ':';
        strTime += strSec;
    }
    else
    {
        strTime += ' ';
        strTime += strAPM;
    }

    return strTime;
}

yl::string Date2String()
{
    time_t tTemp = netGetLocalTime();
    tm * ptmTransit = localtime(&tTemp);
    if (NULL == ptmTransit)
    {
        BASEUI_WARN("[xDateView::Date2String] tmLocal == NULL");
        return "XXXX-XX-XX XX:XX:XX";
    }
    tm objtmTransit = *ptmTransit;
    tm * tmLocal = &objtmTransit;
    if (NULL == tmLocal)
    {
        return "";
    }

    return SettingUILogic_GetDateFormat(tmLocal);
}

bool RTL_GetRealString(const yl::string& strText, yl::string& strShowText)
{
    yl::wstring wstrText = yl::to_utf16(strText);

    Lang_Format(wstrText);

    strShowText = yl::to_utf8(wstrText);

    return true;
}

void PaintRoundFrame(xPainter & p, const chRect & rcFrame, const xColor & clrBG)
{
    xColor clrFrame = T2X_SYSTEM_COLOR_BLACK;
    int nWidth = rcFrame.Width();
    int nHeight = rcFrame.Height();

    if (nWidth < T2X_ROUND_FRAME_PIXEL || nHeight < T2X_ROUND_FRAME_PIXEL)
    {
        p.fillRect(rcFrame, clrBG);
        p.drawRect(rcFrame, clrFrame);

        return;
    }

    chRect rc = rcFrame;

    rc.right -= 1;
    rc.bottom -= 1;

    int nCornerLeft = rc.left + T2X_ROUND_FRAME_PIXEL;
    int nCornerRight = rc.right - T2X_ROUND_FRAME_PIXEL;
    int nCornerTop = rc.top + T2X_ROUND_FRAME_PIXEL;
    int nCornerBottom = rc.bottom - T2X_ROUND_FRAME_PIXEL;

    // 绘制4条边
    p.drawLineX(rc.top, nCornerLeft, nCornerRight + 1, clrFrame);
    p.drawLineY(rc.right, nCornerTop, nCornerBottom + 1, clrFrame);
    p.drawLineX(rc.bottom, nCornerLeft, nCornerRight + 1, clrFrame);
    p.drawLineY(rc.left, nCornerTop, nCornerBottom + 1, clrFrame);

    int arrRoundLeft[T2X_ROUND_FRAME_PIXEL] = {nCornerLeft - 2, nCornerLeft - 1,
                                               nCornerLeft - 3, nCornerLeft - 4,
                                               nCornerLeft - 4
                                              };
    int arrRoundRight[T2X_ROUND_FRAME_PIXEL] = {nCornerRight + 2, nCornerRight + 1,
                                                nCornerRight + 3, nCornerRight + 4,
                                                nCornerRight + 4
                                               };
    int arrRoundTop[T2X_ROUND_FRAME_PIXEL] = {rc.top + 1, rc.top + 1, rc.top + 2,
                                              rc.top + 3, rc.top + 4
                                             };
    int arrRoundBottom[T2X_ROUND_FRAME_PIXEL] = {rc.bottom - 1, rc.bottom - 1,
                                                 rc.bottom - 2, rc.bottom - 3,
                                                 rc.bottom - 4
                                                };

    // 绘制顶部
    for (int i = 0; i < T2X_ROUND_FRAME_PIXEL; ++i)
    {
        int nRoundTop = arrRoundTop[i];
        int nRoundLeft = arrRoundLeft[i];
        int nRoundRight = arrRoundRight[i];

        p.drawPoint(nRoundLeft, nRoundTop, clrFrame);
        p.drawPoint(nRoundRight, nRoundTop, clrFrame);
        p.drawLineX(nRoundTop, nRoundLeft + 1, nRoundRight - 1, clrBG);

        int nRoundBottom = arrRoundBottom[i];

        p.drawPoint(nRoundLeft, nRoundBottom, clrFrame);
        p.drawPoint(nRoundRight, nRoundBottom, clrFrame);
        p.drawLineX(nRoundBottom, nRoundLeft + 1, nRoundRight - 1, clrBG);
    }

    // 绘制中间
    chRect rcBG(rc.left + 1, nCornerTop, rc.right, nCornerBottom + 1);

    p.fillRect(rcBG, clrBG);
}

yl::string getScreenPath(PHONE_TYPE_DEF ePhoneType);
yl::string getLayoutPath(PHONE_TYPE_DEF ePhoneType);
yl::string getKeymapPath(PHONE_TYPE_DEF ePhoneType);
yl::string getPicsPath(PHONE_TYPE_DEF ePhoneType);
yl::string getFontPath(PHONE_TYPE_DEF ePhoneType);
yl::string getMenuPath(PHONE_TYPE_DEF ePhoneType);

yl::string getRealPathByPhoneType(yl::string strRootPath, yl::string strFileName, RESPATHTYPE eType)
{
    yl::string strResult = strRootPath;

    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

    switch (eType)
    {
    case RESPATHTYPE_SCREEN:
        strResult += getScreenPath(ePhoneType);
        break;
    case RESPATHTYPE_LAYOUT:
        strResult += getLayoutPath(ePhoneType);
        break;
    case RESPATHTYPE_KEYMAP:
        strResult += getKeymapPath(ePhoneType);
        break;
    case RESPATHTYPE_PICS:
        strResult += getPicsPath(ePhoneType);
        break;
    case RESPATHTYPE_FONT:
        strResult += getFontPath(ePhoneType);
        break;
    case RESPATHTYPE_MENU:
        strResult += getMenuPath(ePhoneType);
        break;
    default:
        break;
    }

    if (!strFileName.empty())
    {
        strResult += strFileName;
    }

    return strResult;
}

yl::string getScreenPath(PHONE_TYPE_DEF ePhoneType)
{
    yl::string strResult = "";

    switch (ePhoneType)
    {
#if !IS_COLOR
    case PT_T19:
    case PT_T21:
    case PT_T23:
    case PT_T40:
    case PT_T40G:
    case PT_T27:
    case PT_T27S:
    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        strResult = SUB_PATH_BLACK;
        break;

    case PT_CP920:
        strResult = SUB_PATH_CP920;
        break;

#else
    case PT_T46:
    case PT_T46S:
        strResult = SUB_PATH_T46;
        break;

    case PT_T29:
        strResult = SUB_PATH_T29;
        break;

    case PT_T52:
        strResult = SUB_PATH_T52;
        break;

    case PT_T54S:
        strResult = SUB_PATH_T54;
        break;
#endif
    default:
        break;
    }

    return strResult;
}

yl::string getLayoutPath(PHONE_TYPE_DEF ePhoneType)
{
    yl::string strResult = "";

    switch (ePhoneType)
    {
#if !IS_COLOR
    case PT_T19:
        strResult = SUB_PATH_T19;
        break;

    case PT_T21:
        strResult = SUB_PATH_T21;
        break;

    case PT_T23:
    case PT_T40:
    case PT_T40G:
        strResult = SUB_PATH_T23;
        break;

    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        strResult = SUB_PATH_T42;
        break;

    case PT_T27:
    case PT_T27S:
        strResult = SUB_PATH_T27;
        break;

    case PT_CP920:
        strResult = SUB_PATH_CP920;
        break;

#else
    case PT_T46:
    case PT_T46S:
        strResult = SUB_PATH_T46;
        break;

    case PT_T29:
        strResult = SUB_PATH_T29;
        break;

    case PT_T52:
        strResult = SUB_PATH_T52;
        break;

    case PT_T54S:
        strResult = SUB_PATH_T54;
        break;
#endif
    default:
        break;
    }

    return strResult;
}

yl::string getKeymapPath(PHONE_TYPE_DEF ePhoneType)
{
    yl::string strResult = "";

    switch (ePhoneType)
    {
#if !IS_COLOR
    case PT_T19:
        strResult = SUB_PATH_T19;
        break;

    case PT_T21:
        strResult = SUB_PATH_T21;
        break;

    case PT_T23:
        strResult = SUB_PATH_T23;
        break;

    case PT_T40:
    case PT_T40G:
        strResult = SUB_PATH_T40;
        break;

    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        strResult = SUB_PATH_T42;
        break;

    case PT_T27:
    case PT_T27S:
        strResult = SUB_PATH_T27;
        break;

    case PT_CP920:
        strResult = SUB_PATH_CP920;
        break;

#else
    case PT_T46:
    case PT_T46S:
        strResult = SUB_PATH_T46;
        break;

    case PT_T29:
        strResult = SUB_PATH_T29;
        break;

    case PT_T52:
        strResult = SUB_PATH_T52;
        break;

    case PT_T54S:
        strResult = SUB_PATH_T54;
        break;
#endif
    default:
        break;
    }

    return strResult;
}

yl::string getPicsPath(PHONE_TYPE_DEF ePhoneType)
{
    yl::string strResult = "";

    switch (ePhoneType)
    {
#if !IS_COLOR
    case PT_T19:
    case PT_T21:
    case PT_T23:
    case PT_T40:
    case PT_T40G:
    case PT_T41:
    case PT_T42:
    case PT_T43P:
    case PT_T43S:
    case PT_T27:
    case PT_T27S:
        strResult = SUB_PATH_BLACK;
        break;

    case PT_CP920:
        strResult = SUB_PATH_CP920;
        break;

#else
    case PT_T46:
    case PT_T46S:
        strResult = SUB_PATH_T46;
        break;

    case PT_T29:
        strResult = SUB_PATH_T29;
        break;

    case PT_T52:
        strResult = SUB_PATH_T52;
        break;

    case PT_T54S:
        strResult = SUB_PATH_T54;
        break;
#endif
    default:
        break;
    }

    return strResult;
}

yl::string getFontPath(PHONE_TYPE_DEF ePhoneType)
{
    yl::string strResult = "";

    switch (ePhoneType)
    {
#if !IS_COLOR
    case PT_T19:
    case PT_T21:
    case PT_T23:
    case PT_T40:
    case PT_T40G:
    case PT_T41:
    case PT_T42:
    case PT_T43P:
    case PT_T43S:
    case PT_T27:
    case PT_T27S:
        strResult = SUB_PATH_BLACK;
        break;

    case PT_CP920:
        strResult = "";
        break;

#else
    case PT_T46:
    case PT_T29:
    case PT_T46S:
    case PT_T52:
    case PT_T54S:
        strResult = SUB_PATH_COLOR;
        break;
#endif
    default:
        break;
    }

    return strResult;
}

yl::string getMenuPath(PHONE_TYPE_DEF ePhoneType)
{
    yl::string strResult = "";

    switch (ePhoneType)
    {
#if !IS_COLOR
    case PT_T19:
        strResult = PHONE_MENUCONFIG_PATH;
        strResult += SUB_PATH_T19;
        break;

    case PT_T21:
        strResult = PHONE_MENUCONFIG_PATH;
        strResult += SUB_PATH_T21;
        break;

    case PT_T23:
    case PT_T40:
    case PT_T40G:
        strResult = PHONE_MENUCONFIG_PATH;
        strResult += SUB_PATH_T23;
        break;

    case PT_T41:
    case PT_T42:
    case PT_T43P:
    case PT_T43S:
        strResult = PHONE_MENUCONFIG_PATH;
        strResult += SUB_PATH_T42;
        break;

    case PT_T27:
    case PT_T27S:
        strResult = PHONE_MENUCONFIG_PATH;
        strResult += SUB_PATH_T27;
        break;

    case PT_CP920:
        strResult = PHONE_MENU_PATH;
        strResult += SUB_PATH_CP920;
        break;

#else
    case PT_T46:
    case PT_T29:
    case PT_T46S:
        strResult = PHONE_MENU_PATH;
        strResult += SUB_PATH_T46;
        break;
    case PT_T52:
        strResult = PHONE_MENU_PATH;
        strResult += SUB_PATH_T52;
        break;
    case PT_T54S:
        strResult = PHONE_MENU_PATH;
        strResult += SUB_PATH_T54;
        break;
#endif
    default:
        break;
    }

    return strResult;
}
