#include "implperformancepaging.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"


////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformancePaging::ImplDsskeyPerformancePaging()
    : ImplDsskeyPerformanceBase(DT_PAGING)
{
    m_eTalkStatus = DS_NON;
}

ImplDsskeyPerformancePaging::~ImplDsskeyPerformancePaging()
{

}

// 获取显示内容前的操作
bool ImplDsskeyPerformancePaging::GetPerformanceBegin(const DssKeyLogicData & logicData)
{
    YLList<int> listState = logicData.listAuxiliaryState;
    YLList<int>::iterator itr = listState.begin();
    YLList<int>::iterator itrEnd = listState.end();

    m_eTalkStatus = DS_NON;
    for (; itr != itrEnd; itr++)
    {
        if (*itr > DS_SESSION_FIRST
                && *itr < DS_SESSION_END)
        {
            m_eTalkStatus = (Dsskey_Status)(*itr);
        }
    }

    return true;
}

// 获取文字显示
bool ImplDsskeyPerformancePaging::GetLabelPerformance(const DssKeyLogicData & logicData,
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

// 获取灯显示
bool ImplDsskeyPerformancePaging::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    if (DS_SESSION_NORMAL_USING == m_eTalkStatus
            || DS_SESSION_TALK == m_eTalkStatus
            || DS_SESSION_CALL_Out == m_eTalkStatus) // 通话状态暂时和DS_SESSION_NORMAL_USING一样
    {
        // dsskey状态
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
    }
    else if (DS_SESSION_HOLD == m_eTalkStatus)
    {
        // dsskey状态
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
        LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
    }

    if (m_eTalkStatus != DS_NON)
    {
        //LigntData.strIconPath = PIC_DSS_LINE_USING;
        //LigntData.eGroundPhoto = GROUNDPHOTO_MODE_USING;
    }
    else if (DS_SESSION_CALL_IN == m_eTalkStatus)
    {
        // dsskey状态
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
        LigntData.eLightFlashType = REFRESH_TYPE_FAST_FLASH;
    }
    else if (DS_SESSION_DIAL == m_eTalkStatus)
    {
        // dsskey状态
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
    }

    return true;
}

// 获取图标显示
bool ImplDsskeyPerformancePaging::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (m_eTalkStatus != DS_NON)
    {
        IconData.strIconPath = PIC_DSS_LINE_USING;
    }
    else
    {
        IconData.strIconPath = PIC_DSS_PAGING;
    }

    return true;
}

// 获取背景显示
bool ImplDsskeyPerformancePaging::GetBkgroundPerformance(const DssKeyLogicData & logicData,
        DsskeyBkgroundData & BkData)
{
    if (m_eTalkStatus != DS_NON)
    {
        BkData.eGroundPhoto = GROUNDPHOTO_MODE_USING;
    }
    else
    {
        BkData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
    }
    return true;
}
