#pragma once

#include "dsklog/log.h"
#include "ETLLib.hpp"
#include <ylstl/ylvector.h>
#include <ylstringalgorithm.h>

#include "talk/talkproxy/include/scopedtimer.h"
#include "talk/talkproxy/include/helpclass.h"
#include "talk/talkproxy/include/scopedelapse.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/commonfunctions.h"

#include "setting_page_define.h"
#include "talk/fwd_dnd/include/fwddndmsgdefine.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "account/include/accountmessagedefine.h"
#include "account/include/modaccount.h"
#include "account/include/getbalance.h"
#include "etlmsghandler/modetlmsghandler.h"

#include "blf/include/blflistmsgdefine.h"
#include "blf/include/modblf.h"
#include "edk/include/modedk.h"
#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/dsskey_def.h"

#include "exp/include/exp_struct.h"
#include "exp/include/modexp.h"

#include "autoset/include/autoset_def.h"
#include "hotplug/modhotplug.h"
#include "adapterbox/include/modadapterbox.h"
#include "edk/include/modedk.h"

#include <configiddefine.h>
#include "configparser/modconfigparser.h"


#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/modwifi.h"
#endif
#if IF_SUPPORT_BLUETOOTH
#include "wireless/bluetoothNew/include/modbluetooth.h"
#endif

#include "talk/talklistener/include/modtalklistener.h"
#include "talk/talkadapter/include/sessionstateproxy.h"
#include "idlescreen/include/modidlescreen.h"
#include "broadsoft/callcenter/include/modcallcenter.h"

#include "dsskeytalkbaseaction.h"
#include "talkdsskey_data.h"

#include "dsskeyedkaction.h"
#include "dsskeygetbalance.h"
#include "dsskeyprefixaction.h"
#include "dsskeymeetmeconfaction.h"
#include "dsskeymtswcallparkaction.h"
#include "dsskeysrtpaction.h"
#include "dsskeyredialaction.h"
#include "dsskeyrecordaction.h"
#include "dsskeycallreturnaction.h"
#include "dsskeygrouppickupaction.h"
#include "dsskeytalkaction.h"
#include "dsskeyvoicemailaction.h"
#include "dsskeyunparkaction.h"
#include "dsskeypagingaction.h"
#include "dsskeyretrieveparkaction.h"
#include "dsskeycustomaction.h"
#include "dsskeybtbindaction.h"
#include "dsskeycommonaction.h"
#include "dsskeynaaction.h"
#include "dsskeyblflistaction.h"
#include "dsskeyblfbaseaction.h"
#include "dsskeytiptelbtnaction.h"
#include "dsskeyintercomaction.h"
#include "dsskeyconfaction.h"
#include "dsskeycallparkaction.h"
#include "dsskeyvideoconfaction.h"
#include "dsskeyconfservaction.h"
#include "dsskeytransaction.h"
#include "dsskeypickupaction.h"
#include "dsskeyexpswitchaction.h"
#include "dsskeyblfaction.h"
#include "dsskey3rdvmr.h"
#include "dsskeybindaction.h"
#include "dsskeyspeeddial.h"
#include "dsskeyfwdaction.h"
#include "dsskeylineaction.h"
#include "dsskeydndaction.h"
#include "dsskeybindaction.h"
#include "dsskeybindmanager.h"
#include "modtalkdsskey.h"
