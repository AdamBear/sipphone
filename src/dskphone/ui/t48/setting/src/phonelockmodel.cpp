#include "phonelockmodel.h"

#include "configparser/modconfigparser.h"
#include "setting/include/common.h"
#include <configiddefine.h>
#include "ETLLib/ETLLib.hpp"
#include "setting/include/messagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CPhoneLockModel.
PhoneLockType CPhoneLockModel::GetPhoneLockType() const
{
    PhoneLockType eType = PHONE_LOCK_TYPE_NONE;
    // Read phone lock type.
    int nVal = configParser_GetConfigInt(kszKeyGuardType);
    switch (nVal)
    {
    case 1:
        eType = PHONE_LOCK_MENU_KEY_LOCK;
        break;
    case 2:
        eType = PHONE_LOCK_FUNC_KEY_LOCK;
        break;
    case 3:
        eType = PHONE_LOCK_TALL_CALL_ONLY;
        break;
    case 4:
        eType = PHONE_LOCK_ANSWER_CALL_ONLY;
        break;

    default:
        eType = PHONE_LOCK_TYPE_NONE;
    }

    return eType;
}

void CPhoneLockModel::SetPhoneLockType(PhoneLockType ePhoneLockType)
{
    // Set phone lock type to config file.
    configParser_SetConfigInt(kszKeyGuardType, ePhoneLockType, CONFIG_LEVEL_PHONE);
    // Post message.
    etl_NotifyApp(false, PHONE_LOCK_TYPE_CHANGED, 0, ePhoneLockType);
}

CPhoneLockModel::CPhoneLockModel()
{

}

CPhoneLockModel::~CPhoneLockModel()
{

}

}  // namespace NS_SETTING
