#include "msgprocessor.h"
#include <etlmsghandler/modetlmsghandler.h>
#include "account/include/accountmessagedefine.h"
#include "extended/bluetoothmessagedefine.h"
#include "extended/wifimessagedefine.h"
#include "record/include/recordmessagedefine.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "netmsgtypedefine.h"
#include "dsskeycommonfunc.h"
#include <voice/include/modvoice.h>
#include <interfacedefine.h>
#include "exp/include/modexp.h"
#include "dsskeydataentrymanager.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "broadsoft/callcenter/include/ccmessagedefine.h"
#include "devicelib/phonedevice.h"
#include "../include/moddsskey.h"
#include "hotplug/commondefine.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "talk/talklogic/include/talkmsgdefine.h"

void RegisterMsgHandle()
{
    // 注册处理SIP消息处理函数
    // yzh 消息范围待确定
    etl_RegisterMsgHandle(SIP_CALL_START, SIP_CALL_INFORMATION_UPDATE, OnMsgProcess);
    etl_RegisterMsgHandle(SIP_CALL_TRANSFERED_RESULT, SIP_CALL_SEND_INFO_RESULT, OnMsgProcess);

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnMsgProcess);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnMsgProcess);
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_STATUS_CHANGED, OnMsgProcess);

    // XMLBrowser模块消息
    //etl_RegisterMsgHandle(XB_MSG_DESTROY_DSSKEY, XB_MSG_DESTROY_DSSKEY, OnMsgProcess);
    // 扩展台消息
    etl_RegisterMsgHandle(DEV_MSG_EXP_KEYUP, DEV_MSG_EXP_KEYUP, OnMsgProcess);
#if IF_FEATRUE_PTT
    etl_RegisterMsgHandle(DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE,
                          DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE, OnMsgProcess);
#endif

    // 拔插扩展台消息
    etl_RegisterMsgHandle(DEV_MSG_EXP_INSERT, DEV_MSG_EXP_REMOVE, OnMsgProcess);
    // 获取扩展台信息消息
    etl_RegisterMsgHandle(DSK_MSG_ENUM_EXTENSION, DSK_MSG_ENUM_EXTENSION, OnMsgProcess);

    //ACD
    etl_RegisterMsgHandle(SIP_SUB_UPDATE_ACD_STATUS, SIP_SUB_UPDATE_ACD_STATUS, OnMsgProcess);


    etl_RegisterMsgHandle(DEV_MSG_EHS_KEY, DEV_MSG_EHS_KEY, OnMsgProcess);
    etl_RegisterMsgHandle(DEV_MSG_ELP_KEY, DEV_MSG_ELP_KEY, OnMsgProcess);

#ifdef IF_FEATURE_GENESYS_ACD
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE, OnMsgProcess);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE, OnMsgProcess);
    //处理Hoteling消息
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE, OnMsgProcess);
#endif

#ifdef IF_SUPPORT_BLUETOOTH
    //BLUETOOTH
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, OnMsgProcess);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE, OnMsgProcess);
#endif //#ifdef IF_SUPPORT_BLUETOOTH

    //Wi-Fi
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnMsgProcess);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    //USB
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE, OnMsgProcess);
#endif

#ifdef IF_SUPPORT_VIDEO
	//Camera
    etl_RegisterMsgHandle(CAMERA_MSG_STATUS_CHANGE, CAMERA_MSG_STATUS_CHANGE, OnMsgProcess);
#endif //#ifdef IF_SUPPORT_VIDEO

#ifdef IF_SUPPORT_HDMI
	//EXP_DISPLAY
    etl_RegisterMsgHandle(HDMI_MSG_STATUS_CHANGE, HDMI_MSG_STATUS_CHANGE, OnMsgProcess);
    etl_RegisterMsgHandle(HDMI_MSG_SWITCH_CHANGE, HDMI_MSG_SWITCH_CHANGE, OnMsgProcess);
#endif //#ifdef IF_SUPPORT_HDMI

#ifdef _T49
    etl_RegisterMsgHandle(TALK_MSG_SESSION_FOUCS_CHANGE, TALK_MSG_SESSION_FOUCS_CHANGE, OnMsgProcess);
    etl_RegisterMsgHandle(TALK_MSG_TALK_START, TALK_MSG_TALK_START, OnMsgProcess);
#endif
    // Bsft 联系人下载完成
    etl_RegisterMsgHandle(BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT, BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT,
                          OnMsgProcess);
}

LRESULT OnMsgProcess(msgObject & msg)
{
    bool bHandled = true;
    switch (msg.message)
    {
    case BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT:
        _DsskeyManager.RefreshDsskeyStatusByType(DT_BSGROUP);
        break;
    case TM_TIMER:
        bHandled = _DsskeyManager.OnTimer(msg.wParam);
        break;

    case SIP_SUB_UPDATE_BLA_STATUS: // only for Draft bla
        //ReFlashDraftBLA(msg);
        break;


    case CONFIG_MSG_BCAST_CHANGED:
        bHandled = _DsskeyManager.OnConfigChange(msg);
        break;
    case ACCOUNT_DEFAULT_CHANGED:
        _DsskeyManager.RefreshDsskeyStatusByType(DT_LINE);
        break;
    case ACCOUNT_STATUS_CHANGED:
        bHandled = _DsskeyManager.OnLineStateChange(msg);
        break;

#ifdef IF_SUPPORT_BLUETOOTH
    case BLUETOOTH_MSG_CONNECT_STATE:  //BLUETOOTH
    case BLUETOOTH_MSG_STATE_CHANGE:
        bHandled = _DsskeyManager.OnBluetoothStateChange(msg);
        break;
#endif
#ifdef _T49
    case WIFI_MSG_UPDATE_UI:           //WIFI
    case CAMERA_MSG_STATUS_CHANGE:   //CAMERA
    case HDMI_MSG_STATUS_CHANGE:     //EXT DISPLAY
    case HDMI_MSG_SWITCH_CHANGE:
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case RECORD_STORAGE_STATE_CHANGE:  //USB
#endif
    case TALK_MSG_SESSION_FOUCS_CHANGE:
    case TALK_MSG_TALK_START:
        bHandled = _DsskeyManager.OnNotifyDsskeyStateChange(msg);
        break;
#endif

    case DEV_MSG_EXP_INSERT:
        {
            // 扩展台插上消息
            DSSKEY_INFO("t48 exp insert![%d][%d]", msg.wParam, msg.lParam);
            if (50 != msg.lParam)
            {
				exp_RefreshExp(msg.wParam);
            }
			exp_OnExpInsert(msg.wParam, (DSSKEY_MODULE_TYPE)msg.lParam);
        }
        break;

    case DEV_MSG_EXP_REMOVE:
        {
            // 扩展台拔出消息
            DSSKEY_INFO("t48 exp remove![%d][%d]", msg.wParam, msg.lParam);
			exp_OnExpRemove(msg.wParam, (DSSKEY_MODULE_TYPE)msg.lParam);
        }
        break;

    case DEV_MSG_EXP_KEYUP:
        {
            DSSKEY_INFO("Desktop: DEV_MSG_EXP_KEYUP [0x%x]", msg.wParam);
            DSSKEY_INFO("[0x%x][%d],[0x%x][%d]", DEV_MSG_MESSAGE_FIRST, DEV_MSG_MESSAGE_FIRST,
                        DEV_MSG_KEY_RELEASED, DEV_MSG_KEY_RELEASED);
            int iDsskeyId = msg.wParam;

            msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DEV_MSG_FUNKEY_EVENT, 1, iDsskeyId);

        }
        break;
#if IF_FEATRUE_PTT
    case DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE:
        {
            if (exp_IsEXPKey(msg.wParam))
            {
                if (dsskey_IsSupportLongPress(msg.wParam) && dsskey_IsExistLongPress())
                {
                    dsskey_OnDsskeyRelease(msg.wParam);
                }
            }
        }
        break;
#endif
    case DEV_MSG_EHS_KEY:
        {
//              int iDsskeyId = msg.wParam;
//              msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DEV_MSG_FUNKEY_EVENT, 0x100, iDsskeyId);
//              DSSKEY_INFO("Desktop: DEV_MSG_EHS_KEY [0x%x][%d]", msg.wParam, msg.lParam);
        }
        break;
    case DEV_MSG_ELP_KEY:
        {
//          DSSKEY_INFO("Desktop: DEV_MSG_ELP_KEY [0x%x][%d]", msg.wParam, msg.lParam);
//          int iKeyCode = msg.wParam;
            //msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DEV_MSG_ELP_KEY, 1, iKeyCode);
        }
        break;
    case DEV_MSG_FUNKEY_EVENT:
        {
            //DSSKEY_INFO("Desktop: DSK_MSG_EXP_KEY [0x%x][%d]", msg.wParam, msg.lParam);
            //int dsskeyId = msg.lParam;
            //dsskey_OnDsskeyClick(dsskeyId);
        }
        break;
    case DSK_MSG_ENUM_EXTENSION:
        {
            int iNum = exp_GetEXPNum();

            DSSKEY_MODULE_TYPE eModuleType = exp_GetEXPModuleType();

            DSSKEY_INFO("On DSK_MSG_ENUM_EXTENSION, EXPNum [%d] exptype [%d]", iNum, eModuleType);

            int iRet = 0;
            if (DMT_EXP50_MODULE == eModuleType)
            {
                iRet = ((1 << 5) + iNum);
            }
            else
            {
                BOOL bExp38 = eModuleType == DMT_EXP38_MODULE;

                iRet = ((bExp38 << 4) + iNum);

                if (voice_GetEHSIndex() != -1)
                {
                    // 第11位表示是否接上EHS
                    iRet += (1 << 10);
                }
            }

            msg.ReplyMessage(iRet);
            DSSKEY_INFO("DSK_MSG_ENUM_EXTENSION: %d", iRet);
            return iNum;
        }
        break;

#if 0
    case SIP_SUB_UPDATE_ACD_STATUS:
        {
            if (NULL == msg.GetExtraData())
            {
                break;
            }

            // msg.lParam表示个数,一般只有一个
            sip_acd_status_info_t * pStatusInfo = (sip_acd_status_info_t *)msg.GetExtraData();
            for (int i = 0; i < msg.lParam; i++)
            {
                // yzh 需要映射状态
                FlashACDStatus(pStatusInfo->key_index, (ACD_STATUS)pStatusInfo->status);
            }
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    case ACD_MESSAGE_STATUS_CHANGE:
        FlashACDStatus(-1, (ACD_STATUS)msg.lParam, msg.wParam);
        break;

    case HOTELING_MESSAGE_STATUS_CHANGE:
        FlashHotelingStatus(msg.lParam == 1);
        break;
#endif
#endif
#ifdef IF_FEATURE_GENESYS_ACD
    case ACD_MESSAGE_STATUS_UPDATE:
        {
            dsskey_RefressDsskeyByType(DT_LINE);
        }
        break;
#endif
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

void UnRegisterMsgHandle()
{
    // 注册处理SIP消息处理函数
    etl_UnregisterMsgHandle(SIP_CALL_START, SIP_CALL_INFORMATION_UPDATE, OnMsgProcess);
    etl_UnregisterMsgHandle(SIP_CALL_TRANSFERED_RESULT, SIP_CALL_SEND_INFO_RESULT, OnMsgProcess);
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnMsgProcess);

    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnMsgProcess);
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnMsgProcess);

    // XMLBrowser模块消息
    //etl_UnregisterMsgHandle(XB_MSG_DESTROY_DSSKEY, XB_MSG_DESTROY_DSSKEY, OnMsgProcess);
    // 扩展台消息
//  etl_UnregisterMsgHandle(DSK_MSG_EXP_KEY, DSK_MSG_EXP_KEY, OnMsgProcess);
    // 拔插扩展台消息
    etl_UnregisterMsgHandle(DEV_MSG_EXP_INSERT, DEV_MSG_EXP_REMOVE, OnMsgProcess);
    // 获取扩展台信息消息
    etl_UnregisterMsgHandle(DSK_MSG_ENUM_EXTENSION, DSK_MSG_ENUM_EXTENSION, OnMsgProcess);

    //ACD
    etl_UnregisterMsgHandle(SIP_SUB_UPDATE_ACD_STATUS, SIP_SUB_UPDATE_ACD_STATUS, OnMsgProcess);

#ifdef IF_FEATURE_GENESYS_ACD
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE, OnMsgProcess);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE, OnMsgProcess);
    //处理Hoteling消息
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                            OnMsgProcess);
#endif

#ifdef IF_SUPPORT_BLUETOOTH
    //BTLINE
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, OnMsgProcess);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE, OnMsgProcess);
#endif //#ifdef IF_SUPPORT_BLUETOOTH

    //Wi-Fi
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnMsgProcess);
    //USB
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE, OnMsgProcess);
#endif

#ifdef IF_SUPPORT_VIDEO
	//Camera
    etl_UnregisterMsgHandle(CAMERA_MSG_STATUS_CHANGE, CAMERA_MSG_STATUS_CHANGE, OnMsgProcess);
#endif //#ifdef IF_SUPPORT_VIDEO

#ifdef IF_SUPPORT_HDMI
	//EXP_DISPLAY
    etl_UnregisterMsgHandle(HDMI_MSG_STATUS_CHANGE, HDMI_MSG_STATUS_CHANGE, OnMsgProcess);
    etl_UnregisterMsgHandle(HDMI_MSG_SWITCH_CHANGE, HDMI_MSG_SWITCH_CHANGE, OnMsgProcess);
#endif //#ifdef IF_SUPPORT_HDMI

    etl_UnregisterMsgHandle(TALK_MSG_TALK_START, TALK_MSG_TALK_START, OnMsgProcess);
    etl_UnregisterMsgHandle(TALK_MSG_SESSION_FOUCS_CHANGE, TALK_MSG_SESSION_FOUCS_CHANGE, OnMsgProcess);

    // BSFT 下载完成
    etl_UnregisterMsgHandle(BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT,
                            BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT, OnMsgProcess);
}
