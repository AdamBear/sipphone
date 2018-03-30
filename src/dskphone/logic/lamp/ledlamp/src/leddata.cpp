#include "leddata.h"
#include "eventparser.h"
#include "dsklog/log.h"
#include "service_config.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonunits/modcommonunits.h"
IMPLEMENT_GETINSTANCE(CLedData)

CLedData::CLedData()
{

}

CLedData::~CLedData()
{

}

void CLedData::Init()
{
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CMSGSubscriber(this, &CLedData::OnMessage));

    //解析xml文件
    if (!CEventParser::ParseColor(kstrLedColorFilePath, m_MapColorValue))
    {
        LEDLAMP_INFO("Color:Parse file(%s) fail!", kstrLedColorFilePath.c_str());
    }

    if (!CEventParser::Parse(kstrXmlFilePath, m_eventMap))
    {
        LEDLAMP_INFO("Led Lamp:Parse file(%s) fail!", kstrXmlFilePath.c_str());
    }

#if IF_FEATURE_37754
    RefreshPowerLightConfig();
#endif

    Dump();
}

tEventUnit & CLedData::GetEventUnit(LAMP_EVENTTYPE eEventType)
{
    return m_eventMap[eEventType];
}

#if IF_FEATURE_37754
MAPEVENTUNIT & CLedData::GetEventMap()
{
    return m_eventMap;
}
#endif

int CLedData::GetColorValue(int iLedlampID, LED_COLOR eLedColor)
{
    MAPLEDCOLORVALUE::iterator iterLampID = m_MapColorValue.find(iLedlampID);
    if (iterLampID != m_MapColorValue.end())
    {
        MAPCOLORVALUE::iterator iterValue = iterLampID->second.find(eLedColor);
        if (iterValue != iterLampID->second.end())
        {
            return iterValue->second;
        }
    }

    return 1 << iLedlampID;
}

void CLedData::RemoveAllColor(int & iStatus, int iLightNum)
{
    MAPCOLORVALUE & MapValue = m_MapColorValue[iLightNum];

    if (MapValue.size() > 0)
    {
        MAPCOLORVALUE::iterator iter = MapValue.begin();

        for (; iter != MapValue.end(); iter++)
        {
            iStatus &= (~iter->second);
        }
    }
    else
    {
        iStatus &= ~(1 << iLightNum);
    }
}

// 消息处理
bool CLedData::OnMessage(msgObject & msgObj)
{
    bool bHandled = true;

    switch (msgObj.message)
    {
#if IF_FEATURE_37754
    case CONFIG_MSG_BCAST_CHANGED:
        if (msgObj.wParam == ST_POWER_SAVING_LIGHT_TIME)
        {
            RefreshPowerLightConfig(REFRESH_LIGHT_TIME);
        }
        else if (msgObj.wParam == ST_POWER_SAVING_DARK_TIME)
        {
            RefreshPowerLightConfig(REFRESH_DARK_TIME);
        }
        break;
#endif
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// 刷新电源灯配置
void CLedData::RefreshPowerLightConfig(int iRefreshFlag /*= REFRESH_ALL_TIME*/)
{
    LAMP_EVENTTYPE eType = LAMP_EVENTTYPE_NONE;
    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {


        eType = LAMP_EVENTTYPE_POWERSAVING;


    }
    MAPEVENTUNIT::iterator itr = GetEventMap().find(eType);
    if (itr == GetEventMap().end())
    {
        return;
    }

    if (iRefreshFlag & REFRESH_LIGHT_TIME)
    {
        int LightTime = configParser_GetConfigInt(kszPowerSavingLightTime);
        if (LightTime != 0)
        {
            LightTime = yl::min(10000, LightTime);
            LightTime = yl::max(100, LightTime);
        }

        itr->second._nLightTime = LightTime;
        LEDLAMP_INFO("Powersaving led config [light:%d]", LightTime);
    }

    if (iRefreshFlag & REFRESH_DARK_TIME)
    {
        int DarkTime = configParser_GetConfigInt(kszPowerSavingDarkTime);
        if (DarkTime != 0)
        {
            DarkTime = yl::min(10000, DarkTime);
            DarkTime = yl::max(100, DarkTime);
        }

        itr->second._nDarkTime = DarkTime;
        LEDLAMP_INFO("Powersaving led config [dark:%d]", DarkTime);
    }
}

void CLedData::Dump()
{
#if 0
    MAPEVENTUNIT::iterator iter = m_eventMap.begin();
    for (; iter != m_eventMap.end(); iter++)
    {
        //LEDLAMP_INFO("---------------Event[%d]", iter->first);
        LEDLAMP_INFO("pri[%d] Lig[%d] Dar[%d] lamp id[%d] size[%d] ", iter->second._npriority,
                     iter->second._nLightTime, iter->second._nDarkTime, iter->second._nLampNum,
                     iter->second.MapLed.size());

        MPA_LED_COLOR::iterator ledIter = iter->second.MapLed.begin();
        for (; ledIter != iter->second.MapLed.end(); ledIter++)
        {
            LEDLAMP_INFO("Led[%d] Color[%d]", ledIter->first, ledIter->second);
        }
    }
#endif // _DEBUG
}

