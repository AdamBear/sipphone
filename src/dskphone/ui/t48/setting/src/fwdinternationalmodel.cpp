#include "fwdinternationalmodel.h"

#include "configparser/modconfigparser.h"
#include <configiddefine.h>

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CFWDInternationalModel.
bool CFWDInternationalModel::GetFWDInternationalEnabled() const
{
    // 从配置中获取数据
    return configParser_GetConfigInt(kszForwardInternationalEnable) != 0;
}

void CFWDInternationalModel::SetFWDInternationalEnabled(bool bEnabled)
{
    // 设置数据到配置中
    configParser_SetConfigInt(kszForwardInternationalEnable, bEnabled ? 1 : 0, CONFIG_LEVEL_PHONE);
}

CFWDInternationalModel::CFWDInternationalModel()
{

}

CFWDInternationalModel::~CFWDInternationalModel()
{

}

}  // namespace NS_SETTING

