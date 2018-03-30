#include "modsettingui.h"
#include <settinguilogic/include/settingdefine.h>
#include "settinguidefine.h"
#include "uimanager/basedialog.h"
#include "uimanager/uimanager_common.h"
#include <xwindow/xwindowdefine.h>
#include "settinguidialogmanager.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/configfiltermanager.h"
#include "settinguiwizardmanager.h"
#include "settingui/include/setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
// item type
#include "settinguilogic/include/itemtypedefine.h"
// check method
#include "editcheckmethodfactory.h"
#include "editcheckmethod.h"
// 回调函数
#include "uicallbackfun.h"
#include "uicommonfun.h"
// delegate
#include "delegatefactory.h"
#include "settinguidatadelegatebase.h"
#include "settinguimainmenudelegate.h"
#include "settinguistatusdelegate.h"
#include "settinguistatusphonedelegate.h"
#include "settinguistatusnetworkdelegate.h"
#include "settinguistatusnetipv4delegate.h"
#include "settinguistatusnetipv6delegate.h"
#include "settinguistatusaccountdelegate.h"
#include "settinguistatusrtpdelegate.h"
#include "settinguicallforwardlineselectdelegate.h"
#include "settinguicallforwarddelegate.h"
#include "settinguicallforwarddetaildelegate.h"
#include "settinguicallwaitingdelegate.h"
#include "settinguisettingtypedelegate.h"
#include "settinguiadvancedchangepwddelegate.h"
#include "settinguihistorydelegate.h"
#include "settinguidirectorydelegate.h"
#include "acddispositioncodedelegate.h"
#include "acdemergencyescalationdelegate.h"
#include "autoanswerdelegate.h"
#include "settinguifeaturedelegate.h"
#include "settinguianonymouscalldelegate.h"
#include "settinguifeaturesdnddelegate.h"
#include "settinguipaginglistdelegate.h"
#include "settinguipaginglisteditdelegate.h"
#include "pingandtraceoutdelegate.h"
#include "pingandtraceoutresultdelegate.h"
#include "settinguibasicsettinguidelegate.h"
#include "settinguilanguagedelegate.h"
#include "settinguitimeanddatedelegate.h"
#include "settinguisntpsettingdelegate.h"
#include "settinguitimedateformatdelegate.h"
#include "settinguicontrastdelegate.h"
#include "settinguichangepingdelegate.h"
#if !IF_UNLOCK_CHECK_POPBOX
#include "settinguiphoneunlockcheckdelegate.h"
#endif
#include "settinguibtwifidelegate.h"
#include "settinguibluetoothdevdelegate.h"
#include "settinguibtchannelcontrolldelegate.h"
#include "settinguiwifinetdelegate.h"
#include "settinguiwifieditdelegate.h"
#include "settinguiringtonedelegate.h"
#include "settinguiringtonesettingdelegate.h"
#include "settinguinetworkvlanlistdelegate.h"
#include "settinguiwanportdelegate.h"
#include "settinguinetworkvlandelegate.h"
#include "settinguinetwork8021xdelegate.h"
#include "settinguinetworknatdelegate.h"
#include "settinguinetworkvpndelegate.h"
#include "settinguiautoprovisiondelegate.h"
#include "settinguiadvancedresetdelegate.h"
#include "settinguiadvaccountlistdelegate.h"
#include "settinguiadvcommonaccountsettingdelegate.h"
#include "settinguiadvcloudaccountsettingdelegate.h"
#if IF_SUPPORT_SOFTKEY_LABEL
#include "settinguisoftkeylabellistdelegate.h"
#endif
#include "settinguilinekeylistdelegate.h"
#include "settinguidsskeysettingdelegate.h"
#ifdef IF_USB_SUPPORT
#include "usbrecorddelegate.h"
#include "usbaudiolistdelegate.h"
#include "usbaudioplayerdelegate.h"
#include "usbstorageinfodelegate.h"
#ifdef IF_USB_RECORD
#include "usbaudiorecorddelegate.h"
#endif
#endif
#include "settinguiadvpstnaccountsettingdelegate.h"
#include "settinguipstnflashtimedelegate.h"
#include "settinguipstncountrydelegate.h"
#include "settinguiadvanceddelegate.h"
#include "settinguiviewvoicemaildelegate.h"
#include "voicemailcodelistdelegate.h"
#include "textmessageinboxdelegate.h"
#include "textmessagesentboxdelegate.h"
#include "textmessageoutboxdelegate.h"
#include "textmessagedraftboxdelegate.h"
#include "textmessagesenddelegate.h"
#include "textmessageviewdelegate.h"
#include "settinguibstfcallcontrolldelegate.h"
#include "settinguidndlineselectdelegate.h"
#include "settinguilineselectdelegate.h"
#include "settinguilocationlistdelegate.h"
#include "settinguiaddlocationdelegate.h"
#include "settinguieditlocationdelegate.h"
#include "settinguicallcontrollmobilitydelegate.h"
#include "settinguicallcontrollremoteofficedelegate.h"
#include "settinguicallcontrollcfnrdelegate.h"
#include "settinguicallcontrollanonymousdelegate.h"
#include "settinguicallcontrollcallwaitingdelegate.h"
#include "settinguicallcontrollgnfdelegate.h"
#include "settinguicallcontrollforwardselectivedelegate.h"
#include "settinguicallcontrollacdinitialdelegate.h"
#include "settinguicallcontrollACDstatussetdelegate.h"
#include "settinguiacdunavailablereasoncodedelegate.h"
#include "settinguisimultaneousringlistdelegate.h"
#include "settinguisimultaneousringanddandeditdelegate.h"
#include "settinguixsipassworddelegate.h"
#include "settinguiexecutiveandassistantdelegate.h"
#include "settinguisecurityclassficationdelegate.h"
#include "settinguiusermodedelegate.h"
#include "settinguistatusmoredelegate.h"
#include "settinguivoicemaillistdelegate.h"
#include "settinguivoicemaildetaildelegate.h"
#include "settinguivoicemailplaydelegate.h"
#include "settinguivoicemailaddtopersonaldelegate.h"
#include "settinguicallcontrollcallrecorddelegate.h"

#include "hotelinglogindelegate.h"
#include "hotelingguestunlockcheckdelegate.h"
#include "edkpromptdelegate.h"
#include "settinguihotdeskbasedelegate.h"

#include "resetauthordelegate.h"
#ifdef IF_FEATURE_GETBALANCE
#include "settinguibalancedelegate.h"
#endif
#if IF_SUPPORT_WALLPAPER
#include "wallpaperdelegate.h"
#endif
#if IF_SUPPORT_EXPWALLPAPER
#include "expwallpaperdelegate.h"
#endif

#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
#include "usbphotolistitem.h"
#include "usbphotolistadapter.h"
#include "usbphotolistdelegate.h"
#include "dlgusbphoto.h"
#endif

#if IF_FEATURE_METASWITCH
#include "mtswcommportaldelegate.h"
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "mtswvoicemaillistdelegate.h"
#include "mtswvoicemaildetaildelegate.h"
#include "mtswvoicemailplaydelegate.h"
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
#include "voicemailplayadapter.h"
#include "voicemailtimeritemdata.h"
#include "voicemailtimeritem.h"
#endif

#if IF_FEATURE_METASWITCH_ACD
#include "mtswacdlistdelegate.h"
#include "mtswacddetaildelegate.h"
#include "mtswacdmemberdelegate.h"
#include "mtswacdmystatusdelegate.h"
#include "mtswacdselectdispcodedelegate.h"
#include "voicemailplayadapter.h"
#endif

#if IF_FEATURE_GENBAND_E911
#include "e911setlocationdelegate.h"
#include "e911locationdelegate.h"
#endif

#if IF_SUPPORT_DEFAULT_ACCOUNT_DIALOG
#include "settinguidefaultaccountdelegate.h"
#endif

#if IF_SUPPORT_SCREEN_SAVER
#include "settinguiscreensaverdelegate.h"
#endif

#if IF_SUPPORT_BLF_RING
#include "settinguiblfringtonedelegate.h"
#endif

#if FEATURE_BROADTOUCH_UC
#include "settinguimystatusdelegate.h"
#endif

#if IF_SUPPORT_BACK_LIGHT_LEVEL
#include "settinguibacklightdelegate.h"
#endif

#if IF_SUPPORT_IP_SET_SINGLE_MENU
#include "settinguiwanportipsetdelegate.h"
#else
#include "settinguiwanportstaticclientdelegate.h"
#include "settinguiwanportdhcpclientdelegate.h"
#endif

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguinetworkchangebasedelegate.h"
#endif

// adapter
#include "settingadapterfactory.h"
#include "settinguicomboboxadapter.h"
#include "settinguivaluechangeadapter.h"
#include "settinguiscrollbypageadapter.h"
#include "usbaudiolistadapter.h"
// dialog
#include "settingdialogfactory.h"
#include "dlgsettingui.h"
#include "dlgpingandtraceout.h"
#include "dlgmanualsetting.h"
#include "dlgadvancedsetting.h"
#include "dlgboot.h"
#ifdef IF_USB_SUPPORT
#include "dlgusbaudioplayer.h"
#ifdef IF_USB_RECORD
#include "dlgusbaudiorecord.h"
#endif
#endif
#include "dlgnewmessage.h"
#include "dlgviewmessage.h"
#include "dlgvoicemaillist.h"
#include "dlgbtchannelcontroll.h"
#include "dlgmainmenu.h"
#if IF_SUPPORT_WALLPAPER
#include "dlgwallpaper.h"
#endif
// msg pop dlg
#include "messagebox/modmessagebox.h"
// item class
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/radiolistitem.h"
#include "baseui/include/menulistitem.h"
#include "bluetoothdevitem.h"
#include "wifinetitem.h"
#include "pingandtraceresultitem.h"
#include "textmessageitem.h"
#include "btchannelcontrollitem.h"
#include "usbaudiolistitem.h"
#include "mainmenuitem.h"
// item data 和 item data parser、item type
#include "settinguilogic/include/itemtypemanager.h"
#include "settinguilogic/include/itemdatafactory.h"
#include "settinguilogic/include/itemdataparserfactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguicheckboxitemdata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguicustomcomboboxitemdata.h"
#include "settinguimenudetailitemdata.h"
#include "settinguiradioitemdata.h"
#include "settinguibluetoothdevitemdata.h"
#include "settinguiwifinetitemdata.h"
#include "settinguitextmessageitemdata.h"
#include "settinguibtchannelcontrollitemdata.h"
#include "settinguimainmenuitemdata.h"
// setting item data 和 base item类型转换
#include "settinguilogic/include/setitemdatatype2baseitemtypefactory.h"
// setting item data 和 base item数据转换
#include "itemdatavaluetransfactory.h"
#include "itemdatavaluetrans.h"
// page data
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseaccountpagedata.h"

#include "settingmsgboxmgr.h"

#include "settinguidsskeyaction.h"
#include "setting_page_define.h"
#include "settingui/include/settinguimanager.h"

#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uimanager/modvolumebar.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/include/common.h"

#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguihotlinedelegate.h"
#endif

// ---------------------- 函数声明 开始 -------------------
// 注册界面工厂类
void RegisterSettingUIDlgClass();
// 初始化Setting界面Id与界面类名映射
void InitSettingDialog();
// 初始设置回调函数
void InitSetCallBackFunction();
// 界面特殊Delegate初始化
void InitPrivateDelegate();
// 界面Adapter初始化
void InitPrivateAdapter();
// 界面数据类初始化
void InitPageData();
// 界面Item类初始化
void InitItem();
// 界面Item数据类初始化
void InitItemData();
// 界面Item数据解析函数初始化
void InitItemDataParser();
// 界面Item类型初始化
void InitItemType();
// setting item data 和 base item类型转换初始化
void InitItemDataAndItemTypeTrans();
//setting item data 和 base item数据转换初始化
void InitItemDataValueTrans();
// check method初始化
void InitCheckMethod();
// ---------------------- 函数声明 结束 -------------------

//Setting模块初始化
void SettingUI_Init()
{
    g_DataFactory->init();

    // 界面初始化
    RegisterSettingUIDlgClass();

    // 初始化Setting界面Id与界面类名映射
    InitSettingDialog();

    // 回调初始化
    InitSetCallBackFunction();

    // 界面特殊Delegate初始化
    InitPrivateDelegate();

    // 界面Adapter初始化
    InitPrivateAdapter();

    // 界面数据类初始化
    InitPageData();

    // 界面Item类初始化
    InitItem();

    // 界面Item数据类初始化
    InitItemData();

    // 界面Item数据解析函数初始化
    InitItemDataParser();

    // 界面Item类型初始化
    InitItemType();

    // setting item data 和 base item类型转换初始化
    InitItemDataAndItemTypeTrans();

    //setting item data 和 base item数据转换初始化
    InitItemDataValueTrans();

    // check method初始化
    InitCheckMethod();

    SettingUI_InitDsskeyImplActions();

    // 过滤初始化
    g_pConfigFilterMgr->Init();

    // 二级权限初始化
}

void SettingUI_UnInit()
{
    g_SetUICallBackFun.SetFunRefreshUI(NULL);
    g_SetUICallBackFun.SetFunDropOutSettingUI(NULL);
    g_SetUICallBackFun.SetFunBindDelegate(NULL);
}

void SettingUI_SetPageDataPath(const yl::string& strPageDataPath)
{
    CSettingUIDelegateBase::SetPageDataPath(strPageDataPath);
}

bool SettingUI_EnterPage(const yl::string & strPageID, void * pExtraData /*= NULL*/,
                         bool bRefresh /*= true*/)
{
    PageOpenData openPageData(strPageID);
    openPageData.pExtraData = pExtraData;
    return g_pSettingUIDialogManager->OpenPage(openPageData, bRefresh);
}


bool SettingUI_EnterListPage(const yl::string & strPageIDList)
{
    return g_pSettingUIDialogManager->OpenPageOrderByPageID(strPageIDList);
}

void SettingUI_InitDsskeyImplActions()
{
    static bool bInited = false;
    if (!bInited)
    {
        new CMenuAciton();

        new CStatusAction();
#if IF_FEATURE_PAGING
        new CDssKeyPagingList();
#endif
        new CDssKeySMS();

        new CDssKeyNewSMS();
#if IF_FEATURE_METASWITCH_VOICEMAIL
        new CDssKeyMessageList();
#endif
#if IF_FEATURE_METASWITCH
        new CDssKeyMTSWLogout();
        new CDssKeyMTSWLogIn();
#endif
#if FEATURE_BROADTOUCH_UC
        new CDssKeyMyStatus();
#endif
        bInited = true;
    }
}

void RegisterSettingUIDlgClass()
{
    REGISTER_DLG(CDlgSettingUI);
    REGISTER_DLG(CDlgPingAndTraceOut);
    REGISTER_DLG(CDlgManualSetting);
    REGISTER_DLG(CDlgAdvancedSetting);
#ifdef IF_SETTING_WIZARD
    REGISTER_DLG(CDlgBoot);
#endif
#ifdef IF_USB_SUPPORT
    REGISTER_DLG(CDlgUSBAudioPlayer);
#ifdef IF_USB_RECORD
    REGISTER_DLG(CDlgUSBAudioRecord);
#endif
#endif
    REGISTER_DLG(CDlgNewMessage);
    REGISTER_DLG(CDlgViewMessage);
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    REGISTER_DLG(CDlgVoiceMailList);
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    REGISTER_DLG(CDlgBTChannelControll);
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    REGISTER_DLG(CDlgMainMenu);
#endif
#if IF_SUPPORT_WALLPAPER
    REGISTER_DLG(CDlgWallpaper);
#endif
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
    REGISTER_DLG(CDlgUsbPhoto);
#endif
}

void InitSettingDialog()
{
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDStatusRTP, DLG_PINGANDTRACEOUT);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDPingResult, DLG_PINGANDTRACEOUT);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDTraceResult, DLG_PINGANDTRACEOUT);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDManualSetting, DLG_ManualSetting);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDAdvancedSetting, DLG_AdvancedSetting);
#ifdef IF_SETTING_WIZARD
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDGuideLanguaue, DLG_DlgBoot);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDGuideTimeDate, DLG_DlgBoot);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDGuideNetworkWanPort, DLG_DlgBoot);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDGuideCommonSipLine, DLG_DlgBoot);
#if IF_FEATURE_CLOUD_ACCOUNT
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDGuideCloudSipLine, DLG_DlgBoot);
#endif
#endif
#ifdef IF_USB_SUPPORT
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDUsbAudioPlayer, DLG_USB_AUDIO_PLAYER);
#ifdef IF_USB_RECORD
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDUsbIdleRecord, DLG_USB_AUDIO_RECORD);
#endif
#endif
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDTextMsgNew, DLG_NEW_MESSAGE);
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDTextMsgView, DLG_VIEW_MESSAGE);
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDVoiceMailList, DLG_VOCIE_MAIL_LIST);
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDBTChannelControll, DLG_BT_CHANNEL_CONTROLL);
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDMainMenu, DLG_MainMenu);
#endif
#if IF_SUPPORT_WALLPAPER
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDWallPaper, DLG_Wallpaper);
#endif
#if IF_SUPPORT_EXPWALLPAPER
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDExpWallPaper, DLG_Wallpaper);
#endif
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDUsbPhotoBrowser, DLG_USB_PHOTO);
#endif
}

void InitSetCallBackFunction()
{
    g_SetUICallBackFun.SetFunRefreshUI(RefreshUI);
    g_SetUICallBackFun.SetFunCacheDlgData(CacheDlgData);
    g_SetUICallBackFun.SetFunRefreshSoftkey(RefreshSoftkey);
    g_SetUICallBackFun.SetFunRefreshDlgTitle(RefreshDlgTitle);
    g_SetUICallBackFun.SetFunFallBackToPreviousPage(FallBackToPreviousPage);
    g_SetUICallBackFun.SetFunBackToIdle(BackToIdle);
    g_SetUICallBackFun.SetFunDropOutCurrentPage(DropOutCurrentPage);
    g_SetUICallBackFun.SetFunDropOutSettingUI(DropOutSettingUI);
    g_SetUICallBackFun.SetFunEnterPage(EnterPage);
    g_SetUICallBackFun.SetFunBindDelegate(BindDelegate);
    g_SetUICallBackFun.SetFunUnBindDelegate(UnBindDelegate);
    g_SetUICallBackFun.SetFunPopCommonMessageBox(PopCommonMessageBox);
    g_SetUICallBackFun.SetDealWithValidData(DealWithValidData);
}

void InitPrivateDelegate()
{
    // main menu
    g_DelegateFactory.RegisterDelegate(kszPageIDMainMenu,
                                       &CSettingUIMainMenuDelegate::CreateMainMenuDelegate);

    //call controll
#ifdef IF_FEATURE_BROADSOFT_BASE
    g_DelegateFactory.RegisterDelegate(kszPageIDBSFTCallControll,
                                       &CSettinguiBSTFCallControllDelegate::CreateBSTFCallControllDelegate);
#endif
    g_DelegateFactory.RegisterDelegate(kszPageIDLineSelect,
                                       &CSettinguiLineSelectDelegate::CreateLineSelectDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDDNDLineSelect,
                                       &CSettinguiDNDLineSelectDelegate::CreateDNDLineSelectDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallForwardLineSelect,
                                       &CSettinguiCallForwardLineSelectDelegate::CreateCallForwardLineSelectDelegate);
#ifdef IF_FEATURE_BROADSOFT_ANYWHERE
    g_DelegateFactory.RegisterDelegate(kszPageIDAnyWhereLocationList,
                                       &CSettinguiLocationListDelegate::CreateLocationListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAnyWhereAddLocation,
                                       &CSettinguiAddLocationDeletae::CreateAddLocationDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAnyWhereEditLocation,
                                       &CSettinguiEditLocationDeletae::CreateEditLocationDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllMobility,
                                       &CSettinguiCallControllMobilityDelegate::CreateCallControllMobilityDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_REMOTE_OFFICE
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllRemoteOffice,
                                       &CSettinguiCallControllRemoteOfficeDelegate::CreateCallControllRemoteOfficeDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllCFNR,
                                       &CSettinguiCallControllCFNRDelegate::CreateCallControllCFNRDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_CLID
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllHideNumber,
                                       &CSettinguiCallControllAnonymousDelegate::CreateCallControllAnonymousDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllAnonymoRrejection,
                                       &CSettinguiCallControllAnonymousDelegate::CreateCallControllAnonymousDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllCallWaiting,
                                       &CSettinguiCallControllCallWaitingDelegate::CreateCallControllCallWaitingDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllGNF,
                                       &CSettinguiCallControllGNFDelegate::CreateCallControllGNFDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllForwardSelective,
                                       &CSettinguiCallControllForwardSelectiveDelegate::CreateCallControllForwardSelectiveDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllInitialACDStatus,
                                       &CSettinguiCallControllACDinitialDelegate::CreateACDinitialDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllSetACDStatus,
                                       &CSettinguiCallControllACDStatusSetDelegate::CreateACDStatusSetDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDACDUnavailableReasonCode,
                                       &CSettinguiACDUnavailableReasonCodeDelegate::CreateACDUnavailableReasonCodeDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllSimultaneousRingList,
                                       &CSettinguiSimultaneousRingListDelegate::CreateSimultaneousRingListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllSimultaneousRingAdd,
                                       &CSettinguiSimultaneousRingAddAndEditDeletae::CreateSimultaneousRingAddAndEditDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllSimultaneousRingEdit,
                                       &CSettinguiSimultaneousRingAddAndEditDeletae::CreateSimultaneousRingAddAndEditDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllExecutiveDetails,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllExecutiveFilter,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllExecutiveScreen,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllExecutiveAlter,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllExecutiveCommon,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllExecutiveListview,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllAssistantDetails,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllAssistantCommon,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllAssistantListview,
                                       &CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate);
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    g_DelegateFactory.RegisterDelegate(kszPageIDCallControllCallRecord,
                                       &CSettinguiCallControllCallRecordDelegate::CreateCallControllCallRecordDelegate);
#endif

    g_DelegateFactory.RegisterDelegate(kszPageIDSecurityClassfication,
                                       &CSettinguiSecurityClassficationDelegate::CreateSecurityClassficationDelegate);

    g_DelegateFactory.RegisterDelegate(kszPageIDXSIPassword,
                                       &CSettinguiXSIPasswordDelegate::CreateXSIPasswordDelegate);

    //status
    g_DelegateFactory.RegisterDelegate(kszPageIDStatus,
                                       &CSettingUIStatusDelegate::CreateStatusDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDStatusPhone,
                                       &CSettingUIStatusPhoneDelegate::CreateStatusPhoneDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDStatusNetwork,
                                       &CSettingUIStatusNetworkDelegate::CreateStatusNetworkDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDStatusMore,
                                       &CSettinguiStatusMoregDelegate::CreateStatusMoreDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDStatusNetworkIPv4,
                                       &CSettingUIStatusNetIPv4Delegate::CreateStatusNetIPv4Delegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDStatusNetworkIPv6,
                                       &CSettingUIStatusNetIPv6Delegate::CreateStatusNetIPv6Delegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDStatusAccount,
                                       &CSettingUIStatusAccountDelegate::CreateStatusAccountDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDStatusRTP,
                                       &CSettinguiStatusRtpDelegate::CreatStatusRtpDelegate);

    //features
    g_DelegateFactory.RegisterDelegate(kszPageIDFeature,
                                       &CSettingUIFeatureDelegate::CreateFeatureDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallForward,
                                       &CSettingUICallForwardDelegate::CreateCallForwardDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAlwaysForward,
                                       &CSettingUICallForwardDetailDelegate::CreateCallForwardDetailDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBusyForward,
                                       &CSettingUICallForwardDetailDelegate::CreateCallForwardDetailDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNoAnswerForward,
                                       &CSettingUICallForwardDetailDelegate::CreateCallForwardDetailDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallWaiting,
                                       &CSettinguiCallWaitingDelegate::CreateCallWaitingDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAutoAnswer,
                                       &CAutoAnswerDelegate::CreateAutoAnswerDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDIntercom,
                                       &CSettinguiDataDelegateBase::CreateDataDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDCallCompletion,
                                       &CSettinguiDataDelegateBase::CreateDataDelegate);
#if IF_SUPPORT_KEY_AS_SEND_DIALOG
    g_DelegateFactory.RegisterDelegate(kszPageIDKeyAsSend,
                                       &CSettinguiDataDelegateBase::CreateDataDelegate);
#endif
#if IF_SUPPORT_HISTORY_RECORD_DIALOG
    g_DelegateFactory.RegisterDelegate(kszPageIDHistorySetting,
                                       &CSettinguiDataDelegateBase::CreateDataDelegate);
#endif
#if IF_SUPPORT_GENERAL_DIALOG
    g_DelegateFactory.RegisterDelegate(kszPageIDGeneral,
                                       &CSettinguiDataDelegateBase::CreateDataDelegate);
#endif

    g_DelegateFactory.RegisterDelegate(kszPageIDCallAutoRedial,
                                       &CSettinguiDataDelegateBase::CreateDataDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAnonymousCall,
                                       &CSettinguiAnonymousCallDelegate::CreateAnonymousCallDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDPagingList,
                                       &CSettinguiPagingListDelegate::CreatePagingListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDPagingListEdit,
                                       &CSettinguiPagingListEditDelegate::CreatPagingListEditDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTraceOut,
                                       &CPingAndTraceoutDelegate::CreatePingAndTraceDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDPing,
                                       &CPingAndTraceoutDelegate::CreatePingAndTraceDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDPingResult,
                                       CPingAndTraceOutResultDelegate::CreatResultDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTraceResult,
                                       CPingAndTraceOutResultDelegate::CreatResultDelegate);

    g_DelegateFactory.RegisterDelegate(kszPageIDAccountDND,
                                       &CSettinguiFeaturesDNDDelegate::CreateFeatureDNDDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDPhoneDND,
                                       &CSettinguiFeaturesDNDDelegate::CreateFeatureDNDDelegate);

    //basic setting
    g_DelegateFactory.RegisterDelegate(kszPageIDSettingType,
                                       &CSettingUISettingTypeDelegate::CreateSettingTypeDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBasicSetting,
                                       &CSettingUIBasicSettingDelegate::CreateBasicSettingDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDLanguage,
                                       &CSettingUILanguageDelegate::CreateLanguageDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTimeData,
                                       &CSettingUITimeAndDateDelegate::CreateTimeAndDateDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDSNTPSetting,
                                       &CSettingUISNTPSettingDelegate::CreateSNTPSettingDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDSTimeDataFormat,
                                       &CSettinguiTimeDataFormatDelegate::CreateTimeDataFormatDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDContrast,
                                       &CSettinguiContrastDelegate::CreateContrastDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDChangePin,
                                       &CSettinguiChangePingDelegate::CreateChangePingDelegate);
#if !IF_UNLOCK_CHECK_POPBOX
    g_DelegateFactory.RegisterDelegate(kszPageIDPhoneUnlockCheck,
                                       &CSettinguiPhoneUnlockCheckDelegate::CreatePhoneUnlockCheckDelegate);
#endif
    g_DelegateFactory.RegisterDelegate(kszPageIDRingTone,
                                       &CSettingUIRingToneDelegate::CreateRingToneDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDRingToneSetting,
                                       &CSettingUIRingToneSettingDelegate::CreateRingToneSettingDelegate);
#ifdef IF_FEATURE_PSTN
    g_DelegateFactory.RegisterDelegate(kszPageIDPSTNCountry,
                                       &CSetttinguiPSTNCountryDelegate::CreatePSTNCountryDelegate);
#endif
    //basic setting bluetooth
#ifdef IF_SUPPORT_BLUETOOTH
    g_DelegateFactory.RegisterDelegate(kszPageIDBluetoothMenu,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBluetoothScan,
                                       &CSettingUIBluetoothDevDelegate::CreateBluetoothDevDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBluetoothPaired,
                                       &CSettingUIBluetoothDevDelegate::CreateBluetoothDevDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBluetoothDevDetail,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBluetoothEdit,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBluetoothPassword,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDBTChannelControll,
                                       &CSettingUIBtChannelControllDelegate::CreateBtChannelControllDelegate);
#endif
    //basic setting wifi
#ifdef IF_SUPPORT_WIFI
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiMenu,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiScan,
                                       &CSettingUIWifiNetDelegate::CreateWifiNetDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiKnown,
                                       &CSettingUIWifiNetDelegate::CreateWifiNetDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiStatus,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiDetail,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiEdit,
                                       &CSettingUIWifiEditDelegate::CreateWifiEditDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiAdd,
                                       &CSettingUIWifiEditDelegate::CreateWifiEditDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiPassword,
                                       &CSettingUIBtWifiDelegate::CreateBtWifiDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWifiDetail2Add,
                                       &CSettingUIWifiEditDelegate::CreateWifiEditDelegate);
#endif

    //advance setting
    g_DelegateFactory.RegisterDelegate(kszPageIDAdvancedSetting,
                                       &CSettingUIAdvancedDelegate::CreateAdvancedDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDChangePassword,
                                       &CSettingUIAdvancedChangePwdDelegate::CreateChangePwdDelegate);
#if IF_SUPPORT_IP_SET_SINGLE_MENU
    g_DelegateFactory.RegisterDelegate(kszPageIDWanPortIPV4,
                                       &CSettingUIWanPortIPSetDelegate::CreatWanPortIPSetDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWanPortIPV6,
                                       &CSettingUIWanPortIPSetDelegate::CreatWanPortIPSetDelegate);
#else
    g_DelegateFactory.RegisterDelegate(kszPageIDWanPortIpv4StaticClient,
                                       &CSettingUIWanPortStaticClientDelegate::CreatWanPortStaticClientDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWanPortIpv6StaticClient,
                                       &CSettingUIWanPortStaticClientDelegate::CreatWanPortStaticClientDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWanPortIpv4DhcpClient,
                                       &CSettingUIWanPortDhcpClientDelegate::CreatWanPortDhcpClientDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDWanPortIpv6DhcpClient,
                                       &CSettingUIWanPortDhcpClientDelegate::CreatWanPortDhcpClientDelegate);
#endif
    g_DelegateFactory.RegisterDelegate(kszPageIDWanPortOption,
                                       &CSettingUIWanPortDelegate::CreateWanPortDelegate);
#ifdef _CP
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkVlanList,
                                       &CSettingUINetworkVlanListDelegate::CreateNetworkVlanListDelegate);
#endif
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkVlanWanPort,
                                       &CSettinguiNetworkVlanDelegate::CreatNetworkVlanDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkVlanPcPort,
                                       &CSettinguiNetworkVlanDelegate::CreatNetworkVlanDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkVlanDhcp,
                                       &CSettinguiNetworkVlanDelegate::CreatNetworkVlanDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkNAT,
                                       &CSettingUINetworkNATDelegate::CreatNetworkNATDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkVPN,
                                       &CSettingUINetworkVPNDelegate::CreatNetworkVPNDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNetwork8021X,
                                       &CSettingUINetwork8021xDelegate::CreatNetwork8021xDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAutoProvision,
                                       &CSettinguiAutoProvisionDelegate::CreateAutoProvisionDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDResetConfig,
                                       &CSettingUIAdvancedResetDelegate::CreateAdvancedResetDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAdvAccountList,
                                       &CSettinguiAdvAccountListDelegate::CreateAdvAccountListDelegate);
#ifdef IF_FEATURE_PSTN
    g_DelegateFactory.RegisterDelegate(kszPageIDPSTNAccountList,
                                       &CSettinguiAdvAccountListDelegate::CreateAdvAccountListDelegate);
#endif
    g_DelegateFactory.RegisterDelegate(kszPageIDAdvCommonAccountSetting,
                                       &CAdvCommonAccountSettingDelegate::CreateAdvCommonAccountSettingDelegate);
#if IF_FEATURE_CLOUD_ACCOUNT
    g_DelegateFactory.RegisterDelegate(kszPageIDAdvCloudAccountSetting,
                                       &CAdvCloudAccountSettingDelegate::CreateAdvCloudAccountSettingDelegate);
#endif
#if IF_SUPPORT_SOFTKEY_LABEL
    g_DelegateFactory.RegisterDelegate(kszPageIDSoftkeyLabelList,
                                       &CSettinguiSoftkeyLabelListDelegate::CreateSoftkeyLabelListDelegate);
#endif
    g_DelegateFactory.RegisterDelegate(kszPageIDLinekeyList,
                                       &CSettinguiLinekeyListDelegate::CreateLinekeyListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDDsskeySetting,
                                       &CSettinguiDsskeySettingDelegate::CreateDsskeySettingDelegate);
#ifdef IF_FEATURE_PSTN
    g_DelegateFactory.RegisterDelegate(kszPageIDPSTNFlashTime,
                                       &CSetttinguiPSTNFlashTimeDelegate::CreatePSTNFlashTimeDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDAdvPSTNAccountSetting,
                                       &CAdvPSTNAccountSettingDelegate::CreateAdvPSTNAccountSettingDelegate);
#endif
    //guide
#ifdef IF_SETTING_WIZARD
    g_DelegateFactory.RegisterDelegate(kszPageIDGuideLanguaue,
                                       &CSettingUILanguageDelegate::CreateLanguageDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDGuideTimeDate,
                                       &CSettingUITimeAndDateDelegate::CreateTimeAndDateDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDGuideCommonSipLine,
                                       &CAdvCommonAccountSettingDelegate::CreateAdvCommonAccountSettingDelegate);
#if IF_FEATURE_CLOUD_ACCOUNT
    g_DelegateFactory.RegisterDelegate(kszPageIDGuideCloudSipLine,
                                       &CAdvCloudAccountSettingDelegate::CreateAdvCloudAccountSettingDelegate);
#endif
#endif
    // history
    g_DelegateFactory.RegisterDelegate(kszPageIDHistory,
                                       &CSettingUIHistoryDelegate::CreateHistoryDelegate);

    // voice mail
    g_DelegateFactory.RegisterDelegate(kszPageIDViewVoiceMail,
                                       &CSettinguiViewVoiceMailDelegate::CreateViewVoiceMailDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDVoiceMailCodeList,
                                       &CVoiceMailCodeListDelegate::CreateVoiceMailCodeListDelegate);
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    g_DelegateFactory.RegisterDelegate(kszPageIDVoiceMailList,
                                       &CSettinguiVoiceMailListDelegate::CreateVoiceMailListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDVoiceMailDetail,
                                       &CSettinguiVoiceMailDetailDelegate::CreateVoiceMailDetailDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDVoiceMailPlay,
                                       &CSettinguiVoiceMailPlayDelegate::CreateVoiceMailPlayDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDVoiceMailAddToPersonal,
                                       &CSettinguiVoiceMailAddToPersonalDelegate::CreateVoiceMailAddToPersonalDelegate);
#endif
    // directory
    g_DelegateFactory.RegisterDelegate(kszPageIDDirectory,
                                       &CSettingUIDirectoryDelegate::CreateDirectoryDelegate);

    //user mode
    g_DelegateFactory.RegisterDelegate(kszPageIDUserMode,
                                       &CSettinguiUserModeDelegate::CreateUserModeDelegate);

    // usb record
#ifdef IF_USB_SUPPORT
    g_DelegateFactory.RegisterDelegate(kszPageIDUsbRecord,
                                       &CUSBRecordDelegate::CreateUSBRecordDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDUsbAudioList,
                                       &CUSBAudioListDelegate::CreateUSBAudioListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDUsbAudioPlayer,
                                       &CUSBAudioPlayerDelegate::CreateUSBAudioPlayerDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDUsbStorageInfo,
                                       &CUSBStorageInfoDelegate::CreateStorageInfoDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDUsbIdleRecord,
                                       &CUSBAudioRecordDelegate::CreateUSBAudioRecordDelegate);
#endif

    // textmessage
    g_DelegateFactory.RegisterDelegate(kszPageIDTextMsgInbox,
                                       &CTextMessageInboxDelegate::CreatTextMessageInboxDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTextMsgSentbox,
                                       &CTextMessageSentBoxDelegate::CreatTextMessageSentBoxDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTextMsgOutbox,
                                       &CTextMessageOutBoxDelegate::CreatTextMessageOutBoxDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTextMsgDraftbox,
                                       &CTextMessageDraftBoxDelegate::CreatTextMessageDraftBoxDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTextMsgSend,
                                       &CTextMessageSendDelegate::CreateTextMessageSendDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDTextMsgView,
                                       &CTextMessageViewDelegate::CreatTextMessageViewDelegate);

    //acd
    g_DelegateFactory.RegisterDelegate(kszPageIDACDDispositionCode,
                                       &CACDDispositionCodeDelegate::CreateACDDispositionCodeDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDACDEmergencyEscalation,
                                       &CACDEmergencyEscalationDelegate::CreateACDEmergencyEscalationDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDHotelingLogin,
                                       &CHotelingLoginDelegate::CreateHotelingLoginDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDGuestUnlockCheck,
                                       &CHotelingGuestUnlockCheckDelegate::CreateHotelingGuestUnlockCheckDelegate);

    // edk
    g_DelegateFactory.RegisterDelegate(kszPageIDEdkPrompt,
                                       &CEdkPromptDelegate::CreateEdkPromptDelegate);

    // reset
    g_DelegateFactory.RegisterDelegate(kszPageIDIdleResetNeedPwd,
                                       &CResetAuthorDelegate::CreateResetAuthorDelegate);
#ifdef  IF_FEATURE_GETBALANCE
    // reset
    g_DelegateFactory.RegisterDelegate(kszPageIDBalance, &CSettingUIBalanceDelegate::CreateBalanceDelegate);
#endif

#if IF_SUPPORT_WALLPAPER
    g_DelegateFactory.RegisterDelegate(kszPageIDWallPaper, &CWallpaperDelegate::CreatWallpaperDelegate);
#endif
#if IF_SUPPORT_EXPWALLPAPER
    g_DelegateFactory.RegisterDelegate(kszPageIDExpWallPaper, &CExpWallpaperDelegate::CreatExpWallpaperDelegate);
#endif

    // hotdesk
#ifdef IF_FEATURE_HOTDESKING
    g_DelegateFactory.RegisterDelegate(kszPageIDHotDeskBase,
                                       &CHotDeskBaseSettingDelegate::CreateHotDeskBaseSettingDelegate);
#endif
#if IF_FEATURE_METASWITCH_ACD
    g_DelegateFactory.RegisterDelegate(kszPageMTSWAcdList,
                                       &CMtswAcdListDelegate::CreatMtswAcdListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageMTSWAcdDetail,
                                       &CMtswAcdDetailDelegate::CreatMtswAcdDetailDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageMTSWAcdMembers,
                                       &CMtswAcdMemberDelegate::CreatMtswAcdMemberDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageMTSWAcdMyStatus,
                                       &CMtswAcdMyStatusDelegate::CreatMtswAcdMyStatusDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageMTSWAcdSelectDispcode,
                                       &CMtswAcdSelectDispcodeDelegate::CreatMtswAcdSelectDispcodeDelegate);
#endif

#if IF_FEATURE_METASWITCH
    g_DelegateFactory.RegisterDelegate(kszPageMTSWCommportal,
                                       &CMtswCommportalDelegate::CreatMtswCommportalDelegate);
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
    g_DelegateFactory.RegisterDelegate(kszPageMTSWVoiceMailList,
                                       &CMtswVoiceMailListDelegate::CreateMtswVoiceMailListDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageMTSWVoiceMailDetail,
                                       &CMtswVoiceMailDetailDelegate::CreateMtswVoiceMailDetailDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageMTSWVoiceMailPlay,
                                       &CMtswVoiceMailPlayDelegate::CreateMtswVoiceMailPlayDelegate);
#endif

#if IF_FEATURE_GENBAND_E911
    g_DelegateFactory.RegisterDelegate(kszPageIDE911Location,
                                       &CE911LocationDelegate::CreateE911LocationDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDSetE911Location,
                                       &CE911SetLocationDelegate::CreateE911SetLocationDelegate);
#endif

#if IF_SUPPORT_DEFAULT_ACCOUNT_DIALOG
    g_DelegateFactory.RegisterDelegate(kszPageIDDefaultAccount,
                                       &CSettinguiDefaultAccountListDelegate::CreateDefaultAccountListDelegate);

#endif

#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
    g_DelegateFactory.RegisterDelegate(kszPageIDUsbPhotoList,
                                       &CUSBPhotoListDelegate::CreateUSBPhotoListDelegate);
#endif

#if IF_SUPPORT_SELECT_CONTACT_INFO
    g_DelegateFactory.RegisterDelegate(kszPageIDHotLine,
                                       &CSettingUIHotLineDelegate::CreateSettingUIHotLineDelegate);
#endif
#if IF_SUPPORT_SCREEN_SAVER
    g_DelegateFactory.RegisterDelegate(kszPageIDScreenSaver,
                                       &CSettingUIScreenSaverDelegate::CreateSettingUIScreenSaverDelegate);
#endif

#if IF_SUPPORT_BLF_RING
    g_DelegateFactory.RegisterDelegate(kszPageIDBLFRingType,
                                       &CSettingUIBLFRingToneDelegate::CreateBLFRingToneDelegate);
#endif

#if FEATURE_BROADTOUCH_UC
    g_DelegateFactory.RegisterDelegate(kszPageIDMyStatus,
                                       &CSettingUIMyStatusDelegate::CreateSettingUIMyStatusDelegate);
#endif

#if IF_SUPPORT_BACK_LIGHT_LEVEL
    g_DelegateFactory.RegisterDelegate(kszPageIDBackLight,
                                       &CSettingUIBackLightDelegate::CreateSettingUIBackLightDelegate);
#endif

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkWebServer,
                                       &CSettingUINetworkChangeBaseDelegate::CreatNetworkChangeBaseDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkLLDP,
                                       &CSettingUINetworkChangeBaseDelegate::CreatNetworkChangeBaseDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDNetworkCDP,
                                       &CSettingUINetworkChangeBaseDelegate::CreatNetworkChangeBaseDelegate);

#endif

}

void InitPrivateAdapter()
{
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDStatusRTP,
            CSettinguiScrollByPageAdapter::CreateScrollByPageAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDPingResult,
            CSettinguiScrollByPageAdapter::CreateScrollByPageAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDTraceResult,
            CSettinguiScrollByPageAdapter::CreateScrollByPageAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDSNTPSetting,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#if IF_SUPPORT_IP_SET_SINGLE_MENU
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWanPortIPV4,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWanPortIPV6,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#else
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWanPortIpv6DhcpClient,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWanPortIpv4DhcpClient,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#endif
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDNetwork8021X,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDDsskeySetting,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDContrast,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWanPortOption,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#if IF_FEATURE_CLOUD_ACCOUNT
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDAdvCloudAccountSetting,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#ifdef IF_SETTING_WIZARD
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDGuideCloudSipLine,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#endif
#endif
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDCallControllExecutiveFilter,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDCallControllExecutiveAlter,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDCallControllAssistantCommon,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#endif
#ifdef IF_SUPPORT_WIFI
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWifiEdit,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWifiAdd,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWifiDetail2Add,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#endif
#ifdef IF_USB_SUPPORT
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDUsbAudioList,
            CUSBAudioListAdapter::CreateUSBAudioListAdapter);
#endif
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDAutoAnswer,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDTextMsgView,
            CSettinguiScrollByPageAdapter::CreateScrollByPageAdapter);
#if IF_SUPPORT_WALLPAPER
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDWallPaper,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#endif
#if IF_SUPPORT_EXPWALLPAPER
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDExpWallPaper,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageMTSWVoiceMailPlay,
            CVoiceMailPlayAdapter::CreateVoiceMailPlayAdapter);
#endif
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDVoiceMailPlay,
            CVoiceMailPlayAdapter::CreateVoiceMailPlayAdapter);
#endif
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDUsbPhotoList,
            CUSBPhotoListAdapter::CreateUSBPhotoListAdapter);
#endif
#if IF_SUPPORT_SELECT_CONTACT_INFO
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDDsskeySetting,
            CSettingUIValueChangeAdapter::CreateValueChangeAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDTextMsgSend,
            CSettingUIValueChangeAdapter::CreateValueChangeAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDHotLine,
            CSettingUIValueChangeAdapter::CreateValueChangeAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDAlwaysForward,
            CSettingUIValueChangeAdapter::CreateValueChangeAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDBusyForward,
            CSettingUIValueChangeAdapter::CreateValueChangeAdapter);
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDNoAnswerForward,
            CSettingUIValueChangeAdapter::CreateValueChangeAdapter);
#endif
#if IF_SUPPORT_BACK_LIGHT_LEVEL
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDBackLight,
            &CSettingUIComboboxAdapter::CreateComboboxAdapter);
#endif
}

// 界面数据类初始化
void InitPageData()
{
    g_PageDataFactory.RegisterPageData(kszPageIDAlwaysForward,
                                       CSettingUIAccountPageDataBase::CreateAccountPageData);
    g_PageDataFactory.RegisterPageData(kszPageIDBusyForward,
                                       CSettingUIAccountPageDataBase::CreateAccountPageData);
    g_PageDataFactory.RegisterPageData(kszPageIDNoAnswerForward,
                                       CSettingUIAccountPageDataBase::CreateAccountPageData);
    g_PageDataFactory.RegisterPageData(kszPageIDCallForward,
                                       CSettingUIAccountPageDataBase::CreateAccountPageData);
}

void InitItem()
{
    g_ItemFactory.RegisterItem(CPingAndTraceoutItem::GetPingAndTraceoutItemType(),
                               &CPingAndTraceoutItem::CreatePingAndTraceoutListItem);
#ifdef IF_SUPPORT_BLUETOOTH
    g_ItemFactory.RegisterItem(CBluetoothDevItem::GetBluetoothDevItemType(),
                               &CBluetoothDevItem::CreateBluetoothDevItem);
    g_ItemFactory.RegisterItem(CBTChannelControllItem::GetBTChannelControllItemType(),
                               &CBTChannelControllItem::CreateBTChannelControllItem);
#endif
#ifdef IF_SUPPORT_WIFI
    g_ItemFactory.RegisterItem(CWifiNetItem::GetWifiNetItemType(), &CWifiNetItem::CreateWifiNetItem);
#endif
    g_ItemFactory.RegisterItem(CTextMessageItem::GetTextMessageItemType(),
                               &CTextMessageItem::CreateTextMessageItem);
#ifdef IF_USB_SUPPORT
    g_ItemFactory.RegisterItem(CUSBAudioListItem::GetUSBAudioListItemType(),
                               &CUSBAudioListItem::CreateUSBAudioListItem);
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    g_ItemFactory.RegisterItem(CMainMenuItem::GetMainMenuItemType(),
                               &CMainMenuItem::CreateMainMenuItem);
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
    g_ItemFactory.RegisterItem(CVoiceMailTimerItem::GetVoiceMailTimerItemType(),
                               &CVoiceMailTimerItem::CreateVoiceMailTimerItem);
#endif
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
    g_ItemFactory.RegisterItem(CUSBPhotoListItem::GetUSBPhotoListItemType(),
                               &CUSBPhotoListItem::CreateUSBPhotoListItem);
#endif
}

void InitItemData()
{
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameInupt,
                                       CSettingUILineEditItemData::CreateLineEditDataItem);
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameCombobox,
                                       CSettingUIComboBoxItemData::CreateComboBoxDataItem);
#if IF_SUPPORT_CHECK_BOX
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameSwitch,
                                       CSettingUICheckBoxItemData::CreateCheckBoxDataItem);
#endif
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameMenu,
                                       CSettingUIMenuItemData::CreateMenuListDataItem);
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameLabel,
                                       CSettingUIItemDataBase::CreateBaseDataItem);
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameDetailMenu,
                                       CSettingUIDetailMenuItemData::CreateDetailMenuListDataItem);
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameRadio,
                                       CSettingUIRadioItemData::CreateRadioListDataItem);
#if IF_SUPPORT_CUSTOM_COMBO_BOX
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameCustomCombobox,
                                       CSettingUICustomComboBoxItemData::CreateCustomComboBoxDataItem);
#endif
#ifdef IF_SUPPORT_WIFI
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameWifiNet,
                                       CSettingUIWifiNetItemData::CreateWifiNetDataItem);
#endif
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameTextMessage,
                                       CSettingUITextMessageItemData::CreateTextMessageDataItem);
#ifdef IF_SUPPORT_BLUETOOTH
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameBluetoothDev,
                                       CSettingUIBluetoothDevItemData::CreateBluetoothDevDataItem);
    g_ItemDataFactory.RegisterItemData(kszItemTypeBtChannelControll,
                                       CSettingUIBTChannelControllItemData::CreateBTChannelControllDataItem);
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameMainMenu,
                                       CSettingUIMainMenuItemData::CreateMainMenuDataItem);
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
    g_ItemDataFactory.RegisterItemData(kszItemTypeNameVoiceMailTimer,
                                       CVoiceMailTimerItemData::CreateVoiceMailTimerDataItem);
#endif
}

void InitItemDataParser()
{
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameInupt,
            CSettingUILineEditItemData::ParserLineEditItemData);
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameCombobox,
            CSettingUIComboBoxItemData::ParserComboBoxItemData);
#if IF_SUPPORT_CHECK_BOX
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameSwitch,
            CSettingUICheckBoxItemData::ParserCheckBoxItemData);
#endif
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameMenu,
            CSettingUIMenuItemData::ParserMenuItemData);
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameLabel,
            CSettingUIItemDataBase::ParserItemData);
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameDetailMenu,
            CSettingUIDetailMenuItemData::ParserDetailMenuItemData);
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameRadio,
            CSettingUIRadioItemData::ParserRadioItemData);
#if IF_SUPPORT_CUSTOM_COMBO_BOX
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameCustomCombobox,
            CSettingUICustomComboBoxItemData::ParserCustomComboBoxItemData);
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    g_ItemDataParserFactory.RegisterItemDataParser(kszItemTypeNameMainMenu,
            CSettingUIMainMenuItemData::ParserMainMenuItemData);
#endif
}

void InitItemType()
{
    g_ItemTypeMgr.AddItemType(kszItemTypeNameInupt,
                              CSettingUILineEditItemData::GetLineEditDataItemType());
    g_ItemTypeMgr.AddItemType(kszItemTypeNameCombobox,
                              CSettingUIComboBoxItemData::GetComboBoxDataItemType());
#if IF_SUPPORT_CHECK_BOX
    g_ItemTypeMgr.AddItemType(kszItemTypeNameSwitch,
                              CSettingUICheckBoxItemData::GetCheckBoxDataItemType());
#endif
    g_ItemTypeMgr.AddItemType(kszItemTypeNameMenu, CSettingUIMenuItemData::GetMenuDataItemType());
    g_ItemTypeMgr.AddItemType(kszItemTypeNameLabel, CSettingUIItemDataBase::GetBaseDataItemType());
    g_ItemTypeMgr.AddItemType(kszItemTypeNameDetailMenu,
                              CSettingUIDetailMenuItemData::GetDetailMenuDataItemType());
    g_ItemTypeMgr.AddItemType(kszItemTypeNameRadio, CSettingUIRadioItemData::GetRadioDataItemType());
#if IF_SUPPORT_CUSTOM_COMBO_BOX
    g_ItemTypeMgr.AddItemType(kszItemTypeNameCustomCombobox,
                              CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItemType());
#endif
#ifdef IF_SUPPORT_WIFI
    g_ItemTypeMgr.AddItemType(kszItemTypeNameWifiNet,
                              CSettingUIWifiNetItemData::GetWifiNetDataItemType());
#endif
    g_ItemTypeMgr.AddItemType(kszItemTypeNameTextMessage,
                              CSettingUITextMessageItemData::GetTextMessageDataItemType());
#ifdef IF_SUPPORT_BLUETOOTH
    g_ItemTypeMgr.AddItemType(kszItemTypeNameBluetoothDev,
                              CSettingUIBluetoothDevItemData::GetBluetoothDevDataItemType());
    g_ItemTypeMgr.AddItemType(kszItemTypeBtChannelControll,
                              CSettingUIBTChannelControllItemData::GetBTChannelControllDataItemType());
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    g_ItemTypeMgr.AddItemType(kszItemTypeNameMainMenu,
                              CSettingUIMainMenuItemData::GetMainMenuDataItemType());
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
    g_ItemTypeMgr.AddItemType(kszItemTypeNameVoiceMailTimer,
                              CVoiceMailTimerItemData::GetVoiceMailTimerDataItemType());
#endif
}

void InitItemDataAndItemTypeTrans()
{
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUILineEditItemData::GetLineEditDataItemType()
        , CEditListItem::GetEditListItemType());
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUIComboBoxItemData::GetComboBoxDataItemType()
        , CComboBoxListItem::GetComboBoxListItemType());
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(CSettingUIMenuItemData::GetMenuDataItemType()
            , CMenuListItem::GetMenuItemType());
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(CSettingUIItemDataBase::GetBaseDataItemType()
            , CDetailListItem::GetDetailListItemType());
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUIDetailMenuItemData::GetDetailMenuDataItemType()
        , CDetailListItem::GetDetailListItemType());
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(CSettingUIRadioItemData::GetRadioDataItemType()
            , CRadioListItem::GetRadioItemType());
#if IF_SUPPORT_CUSTOM_COMBO_BOX
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItemType()
        , CComboBoxListItem::GetComboBoxListItemType());
#endif
#ifdef IF_SUPPORT_WIFI
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUIWifiNetItemData::GetWifiNetDataItemType()
        , CWifiNetItem::GetWifiNetItemType());
#endif
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUITextMessageItemData::GetTextMessageDataItemType()
        , CTextMessageItem::GetTextMessageItemType());
#ifdef IF_SUPPORT_BLUETOOTH
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUIBluetoothDevItemData::GetBluetoothDevDataItemType()
        , CBluetoothDevItem::GetBluetoothDevItemType());
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUIBTChannelControllItemData::GetBTChannelControllDataItemType()
        , CBTChannelControllItem::GetBTChannelControllItemType());
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CSettingUIMainMenuItemData::GetMainMenuDataItemType()
        , CMainMenuItem::GetMainMenuItemType());
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
    g_ItemDataTypeTransFactory.RegisterItemDataTypeTrans(
        CVoiceMailTimerItemData::GetVoiceMailTimerDataItemType()
        , CVoiceMailTimerItem::GetVoiceMailTimerItemType());
#endif
}

void InitItemDataValueTrans()
{
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(CSettingUIItemDataBase::GetBaseDataItemType()
            , CItemDataValueTransBase::CreatBaseItemDataValueTrans);
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(CSettingUIMenuItemData::GetMenuDataItemType()
            , CMenuItemDataValueTrans::CreatMenuItemDataValueTrans);
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUIComboBoxItemData::GetComboBoxDataItemType()
        , CComboxBoxItemDataTrans::CreatComboxboxItemDataValueTrans);
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUILineEditItemData::GetLineEditDataItemType()
        , CEditItemDataTrans::CreatEditItemDataValueTrans);
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUIDetailMenuItemData::GetDetailMenuDataItemType()
        , CDetailMenuItemDataValueTrans::CreatDetailMenuItemDataValueTrans);
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUIRadioItemData::GetRadioDataItemType()
        , CRadioItemDataValueTrans::CreatRadioItemDataValueTrans);
#if IF_SUPPORT_CUSTOM_COMBO_BOX
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItemType()
        , CComboxBoxItemDataTrans::CreatComboxboxItemDataValueTrans);
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUIBluetoothDevItemData::GetBluetoothDevDataItemType()
        , CBluetoothDevItemValueTrans::CreatBluetoothDevItemDataValueTrans);
#endif
#ifdef IF_SUPPORT_WIFI
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUIWifiNetItemData::GetWifiNetDataItemType()
        , CWifiNetItemValueTrans::CreatWifiNetItemDataValueTrans);
#endif
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUITextMessageItemData::GetTextMessageDataItemType()
        , CTextMessageItemValueTrans::CreatTextMessageItemDataValueTrans);
#ifdef IF_SUPPORT_BLUETOOTH
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUIBTChannelControllItemData::GetBTChannelControllDataItemType()
        , CBTChannelControllItemValueTrans::CreatBTChannelControllItemDataValueTrans);
#endif
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CSettingUIMainMenuItemData::GetMainMenuDataItemType()
        , CMainMenuItemValueTrans::CreatMainMenuItemDataValueTrans);
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
    g_ItemDataValueTransFactory.RegisterItemDataValueTrans(
        CVoiceMailTimerItemData::GetVoiceMailTimerDataItemType()
        , CVoiceMailTimerItemValueTrans::CreatVoiceMailTimerItemDataValueTrans);
#endif
}

void InitCheckMethod()
{
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeIP, IsValidIP);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeIPv4, IsValidIPv4);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeIPv4NetMask, IsValidIPv4NetMask);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeIPv6, IsValidIPv6);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeIPv6GateWay, IsValidIPv6GateWay);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeIPv6Prefix, IsValidIPv6Prefix);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypePort, IsValidPort);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeDTMF, IsValidDTMF);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeNumber, IsValidNumber);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeAES, IsValidAES);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeEmpty, IsContentEmpty);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeURL, IsValidURLServer);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeServer, IsValidServer);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeServerName, IsValidServerName);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeAccountName, IsValidAccountName);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypePwd, IsValidPassword);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeQos, IsValidQos);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeVideoMTU, IsValidVideoMTU);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeVlanVid, IsValidVlanVID);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypePacketInterval, IsValidPacketInterval);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeServerExpires, IsValidServerExpires);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeUdpPort, IsValidUdpPort);
    g_CheckMethodFactory.RegisterCheckMethod(kszCheckMethodTypeConferencePwd, IsValidConferencePwd);
}

void SettingUI_Reboot(int nTimeDelay/* = 1*/)
{
    g_SettingMsgBoxMgr.ShowReboot(nTimeDelay);
}

void SettingUI_Reset(int nResetType)
{
    g_SettingMsgBoxMgr.ShowResetFactory(nResetType);
}

void SettingUI_UploadConfig()
{
    g_SettingMsgBoxMgr.ShowUploadConfig();
}

void SettingUI_UpdateFirware()
{
    g_SettingMsgBoxMgr.ShowUpdateFirware();
}

bool SettingUI_EnterPageByType(const int ePageType, int nIndex)
{
    bool bRet = false;
    yl::string strPageName = "";
    void * pExtraData = NULL;
    int nExtra = -1;
    bool bExtra = false;

    switch (ePageType)
    {
    case CC_PAGE_ACD_LOGIN:
        {
            strPageName = kszPageIDHotelingLogin;
        }
        break;
    case CC_PAGE_ACD_STATUS:
        {
            strPageName = kszPageIDCallControllInitialACDStatus;
        }
        break;
    case CC_PAGE_CALLCENTER_ACDSELECTSTATE:
        {
            if (ACD_GENESYS == Get_ACDType())
            {
                strPageName = kszPageIDCallControllInitialACDStatus;
            }
            else
            {
                strPageName = kszPageIDCallControllSetACDStatus;
            }
        }
        break;
    case CC_PAGE_CALLCENTER_HOTELINGLOGIN:
        {
            strPageName = kszPageIDHotelingLogin;
            nExtra = HOTELING_TYPE_ACDON;
            pExtraData = (void *)(&nExtra);
        }
        break;
    case CC_PAGE_CALLCENTER_HOTELINGLOGINACDOFF:
        {
            strPageName = kszPageIDHotelingLogin;
            nExtra = HOTELING_TYPE_ACDOFF;
            pExtraData = (void *)(&nExtra);
        }
        break;
#warning CC_PAGE_CALLCENTER_HOTELINGLOGINXMLBROWSER
    //case CC_PAGE_CALLCENTER_HOTELINGLOGINXMLBROWSER:
    //    {
    //        strPageName = kszPageIDHotelingLogin;
    //        nExtra = HOTELING_TYPE_XMLBROWSER;
    //        pExtraData = (void *)(&nExtra);
    //    }
    //    break;
    case CC_PAGE_CALLCENTER_DISPOSITIONCODE:
        {
            strPageName = kszPageIDACDDispositionCode;
        }
        break;
    case CC_PAGE_CALLCENTER_EMERGENCYESCALATION:
        {
            strPageName = kszPageIDACDEmergencyEscalation;
        }
        break;
    case MENU_PAGE_SET_VOICEMAIL:
        {
            strPageName = kszPageIDVoiceMailCodeList;
        }
        break;
    case MENU_PAGE_VIEW_VOICEMAIL:
        {
            strPageName = kszPageIDViewVoiceMail;
        }
        break;
    case MENU_PAGE_MAIN:
        {
            strPageName = kszPageIDMainMenu;
        }
        break;
    case MENU_PAGE_ALWAYS_FORWARD:
        {
            strPageName = kszPageIDAlwaysForward;
        }
        break;
    case MENU_PAGE_BUSY_FORWARD:
        {
            strPageName = kszPageIDBusyForward;
        }
        break;
    case MENU_PAGE_NOANSWER_FORWARD:
        {
            strPageName = kszPageIDNoAnswerForward;
        }
        break;
    case MENU_PAGE_STATUS:
        {
            strPageName = kszPageIDStatus;
        }
        break;
    case MENU_PAGE_AUTOPCODEAUTH:
        {
            strPageName = kszPageIDZeroAutoP;
            bExtra = true;
            pExtraData = (void *)(&bExtra);
        }
        break;
    case PAGE_TYPE_HOTDESK:
        {
            strPageName = kszPageIDHotDeskBase;
            nExtra = HDT_LOGON_WIZRAD;
            pExtraData = (void *)(&nExtra);
        }
        break;
    default:
        break;
    }

    if (g_pSettingUIManager->IsPageDelegateCreate(strPageName))
    {
        SETTINGUI_INFO("Current PageUI [%s] no need to enter twice", strPageName.c_str());
        return false;
    }

    if (strPageName.empty())
    {
        return false;
    }
    else
    {
        if (NULL == pExtraData)
        {
            bRet = SettingUI_EnterPage(strPageName);
        }
        else
        {
            bRet = SettingUI_EnterPage(strPageName, pExtraData);
        }
    }

    return bRet;
}

#ifdef IF_SETTING_WIZARD
void SettingUI_EnterSetUpWizard()
{
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsWizardDEnable())
    {
        g_pSettingUIWizardManager->EnterWizard();
    }
}

// 功能:是否正在设置向导
bool SettingUI_IsWizarding()
{
    if (NULL != g_pSettingUIWizardManager)
    {
        return g_pSettingUIWizardManager->IsInWizard();
    }

    return false;
}

#endif

#if IF_FEATURE_METASWITCH
void SettingUI_MTSWLoginReturnCallBack(bool bLoginSucceed)
{
    if (!bLoginSucceed)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
    }
}
#endif

bool SettingUI_GetSwitchIndex(int nTotal, int nCurIndex, bool bNext, int& nSwitchIndex)
{
    nSwitchIndex = nCurIndex;
    if (0 >= nTotal || nCurIndex < 0)
    {
        return false;
    }

    if (bNext)
    {
        nSwitchIndex = (nCurIndex + 1) % nTotal;
    }
    else
    {
        if (0 == nCurIndex)
        {
            nSwitchIndex = nTotal - 1;
        }
        else
        {
            nSwitchIndex = nCurIndex - 1;
        }
    }
    return true;
}

bool SettingUI_IsTopPage(const yl::string& strPageID)
{
    if (NULL == g_pSettingUIDialogManager->GetTopActiveDialog())
    {
        return false;
    }

    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();

    if (NULL == pDelegate || strPageID != pDelegate->GetPageID())
    {
        return false;
    }

    return true;
}

bool SettingUI_EnterUnlockCheck()
{
    bool bEnterBySetting = false;
    Volumebar_SetVolumeBarVisible(false);

#if IF_UNLOCK_CHECK_POPBOX
    g_SettingMsgBoxMgr.ShowUnlockCheckMsgBox(bEnterBySetting);
#else
    SettingUI_EnterPage(kszPageIDPhoneUnlockCheck, (void *)(&bEnterBySetting));
#endif
    return true;
}

void SettingUI_ShowNoteBox(const yl::string & strContent)
{
    MSGBOX_INFO infoMsgBox;
    infoMsgBox.eType = MESSAGEBOX_NOTE;
    infoMsgBox.pAction = NULL;
    infoMsgBox.iID = -1;
    infoMsgBox.strNote = _LANG(strContent);
    infoMsgBox.nMsTime = SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME;
    MessageBox_ShowCommonMsgBox(infoMsgBox);
}

