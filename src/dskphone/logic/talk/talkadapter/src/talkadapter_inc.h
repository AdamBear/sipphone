#pragma once


#include "dsklog/log.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"

#include "contacts/directory/include/moddirinterface.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "talk/talklogic/include/sipstructmapfunc.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "dsskey/include/moddsskey.h"
#include "talklistener/include/modtalklistener.h"

#include "voip/modvoip.h"
#include "talkasync/include/modtalkasync.h"
#include "adapterbox/include/modadapterbox.h"
#include "setting_page_define.h"
#include "talk/talkdsskey/include/modtalkdsskey.h"

#include "modtalkadapter.h"
#include "talkadapterlistener.h"
#include "talkupdatelistener.h"
#include "updatemanager.h"
#include "talkadaptermanager.h"
#include "sessionstateproxy.h"
#include "scopedwndupdate.h"

#include "talk/talkproxy/include/helpclass.h"
#include "talk/talkproxy/include/scopedelapse.h"

#include <yllist.h>
#include <ylvector.h>
#include <ylstring.h>
#include <ETLLib.hpp>
#include <ylhashmap.h>
#include <ylstringalgorithm.h>

