#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_UC_FAVORITE_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_UC_FAVORITE_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
class  ImplDsskeyPerformanceUCFavorite : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceUCFavorite();

    virtual ~ImplDsskeyPerformanceUCFavorite();

public:
    // 获取显示内容前的操作
    virtual bool GetPerformanceBegin(const DssKeyLogicData & logicData);

    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

private:
    bool   m_bEnableUC;
};
///////////////////////////////////////////////////////////////////

#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_UC_FAVORITE_H_
