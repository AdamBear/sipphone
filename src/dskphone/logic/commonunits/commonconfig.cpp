#include "commonconfig.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

namespace commonUnits
{
CCommonConfig & CCommonConfig::GetInstance()
{
    static CCommonConfig s_objCommonConfig;

    return s_objCommonConfig;
}

CCommonConfig::CCommonConfig(void)
{
    //西伯拉亚语配置
    m_iEnableHebrew = configParser_GetConfigInt(kszEnableHebrew);
}

CCommonConfig::~CCommonConfig(void)
{
}


}
