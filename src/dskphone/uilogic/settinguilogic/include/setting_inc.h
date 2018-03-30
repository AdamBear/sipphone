#ifndef __SETTING_INC_H__
#define __SETTING_INC_H__

#include <ylstring.h>
#include <yllist.h>
#include <ylalgorithm.h>
#include <ETLLib.hpp>
#include <interfacedefine.h>
#include <singletonclassdefine.h>
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"

#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"

#include "xmlparser/xmlparser.hpp"

#include "common.h"
#include "settinguilogic/src/commondefine.h"
#include "innermsgdefine.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "settinguilogic/src/dateandtimecontroller.h"
#include "settinguilogic/src/featurecontroller.h"
#include "settinguilogic/src/networkcontroller.h"
#include "settinguilogic/src/statuscontroller.h"
#include "modsetuilogic.h"

#include "commonunits/modcommonunits.h"
#ifndef _WIN32
#warning
#endif
//#include "commonunits/sipsender.h"

#include "idlescreen/include/modidlescreen.h"

#include "lamp/backlight/include/modbacklight.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "lamp/powerled/include/powerledmessagedefine.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/ledlamp/src/factory.h"

#include "devicelib/networkserver.h"
#ifndef _WIN32
#warning
#endif
//#include "taskaction/networkfileaction.h"
#include "taskaction/modtaskaction.h"

#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "uimanager/uimanager_common.h"
#include "uimanager/moduimanager.h"

#include "account/include/modaccount.h"
#include "contacts/directory/include/moddirinterface.h"

#include "voice/include/modvoice.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "talk/fwd_dnd/include/modfwddnd.h"

#include "adapterbox/include/modadapterbox.h"

#endif  // __SETTING_INC_H__
