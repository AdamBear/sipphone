#ifndef __TALKLOGIC_INC_H__
#define __TALKLOGIC_INC_H__

// 通话模块调试宏定义：增加一些log，方便移植代码调试时使用
#define DEBUG_TALK_LOGIC 1

#include <ylstring.h>
#include <ylstringalgorithm.h>

#include <ETLLib.hpp>
#include "globalmodel.h"
#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <ylhashmap.h>
#include <yllist.h>
#include "xmlparser.hpp"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "devicelib/networkserver.h"
#include "singletonclassdefine.h"

#include "../include/callinfo.h"
#include "../include/commonfunctions.h"
#include "../include/talklogic_common.h"
#include "../include/uiandlogic_common.h"
#include "../include/modtalklogic.h"
#include "../include/callinfo.h"

#include "voice/include/modvoice.h"

#include "basesession.h"
#include "baseroutine.h"
#include "singlesession.h"

#include "basedialroutine.h"
#include "baseconnectingroutine.h"
#include "baseringroutine.h"
#include "basefinishedroutine.h"
#include "basetalkroutine.h"

#include "dialroutine.h"
#include "predialroutine.h"
#include "pretranroutine.h"
#include "preretrieveparkroutine.h"
#include "connectroutine.h"
#include "blaconnectroutine.h"
#include "networkconfconnectroutine.h"
#include "linedevicetalkroutine.h"
#include "ringbackroutine.h"
#include "ringroutine.h"
#include "recallroutine.h"
#include "talkroutine.h"
#include "finishedroutine.h"
#include "autoredialroutine.h"
#include "networkconfroutine.h"
#include "betransferredsession.h"
#include "cparknotifyroutine.h"

#ifdef IF_BT_SUPPORT_PHONE
#include "btdialroutine.h"
#include "btpredialroutine.h"
#include "btconnectroutine.h"
#include "btringroutine.h"
#include "bttalkroutine.h"
#include "btfinishedroutine.h"
#include "bttalkmanager.h"
#endif // IF_BT_SUPPORT_PHONE

#include "aocfinishroutine.h"
#include "aocmanager.h"

#include "localconfsession.h"
#include "sessionmanager.h"
#include "talklogicmanager.h"

#include "recallroutine.h"
#include "autoredialmanager.h"
#include "multicastpage.h"
#include "tipteladapter.h"

#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "talk/callcompletion/include/modcallcompletion.h"

#include "commonunits/modcommonunits.h"
#include "voip/modvoip.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "feature/include/modfeature.h"

#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"

#include "dsskey/include/dsskey_enumtypes.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/idsskeyimplaction.h"
#ifndef _LANG
#define _LANG(str)  str
#endif

#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#endif
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "extended/csta/include/modcsta.h"

#include "contacts/directory/include/moddirinterface.h"
#include "contacts/directory/include/directorymessagedefine.h"

#include "contacts/ldaplogic/include/modldap.h"

#include "broadsoft/xsiactionhelper/include/modxsicommon.h"
#include "broadsoft/callpark/include/callparkcommon.h"
#include "broadsoft/callpark/include/modcallpark.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "broadsoft/callrecording/include/modcallrecording.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"

#include "account/include/modaccount.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "commonunits/modcommonunits.h"
#include "keyguard/include/modkeyguard.h"
#include "blf/include/modblf.h"

#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/backlight/include/modbacklight.h"

#include "taskaction/common.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/networkfileaction.h"
#include "threadtask/include/modthreadtask.h"
#include "adapterbox/include/modadapterbox.h"
#include "voiceinterface.h"
#include "presence/include/modpresence.h"
#include "translateiddefine.h"

// 通话子模块
#include "talk/dialplan/include/moddialplan.h"
#include "talk/vqrtcp/include/modvqrtcp.h"
#include "talk/talkhelp/include/modtalkhelp.h"
#include "talk/talkasync/include/modtalkasync.h"

#ifdef IF_SUPPORT_BLUETOOTH
#include "wireless/bluetoothNew/include/modbluetooth.h"
#endif // IF_SUPPORT_BLUETOOTH

#include "hotplug/modhotplug.h"
#include "media/mediacommonfun.h"

#include "broadsoft/xsiactionhelper/include/xsicommon.h"

#include "talk/talklogic/include/talkmsgdefine.h"

#if IF_FEATURE_CTRIP_CTI
#include "extended/ctripcti/include/modctripcti.h"
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/modrecord.h"
#endif

#include "talk/digitmap/include/moddigitmap.h"
#include "talk/emergency/include/modemergency.h"

#include "application/include/modapplication.h"
#include "commonunits/t2xpath.h"
#include "setting_page_define.h"

#ifdef IF_FEATURE_PSTN
#include "pstnadapter/include/modpstnadapter.h"
#endif // IF_FEATURE_PSTN
#include "talk/negohelper/modnego.h"

#include "autopcontrol/include/modautopcontrol.h"
#include "regexp/modregexp.h"

#include "talk/talkdsskey/include/modtalkdsskey.h"
#include "talk/talklistener/include/modtalklistener.h"
#include "talk/talkproxy/include/senddtmfasyncproxy.h"

//////////////////////////////////H323////////////////////////////////////////
#include "service_h323_define.h"
#include "service_h323_interface.h"
#include "service_h323_message.h"
#endif // __TALKLOGIC_INC_H__
