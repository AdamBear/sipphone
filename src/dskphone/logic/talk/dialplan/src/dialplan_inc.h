#ifndef __DIALPLAN_TALK_HELP_INC_H__
#define __DIALPLAN_TALK_HELP_INC_H__

#include "xmlparser/pugixml.hpp"
#include "regexp/modregexp.h"
#include "dsklog/log.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonapi/stringhelperapi.h"
#include "path_define.h"

#include "dialplanmanager.h"
#include "dialrulecontroller.h"
#include "moddialplan.h"

#include "talk/digitmap/include/moddigitmap.h"
#include "talk/emergency/include/modemergency.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "talk/talklogic/include/commonfunctions.h"

#include "feature/include/modfeature.h"
#include "commonunits/modcommonunits.h"
#include "adapterbox/include/modadapterbox.h"

#include <yllist.h>
#include <ylstring.h>
#include <ETLLib.hpp>
#include "etlmsghandler/modetlmsghandler.h"
#include "autopcontrol/include/modautopcontrol.h"

#include "keyguard/include/modkeyguard.h"
#include "extended/actionuri_url/include/modactionuri_url.h"

#include "dsskey/include/moddsskey.h"

#endif // __DIALPLAN_TALK_HELP_INC_H__
