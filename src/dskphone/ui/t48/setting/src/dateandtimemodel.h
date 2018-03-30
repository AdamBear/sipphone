#ifndef  SETTING_DATEANDTIMEMODEL_H_
#define  SETTING_DATEANDTIMEMODEL_H_

#include "commondefine.h"

#include <list>
#include <ylstring.h>

struct TimeZoneListData;
struct TimeZoneCityListData;
namespace NS_SETTING
{
//Class CDateAndTimeModel
class CDateAndTimeModel
{
public:
    // struct TimeZoneData.
    struct TimeZoneData
    {
        // Invalid time zone type.
        enum { INVALID_TIMEZONE_TYPE = -1, };

        yl::string m_strTime;  // Time, such as "+8:.
        yl::string m_strZone;  // Zone, such as "Cuba(Havana)".
        yl::string m_strBegin;  // Begin time.
        yl::string m_strEnd;  // End time.
        yl::string m_strOffset;  // Offset.
        int m_nType;  // Time zone type.

        TimeZoneData()
        {
            Reset();
        }

        void Reset()
        {
            m_strTime = "";
            m_strZone = "";
            m_strBegin = "";
            m_strEnd = "";
            m_strOffset = "";
            m_nType = INVALID_TIMEZONE_TYPE;
        }
    };

    // Get the Time Type.
    int GetTimeType() const;
    // Set the Time Type(SNTP or Manual)
    // | nType | is the time type, such as SNTP.
    void SetTimeType(int nType);

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

    // Get NTP server1.
    const yl::string GetNtpServer1() const;
    // Set NTP server1.
    // | strNtpServer1 | is the NTP server1 to set.
    void SetNtpServer1(const yl::string & strNtpServer1);

    // Get NTP server2.
    const yl::string GetNtpServer2() const;
    // Set NTP server2.
    // | strNTPServer2 | is the NTP server2 to set.
    void SetNtpServer2(const yl::string & strNtpServer2);

    // Get the Daylight saving.
    int GetDaylightSavingStatus() const;
    // Set Daylight saving(on/ off/ auto).
    // | nStatus | is Daylight Saving Status, such as on.
    void SetDaylightSavingStatus(int nStatus);

    // Get DateFormatType.
    int GetDateFormatType() const;
    // Set DateFormatType.
    // | nType | is the DateFormatType, such as "YYYY-MM-DD".
    void SetDateFormatType(int nType);

    // Get TimeFormatType.
    int GetTimeFormatType() const;

    yl::string GetCustomDateFmt() const;

    bool IsDateShowBeforeTime() const;

    // Set TimeFormatType.
    // | nType | is the TimeFormatType,such as 12 hour.
    void SetTimeFormatType(int nType);

    // Get DHCP status.
    bool IsDHCPOn() const;
    // Set DHCP status.
    // | isOn | is the bool, return true means DHCP is on.
    void SetDHCPStatus(bool isOn);

    // Reload TimeZone from AutoDST.xml.
    // Returns true if reload successfully, otherwise returns false.
    bool ReloadTimeZoneList();

private:
    // Ctor and Dtor.
    CDateAndTimeModel();
    ~CDateAndTimeModel();

    // Initialize. It loads DST from XML file.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize. It clears and frees the memory.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyFinal();

    // Clear TimeZone List
    // Returns true if clear successfully, otherwise returns false.
    bool MyClearTimeZone();

    // Load XML file.
    // | strXmlFileName | is the XML file.
    bool MyLoadXmlFile(const yl::string & strXmlFileName);

private:
    std::list<TimeZoneData *> m_listTimeZone;
    friend class CDateAndTimeController;
};

}   // namespace NS_SETTING

#endif   // SETTING_DATEANDTIMEMODEL_H_
