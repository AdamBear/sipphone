#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_GROUP_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_GROUP_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>
#include <ylvector.h>
#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"
#include "directoryui/include/moddirectoryui.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
class  ImplDsskeyPerformanceGroupBase : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceGroupBase(DssKey_Type type);

    virtual ~ImplDsskeyPerformanceGroupBase();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取组名称文本显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);

};
///////////////////////////////////////////////////////////////////

////////////////DT_LOCALGROUP/////////////////////////
class  ImplDsskeyPerformanceLocalGroup : public ImplDsskeyPerformanceGroupBase
{
public:
    ImplDsskeyPerformanceLocalGroup();

    virtual ~ImplDsskeyPerformanceLocalGroup();
};

////////////////DT_XMLGROUP/////////////////////////
class  ImplDsskeyPerformanceXMLGroup : public ImplDsskeyPerformanceGroupBase
{
public:
    ImplDsskeyPerformanceXMLGroup();

    virtual ~ImplDsskeyPerformanceXMLGroup();
};

////////////////DT_BSGROUP/////////////////////////
class  ImplDsskeyPerformanceBrsfGroup : public ImplDsskeyPerformanceGroupBase
{
public:
    ImplDsskeyPerformanceBrsfGroup();

    virtual ~ImplDsskeyPerformanceBrsfGroup();
};

#if IF_BUG_PLCMDIR
////////////////DT_PLCM_PHONEBOOK/////////////////////////
class  ImplDsskeyPerformancePlcm : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformancePlcm();

    virtual ~ImplDsskeyPerformancePlcm();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};
#endif


#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_GROUP_H_
