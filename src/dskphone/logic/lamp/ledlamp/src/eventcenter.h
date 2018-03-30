#ifndef _EVENTCENTERH_
#define _EVENTCENTERH_
#include <ETLLib.hpp>
#include <ylstring.h>
#include "ledlampstruct.h"
#include "eventslot.h"

/*
外界事件处理中心
*/
class CEventCenter
{
public :
    static CEventCenter * Instance() DECLARE_ATTR;

public:
    ~CEventCenter();
    /*
    处理具体事件
    @param
    nOutEventID: 事件ID
    bRegister: 是否为注册事件
    以下三个参数为Custom保留
    nLightNumForCustom: 灯号
    nLightTimeForCustom: 亮时间
    nDarkTimeForCustom: 暗时间
    */
    bool PostEvent(LAMP_EVENTTYPE eEventType,
                   bool bRegister,
                   int nLightNumForCustom = 0,
                   int nLightTimeForCustom = 0,
                   int nDarkTimeForCustom = 0);

    bool PostDsskeyEvent(int iIndex, int nLightTime = 0, int nDarkTime = 0, bool bGreen = true,
                         bool bRed = false);

    //初始化
    void Init();

    bool SwitchControl(bool bIsOn);
private:
    CEventSlot m_eventSlot; /* 事件执行者 */
    static CEventCenter ms_Center;
};

#endif
