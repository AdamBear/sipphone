#include "dateandtimemodel.h"

#include <assert.h>

#include "ETLLib/ETLLib.hpp"
#include "interfacedefine.h"
#include "setting/include/common.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "path_define.h"
#include "ETLLib.hpp"

#include "basemethod.h"
#include "xmlparser/xmlparser.hpp"
#include "language/input/include/modInput.h"

namespace NS_SETTING
{
////////////////////////////////////////////////
// Class CDateAndTimeModel

// The XML file name. it has Daylight Saving Time data.
static const char * const kszXmlFileName = "AutoDST.xml";
// The separator between time and zone.

CDateAndTimeModel::CDateAndTimeModel()
{
    MyInit();
}

CDateAndTimeModel::~CDateAndTimeModel()
{
    MyFinal();
}

bool CDateAndTimeModel::MyInit()
{
    // Load AutoDST XMl file.
    return MyLoadXmlFile(kszXmlFileName);
}

bool CDateAndTimeModel::MyFinal()
{
    return MyClearTimeZone();
}

bool CDateAndTimeModel::MyClearTimeZone()
{
    // Clear and free list.
    std::list<TimeZoneData *>::iterator iterList = m_listTimeZone.begin();
    while (iterList != m_listTimeZone.end())
    {
        if (*iterList != NULL)
        {
            //Free the memory.
            delete *iterList;
            *iterList = NULL;
        }

        // Remove from the list.
        iterList = m_listTimeZone.erase(iterList);
    }
    return true;
}

int CDateAndTimeModel::GetTimeType() const
{
    // Read TimeType
    return configParser_GetConfigInt(kszTimeType);
}

void CDateAndTimeModel::SetTimeType(int nType)
{
    // Save TimeType to config file.
    configParser_SetConfigInt(kszTimeType, nType, CONFIG_LEVEL_PHONE);

    // Depend on TimeType to emit broadcast
    if (1 == nType)
    {
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);
    }
    else
    {
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 1);
    }
}

// 比较时区 | add by dlw 2014-11-05
bool TimeZoneEqual(CDateAndTimeModel::TimeZoneData * lhs, CDateAndTimeModel::TimeZoneData * rhs)
{
    return lhs->m_strTime == rhs->m_strTime;
}

bool CDateAndTimeModel::GetTimeZoneList(TimeZoneListData * pData) const
{
    if (pData != NULL)
    {
        // Generate time zone string.
        //去除重复添加的时区
        std::list<TimeZoneData *> tmpTimeZone = m_listTimeZone;

        std::list<TimeZoneData *>::iterator iterLast = yl::unique(tmpTimeZone.begin(), tmpTimeZone.end(),
                TimeZoneEqual);
        //填充列表
        for (std::list<TimeZoneData *>::iterator iterPos = tmpTimeZone.begin(); iterPos != iterLast;
                ++iterPos)
        {
            if (const TimeZoneData * pTimeZone = (*iterPos))
            {
                pData->m_listTimeZone.push_back(pTimeZone->m_strTime);
            }
        } // for.
        return true;
    }
    return false;
}

bool CompareCity(yl::string & lCity, yl::string & rCity)
{
    return strcasecmp(lCity.c_str(), rCity.c_str()) < 0;
}

bool CDateAndTimeModel::GetTimeZoneCityList(TimeZoneCityListData * pData, yl::string strTimeZone)
{
    if (pData != NULL)
    {
        // Generate city string.
        std::list<TimeZoneData *>::iterator citer = m_listTimeZone.begin();
        for (; citer != m_listTimeZone.end(); ++citer)
        {
            if (const TimeZoneData * pTimeZone = (*citer))
            {
                if (pTimeZone->m_strTime == strTimeZone)
                {
                    pData->m_listTimeZoneCity.push_back(pTimeZone->m_strZone);
                }
            }
        }  // for.

        // 对城市按字母顺序排序
        Sort(pData->m_listTimeZoneCity, CompareCity);
        return true;
    }
    return false;
}

const yl::string CDateAndTimeModel::GetCurrentTimeZone() const
{
    // Get current timezone from config file.
    return configParser_GetConfigString(kszCurrentTimeZoneTime);
}

void CDateAndTimeModel::SetCurrentTimeZone(const yl::string & strCurrentTimeZone)
{
    // Save current timezone to config file.
    configParser_SetConfigString(kszCurrentTimeZoneTime, strCurrentTimeZone.c_str(),
                                 CONFIG_LEVEL_PHONE);
}

const yl::string CDateAndTimeModel::GetCurrentTimeZoneCity()
{
    // Save current timezone city to config file.
    return configParser_GetConfigString(kszCurrentTimeZoneZone);
}

void CDateAndTimeModel::SetCurrentTimeZoneCity(const yl::string & strCurrentTimeZoneCity)
{
#if IF_BUG_21893
    configParser_SetConfigString(kszCurrentTimeZoneZone, strCurrentTimeZoneCity.c_str(),
                                 CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigString(kszCurrentTimeZoneZone, strCurrentTimeZoneCity.c_str());
#endif //IF_BUG_21893
}

const yl::string CDateAndTimeModel::GetNtpServer1() const
{
    // Read NTP server1.
    return configParser_GetConfigString(kszNtpServer1);
}

void  CDateAndTimeModel::SetNtpServer1(const yl::string & strNtpServer1)
{
    // Save NTP server1 to config file.
    configParser_SetConfigString(kszNtpServer1, strNtpServer1.c_str(), CONFIG_LEVEL_PHONE);
}

const yl::string CDateAndTimeModel::GetNtpServer2() const
{
    // Read NTP server2.
    return configParser_GetConfigString(kszNtpServer2);
}

void CDateAndTimeModel::SetNtpServer2(const yl::string & strNtpServer2)
{
    // Save NTP server2 to config file.
    configParser_SetConfigString(kszNtpServer2, strNtpServer2.c_str(), CONFIG_LEVEL_PHONE);
}

int CDateAndTimeModel::GetDaylightSavingStatus() const
{
    // Read Daylight Saving.
    return configParser_GetConfigInt(kszDaylightSaving);
}

void CDateAndTimeModel::SetDaylightSavingStatus(int nStatus)
{
    // Save Daylight Saving to config file.
    configParser_SetConfigInt(kszDaylightSaving, nStatus, CONFIG_LEVEL_PHONE);
}

int CDateAndTimeModel::GetDateFormatType() const
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
        case DATE_FORMAT_YYYY_MM_DD:
        default:
            eNewFmt = (DateFormatType)iDateFormat;
            break;
        }

        return eNewFmt;
    }

    // Read DateFormat.
    return iDateFormat;
}

void CDateAndTimeModel::SetDateFormatType(int nType)
{
    // Save DateFormat to config file.
    configParser_SetConfigInt(kszDateFormatType, nType, CONFIG_LEVEL_PHONE);
}

int CDateAndTimeModel::GetTimeFormatType() const
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

void CDateAndTimeModel::SetTimeFormatType(int nType)
{
    // Save TimeForm to config file.
    configParser_SetConfigInt(kszTimeFormatType, nType, CONFIG_LEVEL_PHONE);
}

bool CDateAndTimeModel::IsDHCPOn() const
{
    // Read DHCP status from config file.
    return StrictCheck(configParser_GetConfigInt(DhcpTimeSwitch)) == CONFIG_ON;
}

void CDateAndTimeModel::SetDHCPStatus(bool isOn)
{
    // Save DHCP status to config file.
    configParser_SetConfigInt(DhcpTimeSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);
}

bool CDateAndTimeModel::ReloadTimeZoneList()
{
    // clear old list
    MyClearTimeZone();
    // reload timezone
    return MyLoadXmlFile(kszXmlFileName);
}

bool CDateAndTimeModel::MyLoadXmlFile(const yl::string & strXmlFileName)
{
    const char * const kszRootDSTData = "DSTData"; // The root node name.
    const char * const kszNodeNameDST = "DST"; // The DST node name.
    const char * const kszAttributeTimeszTime = "szTime"; // The szTime attribute name.
    const char * const kszAttributeZoneszZone = "szZone"; // The szZone attribute name.
    const char * const kszAttributeTypeiType = "iType"; // The iType attribute name.
    const char * const kszAttributeStartszStart = "szStart"; // The szStart attribute name.
    const char * const kszAttributeEndszEnd = "szEnd"; // The szEND attribute name.
    const char * const kszAttributeOffszOffset = "szOffset"; // The szOffset attribute name.

    // Get the full XML file path.
    yl::string strFullXmlFilePath = CONFIG_DATA_PATH + strXmlFileName;

    //xmlFile objXmlFile = NULL;
    xml_document doc;
    if (!pathFileExist(strFullXmlFilePath.c_str()) || !doc.load_file(strFullXmlFilePath.c_str()))
    {
        SETTING_WARN("Warning! Can't load timezone from [%s] !", strFullXmlFilePath.data());
        strFullXmlFilePath = FACTORY_DATA_PATH + strXmlFileName;
        if (!pathFileExist(strFullXmlFilePath.c_str()) || !doc.load_file(strFullXmlFilePath.c_str()))
        {
            SETTING_WARN("Error !! Can't load timezone from factory config [%s] !", strFullXmlFilePath.data());
            return false;
        }
    }

    //xmlElement* pNodeRoot = doc.child(kszRootDSTData);
    xml_node nodeRoot = doc.child(kszRootDSTData);
    if (!nodeRoot)
    {
        return false;
    }

    yl::string strAttributeName;

    // Iterate to load child items.
    xml_node nodeDST = nodeRoot.child(kszNodeNameDST);
    while (nodeDST)
    {
        TimeZoneData * pTimeZoneData = new TimeZoneData();
        if (pTimeZoneData == NULL)
        {
            continue;
        }
        // Reset the struct.
        pTimeZoneData->Reset();

        // Read szTime attribute.
        yl::string strAttributeyTimeValue = nodeDST.attribute(kszAttributeTimeszTime).value();
        // Read szZone attribute.
        yl::string strAttributeyZoneValue = nodeDST.attribute(kszAttributeZoneszZone).value();

        // 如果某一项值不存在或为空，则跳过
        if (strAttributeyTimeValue.empty() || strAttributeyZoneValue.empty())
        {
            nodeDST = nodeDST.next_sibling();
            continue;
        }

        pTimeZoneData->m_strTime = strAttributeyTimeValue;
        pTimeZoneData->m_strZone = strAttributeyZoneValue;

        // Read iType attribute.
        strAttributeName = nodeDST.attribute(kszAttributeTypeiType).value();
        if (!strAttributeName.empty())
        {
            pTimeZoneData->m_nType = atoi(strAttributeName.c_str());
        }

        // Read szStart attribute.
        strAttributeName = nodeDST.attribute(kszAttributeStartszStart).value();
        if (!strAttributeName.empty())
        {
            pTimeZoneData->m_strBegin = strAttributeName;
        }

        // Read szEnd attribute.
        strAttributeName = nodeDST.attribute(kszAttributeEndszEnd).value();
        if (!strAttributeName.empty())
        {
            pTimeZoneData->m_strEnd = strAttributeName;
        }

        // Read szOffset attribute.
        strAttributeName = nodeDST.attribute(kszAttributeOffszOffset).value();
        if (!strAttributeName.empty())
        {
            pTimeZoneData->m_strOffset = strAttributeName;
        }

        m_listTimeZone.push_back(pTimeZoneData);
        nodeDST = nodeDST.next_sibling();
    }

    return true;
}

yl::string CDateAndTimeModel::GetCustomDateFmt() const
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

bool CDateAndTimeModel::IsDateShowBeforeTime() const
{
#if 0
    const yl::string & strLang = configParser_GetConfigString(kszCurrentLang);

    const yl::string & strDateShowFirst = configParser_GetConfigString(commonAPI_FormatString("%s.%s",
                                          strLang.c_str(), kszDateShowBefore).c_str());

    if (strDateShowFirst.empty())
    {
        return configParser_GetConfigInt(kszDateShowBefore) != 0;
    }

    return atoi(strDateShowFirst.c_str()) != 0;
#else
    return configParser_GetConfigInt(kszDateShowBefore) != 0;
#endif
}

}   // namespace NS_SETTING
