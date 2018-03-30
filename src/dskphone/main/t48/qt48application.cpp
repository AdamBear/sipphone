#include "qt48application.h"

// baseLibs
#include <ETLLib.hpp>
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "dsklog/log.h"

//include
#include "keymap.h"

// logic
#include "commonunits/modcommonunits.h"
// #include "commonUnits/modBluetooth.h"
// #include "commonunits/statusmanager.h"
#include "account/include/modaccount.h"
#include "setting/include/common.h"
#include "setting/src/languagecontroller.h"
#include "contacts/directory/include/moddirinterface.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "contacts/supersearch/include/modsupersearch.h"
#include "broadsoft/callpark/include/modcallpark.h"
// #include "broadsoft/callcenter/include/modcallcenter.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "talk/callcompletion/include/modcallcompletion.h"
#include "extended/csta/include/modcsta.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "extended/hotdesking/include/modhotdesking.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "lamp/backlight/include/modbacklight.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "voice/include/modvoice.h"
#include "dsskey/include/moddsskey.h"
#include "presence/include/modpresence.h"
#include "edkui/modedkui.h"
#include "screensavers/include/modscreensavers.h"
#include "threadtask/include/modthreadtask.h"
//ui
//include
#include "picpath.h"

// UICommon
#include "globalinputhook/modinputhook.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/languagehelper.h"
// #include "uikernel/stylebase.h"
#include "uimanager/moduimanager.h"
#include "customtheme/modcustomtheme.h"

// T48
#include "mainwnd/mainwnd.h"
#include "mainwndhelp/modmainwndhelp.h"
#include "mainwndhelp/keyguardhelp.h"
#include "baseui/t4xpicpath.h"
#include "baseui/t4xstyle.h"
#include "baseui/modbaseui.h"
#include "talkui/include/modtalkui.h"
#include "callmanager/modcallmanager.h"
#include "setting/include/modsetting.h"
#include "setting/include/modmenu.h"
#include "setting/include/modzero.h"
#include "zerosptouchui/include/modzerosptouch.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "xmlbrowserui/modxmlbrowserui.h"
#include "settingui/include/modsettingui.h"
#include "messageui/include/modmessageui.h"
#include "directoryui/include/moddirectoryui.h"
#include "idlescreen/include/modidlescreen.h"
#include "idlescreenui/modidlescreenui.h"
#include "adapterbox/include/modsetadapterbox.h"
#include <resource/modresource.h>
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "setting_page_define.h"

#include "uicommon/messagebox/modmessagebox.h"
#include "uicommon/qtcommon/qImageFunc.h"
#include "adapterbox/include/modadapterbox.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"

#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/modwifi.h"
#endif

#include "feature/include/modfeature.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "testmode/include/modtestmode.h"
#include "autoset/include/modautoset.h"
#include "inputmethod/inputmethod.h"

#include "edk/include/modedk.h"
#include "mainwnd/modmainwnd.h"
#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#endif

#include "exp/include/modexp.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

// #include "pluginobject.h"
#include "resource/modresource.h"
#include "commonunits/t2xpath.h"

#ifdef IF_USB_SUPPORT
#include "hotplug/modhotplug.h"
#include "record/include/modrecord.h"
#endif
#include "blf/include/modblf.h"
#include "taskaction/modtaskaction.h"

#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#endif
#ifdef IF_SUPPORT_USB_AUDIO
#include "usb_headset/include/modusbheadset.h"
#endif

#include "notify/include/modnotify.h"

#if IF_FEATURE_GENBAND_E911
#include "e911address/include/mode911address.h"
#endif

#include "extended/warningcenter/include/modwarningcenter.h"


extern bool g_bAfterInit;

bool PreKeyHookBegin(int nPhoneKeyCode, bool bPress);
bool PreKeyHookEnd(int nPhoneKeyCode, bool bPress);
bool PreMouseHookBegin(QObject * pObject, QMouseEvent * pEvent);
bool PreMouseHookEnd(QObject * pObject, QMouseEvent * pEvent);

// 内部函数定义
extern bool IsDigitKey(int iKey);
// 播放按键音
void qApp_PlayKeyTone(int iKeyCode)
{
    if (talklogic_SessionExist()        // 存在通话时，不播放按键音
            || dsskey_IsDsskey(iKeyCode)    // Dsskey不播放按键音
            || iKeyCode == PHONE_KEY_VOLUME_DECREASE
            || iKeyCode == PHONE_KEY_VOLUME_INCREASE // 调节音量键不播放按键音
            || iKeyCode == PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD
            || voice_IsChannelKey(iKeyCode) // 通道键不播放按键音
            || ScreenSavers_IsExist())
    {

    }
    else if (UIManager_IsInIdle()) // 在IDLE的时,需要耳麦开启才播放funtionkey的按键音
    {
        if (voice_GetHeadsetStatus()
                && !IsDigitKey(iKeyCode))
        {
            voice_PlayDTMFInMenu(PHONE_KEY_A);
        }
    }
    else
    {
        // 其他界面需要播放数字和funtionkey的按键音
        voice_PlayDTMFInMenu(IsDigitKey(iKeyCode) ? iKeyCode : PHONE_KEY_A);
    }
}

void qApp_RebootTip(const yl::string & strRebootTip)
{
    _MainWND.ShowRebootMessage(LANG_TRANSLATE(strRebootTip.c_str()));
}

void qApp_ResetFactory(int iResetType)
{
    yl::string strRebootTip;
    bool bNeedLogoutAccont = false;

    if (RESET_OPTION_USERDATA_ONLY == iResetType)
    {
        strRebootTip = TRID_RESETTING_USEDATA_PLEASE_WAIT;
    }
    else if (RESET_OPTION_CONFIG_ONLY == iResetType)
    {
        strRebootTip = TRID_RESETTING_CONFIGURATION_PLEASE_WAIT;
    }
    else if (RESET_OPTION_ALL == iResetType)
    {
        strRebootTip = TRID_RESETTING_TO_FACTORY_PLEASE_WAIT;
        bNeedLogoutAccont = true;
    }
    else if (RESET_OPTION_USERDATA_USERCFG == iResetType)
    {
        strRebootTip = TRID_RESETTING_USERDATA_CFG_PLEASE_WAIT;
    }
    else
    {
        APP_WARN("qApp_ResetFactory Type Error, ResetType[%d].", iResetType);
        strRebootTip = TRID_RESETTING_TO_FACTORY_PLEASE_WAIT;
        bNeedLogoutAccont = true;
    }

    _MainWND.ShowRebootMessage(LANG_TRANSLATE(strRebootTip.c_str()));

#if IF_BUG_20555
    if (bNeedLogoutAccont)
    {
        acc_LogoutBeforeReset();
    }
#endif  //IF_BUG_20555

    commonUnits_ResetToFatory((ResetOptionType)iResetType);
}

qT48Application::qT48Application(int & argc, char ** arg)
    : QDSKApplication(argc, arg)
{
#ifdef WIN32
    extern void ParseRunCommand(int argc, char * arg[]);
    ParseRunCommand(argc, arg);

    etl_RegisterMsgHandle(DEV_MSG_FUNKEY_EVENT, DEV_MSG_FUNKEY_EVENT, &qT48Application::OnMessageEx);
    etl_RegisterMsgHandle(DEV_MSG_DIGITKEY_EVENT, DEV_MSG_DIGITKEY_EVENT,
                          &qT48Application::OnMessageEx);
    etl_RegisterMsgHandle(DEV_MSG_KEY_RELEASED, DEV_MSG_KEY_RELEASED, &qT48Application::OnMessageEx);
#endif
    inputHook_RegisterKeyPadHookProc(PreKeyHookBegin, HOOK_MODULE_PREHOOKBEGIN);
    inputHook_RegisterKeyPadHookProc(PreKeyHookEnd, HOOK_MODULE_PREHOOKEND);
    inputHook_RegisterMouseHookProc(PreMouseHookBegin, HOOK_MODULE_PREHOOKBEGIN);
    inputHook_RegisterMouseHookProc(PreMouseHookEnd, HOOK_MODULE_PREHOOKEND);

    //延时1秒后将界面加载一遍
    QTimer::singleShot(1000, this, SLOT(OnDelayInit()));
}

qT48Application::~qT48Application(void)
{
    inputHook_UnRegisterKeyPadHookProc(PreKeyHookBegin);
    inputHook_UnRegisterKeyPadHookProc(PreKeyHookEnd);
    inputHook_UnRegisterMouseHookProc(PreMouseHookBegin);
    inputHook_UnRegisterMouseHookProc(PreMouseHookEnd);
}

// 回调函数，遍历消息函数表
BOOL qT48Application::OnMessage(msgObject & objMessage)
{
    /*
        APP_INFO("~~~~~~~~~~~~~~~~~~~APP Receive MSG:0x%x | wP:%d | lP:%d~~~~~~~~~~~~~~~~~~~~~~~",
            objMessage.message, objMessage.wParam, objMessage.lParam);
    */
    return etl_OnMessageProcess(objMessage);
}

LRESULT qT48Application::OnMessageEx(msgObject & objMessage)
{
    /*APP_INFO("~~~~~~~~~~~~~~~~~~~APP Receive MSG:0x%x | wP:%d | lP:%d~~~~~~~~~~~~~~~~~~~~~~~",
        objMessage.message, objMessage.wParam, objMessage.lParam);*/
#ifdef WIN32
    if (objMessage.message == DEV_MSG_KEY_RELEASED)
    {
        inputHook_SendKey(objMessage.lParam, false);
    }
    else if (objMessage.message == DEV_MSG_FUNKEY_EVENT
             || objMessage.message == DEV_MSG_DIGITKEY_EVENT)
    {
        inputHook_SendKey(objMessage.lParam, true);
    }
#endif
    return TRUE;
}

bool IsPowerSavingAllowThroughKey(int nPhoneKeyCode)
{
    if ((nPhoneKeyCode >= PHONE_KEY_0 && nPhoneKeyCode <= PHONE_KEY_9)
            || PHONE_KEY_STAR == nPhoneKeyCode
            || PHONE_KEY_POUND == nPhoneKeyCode
            || PHONE_KEY_HANDSET == nPhoneKeyCode
            || PHONE_KEY_HANDSET_ON_HOOK == nPhoneKeyCode
            || PHONE_KEY_HANDFREE == nPhoneKeyCode
            || PHONE_KEY_HANDSET_OFF_HOOK == nPhoneKeyCode
            || PHONE_KEY_HEADSET_CTRL == nPhoneKeyCode)
    {
        return true;
    }

    return false;
}

bool PreKeyHookBegin(int nPhoneKeyCode, bool bPress)
{
    if (commonUnits_IsSystemMaintenanceStatus())
    {
        return true;
    }

    // 必须先获取值，再点亮屏幕
    bool bInPowerSaving = PowerSaving_IsInPowerSavingMode();

	// 按下时候去点亮屏幕，按键UP时候不需要了
    if (bPress)
    {
        // 点亮屏幕
        backlight_Reset(true);
    }

    if (bInPowerSaving && UIManager_IsInIdle() && !IsPowerSavingAllowThroughKey(nPhoneKeyCode))
    {
        return true;
    }

    // ----声道按键处理是否要提前(功能：禁用通道按键)
    if (voice_ProcessChannelKeyAccess(nPhoneKeyCode))
    {
        return true;
    }

    // 处理音量条事件
    if (_MainWND.ProcessVolumeBarKeyEvent(nPhoneKeyCode, bPress))
    {
        return true;
    }

    if (nPhoneKeyCode == PHONE_KEY_MUTE && bPress)
    {
        //持续mute功能开启,拦截mute事件提前处理掉
        if (configParser_GetConfigInt(kszMuteMode) == 1)
        {
            talklogic_SetPersistentMuteStatus();
            return true;
        }
    }

#if IF_BUG_30885
    //自动启动zero sp touch情况下过滤免提等事件
    if (voice_IsChannelKey(nPhoneKeyCode)
            && zero_IsZeroStart())
    {
        return true;
    }
#endif

    // 测试模式让测试流程自己处理声音
    // xmlbrowser自己处理声音
    if (!testmode_IsTestMode() &&
            !XmlBrowserUI_IsPopuping() &&
            bPress)
    {
        //通道模块处理
        voice_OnKeyPress(nPhoneKeyCode);
        // 播放按键音
        qApp_PlayKeyTone(nPhoneKeyCode);
    }

    return false;
}

bool OnGlobalKeyPress(int nKey)
{
    if (talklogic_GetAmountOfSession() == 0)
    {
        if (voice_IsCallCenterMode())
        {
            if (nKey == PHONE_KEY_HANDSET_OFF_HOOK
                    || nKey == PHONE_KEY_HANDSET_ON_HOOK)
            {
                // 双耳麦模式下Idle界面摘挂机无效
                return false;
            }
        }

        // Idle模式下
        if (nKey == PHONE_KEY_HANDSET_ON_HOOK
                || nKey == PHONE_KEY_HANDSET_OFF_HOOK
                || nKey == PHONE_KEY_HANDFREE
                || nKey == PHONE_KEY_EHS_OPEN
                || configParser_GetConfigInt(kszHeadsetCtrlCall) == 1
                && nKey == PHONE_KEY_HEADSET_CTRL)
        {
            // 通道按键
            if (voice_GetHandsetStatus()
                    || voice_GetHandfreeStatus()
                    || nKey == PHONE_KEY_EHS_OPEN
                    || configParser_GetConfigInt(kszHeadsetCtrlCall) == 1
                    && voice_GetHeadsetStatus())
            {
                // 手柄摘起或免提按下，进入拨号界面
                //http://bugfree.yealink.com/Bug.php?BugID=89347
                if (!talklogic_EnterDialUI("") && !talklogic_SessionExist())
                {
                    voice_SwitchChannel();
                }

                return true;
            }
        }
    }
    else
    {
        if (nKey == PHONE_KEY_HANDSET_ON_HOOK
                || nKey == PHONE_KEY_HANDSET_OFF_HOOK
                || nKey == PHONE_KEY_HANDFREE
                || configParser_GetConfigInt(kszHeadsetCtrlCall) == 1
                && nKey == PHONE_KEY_HEADSET_CTRL)
        {
            if (UIManager_ExistCallManager()
                    && DLG_Dial != callmanager_GetCurrentPageName())
            {
                //判断是否存在 拨号界面
                if (NULL != callmanager_GetPageByUIName(DLG_Dial))
                {
                    //如果有存在拨号  那么焦点窗口肯定是拨号窗口， 所以这边调用逻辑层的刷新窗口接口就可以了
                    talklogic_UpdateWnd();
                }
                else
                {
                    talklogic_EnterDialUI("");
                }

                return false;
            }

            if (!UIManager_IsInTalk())
            {
                // 如果有其他模块存在，则退出
                //直接使用 UIManager_ReturnToIdle 处理
                UIManager_ReturnToIdle(RETURN_IDLE_TYPE_NORMAL);
            }
        }
    }

    return false;
}

bool PreKeyHookEnd(int nPhoneKeyCode, bool bPress)
{
    if (bPress)
    {
        // ----声道按键处理是否要提前
        if (voice_ProcessChannelKeyAccess(nPhoneKeyCode))
        {
            return TRUE;
        }

        // 摘挂机操作发送ActionURL
        if (nPhoneKeyCode == PHONE_KEY_HANDSET_ON_HOOK)
        {
#if IF_FEATURE_EVENT_MODULE
            event_SendEvent(EVENT_ONHOOK);
#else
            actURL_SendActionURL(ACTION_URL_ONHOOK);
#endif
        }
        else if (nPhoneKeyCode == PHONE_KEY_HANDSET_OFF_HOOK)
        {
#if IF_FEATURE_EVENT_MODULE
            event_SendEvent(EVENT_OFFHOOK);
#else
            actURL_SendActionURL(ACTION_URL_OFFHOOK);
#endif
        }
        else if (nPhoneKeyCode == PHONE_KEY_HANDFREE)
        {
#if IF_FEATURE_EVENT_MODULE
            event_SendEvent(EVENT_HANDFREE);
#else
            actURL_SendActionURL(ACTION_URL_HANDFREE);
#endif
        }
        else if (nPhoneKeyCode == PHONE_KEY_HEADSET_CTRL)
        {
#if IF_FEATURE_EVENT_MODULE
            event_SendEvent(EVENT_HEADSET);
#else
            actURL_SendActionURL(ACTION_URL_HEADSET);
#endif
        }

        //通话按键处理
        if (OnGlobalKeyPress(nPhoneKeyCode))
        {
            return true;
        }

    }

    return false;
}

bool PreMouseHookBegin(QObject * pObject, QMouseEvent * pEvent)
{
    if (commonUnits_IsSystemMaintenanceStatus())
    {
        return true;
    }

    if (NULL == pEvent)
    {
        return false;
    }

    QEvent::Type evtType = pEvent->type();

    // 点亮屏幕
    if (QEvent::MouseButtonPress == evtType)
    {
        //鼠标按下点亮屏幕
        backlight_Reset(true);
        // 打印信息
        GLOBALINPUTHOOK_INFO("Global MouseButtonPress, x is %d, y is %d. ", pEvent->globalX(),
                             pEvent->globalY());
        const QMetaObject * pmetaboj = pObject->metaObject();
        if (pmetaboj != NULL)
        {
            // 打印信息
            GLOBALINPUTHOOK_DEBUG("objectName is %s, classname is %s.", pObject->objectName().toUtf8().data(),
                                  pmetaboj->className());
        }
        else
        {
            // 打印信息
            GLOBALINPUTHOOK_DEBUG("objectName is %s", pObject->objectName().toUtf8().data());
        }
    }
    else if (QEvent::MouseButtonRelease == evtType)
    {
        // 打印信息
        GLOBALINPUTHOOK_INFO("Global MouseButtonRelease, x is %d, y is %d.", pEvent->globalX(),
                             pEvent->globalY());
        const QMetaObject * pmetaboj = pObject->metaObject();
        if (pmetaboj != NULL)
        {
            // 打印信息
            GLOBALINPUTHOOK_DEBUG("objectName is %s, classname is %s.", pObject->objectName().toUtf8().data(),
                                  pmetaboj->className());
        }
        else
        {
            // 打印信息
            GLOBALINPUTHOOK_DEBUG("objectName is %s", pObject->objectName().toUtf8().data());
        }
    }

    return false;
}

bool PreMouseHookEnd(QObject * pObject, QMouseEvent * pEvent)
{
    return false;
}

void qT48Application::Init()
{
    MAINWND_INFO("qApp Init Begin");

    // 设置适配函数
    SetAdapterBox();

    devicelib_Init();
    devicelib_InitLcd();
    devicelib_startWatchDog();

    TaskAction_InitDLConf();

    //如果没有特殊需求，不用在各个界面中分别设置字体
    InitAppFont();

    //初始化配置加载模块
    configParser_Init();

    res_init();

#ifdef IF_FEATURE_DIAGNOSE
    notify_Init();
#endif // IF_FEATURE_DIAGNOSE

    //注册Style图片
    InitStyleImages();

    InitDefaultIcon();

    setStyle(new qT4XStyle());

    //把yl::string注册到Qt MeataType中，使Qt可以通过qmetatype机制动态创建yl::string
    qRegisterMetaType<yl::string>("yl::string");

    // 自定义主题
    CustomTheme_Init();

    // 加载主题资源
    THEME_LOAD(res_getPathByName(RESOURCE_PATH_THREME).c_str());
    // 加载图片完成设置环境变量
    comminUnits_WriteInitInfo("InitTime_ImageLoadSucceed = 1");

    // 防止多线程构造xTimer出错http://10.2.1.199/Bug.php?BugID=55974
    etlGetTickCount();

    // 向msgkit注册服务器,才能接收到其他进程的注册消息
    mkservice_register(mkGetHandle(), MKSRV_ID_DSK);

    // 各模块的初始化
    InitModule();

    InitUIManager();

    // 输入法模块后于UI初始化，确保全键盘父窗口
    InputMethod_Init();

    // 初始EdkUI模块
    EDKUI_Init();

    // callManager初始化操作
    callmanager_Init();
    DM_SetCallManagerInfo();
    talkui_SetCallManagerInfo();

    // 显示Idle界面
    // 获得IdleScreen的窗口指针, Idle为顶层窗口
    CBaseDialog * pDlg = UIManager_CreatePrivateDlg(DLG_IdleScreen);

    if (pDlg != NULL)
    {
        // 设置IdleScreen的窗口数据
        pDlg->SetData();
    }

    dsskeyUI_Init();

    // 显示IdleScreen窗口
    UIManager_UpdateWnd(pDlg);

	//初始化拓展台
	exp_InitExtFollowPhoneType();

    hotDesking_Init();

    //如果mac地址等于00-15-65-00-00-00，启动测试模式
    if (commonUnit_IsMatchTestMode())
    {
        // 进入测试模式
        AdapterBox_EnterTestMode(TST_NORMAL);
    }
    //是否自动启动zero sp touch, bEnable
//  else if (configParser_GetConfigInt(kszRedirectEnable) == 1
//      || configParser_GetConfigInt(kszForbidZeroSwitch) == 0)
    else if (configParser_GetConfigInt(kszForbidZeroSwitch) == 0
             && !(1 == configParser_GetConfigInt(kszRedirectEnable)
                  && 1 == configParser_GetConfigInt(kszRPSPriority)))
    {
        zero_Enter(true);
    }
    else
    {
        if (!hotDesking_EnterLoginWizard())
        {
            // 模拟一次进入拨号界面的过程
            // http://10.3.5.199/Bug.php?BugID=70552
            g_bAfterInit = false;
            talklogic_EnterPredialUI(PHONE_KEY_0);
            talklogic_ExitFocusSession();
        }

        zero_EnterRPSWaitting();
    }

#if defined(PHONE_APP_PROFILER)
    m_prof.RegisterFlushMsg();
#endif

#ifdef IF_FEATURE_TOUCHCHECK
    TestMode_TestBegin();
#endif

    MAINWND_INFO("qApp Init End");
    comminUnits_WriteInitInfo("InitTime_ModulesInitSucceed = 1");

#ifdef WIN32
    // win32 启动过程中弹窗可能造成没有刷新，因此启动完成后还需要刷新
    QTimer::singleShot(1000, this, SLOT(OnDelayInit()));
#endif
}

void qT48Application::UnInit()
{
#if defined(PHONE_APP_PROFILER)
    m_prof.UnRegisterFlushMsg();
#endif
}

void qT48Application::InitAppFont()
{
    QFont font;

    font.setPointSize(18);
    font.setWeight(75);
    font.setBold(true);
    setFont(font);
}

void qT48Application::InitDefaultIcon()
{
    //注册Directory用到的默认图标
    //RegisterDirectoryDefaultIcons();

    //注册CommonUnits用到的默认图标
    //RegisterCommonUnitsDefaultIcons();

    //注册Setting用到的默认图标
    //RegisterSettingDefaultIcons();

    //注册CallLog用到的默认图标
    //RegisterCallLogDefaultIcons();

    //注册XmlBrowser用到的默认图标
    //RegisterXmlBrowserDefaultIcons();

    //注册CommonUnits用到的默认图标
    //RegisterCommonUnitsDefaultIcons();

    //注册Menu用到的默认图标
    //RegisterMenuDefaultIcons();

    //注册application用到的默认图标
    //RegisterApplicationDefaultIcons();
}

void qT48Application::InitStyleImages()
{
#ifdef _T4X
    //combox箭头
    myPxCache::RegisterImage(qT4XStyle::PxComboBoxArrow,            PIC_COMBOX_NORMAL);
    myPxCache::RegisterImage(qT4XStyle::PxComboBoxArrowDown,        PIC_COMBOX_CLICK_DOWN);
    myPxCache::RegisterImage(qT4XStyle::PxComboBoxArrowUp,          PIC_COMBOX_CLICK_UP);
    myPxCache::RegisterImage(qT4XStyle::PxComboBoxArrowFocus,       PIC_COMBOX_NORMAL_FOCUS);
    myPxCache::RegisterImage(qT4XStyle::PxComboBoxArrowDownFocus,   PIC_COMBOX_CLICK_DOWN_FOCUS);
    myPxCache::RegisterImage(qT4XStyle::PxComboBoxArrowUpFocus,     PIC_COMBOX_CLICK_UP_FOCUS);
    //Option菜单背景
    myPxCache::RegisterImage(qT4XStyle::PxPopMenuBackground,        PIC_POPMENU_BKG);
    //Option选中条
    myPxCache::RegisterImage(qT4XStyle::PxPoPMenuSeletcedItem,      PIC_POPMEU_SEL_ITEM);
    //RadioButton On
    myPxCache::RegisterImage(qT4XStyle::PxRadioButtonOn,            PIC_RADIO_ON);
    //RadioButton Off
    myPxCache::RegisterImage(qT4XStyle::PxRadioButtonOff,           PIC_RADIO_OFF);
    //RadioButton Disable
    myPxCache::RegisterImage(qT4XStyle::PxRadioButtonDisable,       PIC_RADIO_DISABLE);
    //Icon View Cover
    myPxCache::RegisterImage(qT4XStyle::PxIconViewCover,            PIC_MENU_COVER);
    //scrollbar vertical background
    myPxCache::RegisterImage(qT4XStyle::PxScrollBarVertBackGound,   PIC_SCROLL_SCROLLBAR);
    //scrollbar vertical slider
    myPxCache::RegisterImage(qT4XStyle::PxScrollBarVertSlider,      PIC_SCROLL_SCROLL);
#else
    //combox箭头
    myPxCache::RegisterImage(qT3XStyle::PxComboBoxArrow,            PIC_ARROWS_COMBOX);
    //Option菜单背景
    myPxCache::RegisterImage(qT3XStyle::PxPopMenuBackground,        PIC_POPMENU_BKG);
    //Option选中条
    myPxCache::RegisterImage(qT3XStyle::PxPoPMenuSeletcedItem,  PIC_POPMEU_SEL_ITEM);
    //RadioButton On
    myPxCache::RegisterImage(qT3XStyle::PxRadioButtonOn,                PIC_RADIO_ON);
    //RadioButton Off
    myPxCache::RegisterImage(qT3XStyle::PxRadioButtonOff,               PIC_RADIO_OFF);
    //Icon View Cover
    myPxCache::RegisterImage(qT3XStyle::PxIconViewCover,                PIC_MENU_COVER);
    //scrolbar up arrow
    myPxCache::RegisterImage(qT3XStyle::PxScrollBarUpArrow,         PIC_ARROWS_SCROLLUP);
    //scrollbar down arrow
    myPxCache::RegisterImage(qT3XStyle::PxScrollBarDownArrow,       PIC_ARROWS_SCROLLDOWN);
    //scrollbar vertical background
    myPxCache::RegisterImage(qT3XStyle::PxScrollBarVertBackGound, PIC_SCROLL_SCROLLBAR);
    //scrollbar vertical slider
    myPxCache::RegisterImage(qT3XStyle::PxScrollBarVertSlider,          PIC_SCROLL_SCROLL);
#endif
}

void qT48Application::InitUIManager()
{
    // 初始化UIManager
    UIManager_Init();

    // 创建主窗口
    UIManager_SetMainWnd(&_MainWND);

    // 初始化MainWnd的代理
    MainWndHelp_Init();

//  // 主窗口显示
//  UIManager_ShowMainWnd();
}

void qT48Application::InitModule()
{
    MAINWND_INFO("qApp Init Module");

    FullKeyboard_Init();

    BaseUI_Init();

    // 注册IdleScreen模块的窗口
    idleScreenUI_RegisterDlgClass();

    TaskAction_Init(3, 20);

#if IF_SUPPORT_WIFI
    // WIFI模块初始化
    WIFI_Init();
#endif // IF_SUPPORT_WIFI

    Dir_Init(CONFIG_DATA_PATH, TEMP_PATH);

    backlight_Init();

    //信号灯初始化
    ledlamp_Init();

    //语言翻译初始化
    Ime_Lang_Init();

    // 声音通道模块初始化
    voice_Init();

    // 初始化全局按键过滤
    inputHook_EnableHook();

    // 注册TestMode的模块
    testmode_Init();

    dsskey_setUpdateFunCallback(dsskeyUI_RefreshCallback);

    idleScreen_Init(false);

    WarningCenter_Init();

    callmanager_RegisterDlgClass();

    // 屏保初始化
    ScreenSavers_Init();

    //初始化TalkUI模块
    talkui_Init();

    // 注册基础界面模块
    BaseUI_RegisterDlgClass();

    //注册SettingUI模块窗口
    SettingUI_RegisterDlgClass();

    //注册SettingUI模块权限判断函数
    SettingUI_RegisterOpenUICheckFunc();

    //注册MessageUI
    MessageUI_RegisterDlgClass();

    // 初始化Ldap.
    LDAP_Init();

    // 初始化联系人
    DM_InitData();

    // 实现每个界面打开逻辑的注册
    DM_RegisterOpenUICheckFunc();

    commonUnits_Init();


#if IF_SUPPORT_BLUETOOTH
    // 蓝牙模块初始化
    Bluetooth_Init();
#endif // IF_SUPPORT_BLUETOOTH

    // Setting模块注册消息.
    Setting_RegisterMessage();
    Setting_Init();

    Setting_RegisterRebootTipCallBack(qApp_RebootTip);

    //初始化CallPark
    callPark_Init();

    csta_Init();

    // 超级搜索预初始化.
    SuperSearch_Init();

    // 自动设置初始化
    autoset_Init();

    // 注册Dsskey的模块
    dsskey_Init();

    //初始化AccountObject
    acc_Init();

    // Action Ur模块初始化需要在账号初始之后
    actURL_ActionURLInit();

    // actionURI功能初始化
    actURI_Init();

#if IF_FEATURE_EVENT_MODULE
    event_Init();
#endif

    fwddnd_Init();

    feature_Init();

    //需在账号模块之后注册消息
    CallCenter_Init();


    // SCA初始化
    bsSCA_Init();

    // 初始化Xmlbrowser模块
    xmlbrowser_Init();

    // 初始化CallCompletion
    CallCompletion_Init();

    XSI_BaseHelp_Init();

    blf_Init();

    // 初始化XMLBrowser
    XmlBrowserUI_Init();

    if (Edk_IsEnable())
    {
        Edk_Init();
    }

    // presence初始化
#if FEATURE_BROADTOUCH_UC
    Presence_Init();
#endif

    // 初始化zero
    ZeroSPTouch_RegisterDlgClass();

#ifdef IF_USB_SUPPORT
    modhotplug_Init();
    modRecord_Init();
#endif

#ifdef IF_SUPPORT_USB_AUDIO
    USBHeadset_Init();
#endif

#if IF_FEATURE_GENBAND_E911
    e911_Init();
#endif

    // 显示Idle界面后初始化电源灯状态，勿改变顺序
    powerled_Init();
#if IF_FEATURE_METASWITCH
    MTSW_Init();
#endif
}

void qT48Application::OnDelayInit()
{
    extern bool g_bAfterInit;
    g_bAfterInit = true;

#if 1
    talklogic_ExitFocusSession();
    UIManager_ShowMainWnd();
#endif
}

// 设置适配函数
void qT48Application::SetAdapterBox()
{
    AdapterBox_SetReturnToIdle(UIManager_ReturnToIdle);
    AdapterBox_SetUpdateTopWnd(UIManager_UpdateTopWnd);
    AdapterBox_SetSendKey(inputHook_SendKey);
    AdapterBox_SetTalkUIClearSessionData(talkui_ClearSessionData);
    AdapterBox_SetTalkUISessionExit(talkui_SessionExit);
    AdapterBox_SetTalkUIGetTalkTimeBySessionId(talkui_GetTalkTimeBySessionId);
    AdapterBox_SetTalkUIGetInputContent(talkui_GetInputContent);
    AdapterBox_SetTalkUIGetInputContentEx(talkui_GetInputContent);
    AdapterBox_SetTalkUIUpdateWnd(talkui_UpdateWnd);
    AdapterBox_SetTalkUIUpdateErrorInfo(talkui_UpdateErrorInfo);
    AdapterBox_SetTalkUIGetInputCallID(talkui_GetInputCallID);

    AdapterBox_SetSettingUIEnterPage(SettingUI_EnterPageByType);
    AdapterBox_SetDirUIEnterLocalMenuPage(DM_EnterLocalMenu);

    AdapterBox_SetEnterConnectVoiceMailPage(Setting_ConnectVoiceMail);
    AdapterBox_SetIsTestMode(testmode_IsTestMode);
    AdapterBox_SetEnterTestMode(testmode_Enter);
    AdapterBox_SetExitTestMode(testmode_Exit);
    AdapterBox_SetXmlBrowserShowMessage(XmlBrowserUI_ShowMessage);
    AdapterBox_SetXmlBrowserDestroyMessage(XmlBrowserUI_DestroyMessage);
    AdapterBox_SetXmlBrowserUpdateWnd(XmlBrowserUI_UpdateWnd);
    AdapterBox_SetXmlBrowserExitXBDlg(XmlBrowserUI_ExitXBDlg);
#if IF_FEATURE_XMLBROWSER_HOTELING
    AdapterBox_SetXmlBrowserShowTips(XmlBrowserUI_ShowTips);
#endif
    AdapterBox_SetXmlBrowserCtrlLight(dsskeyUI_DsskeyLightCallback);
    AdapterBox_SetXmlBrowserUpdate(idleScreenUI_UpdateXMLBrowser);

    AdapterBox_SetShowSCAMenu(talkui_EnterSCAMenu);
    AdapterBox_SetIsExistSCAMenu(talkui_IsExistSCAMenu);
    AdapterBox_SetExitSCAMenu(talkui_ExitSCAMenu);
#if IF_FEATURE_SHOW_BLF_CALLID
    AdapterBox_SetShowBLFMenu(talkui_EnterBLFMenu);
    AdapterBox_SetIsExistBLFMenu(talkui_IsExistBLFMenu);
    AdapterBox_SetExitBLFMenu(talkui_ExitBLFMenu);
#endif
    //http://10.2.1.199/Bug.php?BugID=96788
    AdapterBox_SetIsInTalk(UIManager_IsInTalk);

#if IF_FEATURE_EDK
    AdapterBox_SetShowEdkPrompt(MainWndHelp_EDK_Show_MessageBox);
    AdapterBox_SetUpdateSoftKey(MainWnd_UpdateSoftkey);
#endif

    AdapterBox_SetReloadIME(qReloadIME);
    AdapterBox_SetReboot(Setting_Reboot);
    AdapterBox_SetResetFactory(qApp_ResetFactory);
    AdapterBox_SetIsInIdle(UIManager_IsInIdle);

    AdapterBox_SetEnterMenu(EdkUI_DoUIAction);
    AdapterBox_SetExecuteSoftkey(EdkUI_DoSoftkeyAction);
    AdapterBox_SetZEROOpenDlg(SettingUI_EnterSpZeroTouch);

    AdapterBox_SetShowVolumeBar(MainWnd_ShowVolumeBar);

    AdapterBox_SetExitScreenSaver(ScreenSavers_ExitScreensaver);

    AdapterBox_SetImageScale(Image_Scaled);

    AdapterBox_SetShowDsskeyLockPassword(idleScreenUI_ShowDsskeyLockPassword);

    AdapterBox_SetDsskeyLongPress(dsskeyUI_OnDsskeyLongPress);
}
