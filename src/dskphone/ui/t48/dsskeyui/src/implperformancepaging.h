#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_PAGING_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_PAGING_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
class  ImplDsskeyPerformancePaging : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformancePaging();

    virtual ~ImplDsskeyPerformancePaging();

public:
    // 获取显示内容前的操作
    virtual bool GetPerformanceBegin(const DssKeyLogicData & logicData);

    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);

    // 获取背景显示
    virtual bool GetBkgroundPerformance(const DssKeyLogicData & logicData, DsskeyBkgroundData & BkData);
private:
    int  m_eTalkStatus;
};
///////////////////////////////////////////////////////////////////

#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_PAGING_H_
