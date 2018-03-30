#include "resetandrebootmodel.h"

#include "configparser/modconfigparser.h"
#include "setting/include/common.h"
#include <configiddefine.h>

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CResetAndRebootModel.
int CResetAndRebootModel::GetResetOption() const
{
    // Read reset option from config file.
    return configParser_GetConfigInt(kszResetOption);
}

void CResetAndRebootModel::SetResetOption(int nOption)
{
    // Set reset option to config file.
    configParser_SetConfigInt(kszResetOption, nOption, CONFIG_LEVEL_PHONE);
}

CResetAndRebootModel::CResetAndRebootModel()
{

}

CResetAndRebootModel::~CResetAndRebootModel()
{

}

}  // namespace NS_SETTING
