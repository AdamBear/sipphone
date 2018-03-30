#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_CALLPARK_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_CALLPARK_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
class  ImplDsskeyPerformanceCallPark : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceCallPark();

    virtual ~ImplDsskeyPerformanceCallPark();

public:
    // 获取显示内容前的操作
    virtual bool GetPerformanceBegin(const DssKeyLogicData & logicData);

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);

private:
    bool m_bAutoPickUp;
};
///////////////////////////////////////////////////////////////////

#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_CALLPARK_H_
