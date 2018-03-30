#ifndef _EVENTSLOTH_
#define _EVENTSLOTH_
#ifdef _T42
#else
// #include <list>
// #include <vector>
#endif
#include <ETLLib.hpp>
#include "ledlampstruct.h"

class CLedLamp;
class CEventSlot
{
public:
    CEventSlot();
    ~CEventSlot();
    bool PostEvent(LAMP_EVENTTYPE _eventType, const tEventUnit & eventUnit, bool bRegister,
                   int iLampNum);
    //如果当前控制的灯是pLedLamp，且移除
    void RemoveLedLamp(CLedLamp * pLedLamp);

    /* 内存释放 */
    void ClearVLightInfo();
    /* 取list中头元素, 进行灯的操作 */
    bool DoneListHead(int nLightNum);

private:
    static VECTORLIGHTINFO  m_vLightInfo; /* 数组:存储所有灯信息(灯号 + 灯属性) */
};

#endif
