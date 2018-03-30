#include "impldsskeyperformance.h"
#include <ETLLib.hpp>
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "commonfunc.h"

ImplDsskeyPerformanceBase::ImplDsskeyPerformanceBase(DssKey_Type type)
{
    m_type = type;

    bool bRet = _DsskeyPerfManager.AddImplPerformance(type, this);
    if (!bRet)
    {
        DSSKEYUI_WARN("SetImplPerformance failed.");
    }

}

ImplDsskeyPerformanceBase::~ImplDsskeyPerformanceBase()
{
    bool bRet = _DsskeyPerfManager.DelImplPerformance(m_type, this);
    if (!bRet)
    {
        DSSKEYUI_WARN("DelImplPerformance failed.");
    }
    m_type = DT_NA;
}

///////////////////////////////////////
