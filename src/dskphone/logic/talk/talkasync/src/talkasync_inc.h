#ifndef __TALK_RINGHELP_HEADER_INC_H__
#define __TALK_RINGHELP_HEADER_INC_H__


#include "dsklog/log.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "path_define.h"

#include "contacts/directory/include/moddirinterface.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "talk/talklogic/include/sipstructmapfunc.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "threadtask/include/modthreadtask.h"
#include "threadtask/include/networkfileaction.h"
#include "threadtask/include/ringfileaction.h"
#include "dsskey/include/moddsskey.h"
#include "presence/include/modpresence.h"

#include "commonunits/t2xpath.h"
#include "resource/modresource.h"

#include "modtalkasync.h"
#include "sippicaction.h"
#include "talkasync_data.h"
#include "ringhelpimpl.h"
#include "contactmatchaction.h"
#include "msghandler.h"
#include "asynchelper.h"
#include "ldaphelper.h"

#include <yllist.h>
#include <ylvector.h>
#include <ylstring.h>
#include <ETLLib.hpp>
#include <ylhashmap.h>
#include <ylstringalgorithm.h>


#endif
