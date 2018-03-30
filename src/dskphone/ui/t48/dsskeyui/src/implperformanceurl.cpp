#include "implperformanceurl.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"


////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformanceURL::ImplDsskeyPerformanceURL()
    : ImplDsskeyPerformanceBase(DT_URL)
{

}

ImplDsskeyPerformanceURL::~ImplDsskeyPerformanceURL()
{

}

// 获取灯显示
bool ImplDsskeyPerformanceURL::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    switch (logicData.iStatus)
    {
    case DS_URL_OK:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
        }
        break;
    case DS_URL_NETWORK_UNAVAILABLE:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_RED;
            LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
        }
        break;
    // url 未知需红绿交替闪烁 - UI须支持该状态
    case DS_URL_NETWORK_UNKNOW:
        {
            LigntData.eLightColorType = LED_COLOR_RED_GREEN;
            LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
        }
        break;
    default:
        {
            LigntData.eLightColorType = LED_COLOR_OFF;
            LigntData.eLightFlashType = REFRESH_TYPE_NO_FLASH;
        }
        break;
    }

    return true;
}

// 获取图标显示
bool ImplDsskeyPerformanceURL::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_URL;

    switch (logicData.iStatus)
    {
    case DS_URL_OK:
        {
            IconData.strFlashIcon1 = PIC_DSS_URL_GREEN;
            IconData.strFlashIcon2 = "";
            IconData.eRefreshType = REFRESH_TYPE_ALTERNATE_FLASH;
        }
        break;
    case DS_URL_NETWORK_UNAVAILABLE:
        {
            IconData.strFlashIcon1 = PIC_DSS_URL_RED;
            IconData.strFlashIcon2 = "";
            IconData.eRefreshType = REFRESH_TYPE_ALTERNATE_FLASH;
        }
        break;
    // url 未知需红绿交替闪烁
    case DS_URL_NETWORK_UNKNOW:
        {
            IconData.strFlashIcon1 = PIC_DSS_URL_RED;
            IconData.strFlashIcon2 = PIC_DSS_URL_GREEN;
            IconData.eRefreshType = REFRESH_TYPE_ALTERNATE_FLASH;
        }
        break;
    default:
        {
            IconData.eRefreshType = REFRESH_TYPE_NO_FLASH;
        }
        break;
    }
    return true;
}
