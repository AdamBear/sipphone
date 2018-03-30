#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_SESSION_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_SESSION_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
class  ImplDsskeyPerformanceSessionBase : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceSessionBase(DssKey_Type type);

    virtual ~ImplDsskeyPerformanceSessionBase();

public:

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);
};
///////////////////////////////////////////////////////////////////

//////////////////////////DT_SPEEDDIAL/////////////////////////////////////

class  ImplDsskeyPerformanceSpeedDial : public ImplDsskeyPerformanceSessionBase
{
public:
    ImplDsskeyPerformanceSpeedDial();

    virtual ~ImplDsskeyPerformanceSpeedDial();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);
};

//////////////////////////DT_PICKUP/////////////////////////////////////

class  ImplDsskeyPerformancePickup : public ImplDsskeyPerformanceSessionBase
{
public:
    ImplDsskeyPerformancePickup();

    virtual ~ImplDsskeyPerformancePickup();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};

//////////////////////////DT_GROUPPICKUP/////////////////////////////////////

class  ImplDsskeyPerformanceGroupPickup : public ImplDsskeyPerformanceSessionBase
{
public:
    ImplDsskeyPerformanceGroupPickup();

    virtual ~ImplDsskeyPerformanceGroupPickup();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};



#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_SESSION_H_
