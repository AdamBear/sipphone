#include "setting_inc.h"
#include "setting_page_define.h"
#include <devicelib/networkserver.h>
#include "language/common/common_function.h"
#include "pageuiconditioncontroller.h"
#include "language/input/include/modInput.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "textmessagecontroller.h"
#include "zero_common.h"
#include "zerocontroller.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "displaycontroller.h"
#include "settinguicommondata.h"
#include "broadsoft/callcenter/include/modcallcenter.h"

// 最大的buffer长度.
static const int knMaxBufferLen = 260;

namespace
{
#define MIN_TIME        4
#define MAX_TIME        14
}

int SettingUILogic_GetCurDateFormatType()
{
    // 获取当前的日期格式.
    DateAndTimeFormatData stData;
    if (g_pDateAndTimeController.GetDateAndTimeFormat(&stData))
    {
        return stData.m_eDateFormat;
    }
    return DATE_FORMAT_TYPE_NONE;
}

int SettingUILogic_GetCurTimeFormatType()
{
    // 获取当前的时间格式.
    return g_pDateAndTimeController.GetTimeFormatType();
}

yl::string SettingUILogic_Time2String(time_t tTime, bool bAddTime/* = true*/,
                                      const yl::string & strSep /*= " "*/)
{
    yl::string strDT;
    // 获取日期格式.
    tm * tmLocal = localtime(&tTime);
    if (NULL == tmLocal)
    {
        return "";
    }

    // 获取日期.
    strDT.append(SettingUILogic_GetDateFormat(tmLocal));

    if (bAddTime)
    {
        strDT.append(strSep);
        strDT.append(GetTimeFormat(tmLocal, true));
    }
    return strDT;
}

// 获取时分秒. 秒为-1，则不获取对应字符串
yl::string SettingUILogic_GetHourMinuteSecond(int nHour, int nMin, int nSec /*= -1*/, bool bShowMask /*= true*/)
{
    yl::string strAPM;  // 上下午标识, 可能为空.
    int nHourGet = nHour;

    // 根据setting的格式, 返回时分
    yl::string strTime;
    switch (SettingUILogic_GetCurTimeFormatType())
    {
    case TIME_FORMAT_12HOUR:
        if (nHour > 12)
        {
            //下午
            nHourGet = nHour - 12;
            strAPM = _UILOGIC_LANG_TRAN(TRID_PM);
        }
        else if (nHour < 12 && nHour > 0)
        {
            //上午
            nHourGet = nHour;
            strAPM = _UILOGIC_LANG_TRAN(TRID_AM);
        }
        else if (nHour == 12)
        {
            //中午12点
            nHourGet = 12;
            strAPM = _UILOGIC_LANG_TRAN(TRID_PM);
        }
        else if (nHour == 0)
        {
            //半夜12点
            nHourGet = 12;
            strAPM = _UILOGIC_LANG_TRAN(TRID_AM);
        }
        break;
    case TIME_FORMAT_24HOUR:
        nHourGet = nHour;
        break;

    default:
        nHourGet = nHour;
        SETTING_WARN("SettingUILogic_CallLogTime2String Invalid format!");
        break;
    }

    yl::string strHMS;
    if (-1 == nSec)
    {
        strHMS = commonAPI_FormatString(bShowMask ? "%02d:%02d" : "%02d %02d", nHourGet, nMin);
    }
    else
    {
        strHMS = commonAPI_FormatString(bShowMask ? "%02d:%02d:%02d" : "%02d %02d %02d", nHourGet, nMin, nSec);
    }

    if (strAPM.empty())
    {
        strTime = strHMS;
    }
    else
    {
        strHMS = commonAPI_FormatString("%s %s", strHMS.c_str(), strAPM.c_str());
    }

    return strTime;
}

yl::string SettingUILogic_Time2NewFormatString(time_t tTime, bool bAddTime/* = true*/,
        const yl::string & strSep/* = " "*/)
{
    static const long klSecPerDay = 24 * 60 * 60;
    // 日期分隔.
    static const char kszSepStr[] = " ";
    yl::string strDT;

    // 获取当前时间.
    time_t tCurrent = netGetLocalTime(); // http://192.168.1.99/Bug.php?BugID=29526
    time_t tTodayZero = (tCurrent / klSecPerDay) * klSecPerDay;  // 今天0点的时间.
    time_t tTomorrowZero = tTodayZero + klSecPerDay;  // 明天0点的时间.
    time_t tSixDayBeforeZero = tTodayZero - klSecPerDay * 7;  // 7天前的0点时间.

    // 获取日期格式.
    tm * tmLocal = localtime(&tTime);
    if (NULL == tmLocal)
    {
        return "";
    }

    if (tTime >= tTodayZero
            && tTime < tTomorrowZero)
    {
        // 如果是今天, 则显示为Today.
        strDT = _UILOGIC_LANG_TRAN("Today");
        strDT.append(kszSepStr);
        strDT.append(SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min));
        bAddTime = false;
    }
    else if (tTime >= tTodayZero - klSecPerDay
             && tTime < tTodayZero)
    {
        // 如果是昨天
        strDT = _UILOGIC_LANG_TRAN("Yesterday");
        strDT.append(kszSepStr);
        strDT.append(SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min));
        bAddTime = false;
    }
    else if (tTime >= tSixDayBeforeZero
             && tTime < tTodayZero - klSecPerDay)
    {
        // 如果是7天前的0点到昨天23:59之间, 则直接显示星期.
        strDT = _UILOGIC_LANG_TRAN(GetWeedayFull(tmLocal->tm_wday));
        strDT.append(kszSepStr);
        strDT.append(SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min));
        bAddTime = false;
    }
    else
    {
        // 获取日期.
        strDT.append(SettingUILogic_GetDateFormat(tmLocal));
    }

    if (bAddTime)
    {
        // 获取时间.
        strDT.append(strSep);
        strDT.append(GetTimeFormat(tmLocal));
    }
    return strDT;
}

// 获取时间格式
yl::string GetTimeFormat(tm * tmLocal, bool bAddSecond/* = false*/)
{
    if (tmLocal == NULL)
    {
        return "";
    }

    char pBuffer[knMaxBufferLen + 1] = {'\0'};
    // 获取日期格式.
    int nTFType = SettingUILogic_GetCurTimeFormatType();
    switch (nTFType)
    {
    //在时间之后要添加AM/PM
    case TIME_FORMAT_12HOUR:
        //下午
        if (tmLocal->tm_hour > 12)
        {
            if (bAddSecond)
            {
                sprintf(pBuffer, "%02d:%02d:%02dPM", tmLocal->tm_hour - 12, tmLocal->tm_min, tmLocal->tm_sec);
            }
            else
            {
                sprintf(pBuffer, "%02d:%02dPM", tmLocal->tm_hour - 12, tmLocal->tm_min);
            }
        }
        //上午
        else if (tmLocal->tm_hour < 12
                 && tmLocal->tm_hour > 0)
        {
            if (bAddSecond)
            {
                sprintf(pBuffer, "%02d:%02d:%02dAM", tmLocal->tm_hour, tmLocal->tm_min, tmLocal->tm_sec);
            }
            else
            {
                sprintf(pBuffer, "%02d:%02dAM", tmLocal->tm_hour, tmLocal->tm_min);
            }
        }
        else if (tmLocal->tm_hour == 12)
        {
            //中午12点
            if (bAddSecond)
            {
                sprintf(pBuffer, "12:%02d:%02dPM", tmLocal->tm_min, tmLocal->tm_sec);
            }
            else
            {
                sprintf(pBuffer, "12:%02dPM", tmLocal->tm_min);
            }
        }
        else if (tmLocal->tm_hour == 0)
        {
            //半夜12点
            if (bAddSecond)
            {
                sprintf(pBuffer, "12:%02d:%02dAM", tmLocal->tm_min, tmLocal->tm_sec);
            }
            else
            {
                sprintf(pBuffer, "12:%02dAM", tmLocal->tm_min);
            }
        }
        break;
    case TIME_FORMAT_24HOUR:
        if (bAddSecond)
        {
            sprintf(pBuffer, "%02d:%02d:%02d", tmLocal->tm_hour,
                    tmLocal->tm_min, tmLocal->tm_sec);
        }
        else
        {
            sprintf(pBuffer, "%02d:%02d", tmLocal->tm_hour,
                    tmLocal->tm_min);
        }
        break;
    default:
        break;
    }

    pBuffer[knMaxBufferLen] = '\0';
    return pBuffer;
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
                            strYear = _UILOGIC_LANG_TRAN(TRID_YEAR);
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
                            strTmp = commonAPI_FormatString("%s", _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str());
                        }
                        else
                        {
                            strTmp = commonAPI_FormatString("%s", _UILOGIC_LANG_TRAN(GetMonthFull(tmLocal->tm_mon + 1)).c_str());
                        }
                    }
                    break;
                case 'D': // Day
                    {
                        yl::string strDay;
                        if (bAutoPostfix)
                        {
                            strDay = _UILOGIC_LANG_TRAN(TRID_CHINESE_DAY_OF_MONTH);
                        }

                        strTmp = commonAPI_FormatString(bChineseStyleDateFmt ? "%d%s" : "%02d%s", tmLocal->tm_mday,
                                                        strDay.c_str());
                    }
                    break;
                case 'W': // Week
                    {
                        LPCSTR lpsWeek = iCount <= 2 ? GetWeeday(tmLocal->tm_wday) : GetWeedayFull(tmLocal->tm_wday);
                        strTmp = _UILOGIC_LANG_TRAN(lpsWeek);
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

yl::string SettingUILogic_GetDateFormat(tm * tmLocal, bool bShowComma /*= false*/)
{
    /*time_t tTemp = netGetLocalTime();
    tm* tmLocal = localtime(&tTemp);*/

    LPCSTR lpDefault = "XXXX-XX-XX XX:XX:XX";
    if (tmLocal == NULL)
    {
        return lpDefault;
    }

    DateAndTimeFormatData eDateFmt;
    if (!g_pDateAndTimeController.GetDateAndTimeFormat(&eDateFmt))
    {
        return yl::string();
    }

    bool bChineseStyleDateFmt = Lang_IsChineseStyleDate();
    yl::string strDate;

    if (GetCustomDateFmt(tmLocal, strDate, bChineseStyleDateFmt, eDateFmt))
    {
        return strDate;
    }

    yl::string strCurrentLang = configParser_GetConfigString(kszCurrentLang);
    bool bShouldShowComma = (bShowComma && "English" == strCurrentLang && !bChineseStyleDateFmt);

    yl::string strYear, strDay;
    if (bChineseStyleDateFmt)
    {
        strYear = _UILOGIC_LANG_TRAN(TRID_YEAR);
        strDay = _UILOGIC_LANG_TRAN(TRID_CHINESE_DAY_OF_MONTH);
    }

    switch (eDateFmt.m_eDateFormat)
    {
    case DATE_FORMAT_WWW_MMM_DD:  // WWW MMM DD.
        {
            return bShouldShowComma ? commonAPI_FormatString("%s, %s %02d%s",
                    _UILOGIC_LANG_TRAN(GetWeeday(tmLocal->tm_wday)).c_str(),
                    _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(), tmLocal->tm_mday, strDay.c_str())
                   : commonAPI_FormatString("%s %s %02d%s", _UILOGIC_LANG_TRAN(GetWeeday(tmLocal->tm_wday)).c_str(),
                                            _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(), tmLocal->tm_mday, strDay.c_str());
        }
    case DATE_FORMAT_DD_MMM_YY:  // DD-MM-YY.
        {
            return commonAPI_FormatString("%02d%s-%s-%02d%s", tmLocal->tm_mday, strDay.c_str(),
                                          _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(), (tmLocal->tm_year + 1900) % 100, strYear.c_str());
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
            return bShouldShowComma ? commonAPI_FormatString("%02d%s %s, %d%s",
                    tmLocal->tm_mday, strDay.c_str(),
                    _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(), tmLocal->tm_year + 1900, strYear.c_str())
                   : commonAPI_FormatString("%02d%s %s %d%s", tmLocal->tm_mday, strDay.c_str(),
                                            _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(), tmLocal->tm_year + 1900, strYear.c_str());
        }
    case DATE_FORMAT_WWW_DD_MMM:  // WWW DD MMM.
        {
            return bShouldShowComma ? commonAPI_FormatString("%s %02d%s, %s",
                    _UILOGIC_LANG_TRAN(GetWeeday(tmLocal->tm_wday)).c_str(),
                    tmLocal->tm_mday, strDay.c_str(), _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str())
                   : commonAPI_FormatString("%s %02d%s %s", _UILOGIC_LANG_TRAN(GetWeeday(tmLocal->tm_wday)).c_str(),
                                            tmLocal->tm_mday, strDay.c_str(), _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str());
        }
    case DATE_FORMAT_MM_DD_WWW_SPACE:  // MM DD WWW.
        {
            return commonAPI_FormatString("%s%d%s %s", _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(),
                                          tmLocal->tm_mday, strDay.c_str(), _UILOGIC_LANG_TRAN(GetWeedayFull(tmLocal->tm_wday)).c_str());
        }
    case DATE_FORMAT_MM_DD_WW_SPACE: // MM DD WW
        {
            return commonAPI_FormatString("%s%d%s %s", _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(),
                                          tmLocal->tm_mday, strDay.c_str(), _UILOGIC_LANG_TRAN(GetWeeday(tmLocal->tm_wday)).c_str());
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
                                          _UILOGIC_LANG_TRAN(GetMonth(tmLocal->tm_mon + 1)).c_str(), tmLocal->tm_mday, strDay.c_str());
        }
    default:
        break;
    }
    return lpDefault;
}


void SettingUILogic_GetDateDetails(yl::string & strDate, UINT & uTick)
{
    time_t tCurrent = netGetLocalTime();
    SYSTEMTIME ShowTime;
    struct tm * tmLocal;

    tmLocal = localtime(&tCurrent);

    if (tmLocal == NULL)
    {
        return;
    }

    strDate = SettingUILogic_GetDateFormat(tmLocal);

    /*SYSTEMTIME thisDate;
    thisDate.wYear = tmLocal->tm_year + 1900;
    thisDate.wMonth = tmLocal->tm_mon + 1;
    thisDate.wDay = tmLocal->tm_mday;
    thisDate.wHour = tmLocal->tm_hour;
    thisDate.wMinute = tmLocal->tm_min;
    thisDate.wSecond = tmLocal->tm_sec;
    thisDate.wDayOfWeek = tmLocal->tm_wday;*/

    uTick = ((tmLocal->tm_year + 1900) << 16) + ((tmLocal->tm_mon + 1) << 8) + tmLocal->tm_mday;
    //(thisDate.wHour << 16) + (thisDate.wMinute << 8) + thisDate.wSecond;
}

// 新需求:历史记录显示月日及时间
yl::string SettingUILogic_CallLogTime2SimpleString(time_t tTime)
{
#define klSecPerDay (24 * 60 * 60)

    tm * tmLocal = localtime(&tTime);
    if (tmLocal == NULL)
    {
        SETTING_WARN("SettingUILogic_CallLogTime2SimpleString tmLocal == NULL");
        return "XXXX-XX-XX XX:XX:XX";
    }

    // 获取当前时间.
    //time_t tCurrent = time(NULL);
    //time_t tTodayZero = (tCurrent / klSecPerDay) * klSecPerDay;  // 今天0点的时间.
    //time_t tTomorrowZero = tTodayZero + klSecPerDay;  // 明天0点的时间.
    //time_t tSixDayBeforeZero = tTodayZero - klSecPerDay * 7;  // 7天前的0点时间.

    // 新需求:历史记录显示月日及时间
    char szBuffer[knMaxBufferLen + 1] = {'\0'};
    sprintf(szBuffer, "%s %02d %s", GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_mday,
            SettingUILogic_GetHourMinuteSecond(tmLocal->tm_hour, tmLocal->tm_min).c_str());
    szBuffer[knMaxBufferLen] = '\0';
    return szBuffer;
}

// 处理配置发生改变的消息.
// | refMsg | 是消息.
// 如果处理成功则返回TRUE, 否则返回FALSE.
static LRESULT OnMessageProcess(msgObject & refMsg)
{
    switch (refMsg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
//          return OnConfigureMessage(refMsg);
        }
        break;
    default:
        break;
    }

    return FALSE;
}

// 处理配置更改消息
LRESULT SettingUILogic_ConfigChangeMSG(msgObject & msg)
{
    BOOL bHandled = TRUE;
    switch (msg.wParam)
    {
    case ST_FAVORITE_CONFIG:
        {
            // 重新加载favorite
            LoadFavoriteData();
        }
        break;
#if IF_SUPPORT_WALLPAPER
    case ST_WALLPAPER:
        {
            // 重新加载favorite
            if (NULL == g_pDisplayController)
            {
                return FALSE;
            }
            g_pDisplayController->ProcessMessage(msg);
        }
        break;
#endif
    default:
        {
            bHandled = FALSE;
        }
        break;
    }

    return bHandled;
}

void SettingUILogic_Init()
{
    g_pDateAndTimeController;
    g_pTextMessageController;
    LoadFavoriteData();

    // Autop
    Autop_Init();

    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, SettingUILogic_ConfigChangeMSG);
    SingleMsgReg(SYS_MSG_NOTIFY_TIME_CHANGED, OnMessageProcess);

    PageAllowConditionInit();
}

// 处理定时器
bool SettingUILogic_OnTimer(int iTimerID)
{
#ifndef _WIN32
#warning
#endif
    //if (GetAutopController()->OnTimer(iTimerID))
    //{
    //  return true;
    //}

    return false;
}

int SettingUILogic_GetTotalUnReadMessage()
{
    if (NULL == g_pTextMessageController)
    {
        return 0;
    }

    return g_pTextMessageController->GetTotalUnRead();
}

// 设置对比度
void SettingUILogic_Contrast(int iLevel, bool bEXP/* = false*/, int iEXPIndex/* = 0*/)
{
    backlight_ResetContrast(iLevel);
    /*
    //T28或者Joyce版本的T26才有液晶对比度调节
    if (!bEXP)
    {
        devicelib_SetLcdContrast(iLevel);
    }
    else
    {
        devicelib_setExpLCDContrast(iEXPIndex, iLevel + 1);
    }
    */
}

bool SettingUILogic_ConnectVoiceMail(int iAccountID/* = -1*/)
{
    /* http://10.3.5.199/Bug.php?BugID=61375
    int iUsableAccountNum  = acc_GetRegisteredAccountNum();
    // 如果没有账号注册上，则返回
    if (iUsableAccountNum < 1)
    {
        return false;
    }*/

    int iCurrentLineID = (iAccountID == -1) ? acc_GetLastVMailAccountId() : iAccountID;
    iCurrentLineID = (iCurrentLineID == -1) ? acc_GetDefaultAccount() : iCurrentLineID;

    // 无账号注册上 进入SET VOICE MAIL 默认选中第1项
    if (iCurrentLineID < 0 || iCurrentLineID >= acc_AccountNum())
    {
        AdapterBox_SettingUIEnterPage(MENU_PAGE_SET_VOICEMAIL, 0);
        return true;
    }

    yl::string strVoiceNum = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber,
                             iCurrentLineID);
    if (strVoiceNum.empty())
    {
        AdapterBox_SettingUIEnterPage(MENU_PAGE_SET_VOICEMAIL, iCurrentLineID);
        return true;
    }
    else
    {
        if (acc_IsAccountAvailable(iCurrentLineID))
        {
#ifndef _WIN32
#warning
#endif
//#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
#if 0
            idleScreen_ClearPopupFlag(PS_STATE_VOICEMAIL);
#else
            idleScreen_ClearNotifyTypeFlag(PS_STATE_VOICEMAIL, SNT_POPUP);
#endif

            return talklogic_CallOut(strVoiceNum.c_str(), "", iCurrentLineID);
        }
    }

    return false;
}

bool SettingUILogic_ViewVoiceMail(int iAccountID/* = -1*/)
{
    int iCurrentLineID = (iAccountID == -1) ? acc_GetLastVMailAccountId() : iAccountID;
    iCurrentLineID = (iCurrentLineID == -1) ? acc_GetDefaultAccount() : iCurrentLineID;

    // 无账号注册上 进入SET VOICE MAIL 默认选中第1项
    if (iCurrentLineID < 0 || iCurrentLineID >= acc_AccountNum())
    {
        iCurrentLineID = 0;
    }

    AdapterBox_SettingUIEnterPage(MENU_PAGE_VIEW_VOICEMAIL, iCurrentLineID);

    return true;
}

void SettingUILogic_Duration2String(int nDuration, yl::string & strDuration)
{
    if (nDuration < 0)
    {
        return;
    }

    //先转换成秒,上取整
    nDuration = (nDuration + 999) / 1000;

    int nHour = nDuration / 3600;
    int nMinSec = nDuration % 3600;
    int nMin = nMinSec / 60;
    int nSec = nMinSec % 60;

    strDuration = commonAPI_FormatString("%02d:%02d:%02d", nHour, nMin, nSec);
}

int SettingUILogic_GetIMEType(const char * pszIME)
{
    //if (NULL != pszIME)
    //{
    //  return CImeUiAdaptive::GetIMEType(pszIME);
    //}

    //return IT_UPPERFIRST;
#ifndef _WIN32
#warning CImeUiAdaptive::GetIMEType(pszIME)
#endif
    return 0;
}

// 进入Zero的入口函数
void SettingUILogic_EnterZero(bool bZeroLanuched/* = false*/, bool bBootEnter /*= false*/)
{
    if (Autop_IsUpdating()
            && !bBootEnter)
    {
        SETTINGUI_INFO("Zero is already launched");
        return;
    }

    g_pZeroController->SetZeroLaunched(bZeroLanuched);
    g_pZeroController->Enter();

    Autop_StartByZero(bZeroLanuched);
}

bool SettingUILogic_EnterZeroRPS()
{
#if ZERO_TOUCH_DEBUG
    SETTING_INFO("SettingUILogic_EnterZeroRPS");
#endif

    bool bLaunch = false;
    if (AUT_RPS == Autop_GetUpdateType())
    {
        bLaunch = true;
    }
    SETTINGUI_INFO("SettingUI_EnterRPS bLaunch[%d]", bLaunch);
    // 正在执行其他autop过程不执行RPS
    if (!bLaunch
            && Autop_IsUpdating())
    {
        SETTINGUI_INFO("RPS is no launched and autop is updating.");
        return false;
    }

    if (!bLaunch)
    {
        Autop_Start(AUT_RPS);
    }

    // 如果已经处于RPS界面的话已经检查过网络了,不需要重新进网络检测界面
    if ((bLaunch && idleScreen_IsNetworkAvailable())
            || SettingUILogic_CheckingNetwork())
    {
        Autop_StartRPSConnect();
    }

    return TRUE;
}

bool SettingUILogic_CheckingNetwork()
{
    return g_pZeroController->CheckingNetwork();
}

// 退出Zero
void SettingUILogic_ExitZero()
{
    g_pZeroController->Exit();
    CZeroController::ReleaseInstance();
}

// 根据当前页面与softkey操作,进入下一个页面
void SettingUILogic_EnterZeroNextStage(const yl::string & strCurrentPageAction,
                                       const yl::string & strSoftkeyAction)
{
    g_pZeroController->EnterNextStage(strCurrentPageAction, strSoftkeyAction);
}

void SettingUILogic_SetRedirectData(const yl::string strUserName, const yl::string strPassword)
{
    g_pZeroController->SetRedirectData(strUserName, strPassword);
}

void SettingUILogic_AsyncAutop()
{
    if (NULL == g_pZeroController)
    {
        return;
    }
    g_pZeroController->AsyncAutop();
}

#if IF_SUPPORT_WALLPAPER
const yl::string SettingUILogic_GetWallpaperPath(WALLPAPER_Type_E eWallType)
{
    if (NULL == g_pDisplayController)
    {
        return NS_SET::kszEmptyString;
    }

    switch (eWallType)
    {
    case WALLPAPER_Type_CURRENT:
        {
            return g_pDisplayController->GetCurrentWallpaper();
        }
        break;
    case WALLPAPER_Type_DEFAULT:
        {
            return g_pDisplayController->GetDefaultWallpaper();
        }
        break;
    default:
        break;
    }

    return NS_SET::kszEmptyString;
}
#endif


bool SettingUILogic_GetACDStatusString(ACD_STATUS eAcdStatus, yl::string& strAcdStatus)
{
    //如果开启了ACD,优先显示ACD状态
    if (!CallCenter_IsACDOn())
    {
        return false;
    }

#if IF_FEATURE_METASWITCH_ACD
    if (ACD_METASWITCH == Get_ACDType()
#if IF_FEATURE_METASWITCH_ACD
            && !MTSWACD_IsEnableShowState()
#endif
       )
    {
        return false;
    }
#endif

    bool bRet = true;

    switch (eAcdStatus)
    {
    case AS_AVAILABLE:
        {
            strAcdStatus = _UILOGIC_LANG_TRAN(TRID_AVAILABLE);

            // GENESYS类型
            if (ACD_GENESYS == Get_ACDType())
            {
                strAcdStatus = ACD_GetACDLoginName().c_str();
            }
        }
        break;
    case AS_UNAVAILABLE:
        {
            yl::string strUnavailCode;
            //获取Unavailable Code
            ACD_GetUnavailCode(strUnavailCode);
            //如果有code就显示Code，否则显示Unavailable
            if (strUnavailCode.empty())
            {
                strAcdStatus = _UILOGIC_LANG_TRAN(TRID_UNAVAILABLE);

                // GENESYS类型
                if (ACD_GENESYS == Get_ACDType())
                {
                    strAcdStatus = ACD_GetACDLoginName();
                }
            }
            else
            {
                strAcdStatus = strUnavailCode;
            }
        }
        break;
    case AS_WRAPUP:
        {
            strAcdStatus = _UILOGIC_LANG_TRAN(TRID_WRAP_UP);

            // GENESYS类型
            if (ACD_GENESYS == Get_ACDType())
            {
                strAcdStatus = ACD_GetACDLoginName();
            }
#if IF_FEATURE_METASWITCH_ACD
            else if (ACD_METASWITCH == Get_ACDType())
            {
                yl::string strWrapup = MTSWACD_GetLastAnseredMHLG();
                if (!strWrapup.empty())
                {
                    strAcdStatus = strAcdStatus + "(" + strWrapup + ")";
                }
            }
#endif
        }
        break;
    case AS_IDLE:
    default:
        bRet = false;
        break;
    }

    return bRet;
}
