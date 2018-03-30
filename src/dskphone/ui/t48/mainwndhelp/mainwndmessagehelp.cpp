#include "mainwndmessagehelp.h"
#include "mainwnd/mainwnd.h"
#include "setting/include/modsetting.h"
#include "setting/include/messagedefine.h"
#include "service_system.h"
#include "broadsoft/callpark/include/callparkcommon.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "dsklog/log.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/languagehelper.h"
#include "service_system.h"
#include "interfacedefine.h"
#include "devicelib/phonedevice.h"
#include "dsklog/dsklog.h"
#include "baseui/transwidgetbgcontroler.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "translateiddefine.h"
#include "messagebox/modmessagebox.h"
#include "commonunits/commonunits_def.h"
#include "commonunits/modcommonunits.h"
#include "uimanager/moduimanager.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "adapterbox/include/modadapterbox.h"
#include "wifiui/include/wifiuimanagerhelp.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"
#include "voice/include/modvoice.h"
#include "extended/usbmessagedefine.h"
#include "presence/include/presence_inc.h"
#include "imagemanager/modimagemanager.h"
#include "voip/modvoip.h"

LRESULT MainWndMessageHelp(msgObject & refMessage);

LRESULT MainWndMessageHelp(msgObject & refMessage)
{
    MAINWND_INFO("CMainWnd::OnMainWndMessage[%d]", refMessage.message);
    switch (refMessage.message)
    {
    case LANGUAGE_MESSAGE_LANGUAGE_CHANGED:
    {
        // 语言改变时先发出SYSTEM_MSG_CONFIG_CHANGE（ST_LANGUAGE）给LanguageController处理,
        // LanguageController处理完再发此消息刷新窗口
        UIManager_UpdateTopWnd();
    }
    break;
    case SYS_MSG_RESET_FACTORY:
    {
        BASEUI_INFO("SYSTEM_MSG_RESET_FACTORY");
        AdapterBox_ResetFactory(refMessage.lParam);
    }
    break;
    case SYS_MSG_DEVICE_REBOOT:
    {
        BASEUI_INFO("SYS_MSG_DEVICE_REBOOT");
        AdapterBox_Reboot(refMessage.wParam);
    }
    break;
    case XSI_MESSAGE_REQ_FAIL:
    {
        if (refMessage.wParam == XSI_PARK_RETRIVE)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_RETRIEVE_FAILED),
                                        MESSAGEBOX_NOTE, 1500);
        }
    }
    break;
    case DSK_MSG_UPLOAD_CONFIG:
    {
        if (talklogic_SessionExist())
        {
            return 0;
        }

        // 提示正在升级
        _MainWND.ShowRebootMessage(LANG_TRANSLATE(TRID_IMPORT_CONFIGURATION_PLEASE_DO_NOT_POWER_OFF));

        // 关闭看门狗
        devicelib_CloseWatchDog();

        //停止SIP 服务
        modVoip_StopService();

        // 先回复
        refMessage.ReplyMessage(1);

        // 停止一切活动, 等待被Kill
        etlSleep(1000 * 1000);

        return 1;
    }
    break;
    case SYS_MSG_UPDATE_FIRWARE:
    {
        // 提示正在升级
        _MainWND.ShowRebootMessage(LANG_TRANSLATE(TRID_UPDATING_FIRMWARE));

        // 关闭看门狗
        devicelib_CloseWatchDog();

        // 先回复
        refMessage.ReplyMessage(1);

        // 停止一切活动, 等待被Kill
        etlSleep(1000 * 1000);

        return 1;
    }
    break;
    case CONFIG_MSG_BCAST_CHANGED:
    {
        switch (refMessage.wParam)
        {
#ifndef _T48
        case ST_DSSKEY_LENGHT:
        {
            _MainWND.DrawLinekey(UIManager_GetTopWindow());
            UIManager_UpdateWnd(UIManager_GetTopWindow());
        }
        break;
#endif
        //case ST_SYSLOG:
        //    {
        //        // 从配置中读取log等级
        //        run_cmd_t  run_cmd;
        //        memset(&run_cmd, 0, sizeof(run_cmd_t));
        //        int iLevel = configParser_GetConfigInt(kszServerLogLevel);
        //        char strLevel[32] = {0};
        //        sprintf(strLevel, "ANY=%d", iLevel);
        //        run_cmd.loglevel_str = strLevel;

        //        run_cmd_log_apply(&run_cmd);
        //        IDLEUI_WARN("Syslog level change: %d", iLevel);
        //    }
        //    break;
        case ST_TRANSPARENTCY:
        {
            g_TransWidgetBgControler.UpdateTransType();

            UIManager_ChangeTransparency();

            CBaseDialog * pDialog = UIManager_GetTopWindow();
            if (NULL != pDialog && pDialog->IsUseTransparency())
            {
                UIManager_UpdateTopWnd();
            }
        }
        break;
        default:
            break;
        }
    }
    break;
    case POWERSAVINGMEG_SCREEN_CLOSE:
    {
        if (POWERSAVING_MODE_CLOSE == refMessage.wParam)
        {
            _MainWND.ShowBlackFrame();
        }
    }
    break;
    case POWERSAVINGMEG_SCREEN_AWAKE:
    {
        _MainWND.HideBlackFrame();
    }
    break;
    case USB_MSG_HEADSET_PLUG:
    {
        MessageBox_ShowCommonMsgBox(NULL, refMessage.wParam ?
                                    LANG_TRANSLATE(TRID_USB_HEADSET_IS_CONNECTED) : LANG_TRANSLATE(TRID_USB_HEADSET_IS_REMOVE),
                                    MESSAGEBOX_NOTE, 2000);
    }
    break;
    case VOICE_CHANNEL_FORBDDEN:
    {
        yl::string strTips = "";
        if (refMessage.wParam == CHANNEL_HEADSET)
        {
            strTips = TRID_HEADSET_FORBIDDEN;
        }
        else if (refMessage.wParam == CHANNEL_HANDSET)
        {
            strTips = TRID_HANDSET_FORBIDDEN;
        }
        else if (refMessage.wParam == CHANNEL_HANDFREE)
        {
            strTips = TRID_HANDFREE_FORBIDDEN;
        }

        if (!strTips.empty())
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(strTips.c_str()),
                                        MESSAGEBOX_NOTE, 2000);
        }
    }
    break;
    case DEV_MSG_EXP_KEYDOWN:
    {
        // http://10.2.1.199/Bug.php?BugID=90979
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), POWERSAVIGNMEG_NOTIFY_OPERATE, 1, 0);
    }
    break;
    case PRESENCE_MESSAGE_FAVORITE_CHANGE:
    {
        // 清空时删除缓存区图片
        // http://10.2.1.199/Bug.php?BugID=104939
        if (refMessage.lParam == 1)
        {
            Image_ClearImageByImageType(DIT_NOCACHE);
        }
    }
    break;
    default:
    {

    }
    break;
    }

    // to do
    return 0;
}

void MainWndMessageHelp_Init()
{
    etl_RegisterMsgHandle(LANGUAGE_MESSAGE_LANGUAGE_CHANGED, LANGUAGE_MESSAGE_LANGUAGE_CHANGED,
                          MainWndMessageHelp);
    etl_RegisterMsgHandle(XSI_MESSAGE_REQ_FAIL, XSI_MESSAGE_REQ_FAIL, MainWndMessageHelp);
    etl_RegisterMsgHandle(SYS_MSG_UPDATE_FIRWARE, SYS_MSG_UPDATE_FIRWARE, MainWndMessageHelp);
    etl_RegisterMsgHandle(SYS_MSG_RESET_FACTORY, SYS_MSG_RESET_FACTORY, MainWndMessageHelp);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, MainWndMessageHelp);
    etl_RegisterMsgHandle(DSK_MSG_UPLOAD_CONFIG, DSK_MSG_UPLOAD_CONFIG, MainWndMessageHelp);
    etl_RegisterMsgHandle(SYS_MSG_DEVICE_REBOOT, SYS_MSG_DEVICE_REBOOT, MainWndMessageHelp);
    etl_RegisterMsgHandle(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVINGMEG_SCREEN_CLOSE, MainWndMessageHelp);
    etl_RegisterMsgHandle(POWERSAVINGMEG_SCREEN_AWAKE, POWERSAVINGMEG_SCREEN_AWAKE, MainWndMessageHelp);
    etl_RegisterMsgHandle(VOICE_CHANNEL_FORBDDEN, VOICE_CHANNEL_FORBDDEN, MainWndMessageHelp);
    etl_RegisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, MainWndMessageHelp);
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_FAVORITE_CHANGE, PRESENCE_MESSAGE_FAVORITE_CHANGE,
                          MainWndMessageHelp);
    etl_RegisterMsgHandle(USB_MSG_HEADSET_PLUG, USB_MSG_HEADSET_PLUG, MainWndMessageHelp);

    _UIWifiManagerHelp.InitMsg();
}

void MainWndMessageHelp_Uinit()
{
    etl_UnregisterMsgHandle(LANGUAGE_MESSAGE_LANGUAGE_CHANGED, LANGUAGE_MESSAGE_LANGUAGE_CHANGED,
                            MainWndMessageHelp);
    etl_UnregisterMsgHandle(XSI_MESSAGE_REQ_FAIL, XSI_MESSAGE_REQ_FAIL, MainWndMessageHelp);
    etl_UnregisterMsgHandle(SYS_MSG_UPDATE_FIRWARE, SYS_MSG_UPDATE_FIRWARE, MainWndMessageHelp);
    etl_UnregisterMsgHandle(SYS_MSG_RESET_FACTORY, SYS_MSG_RESET_FACTORY, MainWndMessageHelp);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, MainWndMessageHelp);
    etl_UnregisterMsgHandle(DSK_MSG_UPLOAD_CONFIG, DSK_MSG_UPLOAD_CONFIG, MainWndMessageHelp);
    etl_UnregisterMsgHandle(SYS_MSG_DEVICE_REBOOT, SYS_MSG_DEVICE_REBOOT, MainWndMessageHelp);
    etl_UnregisterMsgHandle(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVINGMEG_SCREEN_CLOSE,
                            MainWndMessageHelp);
    etl_UnregisterMsgHandle(POWERSAVINGMEG_SCREEN_AWAKE, POWERSAVINGMEG_SCREEN_AWAKE,
                            MainWndMessageHelp);
    etl_UnregisterMsgHandle(VOICE_CHANNEL_FORBDDEN, VOICE_CHANNEL_FORBDDEN, MainWndMessageHelp);
    etl_UnregisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, MainWndMessageHelp);
    etl_UnregisterMsgHandle(PRESENCE_MESSAGE_FAVORITE_CHANGE, PRESENCE_MESSAGE_FAVORITE_CHANGE,
                            MainWndMessageHelp);
    etl_UnregisterMsgHandle(USB_MSG_HEADSET_PLUG, USB_MSG_HEADSET_PLUG, MainWndMessageHelp);

    _UIWifiManagerHelp.UnInitMsg();
}
