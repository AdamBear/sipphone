#include "implperformancesession.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformanceSessionBase::ImplDsskeyPerformanceSessionBase(DssKey_Type type)
    : ImplDsskeyPerformanceBase(type)
{

}

ImplDsskeyPerformanceSessionBase::~ImplDsskeyPerformanceSessionBase()
{

}

// 获取灯显示
bool ImplDsskeyPerformanceSessionBase::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    YLList<int> listState = logicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();

    Dsskey_Status eTalkStatu = DS_NON;
    for (; itr != itrEnd; itr++)
    {
        if (*itr > DS_SESSION_FIRST
                && *itr < DS_SESSION_END)
        {
            eTalkStatu = (Dsskey_Status)(*itr);
        }
    }
    DSSKEYUI_INFO("ProcessSessionStatus:: eTalkStatu:%d", eTalkStatu);
    switch (eTalkStatu)
    {
    case DS_SESSION_NORMAL_USING:
    case DS_SESSION_TALK:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        break;
    case DS_SESSION_HIDE:
        {
            LigntData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    case DS_SESSION_HOLD:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
        }
        break;
    default:
        {
            LigntData.eLightColorType = LED_COLOR_OFF;
        }
        break;
    }
    return true;
}

//////////////////////////DT_SPEEDDIAL/////////////////////////////////////

ImplDsskeyPerformanceSpeedDial::ImplDsskeyPerformanceSpeedDial()
    : ImplDsskeyPerformanceSessionBase(DT_SPEEDDIAL)
{

}

ImplDsskeyPerformanceSpeedDial::~ImplDsskeyPerformanceSpeedDial()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceSpeedDial::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_Speeddial;
    IconData.strIconPath = PIC_DSS_SPEED_DIAL;
    return true;
}

// 获取文字显示
bool ImplDsskeyPerformanceSpeedDial::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    bool bRet = false;
    if (!logicData.strValue.empty() && logicData.strLabel.empty())//不是空
    {
        LabelData.strFirstRowText = logicData.strValue;
        bRet = true;
    }
    return bRet;
}

//////////////////////////DT_PICKUP/////////////////////////////////////

ImplDsskeyPerformancePickup::ImplDsskeyPerformancePickup()
    : ImplDsskeyPerformanceSessionBase(DT_PICKUP)
{

}

ImplDsskeyPerformancePickup::~ImplDsskeyPerformancePickup()
{

}

// 获取图标显示
bool ImplDsskeyPerformancePickup::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_PICKUP;
    return true;
}

//////////////////////////DT_PICKUP/////////////////////////////////////

ImplDsskeyPerformanceGroupPickup::ImplDsskeyPerformanceGroupPickup()
    : ImplDsskeyPerformanceSessionBase(DT_GROUPPICKUP)
{

}

ImplDsskeyPerformanceGroupPickup::~ImplDsskeyPerformanceGroupPickup()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceGroupPickup::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_GROUPPICKUP;
    return true;
}

