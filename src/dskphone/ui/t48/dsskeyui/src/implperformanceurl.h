#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_URL_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_URL_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
class  ImplDsskeyPerformanceURL : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceURL();

    virtual ~ImplDsskeyPerformanceURL();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);

private:

};
///////////////////////////////////////////////////////////////////

#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_URL_H_
