#include "datetimemanualsettingcontroller.h"
#include "commonapi/stringhelperapi.h"


namespace
{
#define MAX_TIME_VALUE  100
#define MAX_YEAR_STRING_LENGHT 4
#define MAX_TIME_STRING_LENGHT 2
}

IMPLEMENT_GETINSTANCE(CDateTimeManualSettingController)

CDateTimeManualSettingController::CDateTimeManualSettingController()
{

}

CDateTimeManualSettingController::~CDateTimeManualSettingController()
{

}

yl::string CDateTimeManualSettingController::GetDayShowText(const yl::string& strOldValue,
        const yl::string strMonth, const yl::string strYear, bool bEditFinish)
{
    int nMax = 0;
    int nMin = 1;

    int arrDay[12] = {31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int nMonth = atoi(strMonth.c_str());
    if (nMonth <= 0 || nMonth > 12)
    {
        return strOldValue;
    }
    nMax = arrDay[nMonth - 1];
    if (0 == nMax)
    {
        if (0 == (atoi(strYear.c_str()) % 4))
        {
            nMax = 29;
        }
        else
        {
            nMax = 28;
        }
    }
    return GetResult(strOldValue, nMax, nMin, bEditFinish);
}

#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
yl::string CDateTimeManualSettingController::GetResult(const yl::string& strOldValue,
        int nMax, int nMin, bool bEditFinish /*= false*/, bool bFullYear /*= false*/)
#else
yl::string CDateTimeManualSettingController::GetResult(const yl::string& strOldValue,
        int nMax, int nMin, bool bEditFinish /*= false*/)
#endif
{
    if (strOldValue.length() < MAX_TIME_STRING_LENGHT
            && !bEditFinish
#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
            && !bFullYear
#endif
       )
    {
        return strOldValue;
    }

    int nOldValue = atoi(strOldValue.c_str());

    if (nOldValue > nMax)
    {
        nOldValue = nMax;
    }
    else if (nOldValue < nMin)
    {
        nOldValue = nMin;
    }

#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
    return GetStringByIntTime(nOldValue, bFullYear);
#else
    return GetStringByIntTime(nOldValue);
#endif
}

yl::string CDateTimeManualSettingController::GetDateTimeShowText(DATE_TIME_TYPE_E eDTType,
        const yl::string& strValue, bool bEditFinish)
{
    switch (eDTType)
    {
    case DTT_YEAR:
        {
#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
            if (strValue.length() < MAX_YEAR_STRING_LENGHT && !bEditFinish)
            {
                return strValue;
            }

            int nMax = 2037;
            int nMin = 1970;
            return GetResult(strValue, nMax, nMin, bEditFinish, true);
#else
            int nMax = 37;
            int nMin = 0;
            return GetResult(strValue, nMax, nMin, bEditFinish);
#endif
        }
        break;
    case DTT_MONTH:
        {
            int nMax = 12;
            int nMin = 1;
            return GetResult(strValue, nMax, nMin, bEditFinish);
        }
        break;
    case DTT_HOUR:
        {
            int nMax = 23;
            int nMin = 0;
            return GetResult(strValue, nMax, nMin, bEditFinish);
        }
        break;
    case DTT_MINUTE:
    case DTT_SECOND:
        {
            int nMax = 59;
            int nMin = 0;
            return GetResult(strValue, nMax, nMin, bEditFinish);
        }
        break;
    default:
        break;
    }

    return "";
}
#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
yl::string CDateTimeManualSettingController::GetStringByIntTime(int nTime, bool bFullYear /*= false*/)
#else
yl::string CDateTimeManualSettingController::GetStringByIntTime(int nTime)
#endif
{
    yl::string strTime;
#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
    if (bFullYear)
    {
        yl::string strYear = commonAPI_FormatString("%04d", nTime);
        return strYear;
    }
#endif
    nTime = nTime % MAX_TIME_VALUE;
    strTime = commonAPI_FormatString("%02d", nTime);
    return strTime;
}
