#ifndef  SETTING_DATEANDTIMECONTROLLER_H_
#define  SETTING_DATEANDTIMECONTROLLER_H_

#include <time.h>

#include <list>
#include <ylstring.h>
#include "ETLLib.hpp"

#include "commondefine.h"

struct DateAndTimeFormatData;
struct DateTimeGeneralData;
struct DateTimeDHCPData;
struct TimeZoneData;
struct TimeZoneListData;
struct TimeZoneCityListData;
namespace NS_SETTING
{
class CDateAndTimeModel;

// Class CDateAndTimeController
class CDateAndTimeController
{
public:
    // Get the singleton instance, it is a lazy instance.
    static CDateAndTimeController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Reload TimeZone from AutoDST.xml.
    // Returns true if reload successfully, otherwise returns false.
    bool ReloadTimeZoneList();

    // Get TimeZoneData list by a pointer.
    // | pData | [out] is the buffer.
    // Returns true if retrieved successfully, otherwise return false.
    bool GetTimeZoneList(TimeZoneListData * pData) const;

    // add by dlw 2014-11-05
    // 获取时区城市
    // Get TimeZoneCityData list by a pointer.
    // | pData | [out] is the buffer.
    // Returns true if retrieved successfully, otherwise return false.
    bool GetTimeZoneCityList(TimeZoneCityListData * pData, yl::string strTimeZone);

    // Get Current Time Zone.
    const yl::string GetCurrentTimeZone() const;
    // Set Current Time Zone.
    // | strCurrentTimeZone | is the current timezone.
    void SetCurrentTimeZone(const yl::string & strCurrentTimeZone);

    // add by dlw 2014-11-05
    // Get The City In Current Time Zone
    const yl::string GetCurrentTimeZoneCity();
    // Set The City In this Time Zone
    // | strCurrentTimeZoneCity | is the city in current timezone.
    void SetCurrentTimeZoneCity(const yl::string & strCurrentTimeZoneCity);

    // Get date and time general data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetGeneralData(DateTimeGeneralData * pData);
    // Set date and time general data.
    // | refData | is the date and time data to set.
    void SetGeneralData(const DateTimeGeneralData & refData);
    //set time type(manual or ntp)，会发消息刷新时间
    void SetTimeType(int nType);
    // Get local time.
    // Return sec.
    const long GetLocalTime() const;
    // Set local time.
    // | stTime | is the time to set.
    // Return true if set successfully, otherwise return false.
    bool SetLocalTime(const tm & stTime);

    // Get DHCP data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
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

private:
    // Ctor and Dtor.
    CDateAndTimeController();
    ~CDateAndTimeController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    CDateAndTimeModel * m_pModel; // The model instance.
    static CDateAndTimeController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CDateAndTimeController);
};

}   // namespace NS_SETTING

#define g_pDateAndTimeController (NS_SETTING::CDateAndTimeController::GetInstance())

#endif    // SETTNG_DATEANDTIMECONTROLLER_H_
