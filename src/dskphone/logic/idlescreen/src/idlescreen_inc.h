#ifndef __IDLE_SCREEN_INC_H__
#define __IDLE_SCREEN_INC_H__

#include <ETLLib.hpp>
#include <interfacedefine.h>
#include <yllist.h>
#include <ylhashmap.h>
#include <ylvector.h>
#include <ylstring.h>
#include <utility.hpp>

#include "../include/idlescreen_def.h"
#include "idle/idle_data_define.h"
#include "keyguard/include//keyguardmessagedefine.h"

#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "commonunits/modcommonunits.h"
#include "talk/callcompletion/include/modcallcompletion.h"
#include "voice/include/modvoice.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "account/include/modaccount.h"
#include "dsskey/include/moddsskey.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "../include/modidlescreen.h"

#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#else
#include "extended/actionuri_url/include/modactionuri_url.h"
#endif

#include "feature/include/modfeature.h"
#include "lamp/backlight/include/modbacklight.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/powerled/include/powerledmessagedefine.h"
#include "devicelib/networkserver.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "feature/include/modfeature.h"

#include "contacts/directory/include/directorymessagedefine.h"

#include "extended/wifimessagedefine.h"
#include "wireless/wifi/include/modwifi.h"
#include "extended/bluetoothmessagedefine.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"

#endif // __IDLE_SCREEN_INC_H__
