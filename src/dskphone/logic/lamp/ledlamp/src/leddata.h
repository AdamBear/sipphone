#ifndef _LED_DATA_H_
#define _LED_DATA_H_

#include "ledlampstruct.h"
#include "singletonclassdefine.h"
#include "etlmsghandler/modetlmsghandler.h"

class CLedData
{
    DECLARE_SINGLETON_CLASS(CLedData)
public:
    void Init();

    tEventUnit & GetEventUnit(LAMP_EVENTTYPE eEventType);

    //获取颜色对应的值
    int GetColorValue(int iLedlampID, LED_COLOR eLedColor);
    //去除所有灯颜色。
    void RemoveAllColor(int & iStatus, int iLightNum);

#if IF_FEATURE_37754
    MAPEVENTUNIT & GetEventMap();
#endif

private:
    // 消息处理
    bool OnMessage(msgObject& msgObj);

    enum
    {
        REFRESH_LIGHT_TIME = 0x01,
        REFRESH_DARK_TIME = 0x01 << 1,
        REFRESH_ALL_TIME = REFRESH_LIGHT_TIME & REFRESH_DARK_TIME
    };
    // 刷新电源灯配置
    void RefreshPowerLightConfig(int iRefreshFlag = REFRESH_ALL_TIME);

private:
    MAPEVENTUNIT m_eventMap; /* 配置文件中的记录 */

    MAPLEDCOLORVALUE m_MapColorValue; /* 配置文件中的记录 */

    void Dump();
};

#define _LedData GET_SINGLETON_INSTANCE(CLedData)

#endif //_LED_DATA_H_
