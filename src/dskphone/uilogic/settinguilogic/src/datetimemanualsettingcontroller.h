#ifndef  SETTING_DATE_MANUAL_SETTING_CONTROLL_H_
#define  SETTING_DATE_MANUAL_SETTING_CONTROLL_H_

#include <ylstring.h>
#include <yllist.h>
#include <singletonclassdefine.h>
#include "settinguilogic/include/settingdefine.h"


enum DATE_TIME_TYPE_E
{
    DTT_YEAR = 0,
    DTT_MONTH,
    DTT_DAY,
    DTT_HOUR,
    DTT_MINUTE,
    DTT_SECOND
};
//Class CDateTimeManualSettingController
class CDateTimeManualSettingController
{
    DECLARE_SINGLETON_CLASS(CDateTimeManualSettingController);

public:
    yl::string GetDateTimeShowText(DATE_TIME_TYPE_E eDTType, const yl::string& strValue,
                                   bool bEditFinish);
    yl::string GetDayShowText(const yl::string& strOldValue,
                              const yl::string strMonth, const yl::string strYear, bool bEditFinish);


#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
    yl::string GetResult(const yl::string& strOldValue, int nMax, int nMin,
                         bool bEditFinish = false, bool bFullYear = false);
    yl::string GetStringByIntTime(int nTime, bool bFullYear = false);
#else
    yl::string GetResult(const yl::string& strOldValue, int nMax, int nMin, bool bEditFinish = false);
    yl::string GetStringByIntTime(int nTime);
#endif

};

#define g_pDateTimeManualSettingController GET_SINGLETON_INSTANCE(CDateTimeManualSettingController)

#endif   // SETTING_DATE_MANUAL_SETTING_CONTROLL_H_
