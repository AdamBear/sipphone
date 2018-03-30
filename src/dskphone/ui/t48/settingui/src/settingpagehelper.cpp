#include "settingpagehelper.h"
#include "settingpage.h"
#include "cdlgbasesubpage.h"
#include "settingpage.h"
#include "cdlgpagemenu.h"
#include "commonapi/stringhelperapi.h"
#include "baseui/modbaseui.h"
#include "uikernel/languagehelper.h"
#include "setting/include/modmenu.h"
#include "uimanager/moduimanager.h"
#include "dsklog/log.h"
#include "settinguicommon.h"
#include "globalmodel.h"
#include "edkui/edkuimanager.h"
#include "uicommon/qtcommon/qmisc.h"
#include "account/include/modaccount.h"
#include "talk/fwd_dnd/include/modfwddnd.h"

// 内部函数定义
//创建page页内容
static CDlgBaseSubPage * CreatePage(const yl::string & strContent);

static int GetNewPageMenuID()
{
    static int iPagemenuID = 0;
    return iPagemenuID++;
}

CSettingPageHelper::CSettingPageHelper(CSettingWindow * pWnd)
    : m_pSettingWnd(pWnd)
{
}

CSettingPageHelper::~CSettingPageHelper()
{
    m_pSettingWnd = NULL;
}

void CSettingPageHelper::SetSettingWnd(CSettingWindow * pWnd)
{
    if (pWnd != NULL && m_pSettingWnd != pWnd)
    {
        m_pSettingWnd = pWnd;
    }
}

bool CSettingPageHelper::CreateSubPage(const SubMenuData & pageInfo,
                                       const yl::string strContent/* = ""*/)
{
    SETTINGUI_INFO("CSettingPageHelper::CreateSubPage [%s]", pageInfo.strMenuAction.c_str());
    if (NULL == m_pSettingWnd)
    {
        return false;
    }

    yl::string strAction = pageInfo.strMenuAction;

    //简单action用于调用CreatePage
    yl::string strSimpleAction = strContent;
    if (strSimpleAction.empty())
    {
        yl::string strActionType;

        if (!commonAPI_ParseActionInfo(&strActionType, &strSimpleAction, strAction))
        {
            SETTINGUI_ERR("CSettingPageHelper::CreateSubPage parse acion error! action: %s", strAction.c_str());
            return false;
        }
    }

    //创建显示页面
    CDlgBaseSubPage * pSubPage = CreatePage(strSimpleAction);
    if (pSubPage == NULL)
    {
        SETTINGUI_ERR("CSettingPageHelper::CreateSubPage CreatePage failed");
        if (m_pSettingWnd->IsEmpty())
        {
            //如果是新建的wnd，那么要释放资源
            UIManager_PutPrivateDlg(m_pSettingWnd);
        }
        return false;
    }

    //设置当前窗口的提示命令
    pSubPage->SetHintTitle(toQString(pageInfo.strMenuAction));

    pSubPage->BeforeSetSubPage(pageInfo.pBeforeSetSubpageData);
    pSubPage->AfterSetSubPage(pageInfo.pAfterSetSubpageData);

    // 刷新窗口
    m_pSettingWnd->SetSubPage(pSubPage, strAction, pageInfo.iFirstSelectItem);
    return true;
}

bool CSettingPageHelper::CreatePageMenu(const yl::string & strOpenPageAction,
                                        int nDefaultIndex /*= -1*/)
{
    SETTINGUI_INFO("CSettingPageHelper::CreatePageMenu [%s]", strOpenPageAction.c_str());
    if (NULL == m_pSettingWnd)
    {
        return false;
    }

    //用于取title名称
    yl::string strFullSubPageAction = strOpenPageAction;

    //创建显示页面
    yl::string strPageMenu = commonAPI_FormatString("%s_%d", DLG_CDlgPageMenu, GetNewPageMenuID());
    CDlgPageMenu * pPage = (CDlgPageMenu *)(UIManager_CreatePrivateDlg(DLG_CDlgPageMenu, strPageMenu));

    if (pPage == NULL)
    {
        SETTINGUI_ERR("CSettingPageHelper::CreatePageMenu CreatePage failed");
        if (m_pSettingWnd->IsEmpty())
        {
            //如果是新建的wnd，那么要释放资源
            UIManager_PutPrivateDlg(m_pSettingWnd);
        }
        return false;
    }

    //设置当前窗口的提示命令
    pPage->SetHintTitle(toQString(strOpenPageAction));
    //把显示页面加到显示框架中
    m_pSettingWnd->SetSubPage((CDlgBaseSubPage *)pPage, strFullSubPageAction, nDefaultIndex);

    return true;
}

//创建page页内容
static CDlgBaseSubPage * CreatePage(const yl::string & strContent)
{
    SETTINGUI_INFO("::CreatePage [%s]", strContent.c_str());
    // 根据字符串呼出相应的界面
    CBaseDialog * pWnd = NULL;
    if (strContent == "network")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAnonymousCall);
    }
    else if (strContent == "callControl")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgCallControl", DLG_CDlgCallControl);
    }
    else if (strContent == "callcontrol_lineselect")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgCallControlLineSelect);
    }
    else if (strContent == "anywhere")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgAnyWhereLocationList", DLG_CDlgAnyWhereList);
    }
    else if (strContent == "anywhere_new")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgAnyWhereLocationNew", DLG_CDlgAnyWhereNew);
    }
    else if (strContent == "anywhere_Edit")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgAnyWhereLocationEdit", DLG_CDlgAnyWhereEdit);
    }
    else if (strContent == "remote_office")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgRemoteOfficeNew", DLG_CDlgRemoteOfficeNew);
    }
    else if (strContent == "anonymous_rejection")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgAnonymousRejection", DLG_CDlgAnonymousRejection);
    }
    else if (strContent == "hide_numbers")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgHideNumber", DLG_CDlgHideNumber);
    }
    else if (strContent == "simultaneous_ring")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgSimultaneousRingNumbers", DLG_CDlgSimultaneousRingNumbers);
    }
    else if (strContent == "simultaneous_ring_new")
    {
        pWnd = UIManager_CreatePrivateDlg("CDlgSimultaneousRingNew", DLG_CDlgSimultaneousRingNew);
    }
    else if (strContent == "phone")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDoNotDisturb);
    }
    else if (strContent == "advanced_call_feature_auto_redial")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAutoRedial);
    }
    else if (strContent == "call_forward_always_forward")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAlwaysForward);
    }
    else if (strContent == "call_forward_busy_forward")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgBusyForward);
    }
    else if (strContent == "call_forward_no_answer_forward")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNOAnswerForward);
    }
    else if (strContent == "call_waiting")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgCallWaiting);
    }
    else if (strContent == "security_classification")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSecurityClassification);
    }
    else if (strContent == "dnd")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDoNotDisturb);
    }
    else if (strContent == "intercom")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgIntercom);
    }
    else if (strContent == "language")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgLanguage);
    }
    else if (strContent == "userMode")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAuthority);
    }
    else if (strContent == "date_and_time_format")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDateTimeFormat);
    }
    else if (strContent == "date_and_time_dhcp_time")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDHCPTime);
    }
    else if (strContent == "date_and_time_general")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDateTimeGeneral);
    }
    else if (strContent == "set_password")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSetPassword);
    }
    else if (strContent.find("anonymous_call_line") != yl::string::npos)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAnonymousCall);
    }
    else if (strContent == "screensaver")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSetScreenSaver);
    }
    else if (strContent == "transparency")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSetTransparency);
    }
    else if (strContent == "broadsoft_directory_item_1"
             || strContent == "broadsoft_directory_item_2"
             || strContent == "broadsoft_directory_item_3"
             || strContent == "broadsoft_directory_item_4"
             || strContent == "broadsoft_directory_item_5"
             || strContent == "broadsoft_directory_item_6")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgBroadsoftDirectoty);
    }
    else if (strContent == "broadsoft_call_log_item_1"
             || strContent == "broadsoft_call_log_item_2"
             || strContent == "broadsoft_call_log_item_3"
             || strContent == "broadsoft_call_log_item_4")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgBroadsoftCalllog);
    }
    else if (strContent == "advanced_call_feature_call_completion")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgCallCompletion);
    }
    else if (strContent == "advanced_call_feature_general")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgGeneral);
    }
    else if (strContent == "advanced_call_feature_hotline")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgHotline);
    }
    else if (strContent == "advanced_call_feature_default_account")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDefaultAccount);
    }
    else if (strContent.find("dss_key") != yl::string::npos)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDssKey);
    }
    else if (strContent == "status_general")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgStatusGeneral);
    }
    else if (strContent == "testmode_list")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgTestModeStatus);
    }
    else if (strContent == "status_network")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgStatusNetwork);
    }
    /*else if (strContent =="BluetoothDetail")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgBluetoothDetail);
    }*/
    /*else if (strContent == "bluetooth_scan_device_list")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgBluetoothScanDeviceList);
    }*/
    else if (strContent == "status_phone")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgStatusPhone);
    }
    else if ("status_rtp" == strContent)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgStatusRTP);
    }
    else if (strContent.find("line_key_item_") != yl::string::npos
             || strContent.find("memory_key_item_") != yl::string::npos
             || strContent.find("expand_key_item_") != yl::string::npos)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDssKey);
    }
    else if (strContent == "status_accounts" || strContent == "account_list")
    {
        //Status界面下的Accounts界面和Advance Setting界面下的Accounts界面用同一个界面（有点不同，基本一样）
        //用m_strSubPageAction判断Accounts界面是在哪个目录进入从而实现不同的Accounts界面
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgStatusAccounts);
    }
    else if (strContent == "auto_answer")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAutoAnswer);
    }
    else if (strContent.find("account_") != yl::string::npos)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAccount);
    }
    else if (strContent == "network_wvlan")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetwork_VLAN);
        if (pWnd != NULL)
        {
            int iPageType = 0;
            CDlgBaseSubPage * pSubPage = qobject_cast<CDlgBaseSubPage *>(pWnd);
            if (NULL != pSubPage)
            {
                pSubPage->BeforeSetSubPage(&iPageType);
            }
        }
    }
    else if (strContent == "network_pcvlan")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetwork_VLAN);
        if (pWnd != NULL)
        {
            int iPageType = 1;
            CDlgBaseSubPage * pSubPage = qobject_cast<CDlgBaseSubPage *>(pWnd);
            if (NULL != pSubPage)
            {
                pSubPage->BeforeSetSubPage(&iPageType);
            }
        }
    }
    /*else if (strContent == "network_pc_port")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkPCPort);
    }*/
    else if (strContent == "network_router")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkRouter);
    }
    else if (strContent == "network_dhcp_server")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkDHCPServer);
    }
    else if (strContent == "network_wweb_server")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkWebServer);
    }
    else if (strContent == "network_wvpn")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetwork_VPN);
    }
    else if (strContent == "network_lldp")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetwork_LLDP);
    }
    else if (strContent == "network_dhcpvlan")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetwork_VLANDHCP);
    }
    else if (strContent == "network_w802.1x")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkX_Mode);
    }
    else if (strContent == "network_wan_port")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkWANPort);
    }
    else if (strContent == "network_nat")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkNAT);
    }
    else if (strContent == "auto_provision")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAutoProvision);
    }
//  else if (strContent == "redirector")
//  {
//      pWnd = UIManager_CreatePrivateDlg(DLG_CDlgRedirector);
//  }
    else if (strContent == "set_aes_key")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSetAESKey);
    }
    else if (strContent == "FWD_international")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgFWDInternational);
    }
    else if (strContent == "door_phone_operation"
             || strContent == "door_phone_setting_1"
             || strContent == "door_phone_setting_2")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDoorPhone);
    }
    else if (strContent == "ip_camera_operation"
             || strContent == "ip_camera_setting_1"
             || strContent == "ip_camera_setting_2")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgIpCamera);
    }
    else if (strContent == "sound_general")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSoundGeneral);
    }
    else if (strContent == "sound_ringtone")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgRingtone);
    }
    else if ("blf_ring_type" == strContent)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSetAccountRingtone);
    }
    else if (strContent.find("accountsound_ringtone_") != yl::string::npos)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSetAccountRingtone);
    }
    //  else if (strContent == "sound_volume")
    //  {
    //      pWnd = UIManager_CreatePrivateDlg(DLG_C);
    //  }
    else if (strContent == "display_general")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDisplayGeneral);
    }
    else if (strContent == "display_wallpaper")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDisplayWallpaper);
    }
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    else if (strContent == "display_dsskey_wallpaper")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDisplayWallpaper);
    }
#endif
    else if (strContent == "display_theme")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDisplayTheme);
    }
    else if (strContent == "display_screen_saver")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDisplayScreenSaver);
    }
    else if (strContent == "display_photo_manager")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDisplayPhoto);
    }
    else if (strContent == "view_voice_mail")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgViewVoiceMail);
    }
    else if (strContent == "set_voice_mail")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSetVoiceMail);
    }
    else if (strContent == "reset_config")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgResetToFactory);
    }
    else if (strContent == "basic_reset_config")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgResetToFactory);
    }
    else if (strContent == "reboot_page")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgRebootPage);
    }
    else if (strContent == "update_page")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgUpdateConfigurationPage);
    }
    else if (strContent == "reset_default")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgResetLocalConfig);
    }
    else if (strContent == "video_global"
             || strContent.find("video_line") != yl::string::npos)
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgVideoSetting);
    }
    else if (strContent == "disposition_code")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgACDDispositionCode);
    }
    else if (strContent == "emergency_escalation")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgACDEscalateTo);
    }
    else if (strContent == "initial_state")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgACDInitialState);
    }
    else if (strContent == "unavailable_reason")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgACDUnavailableReason);
    }
    else if (strContent == "acd_status_select")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgACDStatusSelect);
    }
    else if (strContent == "hoteling_login")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgHotelingLogin);
    }
    else if (strContent == "call_forward_for_custom")
    {
#ifdef IF_FEATURE_FORWARD_OPT
        LPCSTR strPageName = (acc_GetAvailableAccountCount() == 1 || FWDDND_MODE_GLOBAL == fwd_GetMode())
                             ? DLG_CDlgCustomForwardOption : DLG_CDlgCustomForward;
        pWnd = UIManager_CreatePrivateDlg(strPageName);
#else
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgCustomForward);
#endif
    }
    else if (strContent == "call_forward_option")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgCustomForwardOption);
    }
    else if (strContent == "status_ip_mode_status")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgIPModeStatus);
    }
    else if (strContent == "network_wan_port_option")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgWanPortOption);
    }
    else if (strContent == "vlancdp")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgNetworkVlanCDP);
    }
    else if (strContent == "dnd_custom")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDNDCustom);
    }
    else if (strContent == "unlock_pin")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgUnlockPIN);
    }
    else if (strContent == "check_key_guard_lock")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgCheckKeyGuardLock);
    }
    else if (strContent == "key_guard_lock")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgKeyGuardLock);
    }
    else if (strContent == "contrast")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgContrast);
    }
    else if (strContent == "bluetooth")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgBluetoothBase);
    }
    else if (strContent == "wifi")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgWifiBase);
    }
    else if (strContent == "wifiaddedit")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgWifiAddEdit);
    }
    else if (strContent == "wifistatus")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgWifiStatus);
    }
    else if (strContent == "wifistorage")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgWifiStorage);
    }
    else if (strContent == "wifidetail")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgWifiDetail);
    }
    else if (strContent == "enter_password")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAskPassword);
    }
    else if (strContent == "hotdesk_base")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgHotDeskBase);
    }
    else if (strContent == "my_status")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMyStatus);
    }
    else if (strContent == "dsskey_frame")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgDsskeyFrame);
    }
    else if (strContent == "my_device_information")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgBluetoothDetail);
    }
    else if (strContent == "multicast_paging_list")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMulticastPaingList);
    }
    else if (strContent == "edit_paging_group")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMulticastPagingGroupEdit);
    }
    else if (strContent == "silent_alerting")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgSilentAlerting);
    }
#ifdef IF_FEATURE_MULTIFWD
    else if (strContent == "MultiForward")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMultiForward);
    }
#endif
    //text_message
    else if (strContent == "text_message_inbox")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMessageFrame, "CDlgMessageFrame_inbox");
    }
    else if (strContent == "text_message_sentbox")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMessageFrame, "CDlgMessageFrame_sentbox");
    }
    else if (strContent == "text_message_outbox")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMessageFrame, "CDlgMessageFrame_outbox");
    }
    else if (strContent == "text_message_draftbox")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMessageFrame, "CDlgMessageFrame_draftbox");
    }
    else if (strContent == "text_message_edit_message")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMessageEdit);
    }
    else if (strContent == "text_message_view_message")
    {
        pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMessageView);
    }
    //ping 网络
#ifndef  WIN32
    else
#endif
        if (strContent == "networkping")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgPingAndTracerouteUI);
        }
    //traceroute 网络
        else if (strContent == "networktraceroute")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgPingAndTracerouteUI);
        }
#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
        else if (strContent == "GroupNightForward")
        {
            pWnd = UIManager_CreatePrivateDlg("CDlgGroupNightFWD", DLG_CDlgGroupNightFWD);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
        else if (strContent == "Mobility")
        {
            pWnd = UIManager_CreatePrivateDlg("CDlgMobility", DLG_CDlgMOBILITY);
        }
#endif

#if IF_BUG_23826
        else if (strContent == "autop_code_authorized")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CCDlgAutoPCodeAuthorized);
        }
#endif
#ifdef IF_FEATURE_GETBALANCE
        else if (strContent == "get_balance")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgGetBalance);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
        else if (strContent == "bsft_callwaiting")
        {
            pWnd = UIManager_CreatePrivateDlg("CDlgBSFTCallWaiting", DLG_CDlgBSFTCallWaiting);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
        else if (strContent == "call_record")
        {
            pWnd = UIManager_CreatePrivateDlg("CDlgCallRecord", DLG_CDlgCallRecord);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
        else if (strContent == "exe_common_setting")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgExeAssisCommon);
        }
        else if (strContent == "executive_details")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgExeAssisDetails);
        }
        else if (strContent == "exe_filter_setting")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgExeAssisFilter);
        }
        else if (strContent == "exe_screen_setting")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgExeAssisScreen);
        }
        else if (strContent == "exe_alter_setting")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgExeAssisAlter);
        }
        else if (strContent == "exe_listview_setting")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgExeAssisListView);
        }
#endif
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
        else if (strContent == "browse_usb_record")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgUSBBrowseRecord);
        }
        else if (strContent == "browse_usb_photo")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgUSBBrowsePhoto);
        }
        else if (strContent == "browse_usb_photo_detail")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgUSBPhotoDetail);
        }
        else if (strContent == "browse_usb_storage")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgUSBBrowseStorage);
        }
        else if (strContent == "usb_idle_record")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgUSBIdleRecord);
        }
#endif
#if IF_FEATURE_BROADSOFT_VOICEMAIL
        else if (strContent == "visual_voice_mail_list")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgVisualVoiceMailList);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
        else if (strContent == "forward_selective")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgForwardSelective);
        }
#endif
#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
        else if (strContent == "feature_auto_anwser_timer")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgAutoAnswerTimer);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
        else if (strContent == "Call_Forwarding_Not_Reachable")
        {
            pWnd = UIManager_CreatePrivateDlg("CDlgFWDNotReach", DLG_CDlgFWDNotReach);
        }
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
        else if (strContent == "metaswitch_voice_mail_list")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMTSWVoicemail);
        }
#endif
#if IF_FEATURE_METASWITCH_ACD
        else if (strContent == "mtswacdmlghlist")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMTSWAcdList);
        }
        else if (strContent == "mtswacdqueuedetail")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMTSWAcdDetail);
        }
        else if (strContent == "mtswacdmemberlist")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMTSWAcdMembers);
        }
        else if (strContent == "mtswacdselectdispcode")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMTSWAcdSelectDispCode);
        }
        else if (strContent == "mtswacdmystate")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgMTSWAcdChangeState);
        }
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
        else if (strContent == "xmlbrowser_hoteling_login")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgHotelingLogin);
        }
#endif
#if IF_FEATURE_GENBAND_E911
        else if (strContent == "e911_location")
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgE911Location);
        }
        else if (strContent.find("e911_set_location_") != yl::string::npos)
        {
            pWnd = UIManager_CreatePrivateDlg(DLG_CDlgE911SetLocation);
        }
#endif
        else if (strContent == "xxx")// to do: add all SubPage
        {
        }

    if (NULL == pWnd)
    {
        return NULL;
    }
    else
    {
        CDlgBaseSubPage * pSubPage = qobject_cast<CDlgBaseSubPage *>(pWnd);
        if (pSubPage == NULL)
        {
            UIManager_PutPrivateDlg(pWnd);
            return NULL;
        }
        else
        {
            return pSubPage;
        }
    }
}
