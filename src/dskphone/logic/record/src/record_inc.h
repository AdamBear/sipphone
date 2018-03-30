#ifndef __RECORD_INC_H__
#define __RECORD_INC_H__

#include <ylstring.h>
#include <yllist.h>
#include <ETLLib.hpp>
#include <xmlparser/xmlparser.hpp>
#include <dsklog/log.h>
#include "devicelib/networkserver.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonapi/systemhelperapi.h"
#include "commonapi/stringhelperapi.h"
#include "commonunits/modcommonunits.h"
#include "voice/include/modvoice.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "baserecord.h"
#include "audiorecord.h"
#include "videorecord.h"
#include "picturerecord.h"
#include "recordinterface.h"
#include "recordmanager.h"
#include "../include/common.h"
#include "../include/recordmessagedefine.h"
#include "idlescreen/include/modidlescreen.h"
#include "lamp/ledlamp/include/modledlamp.h"

#include "media/modmedia.h"
#include "hotplug/modhotplug.h"

#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#endif

#endif // __RECORD_INC_H__
