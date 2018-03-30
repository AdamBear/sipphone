#ifndef _DSSKEY_PERFORMANCE_MANAGER_H_
#define _DSSKEY_PERFORMANCE_MANAGER_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>
#include <ylvector.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "impldsskeyperformance.h"
#include "dsskey/include/dsskeystructure.h"
#include <ylhashmap.h>
#include "edk/include/edk_def.h"

typedef YLHashMap<DssKey_Type, ImplDsskeyPerformanceBase *> map_DsskeyPerf;

class  DsskeyPerformanceManager
{
public:
    DsskeyPerformanceManager();

    virtual ~DsskeyPerformanceManager();

    void Init();

    // 添加实际的表现解析
    bool AddImplPerformance(DssKey_Type dsskeyType, ImplDsskeyPerformanceBase * pImplPerf);

    // 删除对应类型的表现解析
    bool DelImplPerformance(DssKey_Type dsskeyType, ImplDsskeyPerformanceBase * pImplPerf);

    bool ParseDisplayData(DsskeyDisplayData & displayData, const DssKeyLogicData & logicData);

    bool ParseDefaultDisplayData(DsskeyDisplayData & displayData, const DssKeyLogicData & logicData);

private:
    ImplDsskeyPerformanceBase * GetDSSKeyPerformance(DssKey_Type dsskeyType);

    void SetEDKCtrlDsskeyDisplay(DsskeyDisplayData & displayData,
                                 const EdkCtrlDsskeyDispData & objEDKDisplayData);

private:
    DssKey_Type m_type;
    map_DsskeyPerf  m_mappPerformance;
};


// 获取全局唯一的主窗口对象
#define _DsskeyPerfManager Get_DsskeyPerfManagerInstance()
DsskeyPerformanceManager & Get_DsskeyPerfManagerInstance();


#endif //_DSSKEY_PERFORMANCE_MANAGER_H_
