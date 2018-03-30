#include "implperformancecallpark.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "account/include/modaccount.h"


////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformanceCallPark::ImplDsskeyPerformanceCallPark()
    : ImplDsskeyPerformanceBase(DT_CALLPARK)
{
    m_bAutoPickUp = false;
}

ImplDsskeyPerformanceCallPark::~ImplDsskeyPerformanceCallPark()
{

}

// 获取显示内容前的操作
bool ImplDsskeyPerformanceCallPark::GetPerformanceBegin(const DssKeyLogicData & logicData)
{
    m_bAutoPickUp = false;
    YLList<int> listState = logicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();

    for (; itr != itrEnd; itr++)
    {
        if (DS_AUTO_PICKUP_ON == *itr)
        {
            m_bAutoPickUp = true;
        }
    }
    return true;
}

// 获取灯显示
bool ImplDsskeyPerformanceCallPark::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    if (logicData.iStatus == DS_BLF_STATUS_UNKOWN || logicData.iStatus == DS_NON)
    {

    }
    else if (logicData.iStatus == DS_BLF_STATUS_IDLE)
    {
        BOOL bBLFLedOnInIdle = (0 == configParser_GetConfigInt(kszBLFLedMode));
        if (bBLFLedOnInIdle)
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
    }
    else if (logicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        LigntData.eLightColorType = LED_COLOR_TYPE_RED;
        LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_TALKING)
    {
        if (m_bAutoPickUp)
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            LigntData.eLightFlashType = REFRESH_TYPE_SLOW_FLASH;
        }
        else
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_RED;
        }
        // page灯状态
        LigntData.ePageColorType = LED_COLOR_TYPE_RED;
        LigntData.iLightPriority = DSS_KEY_LED_PRIORITY_HIGH;
    }
    else
    {
        DSSKEYUI_WARN("Dsskey CallPark status error!");
    }

    return true;
}

// 获取图标显示
bool ImplDsskeyPerformanceCallPark::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (logicData.iStatus == DS_BLF_STATUS_UNKOWN || logicData.iStatus == DS_NON)
    {
        // http://10.3.5.199/Bug.php?BugID=61119
        SIP_SERVER_TYPE eServerType = acc_GetServerType(logicData.iLineID);
        if (SST_BROADSOFT == eServerType
#if IF_FEATURE_START2START_ACD
                || eServerType == SST_START2START
#endif
           )
        {
#if IF_BUG_30886
            IconData.strIconPath = PIC_DSS_DOWN_CALLPARK;
#else
            IconData.strIconPath = PIC_DSS_CALLPARK;
#endif
        }
        else
        {
            IconData.strIconPath = PIC_DSS_CALLPARK_FAIL;
        }
    }
    else if (logicData.iStatus == DS_BLF_STATUS_IDLE)
    {
#if IF_BUG_30886
        if (SST_BROADSOFT == acc_GetServerType(logicData.iLineID))
        {
            IconData.strIconPath = PIC_DSS_DOWN_CALLPARK;
        }
        else
#endif
        {
            IconData.strIconPath = PIC_DSS_CALLPARK;
        }
    }
    else if (logicData.iStatus == DS_BLF_STATUS_RINGING)
    {
        IconData.strIconPath = PIC_DSS_CALLPARK_RINGING;
        IconData.eRefreshType = REFRESH_TYPE_SLOW_FLASH;
    }
    else if (logicData.iStatus == DS_BLF_STATUS_TALKING)
    {
        if (m_bAutoPickUp)
        {
            IconData.strIconPath = PIC_DSS_CALLPARK_AUTO_TALK;
        }
        else
        {
            IconData.strIconPath = PIC_DSS_CALLPARK_MUNUAL_TALK;
        }
    }
    else
    {
        //
    }

    return true;
}
