#ifndef _LIGHTINFOTREATERH_
#define _LIGHTINFOTREATERH_
#include <ETLLib.hpp>
#include "ledlampstruct.h"

class CLightInfoTreater
{
public:
    //处理灯事件
    static bool TreatLightEvent(LAMP_EVENTTYPE eEventType,
                                const tEventUnit & eventUnit,
                                VECTORLIGHTINFO & vLightInfo,
                                bool bPopUp,
                                int iLampNum);

    static const tAttrStruct * GetHighestTime(int nLightNum, VECTORLIGHTINFO & vLightInfo);

private:
    /* 压入事件 */
    static bool Push(tLightInfo * pLightInfo,
                     const tEventUnit & eventUnit,
                     LAMP_EVENTTYPE eEventType);
    /* 弹出事件 */
    static bool PopUp(tLightInfo * pLightInfo,
                      const tEventUnit & eventUnit,
                      LAMP_EVENTTYPE eEventType);

    static tLightInfo * GetLightInfo(int nLightNum,
                                     VECTORLIGHTINFO & vLightInfo);

    static void SortList(ATTRLIST & lightList);
    CLightInfoTreater();
};

#endif
