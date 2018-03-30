#ifndef __CALLCENTER_INC_H__
#define __CALLCENTER_INC_H__

#include <ETLLib.hpp>
#include <ylhashmap.h>
#include <yllist.h>
#include <ylalgorithm.h>
#include "interfacedefine.h"
#include "xmlparser/xmlparser.hpp"
#include "dsklog/log.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#include "callcenter_data_define.h"
#include "ccmessagedefine.h"
#include "acdmodelbase.h"
#include "acdmodel.h"
#include "acdmodelcosmocom.h"
#if IF_FEATURE_METASWITCH_ACD
#include "acdmodelmetaswitch.h"
#endif
#include "callcentercontroller.h"
#include "hotelingmodel.h"
#include "modcallcenter.h"
#include "cdsskeycallcenter.h"
#include "acdmodelgenesys.h"

#include "commonunits/modcommonunits.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "voip/modvoip.h"
#include "account/include/modaccount.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/talkmsgdefine.h"

#include "dsskey/include/moddsskey.h"
#include "feature/include/modfeature.h"

#include "account/include/accountmanager.h"

#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#endif
#endif //__CALLCENTER_INC_H__
