#include "explogic.h"

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "dsskey/src/dsskey_common.h"
#include "adapterbox/include/modadapterbox.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "edk/include/edkmsgdefine.h"
#include "edk/include/modedk.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"
#include "commonunits/commonunits_def.h"


LRESULT OnEXPEvent(msgObject & msg)
{
    return g_pEXPLogicManager->OnEXPEvent(msg);
}

#endif
