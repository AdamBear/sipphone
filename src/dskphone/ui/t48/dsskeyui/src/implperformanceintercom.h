#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_INTERCOM_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_INTERCOM_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
class  ImplDsskeyPerformanceIntercom : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceIntercom();

    virtual ~ImplDsskeyPerformanceIntercom();

public:

    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);

};
///////////////////////////////////////////////////////////////////

#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_INTERCOM_H_
