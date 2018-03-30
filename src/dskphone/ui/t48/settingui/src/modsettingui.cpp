#include "modsettingui.h"

#include "cdlgunlockpin.h"
#include "cdlgalwaysforward.h"
#include "cdlglanguage.h"
#include "dlgdisplaywallpaper.h"
#include "cdlgdndcustom.h"
#include "cdlgdonotdisturb.h"
#include "cdlgaccount.h"
#include "cdlgbusyforward.h"
#include "cdlgnoanswerforward.h"
#include "cdlgcustomforward.h"
#include "cdlgcustomforwardoption.h"
#include "dlganonymouscall.h"
#include "cdlgdatetimegeneral.h"
#include "cdlgautoanswer.h"
#include "cdlgintercom.h"
#include "cdlgcallwaiting.h"
#include "dlghotline.h"
#include "cdlgautoredial.h"
#include "cdlgdefaultaccount.h"
#include "dlggeneral.h"
#include "dlgcallcompletion.h"
#include "dlgdisplaygeneral.h"
#include "dlgsoundgeneral.h"
#include "dlgringtone.h"
#include "dlgsetaccountringtone.h"
#include "cdlgdhcptime.h"
#include "cdlgdatetimeformat.h"
#include "cdlgremoteoffice.h"
#include "cdlgcallcontrol.h"
#include "cdlgmystatus.h"
#include "cdlgstatusgeneral.h"
#include "dlgstatusphone.h"
#include "dlgstatusaccounts.h"
#include "dlgstatusnetwork.h"
#include "dlgstatusrtp.h"

#include "cdlganonymousrejection.h"
#include "cdlghidenumber.h"

#include "cdlganywherelocationlist.h"
#include "cdlganywherelocationnew.h"
#include "cdlganywherelocationedit.h"

#include "cdlgbluetoothdetail.h"
#include "cdlgaskpassword.h"
#include "dlgbluetoothbase.h"

#include "cdlgsimultaneousringnumbers.h"
#include "cdlgsimultaneousringnew.h"
#include "cdlgacdinitialstate.h"
#include "cdlgacdstatusselect.h"
#include "cdlgacdunavailablereason.h"
#include "cdlgacdescalateto.h"
#include "cdlgacddispositioncode.h"
#include "cdlghotelinglogin.h"
#include "dlgviewvoicemail.h"
#include "dlgsetvoicemail.h"

#include "dlgcontrast.h"
#include "cdlgkeygaurdlock.h"
#include "cdlgwanportoption.h"
#include "dlgnetworkwanport.h"
#include "cdlgnetwork_vpn.h"
#include "cdlgnetworkx_mode.h"
#include "cdlgnetworkwebserver.h"
#include "cdlgnetwork_vlan.h"
//#include "cdlgnetworkpcport.h"
#include "cdlgnetworkrouter.h"
#include "cdlgnetworkdhcpserver.h"
#include "cdlgnetwork_lldp.h"
#include "cdlgnetwork_vlandhcp.h"
#include "cdlgsetpassword.h"
#include "dlgsetaeskey.h"
#include "dlgautoprovision.h"
#include "cdlgfwdinternational.h"
#include "cdlgresettofactory.h"
#include "cdlgrebootpage.h"
#include "cdlgresetlocalconfig.h"
#include "cdlgipmodestatus.h"
#include "cdlgdsskeyframe.h"
#include "cdlgdsskey.h"
#include "cdlgpagemenu.h"
#include "cdlgtestmodestatus.h"
#include "dlgcheckkeyguardlock.h"
#include "cdlgupdateconfigurationpage.h"
//故障处理
#include "dlgpingandtracerouteui.h"

#include "setting/include/modmenu.h"
// #include "define/bluetoothmessagedefine.h"
#include "settingui/include/modsettingui.h"

#include "wifiui/src/dlgwifiaddedit.h"
#include "wifiui/src/dlgwifibase.h"
#include "wifiui/src/dlgwifidetail.h"
#include "wifiui/src/dlgwifistatus.h"
#include "wifiui/src/dlgwifistorage.h"

#include "cdlghotdeskbase.h"
#include "cdlgauthority.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "setting/src/resetandrebootcontroller.h"
#include "setting/src/voicemailcontroller.h"
#include "dlgnetworkvlancdp.h"
#include "cdlgnetworknat.h"
#include "settingui/src/settinguidsskeyaction.h"
#include "dlgsetscreensaver.h"
#include "dlgsettransparency.h"

#if IF_BUG_23826
#include "dlgautopcodeauthorized.h"
#endif
#include "commonapi/stringhelperapi.h"
#include "settingpage.h"
#include "settingui/src/settinguifeaturesdefine.h"
#include "settingui/src/settinguicommon.h"
#include "adapterbox/include/modadapterbox.h"
#include "dlgmulticastpaginglist.h"
#include "dlgeditmulticastpaginggroup.h"
#include "cdlgcallcontrollineselect.h"
#include "cdlgsecurityclassification.h"

#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
#include "cdlggroupnightfwd.h"
#endif
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
#include "cdlgmobility.h"
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
#include "cdlgbsftcallwaiting.h"
#endif
#ifdef IF_FEATURE_MULTIFWD
#include "cdlgmultiforward.h"
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
#include "cdlgcallrecord.h"
#endif
#include <configserver/configserverinc.h>
#include "cdlgexeassiscommon.h"
#include "cdlgexeassisdetails.h"
#include "cdlgexeassisfilter.h"
#include "cdlgexeassisscreen.h"
#include "cdlgexeassisalter.h"
#include "cdlgexeassislistview.h"

#ifdef IF_FEATURE_GETBALANCE
#include "cdlggetbalance.h"
#endif

#include "edkui/edkmapdefine.h"
#include "edkui/modedkui.h"
#include "settingui/include/modsettinguiopenlogic.h"
#include "feature/include/modfeature.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "contacts/directory/include/moddirinterface.h"
#include "cdlgsilentalerting.h"

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "dlgusbbrowserecord.h"
#include "dlgusbbrowsestorage.h"
#include "dlgusbbrowsephoto.h"
#include "dlgusbidlerecord.h"
#include "dlgusbphotodetail.h"
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitchui/dlgmtswvoicemail.h"
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
#include "cdlgfwdnotreach.h"
#endif

#if IF_FEATURE_METASWITCH_ACD
#include "metaswitchui/cdlgmtswacdlist.h"
#include "metaswitchui/cdlgmtswacddetail.h"
#include "metaswitchui/cdlgmtswacdmembers.h"
#include "metaswitchui/cdlgmtswacdchangestate.h"
#include "metaswitchui/cdlgmtswacdselectdispcode.h"
#endif

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "dlgvisualvoicemaillist.h"
#endif

#if IF_FEATURE_GENBAND_E911
#include "dlge911location.h"
#include "dlge911setlocation.h"
#include "e911locationcontroller.h"
#endif

// IF_BUG_30885
#include "autopcontrol/include/modautopcontrol.h"
#include "setting/include/modzero.h"
#include "zerosptouchui/include/modzerosptouch.h"
#include "cdlgforwardselective.h"
#include "cdlgautoanswertimer.h"

/*************************************************************/
static int s_iSettingWndId = 0;
static int GetNewSettingWndID()
{
    return s_iSettingWndId++;
}
static int GetCurrentSettingWndID()
{
    return s_iSettingWndId - 1;
}

///////////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 接口  ： SettingUI_InitDsskeyImplActions()                                 */
/* 功能  ： 注册dsskey处理类                                             */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void SettingUI_InitDsskeyImplActions()
{
    static bool bInited = false;
    if (!bInited)
    {
        new CStatusAction();
        new CMenuAciton();
        new CDssKeyZero();
        new CDssKeySMS();
        new CDssKeyNewSMS();
        new CDssKeyPagingList();
#if IF_FEATURE_METASWITCH
        new CDssKeyMTSWLogIn();
        new CDssKeyMTSWLogout();
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
        new CDssKeyMessageList();
#endif
        bInited = true;
    }
}

void SettingUI_RegisterDlgClass()
{
    SettingUI_InitDsskeyImplActions();
#if IF_FEATURE_GENBAND_E911
    g_E911Controller;
#endif

    REGISTER_DLG(CDlgUnlockPIN);           //UnlockPIN
    REGISTER_DLG(CDlgAlwaysForward);       //AlwaysForward
    REGISTER_DLG(CDlgLanguage);            //Language
    REGISTER_DLG(CDlgDisplayWallpaper);    //Wallpaper
    REGISTER_DLG(CDlgDNDCustom);           //Custom_DND
    REGISTER_DLG(CDlgDoNotDisturb);        //Always_DND
    REGISTER_DLG(CDlgAccount);             //Account
    REGISTER_DLG(CDlgBusyForward);         //BusyForward
    REGISTER_DLG(CDlgNoAnswerForward);     //NoAnsForward
    REGISTER_DLG(CDlgCustomForward);       //CustomForward
    REGISTER_DLG(CDlgCustomForwardOption); //CustomForwardOption
    REGISTER_DLG(CDlgAnonymousCall);       //AnonymousCall匿名
    REGISTER_DLG(CDlgDateTimeGeneral);     //DateTimeGeneral
    REGISTER_DLG(CDlgAutoAnswer);          //AutAnswer
    REGISTER_DLG(CDlgIntercom);            //Intercom
    REGISTER_DLG(CDlgCallWaiting);         //CallWaiting
    REGISTER_DLG(CDlgHotline);             //Hotline
    REGISTER_DLG(CDlgAutoRedial);          //AutoRedial
    REGISTER_DLG(CDlgDefaultAccount);      //DefaultAccount
    REGISTER_DLG(CDlgGeneral);             //General
    REGISTER_DLG(CDlgCallCompletion);      //CallCompletion
    REGISTER_DLG(CDlgDisplayGeneral);      //DisplayGeneral
    REGISTER_DLG(CDlgSoundGeneral);        //SoundGeneral
    REGISTER_DLG(CDlgRingtone);            //Ringtone
    REGISTER_DLG(CDlgSetAccountRingTone);
    REGISTER_DLG(CDlgDHCPTime);            //DHCP Time
    REGISTER_DLG(CDlgDateTimeFormat);      //DateTimeFormat
    REGISTER_DLG(CDlgRemoteOfficeNew);     //RemoteOfficeNew
    REGISTER_DLG(CDlgCallControl);         //CallControl
    REGISTER_DLG(CDlgMyStatus);            //MyStatus
    REGISTER_DLG(CDlgStatusGeneral);       //StatusGeneral
    REGISTER_DLG(CDlgIPModeStatus);         //CDlgIPModeStatus
    REGISTER_DLG(CDlgStatusPhone);         //StatusPhone
    REGISTER_DLG(CDlgStatusAccounts);      //StatusAccounts
    REGISTER_DLG(CDlgStatusNetwork);       //StatusNetwork
    REGISTER_DLG(CDlgStatusRTP);         //CDlgStatusRTP
    REGISTER_DLG(CDlgTestModeStatus);       //TestModeStatus

    REGISTER_DLG(CDlgCallControlLineSelect); //CallControlLineSelect
    REGISTER_DLG(CDlgSilentAlerting)

    REGISTER_DLG(CDlgAnyWhereLocationList);     //AnyWhereList
    REGISTER_DLG(CDlgAnyWhereLocationNew);
    REGISTER_DLG(CDlgAnyWhereLocationEdit);

    REGISTER_DLG(CDlgBluetoothDetail);     //BluetoothDetail
    REGISTER_DLG(CDlgAskPassword);
    REGISTER_DLG(CDlgBluetoothBase);

    REGISTER_DLG(CDlgWifiAddEdit);
    REGISTER_DLG(CDlgWifiBase);
    REGISTER_DLG(CDlgWifiDetail);
    REGISTER_DLG(CDlgWifiStatus);
    REGISTER_DLG(CDlgWifiStorage);

    REGISTER_DLG(CDlgSimultaneousRingNumbers); //SimultaneousRingNumbers
    REGISTER_DLG(CDlgSimultaneousRingNew);  //CDlgSimultaneousRingNew
    REGISTER_DLG(CDlgACDInitialState);
    REGISTER_DLG(CDlgACDStatusSelect);
    REGISTER_DLG(CDlgACDUnavailableReason);
    REGISTER_DLG(CDlgACDEscalateTo);
    REGISTER_DLG(CDlgACDDispositionCode);
    REGISTER_DLG(CDlgHotelingLogin);

    REGISTER_DLG(CDlgContrast);            //Contrast
    REGISTER_DLG(CDlgKeyGuardLock);        //KeyGuardLock
    REGISTER_DLG(CDlgWanPortOption);       //WanPortOption
    REGISTER_DLG(CDlgNetworkWANPort);      //NetworkWANPort
    REGISTER_DLG(CDlgNetwork_VPN);         //Network_VPN
    REGISTER_DLG(CDlgNetworkX_Mode);       //NetworkX_Mode
    REGISTER_DLG(CDlgNetworkWebServer);    //NetworkWebServer
    REGISTER_DLG(CDlgNetwork_VLAN);        //Network_VLAN
    //REGISTER_DLG(CDlgNetworkPCPort);       //NetworkPCPort
    REGISTER_DLG(CDlgNetworkRouter);       //NetworkRouter
    REGISTER_DLG(CDlgNetworkDHCPServer);   //DHCPServer
    REGISTER_DLG(CDlgSetPassword);         //SetPassword
    REGISTER_DLG(CDlgSetAESKey);           //SetAESKey
    REGISTER_DLG(CDlgAutoProvision);       //AutoProvision
    REGISTER_DLG(CDlgFWDInternational);    //FWDInternational
    REGISTER_DLG(CDlgResetToFactory);      //ResetToFactory
    REGISTER_DLG(CDlgRebootPage);          //
    REGISTER_DLG(CDlgResetLocalConfig);      //Reset local configration
    REGISTER_DLG(CDlgUpdateConfigurationPage);

    REGISTER_DLG(CDlgViewVoiceMail);       //ViewVoiceMail
    REGISTER_DLG(CDlgSetVoiceMail);        //SetVoiceMail
    REGISTER_DLG(CDlgDsskeyFrame);         //DlgDsskeyFrame
    REGISTER_DLG(CDlgDssKey);              //DlgDssKeySetting

    REGISTER_DLG(CDlgHotDeskBase);         //HotDeskBase
    REGISTER_DLG(CDlgAuthority);         //CDlgAuthority
    REGISTER_DLG(CDlgCheckKeyGuardLock); //CDlgCheckKeyGuardLock
    REGISTER_DLG(CDlgSecurityClassification);

    REGISTER_DLG(CSettingWindow);
    REGISTER_DLG(CDlgPageMenu);
    REGISTER_DLG(CDlgNetwork_LLDP);
    REGISTER_DLG(CDlgNetwork_VLANDHCP);

    REGISTER_DLG(CDlgNetworkVlanCDP);
    REGISTER_DLG(CDlgNetworkNAT);

    REGISTER_DLG(CDlgMulticastPaingList);
    REGISTER_DLG(CDlgMulticastPagingGroupEdit);

    //故障处理
    REGISTER_DLG(CDlgPingAndTracerouteUI);

    REGISTER_DLG(CDlgSetScreenSaver);
    REGISTER_DLG(CDlgSetTransparency);

#ifdef IF_FEATURE_BROADSOFT_MOBILITY
    REGISTER_DLG(CDlgMobility);
#endif

#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
    REGISTER_DLG(CDlgGroupNightFWD);
#endif

#if IF_BUG_23826
    REGISTER_DLG(CDlgAutoPCodeAuthorized);
#endif

    REGISTER_DLG(CDlgAnonymousRejection);
    REGISTER_DLG(CDlgHideNumber);

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    REGISTER_DLG(CDlgBSFTCallWaiting);
#endif
#ifdef IF_FEATURE_MULTIFWD
    REGISTER_DLG(CDlgMultiForward);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    REGISTER_DLG(CDlgCallRecord);
#endif


#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    REGISTER_DLG(CDlgExeAssisCommon);
    REGISTER_DLG(CDlgExeAssisDetails);
    REGISTER_DLG(CDlgExeAssisFilter);
    REGISTER_DLG(CDlgExeAssisScreen);
    REGISTER_DLG(CDlgExeAssisAlter);
    REGISTER_DLG(CDlgExeAssisListView);
#endif

#ifdef IF_FEATURE_GETBALANCE
    REGISTER_DLG(CDlgGetBalance);
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    REGISTER_DLG(CDlgUSBBrowseRecord);
    REGISTER_DLG(CDlgUSBBrowsePhoto);
    REGISTER_DLG(CDlgUSBPhotoDetail);
    REGISTER_DLG(CDlgUSBBrowseStorage);
    REGISTER_DLG(CDlgUSBIdleRecord);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
    REGISTER_DLG(CDlgForwardSelective);
#endif

#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
    REGISTER_DLG(CDlgAutoAnswerTimer);
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
    REGISTER_DLG(CDlgFWDNotReach);
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
    REGISTER_DLG(CDlgMTSWVoicemail);
#endif

#if IF_FEATURE_METASWITCH_ACD
    REGISTER_DLG(CDlgMTSWAcdList);
    REGISTER_DLG(CDlgMTSWAcdDetail);
    REGISTER_DLG(CDlgMTSWAcdMembers);
    REGISTER_DLG(CDlgMTSWAcdChangeState);
    REGISTER_DLG(CDlgMTSWAcdSelectDispCode);
#endif

#if IF_FEATURE_BROADSOFT_VOICEMAIL
    REGISTER_DLG(CDlgVisualVoiceMailList);
#endif
#if IF_FEATURE_GENBAND_E911
    REGISTER_DLG(CDlgE911Location);
    REGISTER_DLG(CDlgE911SetLocation);
#endif
}

void SettingUI_OpenSetVoiceMailPage(int nAccountId)
{
    //跳转到Set Voice Mail界面
    SettingUI_OpenPage(SubMenuData("OpenSubPage(set_voice_mail)", nAccountId));
}

void SettingUI_OpenViewVoiceMailPage(int nAccountId /*= -1*/)
{
    SettingUI_OpenPage(SubMenuData("OpenSubPage(view_voice_mail)", -1));
}

bool SettingUI_VoiceMailCallOut(int nAccountId)
{
    VoiceMailInfoListData listData;
    //语音信箱号码是否设置
    bool bIsSetVoiceMailNumber = false;
    if (g_pVoiceMailController->GetInfoList(&listData))
    {
        YLList<VoiceMailInfoData>::iterator it = listData.m_listInfo.begin();
        YLList<VoiceMailInfoData>::iterator it_end = listData.m_listInfo.end();
        for (int i = 0; it != it_end; i++, ++it)
        {
            if (nAccountId == i)
            {
                VoiceMailInfoData & infoData = *it;
                if (infoData.m_strMailNumber == "")
                {
                    bIsSetVoiceMailNumber = false;
                }
                else
                {
                    bIsSetVoiceMailNumber = true;
                }
                break;
            }
        }
    }

    if (bIsSetVoiceMailNumber)
    {
        //调用控制层：呼叫语音信箱号码
        g_pVoiceMailController->ClickInfoByIndex(nAccountId);
    }
    else
    {
        SettingUI_OpenSetVoiceMailPage(nAccountId);
    }

    return true;
}

//打开查看TestMode Status界面
bool SettingUI_OpenTestModeStatus()
{
    return SettingUI_OpenPage("testmode_list");
}

// 打开Status界面
bool SettingUI_OpenStatus()
{
    return SettingUI_OpenPage(SubMenuData("OpenMenu(status_list.xml)"), true);
}

// 退出Status general界面
bool SettingUI_ExitStatus()
{
    return false;
}

void MsgBoxCallBackConfirmToReboot(CMessageBoxBase * pMsgBox)
{
    if (NULL != pMsgBox
            && pMsgBox->GetResult() == MessageBox_OK)
    {
        // 重启话机
        AdapterBox_Reboot(0);
    }
}

void MsgBoxCallBackConfirmToUpdate(CMessageBoxBase * pMsgBox)
{
    if (NULL != pMsgBox
            && pMsgBox->GetResult() == MessageBox_OK)
    {
        //更新aotop
        Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 0, 1);
    }
}

//确认是否Reboot
void SettingUI_ConfirmToReboot()
{
    MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(MsgBoxCallBackConfirmToReboot),
                                LANG_TRANSLATE(TRID_IF_REBOOT_THE_PHONE),
                                MESSAGEBOX_OkCancel);
}

void MsgBoxCallBackConfirmToResetLocal(CMessageBoxBase * pMsgBox)
{
    if (NULL != pMsgBox
            && pMsgBox->GetResult() == MessageBox_OK)
    {
        //msgPostMsgToThread(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_CLEAR_USERCFG, 0, 0);
        //发消息给configServer
        if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
        {
            msgSendMsgToThread(msgGetThreadByName(VP_AP_THREAD_NAME), ATP_MSG_REQ_CLEAR_CFG, 0, 0);
        }
        else
        {
            msgSendMsgToThread(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_CLEAR_USERCFG, 0, 0);
        }

        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_RESETLOCAL_PLEASE_WAIT),
                                    MESSAGEBOX_NOTE, 1500);
    }
}

//确认是否update
void SettingUI_ConfirmToUpdate()
{
    MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(MsgBoxCallBackConfirmToUpdate),
                                LANG_TRANSLATE(TRID_IF_PROVISION_NOW),
                                MESSAGEBOX_OkCancel);
}

bool SettingUI_OpenDsskeyConfigPage(int iDsskeyIndex)
{
    QString strMenuAction = "OpenSubPage(line_key_item_" + QString::number(iDsskeyIndex) + ")";
    SettingUI_OpenPage(fromQString(strMenuAction));
    return true;
}

//确认是否删除本地mac-local
void SettingUI_ConfirmToResetLocal()
{
    MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(MsgBoxCallBackConfirmToResetLocal),
                                LANG_TRANSLATE(TRID_IF_RESET_LOCAL_SETTING),
                                MESSAGEBOX_OkCancel);
}

void MsgBoxCallBackPopPasswordDlg(CMessageBoxBase * pMsgBox)
{
    if (NULL == pMsgBox
            || !pMsgBox->GetExtraData().canConvert<CPasswordDlgData>())
    {
        return;
    }

    CPasswordDlgData passwordData = pMsgBox->GetExtraData().value<CPasswordDlgData>();
    if (!passwordData.m_extraData.canConvert<int>())
    {
        return;
    }

    int iLineID = passwordData.m_extraData.value<int>();
    QString strUserIDTitle;
    QString strPwdTitle;
    yl::string strUserID;

    bool bIsAllowSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);
    if (bIsAllowSipAuth)
    {
        strUserIDTitle = LANG_TRANSLATE(TRID_SIP_REGISTER_NAME);
        strPwdTitle = LANG_TRANSLATE(TRID_SIP_PASSWORD);
        strUserID = configParser_GetCfgItemStringValue(kszAccountRegisterName, iLineID);
    }
    else
    {
        strUserIDTitle = LANG_TRANSLATE(TRID_USER_ID);
        strPwdTitle = LANG_TRANSLATE(TRID_PASSWORD);
        strUserID = configParser_GetCfgItemStringValue(kszAccountXSIUserID, iLineID);
    }

    QString strUtf8UserID = toQString(strUserID);

    MessageBox_ShowXSIPasswordBox(passwordData.m_pMsgBoxCallBack, strUserIDTitle,
                                  strUtf8UserID, strPwdTitle, MSGBOX_ID_POP_XSI_PWD);
}

// 弹出需要输入密码界面，并设置OK,CANCEL处理函数
void SettingUI_PopPasswordDlg(CMsgBoxCallBackAction * pMsgBoxCallBack, int nLineId)
{
    SETTINGUI_INFO("SettingUI_PopPasswordDlg [%d]", nLineId);

    bool bIsAllowSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

    CPasswordDlgData passwordData;
    passwordData.m_pMsgBoxCallBack = pMsgBoxCallBack;
    passwordData.m_extraData = QVariant::fromValue(nLineId);

    if (bIsAllowSipAuth)
    {
        //弹出保存成功的提示框
        MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(MsgBoxCallBackPopPasswordDlg),
                                    LANG_TRANSLATE(TRID_SIP_CREDENTIAL_PASSWORD_IS_WRONG),
                                    MESSAGEBOX_NOTE, 1500, "", "", -1,
                                    QVariant::fromValue(passwordData));
    }
    else
    {
        //弹出保存成功的提示框
        MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(MsgBoxCallBackPopPasswordDlg),
                                    LANG_TRANSLATE(TRID_WEB_ACCESS_PASSWORD_IS_WRONG),
                                    MESSAGEBOX_NOTE, 1500, "", "", -1,
                                    QVariant::fromValue(passwordData));
    }
}


bool SettingUI_EnterPageByType(const int ePageType, int nIndex)
{
    return Menu_EnterPageByType(ePageType, nIndex, false);
}

bool SettingUI_PageExist(const yl::string & strAction)
{
    SettingWndVec listWnd = CSettingWindow::GetSettingWndList();
    SettingWndVec::iterator iter = listWnd.begin();
    for (; iter != listWnd.end(); ++iter)
    {
        CSettingWindow * pWnd = *iter;
        if (pWnd == NULL)
        {
            continue;
        }

        yl::string strContent = pWnd->GetPageAction();

        CDlgBaseSubPage * pSubPage = (CDlgBaseSubPage *)(pWnd->GetCurrentSubPage());
        if (pSubPage != NULL)
        {
            strContent = pSubPage->GetSubPageAction();
        }

        if (strContent == strAction)
        {
            return true;
        }
        else if (strContent.find("line_key_item_") != yl::string::npos
                 && strAction.find("line_key_item_") != yl::string::npos)
        {
            return true;
        }
        else if (strContent.find("memory_key_item_") != yl::string::npos
                 && strAction.find("memory_key_item_") != yl::string::npos)
        {
            return true;
        }
        else if (strContent.find("expand_key_item_") != yl::string::npos
                 && strAction.find("expand_key_item_") != yl::string::npos)
        {
            return true;
        }
    }

    return false;
}

bool SettingUI_OpenPage(const yl::string & strSubPageAction, int nDefaultIndex/* = 0*/)
{
    SubMenuData objData(strSubPageAction, nDefaultIndex);
    return SettingUI_OpenPage(objData);
}

//统一对外接口
bool SettingUI_OpenPage(const SubMenuData & pageData, const bool bHasLeftMenu/* = false*/)
{
    BASEUI_INFO("SettingUI_OpenPage [%s]", pageData.strMenuAction.c_str());

    if (SettingUI_IsDlgOnTop(pageData.strMenuAction.c_str()))
    {
        BASEUI_INFO("SettingUI_OpenPage Already On Top");
        return false;
    }

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    SETTING_PAGE_STYLE trueType = bHasLeftMenu ? SETTING_PAGE_STYLE_WITH_MENU :
                                  SETTING_PAGE_STYLE_NO_MENU;
#else
    SETTING_PAGE_STYLE trueType = SETTING_PAGE_STYLE_NO_MENU;
#endif

    PAGE_MODE eTempMode = pageData.eMode;
    yl::string strPageAction = pageData.strMenuAction;
    if (strPageAction == "OpenSubPage(reboot)" || strPageAction == "reboot"
            || strPageAction == "OpenSubPage(reboot_page)" || strPageAction == "reboot_page")
    {
        SettingUI_ConfirmToReboot();
        return true;
    }

    if (strPageAction == "OpenSubPage(update)" || strPageAction == "update"
            || strPageAction == "OpenSubPage(update_page)" || strPageAction == "update_page")
    {
        SettingUI_ConfirmToUpdate();
        return true;
    }

    if (strPageAction == "OpenSubPage(reset_default)" || strPageAction == "reset_default")
    {
        SettingUI_ConfirmToResetLocal();
        return true;
    }

    yl::string strActionType;
    yl::string strContent;
    bool bRet = false;

    bRet = commonAPI_ParseActionInfo(&strActionType, &strContent, pageData.strMenuAction);

    BASEUI_INFO("parse action [%s] result[%d], strActionType[%s], strContent[%s]",
                pageData.strMenuAction.c_str(), bRet, strActionType.c_str(), strContent.c_str());

    if (strActionType == kszOpenSubPageAction)
    {
        //通过外界打开的subpage只能是全屏的
        trueType = SETTING_PAGE_STYLE_NO_MENU;
    }

    //简单action,用于调用CreatePage
    yl::string strSimpleAction = strContent.empty() ? pageData.strMenuAction : strContent;
    //用于取title名称
    yl::string strFullSubPageAction = pageData.strMenuAction;

    if (strActionType.empty())
    {
        if (pageData.strMenuAction.find(".xml") != string::npos)
        {
            strFullSubPageAction = trueType != SETTING_PAGE_STYLE_NO_MENU ? kszOpenXmlAction :
                                   kszOpenPageAction;
        }
        else
        {
            trueType = SETTING_PAGE_STYLE_NO_MENU;
            strFullSubPageAction = kszOpenSubPageAction;
        }

        strFullSubPageAction += "(";
        strFullSubPageAction += strSimpleAction;
        strFullSubPageAction += ")";
    }

    BASEUI_INFO("SettingUI_OpenPage  fullaction[%s]", strFullSubPageAction.c_str());
    yl::string strSettingWndId = commonAPI_FormatString("%s_%d", DLG_CSettingWindow,
                                 GetNewSettingWndID());
    CSettingWindow * pSettingWnd = qobject_cast<CSettingWindow *>(UIManager_CreatePrivateDlg(
                                       DLG_CSettingWindow, strSettingWndId));
    if (pSettingWnd == NULL)
    {
        return false;
    }

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    pSettingWnd->SetPageStyle(trueType);
#endif
    pSettingWnd->SetData(&strFullSubPageAction);

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    if (trueType == SETTING_PAGE_STYLE_WITH_MENU)
    {
        pSettingWnd->SetCurrentItem(pageData.iFirstSelectItem);
        UIManager_UpdateWnd(pSettingWnd);
        return true;
    }
#endif

    UIManager_UpdateWnd(pSettingWnd);//调整位置
    SubMenuData info(strFullSubPageAction);
    info.iFirstSelectItem = pageData.iFirstSelectItem;
    info.pAfterSetSubpageData = pageData.pAfterSetSubpageData;
    info.pBeforeSetSubpageData = pageData.pBeforeSetSubpageData;

    //warining: 传入的action必须是完整的action
    pSettingWnd->SetSubPage(info);
    return true;
}

int SettingUI_GetDefaultPageStyle()
{
    return SETTING_PAGE_STYLE_WITH_MENU;
}

//判断某个设置界面是否正在显示
bool SettingUI_IsDlgOnTop(const char * pDlgName)
{
    if (NULL == pDlgName)
    {
        return false;
    }

    CBaseDialog * pTopDlg = UIManager_GetTopWindow();

    if (NULL == pTopDlg)
    {
        SETTINGUI_WARN("Top dlg is NULL!");
        return false;
    }

    if (pTopDlg->inherits(pDlgName))
    {
        return true;
    }

    if (pTopDlg->inherits(DLG_CSettingWindow))
    {
        CSettingWindow * pSetWnd = (CSettingWindow *)pTopDlg;
        CDlgBaseSubPage * pSubPage = pSetWnd->GetCurrentSubPage();

        if (pSubPage != NULL &&
                (pSubPage->inherits(pDlgName) || pSubPage->IsSameSettingDialog(pDlgName)))
        {
            return true;
        }
    }

    return false;
}

bool SettingUI_EnterSpZeroTouch(int iType)
{
    ZeroSpDlgType eType = (ZeroSpDlgType)iType;
    SETTINGUI_INFO("SettingUI Zero OpenDlg [%d]", eType);
    switch (eType)
    {
    case ZERO_DLG_AUTOPCONFIG:
        {
            ZeroSPTouch_OpenDlg(ZERO_AUTOP_PAGE);
        }
        break;
    case ZERO_DLG_UNAUTHORIZED:
        {
            ZeroSPTouch_OpenDlg(ZERO_UNAUTHORIZED_PAGE);
        }
        break;
    case ZERO_DLG_REDIRECTOR_AUTHENTICATION:
        {
            // 弹出认证界面
            ZeroSPTouch_OpenDlg(ZERO_REDIRECTOR_CONFIG);
        }
        break;
    case ZERO_DLG_UPDATEING:
        {
            ZeroSPTouch_OpenDlg(ZERO_UPDATING_PAGE);
        }
        break;
    case ZERO_DLG_UPDATEFAIL:
        {
            // 弹出连接失败提示
            ZeroSPTouch_OpenDlg(ZERO_UPDATEFAIL_PAGE);
        }
        break;
    case ZERO_DLG_UPDATEFINISH:
        {
            ZeroSPTouch_OpenDlg(ZERO_UPDATEFINISH_PAGE);
        }
        break;
    case ZERO_DLG_REBOOTING:
        {
            ZeroSPTouch_OpenDlg(ZERO_REBOOTING_PAGE);
        }
        break;
    case ZERO_DLG_RPS:
        {
            zero_EnterRPS();
        }
        break;
    case ZERO_DLG_READY_REDIRECT:
        {
            ZeroSPTouch_OpenDlg(ZERO_REDIRECTOR_NOTE);
        }
        break;
    case ZERO_DLG_ZERO_AUTHENTICATION:
    case ZERO_DLG_AUTOP_AUTHENTICATION:
        {
            ZeroSPTouch_OpenDlg(ZERO_AUTHENTICATION);
        }
        break;
#if IF_FEATURE_METASWITCH
    case ZERO_DLG_AUTHFAIL_PAGE:
        {
            ZeroSPTouch_OpenDlg(ZERO_AUTHFAIL_PAGE);
        }
        break;
#endif
    case ZERO_DLG_EXIT:
        {
            if (ZeroSPTouch_IsAllowExit())
            {
                ZeroSPTouch_Destroy();
            }
        }
        break;
    default:
        break;
    }
    return true;
}

#if SETTINGUI_TEST
#include "setting_page_define.h"
#include "talkui/include/modtalkui.h"
void Settingui_Test(TEST_CASE_JUMP_ACTION eJumpAction/* = JA_DO_NOT_JUMP*/)
{
    SETTINGUI_INFO("this is a test for settingui!");
    SETTINGUI_INFO("**************************test begin***************************");
    static int iFlag = 28;

    switch (eJumpAction)
    {
    case JA_JUMP_NEXT:
        iFlag++;
        break;
    case JA_JUMP_PRE:
        iFlag--;
        break;
    default:
        break;
    }

    switch (iFlag)
    {
    case 0:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"status_list\")");
            SettingUI_OpenPage("OpenMenu(status_list.xml)");
        }
        break;
    case 1:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(SubMenuData(\"OpenPage(call_forward_list.xml)\",  1), true)");
            SettingUI_OpenPage(SubMenuData("OpenPage(call_forward_list.xml)",  1), true);
        }
        break;
    case 2:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"OpenSubPage(call_forward_for_custom)\")");
            SettingUI_OpenPage("OpenSubPage(call_forward_for_custom)");
        }
        break;
    case 3:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"OpenSubPage(dnd_custom)\")");
            SettingUI_OpenPage("OpenSubPage(dnd_custom)");
        }
        break;
    case 4:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(SubMenuData(\"OpenPage(text_message_list.xml)\", 1), true)");
            SettingUI_OpenPage(SubMenuData("OpenPage(text_message_list.xml)", 1), true);
        }
        break;
    case 5:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(SubMenuData(\"OpenPage(voice_mail_list.xml)\", 1), true)");
            SettingUI_OpenPage(SubMenuData("OpenPage(voice_mail_list.xml)", 1), true);
        }
        break;
    case 6:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"hoteling_login\")");
            SettingUI_OpenPage("hoteling_login");
        }
        break;
    case 7:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"initial_state\")");
            SettingUI_OpenPage("initial_state");
        }
        break;
    case 8:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"acd_status_select\")");
            SettingUI_OpenPage("acd_status_select");
        }
        break;
    case 9:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"disposition_code\")");
            SettingUI_OpenPage("disposition_code");
        }
        break;
    case 10:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"emergency_escalation\")");
            SettingUI_OpenPage("emergency_escalation");
        }
        break;
    case 11:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"unavailable_reason\")");
            SettingUI_OpenPage("unavailable_reason");
        }
        break;
    case 12:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"status_rtp\")");
            SettingUI_OpenPage("status_rtp");
        }
        break;
    case 13:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(SubMenuData(\"OpenSubPage(set_voice_mail)\", 1))");
            SettingUI_OpenPage(SubMenuData("OpenSubPage(set_voice_mail)", 1));
        }
        break;
    case 14:
        {
            SETTINGUI_INFO("testing :SettingUI_OpenPage(SubMenuData(\"OpenSubPage(view_voice_mail)\"))");
            SettingUI_OpenPage(SubMenuData("OpenSubPage(view_voice_mail)"));
        }
        break;
    case 15:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"status_list\")");
            SettingUI_OpenPage("OpenMenu(status_list.xml)");
        }
        break;
    case 16:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_PAGE_DND, 0)");
            SettingUI_EnterPageByType(MENU_PAGE_DND, 0);
        }
        break;
    case 17:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_PAGE_DND, 2)");
            SettingUI_EnterPageByType(MENU_PAGE_DND, 2);
        }
        break;
    case 18:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_PAGE_ALWAYS_FORWARD, 1)");
            SettingUI_EnterPageByType(MENU_PAGE_ALWAYS_FORWARD, 1);
        }
        break;
    case 19:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_PAGE_BUSY_FORWARD, 1)");
            SettingUI_EnterPageByType(MENU_PAGE_BUSY_FORWARD, 1);
        }
        break;
    case 20:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_PAGE_NOANSWER_FORWARD, 2)");
            SettingUI_EnterPageByType(MENU_PAGE_NOANSWER_FORWARD, 1);
        }
        break;
    case 21:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_SETTING_CUSTOM_FORWARD, 1)");
            SettingUI_EnterPageByType(MENU_SETTING_CUSTOM_FORWARD, 1);
        }
        break;
    case 22:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_SETTING_FORWARD, 1)");
            SettingUI_EnterPageByType(MENU_SETTING_FORWARD, 1);
        }
        break;
    case 23:
        {
            SETTINGUI_INFO("testing : SettingUI_EnterPageByType(MENU_PAGE_AUTOPCODEAUTH, 1)");
            SettingUI_EnterPageByType(MENU_PAGE_AUTOPCODEAUTH, 1);
        }
        break;
    case 24:
        {
            SETTINGUI_INFO("testing : talkui_EnterPickupMenu()");
            talkui_EnterPickupMenu();
        }
        break;
    case 25:
        {
            SETTINGUI_INFO("testing : talkui_EnterSCAMenu()");
            talkui_EnterSCAMenu(NULL);
        }
        break;
    case 26:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"reset_default\")");
            SettingUI_OpenPage("reset_default");
        }
        break;
    case 27:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"OpenSubPage(reboot_page)\")");
            SettingUI_OpenPage("OpenSubPage(reboot_page)");
        }
        break;
    case 28:
        {
            SETTINGUI_INFO("testing : SettingUI_OpenPage(\"OpenSubPage(userMode)\")");
            SettingUI_OpenPage("OpenSubPage(userMode)");
        }
        break;
    default:
        {
            switch (eJumpAction)
            {
            case JA_JUMP_NEXT:
                iFlag--;
                break;
            case JA_JUMP_PRE:
                iFlag++;
                break;
            default:
                break;
            }
            SETTINGUI_INFO("!!!!!!!!!!!!!!!!!!!!!!No more test case!!!!!!!!!!!!!!!!!!!!!!!!");
        }
        break;
    }

    SETTINGUI_INFO("**************************TEST MENU***************************");
    SETTINGUI_INFO("	press ZERO to repeat current case");
    SETTINGUI_INFO("	press XMLBrowser to jump to next case");
    SETTINGUI_INFO("	press MyStatus to jump to previous case");
    SETTINGUI_INFO("**************************TEST MENU***************************");
}
#endif

void SettingUI_RegisterOpenUICheckFunc()
{
    //Advanced配置界面不允许进入
    EDKUI_SetFuncAllowEnter(EDK_UI_ADVANCED_SETTING_LIST, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_KEYGUARD_LOCK, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_LIST, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_WAN_PORT, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_WANPORT, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_VLAN_LIST, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_WVLAN, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_PCVLAN, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_DHCPVLAN, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_WWEB_SERVER, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_W8021X, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_WVPN, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_LLDP, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_VLANCDP, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_NAT, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_SET_PASWORD, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_SET_AESKEY, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_AUTOP, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_FWD_INTERNATIONAL, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_ACCOUNT_LIST, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_REBOOT_PAGE, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_RESET_CONFIG, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_UPDATE_PAGE, SettingUILogic_UnableEnter);

    EDKUI_SetFuncAllowEnter(EDK_UI_USER_MODE, feature_UserModeIsOpen);

    EDKUI_SetFuncAllowEnter(EDK_UI_USB_RECORD, modRecord_IsStorageConnect);
    EDKUI_SetFuncAllowEnter(EDK_UI_BROWSE_USB_RECORD, modRecord_IsStorageConnect);
    EDKUI_SetFuncAllowEnter(EDK_UI_BROWSE_USB_PHOTO, modRecord_IsStorageConnect);
    EDKUI_SetFuncAllowEnter(EDK_UI_BROWSE_USB_STORAGE, modRecord_IsStorageConnect);
    EDKUI_SetFuncAllowEnter(EDK_UI_PLAY_USB_RECORD, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_USB_IDLE_RECORD, SettingUILogic_UnableEnter);

    EDKUI_SetFuncAllowEnter(EDK_UI_HOTELING_LOGIN, CallCenter_IsHotelingOn);
    EDKUI_SetFuncAllowEnter(EDK_UI_ACD_STATUS_SELECT, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_UNAVAILABLE_REASON, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_DISPOSITION, SettingUILogic_DispositionCodeIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_EMERGENCY_ESCALATION, SettingUILogic_EmergencyIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_BLUETOOTH_DETAIL, SettingUILogic_BluetoothIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_BLUETOOTH_DEV_LIST, SettingUILogic_BluetoothIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_INIT_STATE, SettingUILogic_InitialStateIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_ENTER_PWD, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_HOTDESK_BASE, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_MY_STATUS, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_EDIT_PAGING_GROUP, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_AUTO_ANSWER_LINE, SettingUILogic_UnableEnter);

    EDKUI_SetFuncAllowEnter(EDK_UI_CALLCONTROL, SettingUILogic_CallControlIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_CALLCONTROL_LINESELECT, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_ANYWHERE_LOCATION, SettingUILogic_AnyWhereIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_ANYWHERE_NEW, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_ANYWHERE_EDIT, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_REMOTE_OFFICE, SettingUILogic_AnyWhereIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_ANONYMOUS_REJECTION, SettingUILogic_AnonymousRejectionIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_ANONYMOUSCALL, SettingUILogic_AnonymousRejectionIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_SIMULT_RING_LOCATION, SettingUILogic_AnonymousRejectionIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_SIMULTANEOUS_RING_NEW, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_CUSTOM_DND, SettingUILogic_AnonymousRejectionIsEnable);

    EDKUI_SetFuncAllowEnter(EDK_UI_ACTION_SILENT_ALERTING, SettingUILogic_CallControlIsEnable);

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    EDKUI_SetFuncAllowEnter(EDK_UI_BSFT_CALLWAITING, SettingUILogic_CallWaitingIsEnable);
#else
    EDKUI_SetFuncAllowEnter(EDK_UI_BSFT_CALLWAITING, SettingUILogic_UnableEnter);
#endif

    EDKUI_SetFuncAllowEnter(EDK_UI_AUTOP_CODE_AUTHORIZED, SettingUILogic_UnableEnter);

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_COMMON_SETTING, SettingUILogic_CallControlIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_DETAILS, SettingUILogic_CallControlIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_FILTER_SETTING, SettingUILogic_CallControlIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_SCREEN_SETTING, SettingUILogic_CallControlIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_ALTER_SETTING, SettingUILogic_CallControlIsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_LISTVIEW_SETTING, SettingUILogic_CallControlIsEnable);
#else
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_COMMON_SETTING, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_DETAILS, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_FILTER_SETTING, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_SCREEN_SETTING, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_ALTER_SETTING, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_EXE_LISTVIEW_SETTING, SettingUILogic_UnableEnter);
#endif

    EDKUI_SetFuncAllowEnter(EDK_UI_NETWORK_PC_PORT, SettingUILogic_UnableEnter);
    EDKUI_SetFuncAllowEnter(EDK_UI_MULTI_FWD, SettingUILogic_UnableEnter);

#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
    EDKUI_SetFuncAllowEnter(EDK_UI_GROUP_NIGHT_FWD, SettingUILogic_AnonymousRejectionIsEnable);
#else
    EDKUI_SetFuncAllowEnter(EDK_UI_GROUP_NIGHT_FWD, SettingUILogic_UnableEnter);
#endif

#ifdef IF_FEATURE_BROADSOFT_MOBILITY
    EDKUI_SetFuncAllowEnter(EDK_UI_MOBILITY, SettingUILogic_AnonymousRejectionIsEnable);
#else
    EDKUI_SetFuncAllowEnter(EDK_UI_MOBILITY, SettingUILogic_UnableEnter);
#endif

#ifdef IF_FEATURE_GETBALANCE
    EDKUI_SetFuncAllowEnter(EDK_UI_GET_BALANCE, SettingUILogic_GetBanlanceIsEnable);
#else
    EDKUI_SetFuncAllowEnter(EDK_UI_GET_BALANCE, SettingUILogic_UnableEnter);
#endif

    EDKUI_SetFuncAllowEnter(EDK_UI_CALLFWD_NOTREACH, SettingUILogic_AnonymousRejectionIsEnable);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    EDKUI_SetFuncAllowEnter(EDK_UI_CALL_RECORD, SettingUILogic_CallRecordIsEnable);
#else
    EDKUI_SetFuncAllowEnter(EDK_UI_CALL_RECORD, SettingUILogic_UnableEnter);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    EDKUI_SetFuncAllowEnter(EDK_UI_CALL_WAITING, SettingUILogic_CallWaitingIsEnable);
#else
    EDKUI_SetFuncAllowEnter(EDK_UI_CALL_WAITING, SettingUILogic_UnableEnter);
#endif
}

#if IF_FEATURE_GENBAND_E911
bool SettingUI_EnterE911Setting(int nLocationID)
{
    if (nLocationID < 0)
    {
        return FALSE;
    }

    yl::string strAction = commonAPI_FormatString("OpenSubPage(e911_set_location_%d)", nLocationID);
    SettingUI_OpenPage(SubMenuData(strAction));

    return TRUE;
}

void SettingUI_ProcessIdleEvent()
{
    g_E911Controller.ProcessIdleEvent();
}
#endif
