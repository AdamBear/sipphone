#include "setting_inc.h"
#include "language/input/include/modInput.h"
#include "settinguilogic/include/settingdefine.h"

// The separator between time and zone.
#define kszSepTimeAndZone (" ")

IMPLEMENT_GETINSTANCE(CDateAndTimeController)

CDateAndTimeController::CDateAndTimeController()
{

}

CDateAndTimeController::~CDateAndTimeController()
{

}

bool CDateAndTimeController::GetGeneralData(DateTimeGeneralData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    // Read time type.
    pData->m_eDateTimeType = static_cast<DateAndTimeType>(GetTimeType());
    // Read TimeZone.
    pData->m_strTimeZone = GetCurrentTimeZone();
    // Read NTP server1.
    pData->m_strNtpServer1 = GetNtpServer1();
    // Read NTP server2.
    pData->m_strNtpServer2 = GetNtpServer2();
    // Read DayLightType.
    pData->m_eDayLightType = static_cast<DaylightSavingType>(GetDaylightSavingStatus());
    // add by dlw 2014-11-05
    // Read time zone city
    pData->m_strTimeZoneCity = GetCurrentTimeZoneCity();
    return true;
}

void CDateAndTimeController::SetGeneralData(const DateTimeGeneralData & refData)
{
    // Set time type.
    SetTimeType(refData.m_eDateTimeType);
    // Set TimeZone.
    SetCurrentTimeZone(refData.m_strTimeZone);
    // Set NTP server1.
    SetNtpServer1(refData.m_strNtpServer1);
    // Set NTP server2.
    SetNtpServer2(refData.m_strNtpServer2);
    // Set DayLightType.
    SetDaylightSavingStatus(refData.m_eDayLightType);
    // add by dlw 2014-11-05
    // Set Time Zone City
    SetCurrentTimeZoneCity(refData.m_strTimeZoneCity);
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);
}

bool CDateAndTimeController::GetDHCPData(DateTimeDHCPData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Read DHCP Time.
    pData->m_isDHCPTimeOn = IsDHCPOn();
    return true;
}

void CDateAndTimeController::SetDHCPData(const DateTimeDHCPData & refData)
{
#if IF_BUG_21893
    configParser_SetConfigInt(DhcpTimeSwitch, refData.m_isDHCPTimeOn ? 1 : 0, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(DhcpTimeSwitch, refData.m_isDHCPTimeOn ? 1 : 0);
#endif

    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);
}

bool CDateAndTimeController::GetDateAndTimeFormat(DateAndTimeFormatData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Read data format type.
    pData->m_eDateFormat = static_cast<DateFormatType>(GetDateFormatType());
    // Read time format type.
    pData->m_eTimeFormat = static_cast<TimeFormatType>(GetTimeFormatType());

    pData->m_strDiyDateFmt = GetCustomDateFmt();

    return true;
}

void CDateAndTimeController::SetDateAndTimeFormat(const DateAndTimeFormatData & refData)
{
    // Set date format type.
    SetDateFormatType(refData.m_eDateFormat);
    // Set time format type.
    SetTimeFormatType(refData.m_eTimeFormat);
    //etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 2);
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);
}

// Set menual time
void CDateAndTimeController::SetDataTimeData(const DateTime & refData)
{
    netSetLocalTime(
#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
        atoi(refData.m_strYear.c_str()),
#else
        atoi(refData.m_strYear.c_str()) + 2000,
#endif
        atoi(refData.m_strMonth.c_str()),
        atoi(refData.m_strDay.c_str()),
        atoi(refData.m_strHour.c_str()), atoi(refData.m_strMinute.c_str()),
        atoi(refData.m_strSecond.c_str()));

    //向所有线程广播配置文件更改的消息
    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SETTIME,
                  0); // 通知GUI内部线程：powersaving模块
    // http://10.2.1.199/Bug.php?BugID=58021
    msgBroadpostThreadMsg(SYS_MSG_NOTIFY_TIME_CHANGED, 0, 0);
    etl_NotifyApp(FALSE, SYS_MSG_NOTIFY_TIME_CHANGED, 0, 0);
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);
}

int CDateAndTimeController::GetTimeType()
{
    // Read TimeType
    return configParser_GetConfigInt(kszTimeType);
}

void CDateAndTimeController::SetTimeType(int nType)
{
    // Save TimeType to config file.
#if IF_BUG_21893
    configParser_SetConfigInt(kszTimeType, nType, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszTimeType, nType);
#endif
}

const yl::string CDateAndTimeController::GetCurrentTimeZone()
{
    // Get current timezone from config file.
    yl::string strRet(configParser_GetConfigString(kszCurrentTimeZoneTime));
    return strRet;
}

void CDateAndTimeController::SetCurrentTimeZone(const yl::string & strCurrentTimeZone)
{
    // Save current timezone to config file.
#if IF_BUG_21893
    configParser_SetConfigString(kszCurrentTimeZoneTime, strCurrentTimeZone.c_str(),
                                 CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigString(kszCurrentTimeZoneTime, strCurrentTimeZone.c_str());
#endif //IF_BUG_21893
}

// add by dlw 2014-11-05
// 获取当前时区城市
const yl::string CDateAndTimeController::GetCurrentTimeZoneCity()
{
    // Save current timezone city to config file.
    return configParser_GetConfigString(kszCurrentTimeZoneZone);
}

// 设置当前时区城市
void CDateAndTimeController::SetCurrentTimeZoneCity(const yl::string & strCurrentTimeZoneCity)
{
#if IF_BUG_21893
    configParser_SetConfigString(kszCurrentTimeZoneZone, strCurrentTimeZoneCity.c_str(),
                                 CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigString(kszCurrentTimeZoneZone, strCurrentTimeZoneCity.c_str());
#endif //IF_BUG_21893
}

const yl::string CDateAndTimeController::GetNtpServer1()
{
    // Read NTP server1.
    return configParser_GetConfigString(kszNtpServer1);
}

void  CDateAndTimeController::SetNtpServer1(const yl::string & strNtpServer1)
{
    // Save NTP server1 to config file.
#if IF_BUG_21893
    configParser_SetConfigString(kszNtpServer1, strNtpServer1.c_str(), CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigString(kszNtpServer1, strNtpServer1.c_str());
#endif
}

const yl::string CDateAndTimeController::GetNtpServer2()
{
    // Read NTP server2.
    return configParser_GetConfigString(kszNtpServer2);
}

void CDateAndTimeController::SetNtpServer2(const yl::string & strNtpServer2)
{
    // Save NTP server2 to config file.
#if IF_BUG_21893
    configParser_SetConfigString(kszNtpServer2, strNtpServer2.c_str(), CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigString(kszNtpServer2, strNtpServer2.c_str());
#endif
}

int CDateAndTimeController::GetDaylightSavingStatus()
{
    // Read Daylight Saving.
    return configParser_GetConfigInt(kszDaylightSaving);
}

void CDateAndTimeController::SetDaylightSavingStatus(int nStatus)
{
    // Save Daylight Saving to config file.
#if IF_BUG_21893
    configParser_SetConfigInt(kszDaylightSaving, nStatus, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszDaylightSaving, nStatus);
#endif
}

int CDateAndTimeController::GetDateFormatType()
{
    bool bChineseStyleDateFmt = Lang_IsChineseStyleDate();

    // Read DateFormat.
    int iDateFormat = configParser_GetConfigInt(kszDateFormatType);

    if (bChineseStyleDateFmt && iDateFormat <= DATE_FORMAT_WWW_DD_MMM)
    {
        DateFormatType eNewFmt = DATE_FORMAT_TYPE_NONE;

        switch (iDateFormat)
        {
        case DATE_FORMAT_WWW_MMM_DD:
            eNewFmt = DATE_FORMAT_MM_DD_WWW_SPACE;
            break;
        case DATE_FORMAT_DD_MMM_YY:
            eNewFmt = DATE_FORMAT_YY_MM_DD_HYPHEN;
            break;
        case DATE_FORMAT_DD_MM_YYYY:
            eNewFmt = DATE_FORMAT_YYYY_MM_DD_VIRGULE;
            break;
        case DATE_FORMAT_MM_DD_YY:
            eNewFmt = DATE_FORMAT_YY_MM_DD_VIRGULE;
            break;
        case DATE_FORMAT_DD_MMM_YYYY:
            eNewFmt = DATE_FORMAT_YYYY_MM_DD_SPACE;
            break;
        case DATE_FORMAT_WWW_DD_MMM:
            eNewFmt = DATE_FORMAT_MM_DD_WW_SPACE;
            break;
        case DATE_FORMAT_T20_MM_DD_YY:
        case DATE_FORMAT_T20_DD_MM_YY:
        case DATE_FORMAT_T20_YY_MM_DD:
        case DATE_FORMAT_YYYY_MM_DD:
        default:
            eNewFmt = (DateFormatType)iDateFormat;
            break;
        }

        return eNewFmt;
    }
    else if (!bChineseStyleDateFmt && iDateFormat > DATE_FORMAT_WWW_DD_MMM)
    {
        DateFormatType eNewFmt = DATE_FORMAT_TYPE_NONE;

        switch (iDateFormat)
        {
        case DATE_FORMAT_MM_DD_WWW_SPACE:
            eNewFmt = DATE_FORMAT_WWW_MMM_DD;
            break;
        case DATE_FORMAT_YY_MM_DD_HYPHEN:
            eNewFmt = DATE_FORMAT_DD_MMM_YY;
            break;
        case DATE_FORMAT_YYYY_MM_DD_VIRGULE:
            eNewFmt = DATE_FORMAT_DD_MM_YYYY;
            break;
        case DATE_FORMAT_YY_MM_DD_VIRGULE:
            eNewFmt = DATE_FORMAT_MM_DD_YY;
            break;
        case DATE_FORMAT_YYYY_MM_DD_SPACE:
            eNewFmt = DATE_FORMAT_DD_MMM_YYYY;
            break;
        case DATE_FORMAT_MM_DD_WW_SPACE:
            eNewFmt = DATE_FORMAT_WWW_DD_MMM;
            break;
        case DATE_FORMAT_T20_MM_DD_YY:
        case DATE_FORMAT_T20_DD_MM_YY:
        case DATE_FORMAT_T20_YY_MM_DD:
        case DATE_FORMAT_YYYY_MM_DD:
        default:
            eNewFmt = (DateFormatType)iDateFormat;
            break;
        }

        return eNewFmt;
    }

    return iDateFormat;
}

void CDateAndTimeController::SetDateFormatType(int nType)
{
    // Save DateFormat to config file.
#if IF_BUG_21893
    configParser_SetConfigInt(kszDateFormatType, nType, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszDateFormatType, nType);
#endif
}

int CDateAndTimeController::GetTimeFormatType()
{
#if 0
    const yl::string & strLang = configParser_GetConfigString(kszCurrentLang);

    const yl::string & strTimeFormat = configParser_GetConfigString(commonAPI_FormatString("%s.%s",
                                       strLang.c_str(), kszTimeFormatType).c_str());

    if (strTimeFormat.empty())
    {
        return configParser_GetConfigInt(kszTimeFormatType);
    }

    return atoi(strTimeFormat.c_str());
#else
    return configParser_GetConfigInt(kszTimeFormatType);
#endif
}

void CDateAndTimeController::SetTimeFormatType(int nType)
{
    // Save TimeForm to config file.
#if IF_BUG_21893
    configParser_SetConfigInt(kszTimeFormatType, nType, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszTimeFormatType, nType);
#endif
}

bool CDateAndTimeController::IsDHCPOn()
{
    // Read DHCP status from config file.
    return StrictCheck(configParser_GetConfigInt(DhcpTimeSwitch)) == CONFIG_ON;
}

yl::string CDateAndTimeController::GetCustomDateFmt() const
{
#if 0
    const yl::string & strLang = configParser_GetConfigString(kszCurrentLang);

    const yl::string & strDateFormat = configParser_GetConfigString(commonAPI_FormatString("%s.%s",
                                       strLang.c_str(), kszCustomDateFmt).c_str());

    if (strDateFormat.empty())
    {
        return configParser_GetConfigString(kszCustomDateFmt);
    }

    return strDateFormat;
#else
    return configParser_GetConfigString(kszCustomDateFmt);
#endif
}

