#ifndef __AUTOSET_INC_H_
#define __AUTOSET_INC_H_


#include "autosetaccountstrategy.h"
#include "account/include/modaccount.h"
#include "dsskey/include/moddsskey.h"
#include "exp/include/modexp.h"
#include "dsskey/src/dsskeycommonfunc.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "autosetmanager.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"

#include "autosetbasestrategy.h"
#include <configiddefine.h>
#include <configparser/modconfigparser.h>

#include "autosetblfliststrategy.h"
#include "blf/include/blflistmsgdefine.h"
#include "blf/include/modblf.h"

#include "autosetbtstrategy.h"
#include "dsskey/include/dsskeymsgdefine.h"
#include "extended/bluetoothmessagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "autosetucfavoritestrategy.h"
#include "account/include/accountmessagedefine.h"
#include "account/include/account_datadefine.h"
#include <service_config.h>
#include <innermsgdefine.h>
#include <etlmsghandler/modetlmsghandler.h>
#include "presence/include/presence_inc.h"
#include "semiautosetaccountstrategy.h"
#if IF_FEATURE_FAVORITE_DIR
#include "autosetfavoritedirstrategy.h"
#endif //IF_FEATURE_FAVORITE_DIR
#include "contacts/directory/include/directorymessagedefine.h"
#include "contacts/directory/include/commondata.h"
#include "contacts/directory/include/directorystructure.h"
#include "contacts/directory/include/moddirinterface.h"
#include "presence/include/modpresence.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "autopcontrol/include/modautopcontrol.h"
#endif
