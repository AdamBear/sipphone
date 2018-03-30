#ifndef  SETTING_DATEANDTIMEMODEL_H_
#define  SETTING_DATEANDTIMEMODEL_H_

#include "commondefine.h"
#include <ylstring.h>
#include <yllist.h>
#include <singletonclassdefine.h>

struct TimeZoneListData;
struct DateTimeGeneralData;
struct DateTimeDHCPData;
struct DateAndTimeFormatData;
struct DateTime;

//Class CDateAndTimeModel
class CDateAndTimeController
{
    DECLARE_SINGLETON_CLASS(CDateAndTimeController)
public:

    bool GetGeneralData(DateTimeGeneralData * pData);
    // Set date and time general data.
    // | refData | is the date and time data to set.
    void SetGeneralData(const DateTimeGeneralData & refData);

    bool GetDHCPData(DateTimeDHCPData * pData);
    // Set DHCP data.
    // | refData | is the DHCP to set.
    void SetDHCPData(const DateTimeDHCPData & refData);

    // Set date and time format.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetDateAndTimeFormat(DateAndTimeFormatData * pData);
    // Set date and time format.
    // | refData | is the date and time format to set.
    void SetDateAndTimeFormat(const DateAndTimeFormatData & refData);

    //Set data and time
    // | refData | is the date and time format to set.
    void SetDataTimeData(const DateTime & refData);

    // Get the Time Type.
    int GetTimeType();
    // Set the Time Type(SNTP or Manual)
    // | nType | is the time type, such as SNTP.
    void SetTimeType(int nType);

    // Get Current Time Zone.
    const yl::string GetCurrentTimeZone();
    // Set Current Time Zone.
    // | strCurrentTimeZone | is the current timezone.
    void SetCurrentTimeZone(const yl::string & strCurrentTimeZone);

    // add by dlw 2014-11-05
    // Get The City In Current Time Zone
    const yl::string GetCurrentTimeZoneCity();
    // Set The City In this Time Zone
    // | strCurrentTimeZoneCity | is the city in current timezone.
    void SetCurrentTimeZoneCity(const yl::string & strCurrentTimeZoneCity);

    // Get NTP server1.
    const yl::string GetNtpServer1();
    // Set NTP server1.
    // | strNtpServer1 | is the NTP server1 to set.
    void SetNtpServer1(const yl::string & strNtpServer1);

    // Get NTP server2.
    const yl::string GetNtpServer2();
    // Set NTP server2.
    // | strNTPServer2 | is the NTP server2 to set.
    void SetNtpServer2(const yl::string & strNtpServer2);

    // Get the Daylight saving.
    int GetDaylightSavingStatus();
    // Set Daylight saving(on/ off/ auto).
    // | nStatus | is Daylight Saving Status, such as on.
    void SetDaylightSavingStatus(int nStatus);

    // Get DateFormatType.
    int GetDateFormatType();
    // Set DateFormatType.
    // | nType | is the DateFormatType, such as "YYYY-MM-DD".
    void SetDateFormatType(int nType);

    // Get TimeFormatType.
    int GetTimeFormatType();
    // Set TimeFormatType.
    // | nType | is the TimeFormatType,such as 12 hour.
    void SetTimeFormatType(int nType);

    // Get DHCP status.
    bool IsDHCPOn();

    yl::string GetCustomDateFmt() const;

};

#define g_pDateAndTimeController GET_SINGLETON_INSTANCE(CDateAndTimeController)

#endif   // SETTING_DATEANDTIMEMODEL_H_
