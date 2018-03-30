#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"

class  ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceBase(DssKey_Type type);

    virtual ~ImplDsskeyPerformanceBase();

public:

    // 设置是否扩展台标志
    virtual void SetExtendFlag(bool bExt)
    {
        m_bExt = bExt;
    };

    // 获取是否扩展台标志
    virtual bool GetExtendFlag(void)
    {
        return m_bExt;
    };

    // 获取显示内容前的操作
    virtual bool GetPerformanceBegin(const DssKeyLogicData & logicData)
    {
        return false;
    };

    // 获取显示内容后的操作
    virtual bool GetPerformanceEnd(const DssKeyLogicData & logicData)
    {
        return false;
    };

    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData)
    {
        return false;
    };

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData)
    {
        return false;
    };

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData)
    {
        return false;
    };

    // 获取背景显示
    virtual bool GetBkgroundPerformance(const DssKeyLogicData & logicData, DsskeyBkgroundData & BkData)
    {
        return false;
    };

private:
    DssKey_Type m_type;         // Dsskey类型
    bool        m_bExt;         // 是否扩展台
};





#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_H_
