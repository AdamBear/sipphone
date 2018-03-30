#ifndef _DSSKEY_EXP_LOGIC_H__
#define _DSSKEY_EXP_LOGIC_H__

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "explogicmanager.h"
#include "explogiccolormanager.h"
#include "explogiclcdmanager.h"
#include "exp/include/explogicdefine.h"
#include "devicelib/phonedevice.h"


LRESULT OnEXPEvent(msgObject & msg);

inline CEXPLogicManager * GetEXPLogicManager()
{

    switch (devicelib_GetPhoneType())
    {
#ifdef _EXP_COLOR_SUPPORT
    case PT_T54S:
    case PT_T52:
        return dynamic_cast<CEXPLogicManager *>(&(CEXPLogicColorManager::GetInstance()));
	default:
		break;
#endif
#ifdef _EXP_BLACK_SUPPORT
    default:
        return dynamic_cast<CEXPLogicManager *>(&(CEXPLogicLCDManager::GetInstance()));
        break;
#endif
    }

	return NULL;
};

#ifdef _EXP_COLOR_SUPPORT
#define BASECAST2COLOR(X)  dynamic_cast<CEXPLogicColorManager*> (X)
#endif
#ifdef _EXP_BLACK_SUPPORT
#define BASECAST2LCD(X)  dynamic_cast<CEXPLogicLCDManager*> (X)
#endif

#define g_pEXPLogicManager (GetEXPLogicManager())

#endif
#endif // !_DSSKEY_EXP_LOGIC_H__

