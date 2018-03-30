#ifndef __DIAL_DIGITMAP_INC_H__
#define __DIAL_DIGITMAP_INC_H__

#include <ylvector.h>
#include <yllist.h>

#include <ylstring.h>
#include <ylstringalgorithm.h>

#include <ETLLib.hpp>

#include "regexp/modregexp.h"

#include "emergency.h"
#include "emergencydialplan.h"
#include "modemergency.h"
#include "emergencybase.h"
#include "emergencymanager.h"

#include "dsklog/log.h"

#include "commonapi/stringhelperapi.h"

#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "service_config.h"

#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#include "talk/talklogic/include/commonfunctions.h"
#include "talk/talklogic/include/modtalklogic.h"

#if IF_FEATURE_EMERGENCY_HELD
#include "talk/emergency/include/modemergencyheld.h"
#endif

#endif // __DIAL_DIGITMAP_INC_H__

