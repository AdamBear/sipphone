#include "dateandtimecontroller.h"

#include "setting/include/common.h"
#include "ETLLib.hpp"
#include "devicelib/networkserver.h"  // 该头文件依赖于ETLLib.hpp.

#include "dateandtimemodel.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////
// Class CDateAndTimeController
CDateAndTimeController * CDateAndTimeController::m_pController = NULL;

CDateAndTimeController * CDateAndTimeController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CDateAndTimeController();
        if (m_pController != NULL)
        {
            // Do initialization.
            m_pController->MyInit();
        }
    }

    return m_pController;
}

void CDateAndTimeController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (m_pController != NULL)
    {
        // Do finalization.
        m_pController->MyFinal();

        delete m_pController;
        m_pController = NULL;
    }
}

// .
bool CDateAndTimeController::ReloadTimeZoneList()
{
    return m_pModel != NULL ? m_pModel->ReloadTimeZoneList() : FALSE;
}

bool CDateAndTimeController::GetTimeZoneList(TimeZoneListData * pData) const
{
    // 从model中获取时区列表.
    return m_pModel != NULL ? m_pModel->GetTimeZoneList(pData) : false;
}

bool CDateAndTimeController::GetTimeZoneCityList(TimeZoneCityListData * pData,
        yl::string strTimeZone)
{
    // 从model中获取时区城市列表.
    return m_pModel != NULL ? m_pModel->GetTimeZoneCityList(pData, strTimeZone) : false;
}

const yl::string CDateAndTimeController::GetCurrentTimeZone() const
{
    // 从model中获取当前设置的时区.
    return m_pModel != NULL ? m_pModel->GetCurrentTimeZone() : false;
}

void CDateAndTimeController::SetCurrentTimeZone(
    const yl::string & strCurrentTimeZone)
{
    // 向model中设置当前的时区.
    if (m_pModel != NULL)
    {
        m_pModel->SetCurrentTimeZone(strCurrentTimeZone);
    }
}

const yl::string CDateAndTimeController::GetCurrentTimeZoneCity()
{
    // 从model中获取当前设置的时区.
    return m_pModel != NULL ? m_pModel->GetCurrentTimeZoneCity() : false;
}

void CDateAndTimeController::SetCurrentTimeZoneCity(const yl::string & strCurrentTimeZoneCity)
{
    // 向model中设置当前的时区.
    if (m_pModel != NULL)
    {
        m_pModel->SetCurrentTimeZoneCity(strCurrentTimeZoneCity);
    }
}

bool CDateAndTimeController::GetGeneralData(DateTimeGeneralData * pData)
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Read time type.
    pData->m_eDateTimeType = static_cast<DateAndTimeType>(m_pModel->GetTimeType());
    // Read TimeZone.
    pData->m_strTimeZone = m_pModel->GetCurrentTimeZone();
    // Read NTP server1.
    pData->m_strNtpServer1 = m_pModel->GetNtpServer1();
    // Read NTP server2.
    pData->m_strNtpServer2 = m_pModel->GetNtpServer2();
    // Read DayLightType.
    pData->m_eDayLightType = static_cast<DaylightSavingType>(m_pModel->GetDaylightSavingStatus());
    // add by dlw 2014-11-06
    // Read City
    pData->m_strTimeZoneCity = m_pModel->GetCurrentTimeZoneCity();
    return true;
}

void CDateAndTimeController::SetGeneralData(const DateTimeGeneralData & refData)
{
    if (m_pModel != NULL)
    {
        // Set time type.
        m_pModel->SetTimeType(refData.m_eDateTimeType);
        // Set TimeZone.
        m_pModel->SetCurrentTimeZone(refData.m_strTimeZone);
        // Set NTP server1.
        m_pModel->SetNtpServer1(refData.m_strNtpServer1);
        // Set NTP server2.
        m_pModel->SetNtpServer2(refData.m_strNtpServer2);
        // Set DayLightType.
        m_pModel->SetDaylightSavingStatus(refData.m_eDayLightType);
        // add by dlw 2014-11-06
        // Set City
        m_pModel->SetCurrentTimeZoneCity(refData.m_strTimeZoneCity);
    }
}

// Set time type.
void CDateAndTimeController::SetTimeType(int nType)
{
    if (m_pModel != NULL)
    {
        m_pModel->SetTimeType(nType);
    }
}

const long CDateAndTimeController::GetLocalTime() const
{
    return netGetLocalTime();// 返回获取的系统时间
}

bool CDateAndTimeController::SetLocalTime(const tm & stTime)
{
    // Set local time to network.
    netSetLocalTime(stTime.tm_year,
                    stTime.tm_mon,
                    stTime.tm_mday,
                    stTime.tm_hour,
                    stTime.tm_min,
                    stTime.tm_sec);
    return true;
}

bool CDateAndTimeController::GetDHCPData(DateTimeDHCPData * pData)
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Read DHCP Time.
    pData->m_isDHCPTimeOn = m_pModel->IsDHCPOn();

    return true;
}

void CDateAndTimeController::SetDHCPData(const DateTimeDHCPData & refData)
{
    if (m_pModel != NULL)
    {
        // Set DHCP type.
        m_pModel->SetDHCPStatus(refData.m_isDHCPTimeOn);
    }
}

bool CDateAndTimeController::GetDateAndTimeFormat(DateAndTimeFormatData * pData)
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Read data format type.
    pData->m_eDateFormat = static_cast<DateFormatType>(m_pModel->GetDateFormatType());
    // Read time format type.
    pData->m_eTimeFormat = static_cast<TimeFormatType>(m_pModel->GetTimeFormatType());

    pData->m_strDiyDateFmt = m_pModel->GetCustomDateFmt();

    pData->m_bDateShowFirst = m_pModel->IsDateShowBeforeTime();

    return true;
}

void CDateAndTimeController::SetDateAndTimeFormat(const DateAndTimeFormatData & refData)
{
    if (m_pModel != NULL)
    {
        // Set date format type.
        m_pModel->SetDateFormatType(refData.m_eDateFormat);
        // Set time format type.
        m_pModel->SetTimeFormatType(refData.m_eTimeFormat);
    }
}

CDateAndTimeController::CDateAndTimeController()
    : m_pModel(NULL)
{

}

CDateAndTimeController::~CDateAndTimeController()
{

}

bool CDateAndTimeController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CDateAndTimeModel;
    }

    return m_pModel != NULL;
}

bool CDateAndTimeController::MyFinal()
{
    // Release the model instance.
    if (m_pModel != NULL)
    {
        delete m_pModel;
        m_pModel = NULL;
    }

    return m_pModel == NULL;
}

}  // namespace NS_SETTING
