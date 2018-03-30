#include "mainstation.h"
#include "msgkithandler.h"

// baseLibs
#include <ETLLib.hpp>
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "taskaction/modtaskaction.h"
#include "resource/modresource.h"
#include "dsklog/log.h"
#include "dsklog/dsklog.h"
#include "path_define.h"
#include "interfacedefine.h"
#include "language/input/include/modInput.h"
#include "languagehelper/modlangtranslate.h"
#include "phonedev/modphonedev.h"
#include "commonunits/modcommonunits.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "threadtask/include/modthreadtask.h"
#include "extended/warningcenter/include/modwarningcenter.h"
//include
#include "keymap.h"

//xWindow
#include "xwindow/xSoftkeyBar.h"


#if WINDOWS_SYSTEM
#define SCREEN_CONFIG_PATH  "./screen.ini"
#define KEYMAP_CONFIG_PATH  "./keymap.ini"
#else
#define SCREEN_CONFIG_PATH  "screen.ini"
#define KEYMAP_CONFIG_PATH  "keymap.xml"
#endif

#if IS_SCREEN_USE_HIFB
#include "hifbscreen/include/hifbscreen.h"
#else
#include "fbscreen/include/fbscreen.h"
#endif

#include "adapterbox/include/modadapterbox.h"
#include "adapterbox/include/modsetadapterbox.h"
#include "autoset/include/modautoset.h"
#include "blf/include/modblf.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "broadsoft/callpark/include/modcallpark.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "contacts/directory/include/moddirinterface.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "contacts/supersearch/include/modsupersearch.h"
#include "dsskey/include/moddsskey.h"
#include "dsskeyui/moddsskeyui.h"
#include "edk/include/modedk.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "extended/csta/include/modcsta.h"
#include "extended/event/include/modevent.h"
#include "extended/hotdesking/include/modhotdesking.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "feature/include/modfeature.h"
#include "hotplug/modhotplug.h"
#include "keyguard/include/modkeyguard.h"
#include "lamp/backlight/include/modbacklight.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "record/include/modrecord.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "talk/callcompletion/include/modcallcompletion.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "talk/linedevice/include/modlinedevice.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "voice/include/modvoice.h"
#include "idlescreen/include/modidlescreen.h"
#include "settinguilogic/include/modsetuilogic.h"
#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/modwifi.h"
#endif

#if FEATURE_BROADTOUCH_UC
#include "presence/include/modpresence.h"
#endif

#include "exp/include/modexp.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

// bluetooth
#include "wireless/bluetoothNew/include/modbluetooth.h"

#include "globalinputhook/modinputhook.h"
#include "uimanager/moduimanager.h"

#include "baseui/include/baseuicommon.h"
#include "baseui/include/modbaseui.h"
#include "baseui/include/inputmethod.h"
#include "baseui/include/t2xpicpath.h"
#include "directoryui/include/moddirectoryui.h"
#include "edkui/include/modedkui.h"
#include "idlescreenui/modidleui.h"
#include "settingui/include/modsettingui.h"
#include "talkuilogic/modtalkuilogic.h"
#include "talkui/include/modtalkui.h"
#include "dsskeyui/moddsskeyui.h"
#include "zerotouchui/include/modzerotouchui.h"
#include "testmode/include/modtestmode.h"
#include "xmlbrowserui/include/modxmlbrowserui.h"
#include "idleuilogic/include/modidleul.h"
#include "screensavers/include/modscreensavers.h"   //初始屏保
#include "uimanager/modvolumebar.h"

#if IS_COLOR
#include "imagecache/include/modimagecache.h"
#endif

#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#endif

#if IF_FEATURE_GENBAND_E911
#include "e911address/include/mode911address.h"
#endif

bool g_bAfterInit = false;
int g_iKeyDes = -1;

namespace
{
#define RPS_WAIT_NETWORK_TIME       20
#define RPS_WAIT_TIME_1S            1000

#define DEFAULT_FONT_SIZE_CP920     9
#define DEFAULT_FONT_SIZE_T27       10
#define DEFAULT_FONT_SIZE_T46       18
#define DEFAULT_FONT_SIZE_T52       14
#define DEFAULT_FONT_SIZE_T54       18
#define DEFAULT_FONT_SIZE_BLACK     12
#define DEFAULT_FONT_SIZE_COLOR     20
}

CMainStation::CMainStation()
    : xWorkStation(VP_PHONE_THREAD_NAME)
#if IF_CUSTOM_SCREEN_DEVICE && defined(_WIN32)
    , m_pScreenDevice(NULL)
#endif
    , m_pDev(NULL)
{
    setDrawableRootPath(getRealPathByPhoneType(THEME_DEFAULT_PATH, "", RESPATHTYPE_PICS));
    setLayoutRootPath(getRealPathByPhoneType(PHONE_LAYOUT_PATH, "", RESPATHTYPE_LAYOUT));
}

CMainStation::~CMainStation()
{
}

CMainStation & CMainStation::getInstance()
{
    static CMainStation * s_pMainStation = NULL;

    if (NULL == s_pMainStation)
    {
        s_pMainStation = new CMainStation;
    }

    return (*s_pMainStation);
}

bool CMainStation::IsPowerSavingAllowThroughKey(int nKey)
{
    if ((nKey >= PHONE_KEY_0 && nKey <= PHONE_KEY_9)
            || PHONE_KEY_STAR == nKey
            || PHONE_KEY_POUND == nKey
            || PHONE_KEY_HANDFREE == nKey
            || PHONE_KEY_HANDSET == nKey
            || PHONE_KEY_HANDSET_ON_HOOK == nKey
            || PHONE_KEY_HANDSET_OFF_HOOK == nKey
            || PHONE_KEY_HEADSET_CTRL == nKey)
    {
        return true;
    }

    return false;
}

bool CMainStation::PreKeyHookBegin(int nPhoneKeyCode, bool bPress)
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

    //音量条处理
    if (_UIManager.ProcessVolumeBarKey(nPhoneKeyCode, bPress))
    {
        // 逻辑层对通道键处理有顺序要求
        // http://10.2.1.199/Bug.php?BugID=122635
        return true;
    }

    if (PHONE_KEY_MUTE == nPhoneKeyCode && bPress && testmode_IsTestMode())
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

    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

    // 测试模式让测试流程自己处理声音, CP920 xmlbrowser界面也需要响按键音
    if ((!XmlBrowserUI_IsPopuping() || PT_CP920 == ePhoneType)
            && bPress && !testmode_IsTestMode())
    {
        //通道模块处理
        voice_OnKeyPress(nPhoneKeyCode);
        // 播放按键音
        PlayKeyTone(nPhoneKeyCode);
    }

    // keypadlock filter
    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();
    if (NULL == pBaseDlg)
    {
        return false;
    }

    if (keyGuard_IsLock() && bPress
            && !pBaseDlg->AllowKeyPadEventOnKeyLock(nPhoneKeyCode, bPress))
    {
        SettingUI_EnterUnlockCheck();
        return true;
    }

    // 当长按+号才允许进入提示模式
    if (Edk_IsEnable() && 1 == configParser_GetConfigInt(kszEdkTipMode)
            && PHONE_KEY_VOLUME_INCREASE == nPhoneKeyCode)
    {
        if (bPress)
        {
            EdkUI_StartKeyTimer();
        }
        else
        {
            EdkUI_StopKeyTimer();
        }
    }

    return false;
}

bool CMainStation::PreKeyHookEnd(int nPhoneKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }
    // ----声道按键处理是否要提前
    if (voice_ProcessChannelKeyAccess(nPhoneKeyCode))
    {
        return true;
    }

    // 摘挂机操作发送ActionURL
    if (PHONE_KEY_HANDSET_ON_HOOK == nPhoneKeyCode)
    {
#if IF_FEATURE_EVENT_MODULE
        event_SendEvent(EVENT_ONHOOK);
#else
        actURL_SendActionURL(ACTION_URL_ONHOOK);
#endif
    }
    else if (PHONE_KEY_HANDSET_OFF_HOOK == nPhoneKeyCode)
    {
#if IF_FEATURE_EVENT_MODULE
        event_SendEvent(EVENT_OFFHOOK);
#else
        actURL_SendActionURL(ACTION_URL_OFFHOOK);
#endif
    }
    else if (PHONE_KEY_HEADSET_CTRL == nPhoneKeyCode)
    {
#if IF_FEATURE_EVENT_MODULE
        event_SendEvent(EVENT_HEADSET);
#else
        actURL_SendActionURL(ACTION_URL_HEADSET);
#endif
    }
    else if (PHONE_KEY_HANDFREE == nPhoneKeyCode)
    {
#if IF_FEATURE_EVENT_MODULE
#ifndef _WIN32
#warning
#endif
//      event_SendEvent(DEVICELIB_IS_CP(devicelib_GetPhoneType()) ? EVENT_OFFHOOK : EVENT_HANDFREE);
#else
#ifndef _WIN32
#warning
#endif
//      actURL_SendActionURL(DEVICELIB_IS_CP(devicelib_GetPhoneType()) ? ACTION_URL_OFFHOOK : ACTION_URL_HANDFREE);
#endif
    }

    //通话按键处理
    if (OnGlobalKeyPress(nPhoneKeyCode))
    {
        return true;
    }
    return false;
}

bool CMainStation::OnGlobalKeyPress(int nKey)
{
    if (UIManager_IsInIdle() || UIManager_IsInTalk())
    {
        return false;
    }

    switch (nKey)
    {
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            UIManager_ReturnToIdle();

            return true;
        }
        break;
    case PHONE_KEY_HANDFREE:
        {
            talklogic_EnterDialUI("", AUTO_SELECT_LINE_ID, TCT_NORMAL);

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

void CMainStation::PlayKeyTone(int nKey)
{
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (talklogic_SessionExist()        // 存在通话时，不播放按键音
            || dsskey_IsDsskey(nKey)    // Dsskey不播放按键音
            || PHONE_KEY_VOLUME_DECREASE == nKey
            || PHONE_KEY_VOLUME_INCREASE == nKey // 调节音量键不播放按键音
            || (voice_IsChannelKey(nKey) && PHONE_KEY_HANDSET_ON_HOOK != nKey) // 通道键不播放按键音
            || (PT_CP920 != ePhoneType
                && PHONE_KEY_HANDSET_ON_HOOK == nKey))// CP920 挂机键需要响按键音
    {
        return;
    }

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    if (modRecord_IsIdleRecording(RECORD_AUDIO))
    {
        return;
    }
#endif

    if (UIManager_IsInIdle()) // 在IDLE的时,需要耳麦开启才播放funtionkey的按键音
    {
        if ((voice_GetHeadsetStatus() || PT_CP920 == ePhoneType) && !IsDigitKey(nKey))
        {
            voice_PlayDTMFInMenu(PHONE_KEY_A);
        }
    }
    else
    {
        // 其他界面需要播放数字和funtionkey的按键音
        if (PT_CP920 == ePhoneType)
        {
#warning PHONE_KEY_TOUCH
            //voice_PlayDTMFInMenu(PHONE_KEY_TOUCH);
        }
        else
        {
            voice_PlayDTMFInMenu(IsDigitKey(nKey) ? nKey : PHONE_KEY_A);
        }
    }
}

BOOL CMainStation::AfterThreadLaunched()
{
#ifndef _WIN32
    int nMKFD = g_MsgKitHandler.GetMKFD();

    if (INVALID_SOCKET != nMKFD)
    {
        ConnectMsgReadHandler(nMKFD, MSG_METHOD(this, CMainStation::OnHandlerMsgKit));
    }
#endif

    xWorkStation::SetFontPath(getRealPathByPhoneType(FONTS_FILE_PATH, "", RESPATHTYPE_FONT));
    xWorkStation::OpenStation("phone0", 4 * 1024);
    xWorkStation::setStationViewBGColor(xColor(255, 255, 255));

    yl::string strScreenConfigPath = getRealPathByPhoneType(PHONE_SCREEN_PATH, SCREEN_CONFIG_PATH,
                                     RESPATHTYPE_SCREEN);

#if IF_SCREEN_PLUGIN_DEBUG || !IF_SUPPORT_SCREEN_PLUGIN
    xWorkStation::SetFunCreateScreen(&::CreateScreen);
    xWorkStation::SetFunReleaseScreen(&::ReleaseScreen);
#else
    m_objScreenLoader.Init(strScreenConfigPath.c_str());

    xWorkStation::SetFunCreateScreen(m_objScreenLoader.GetFunCreateScreen());
    xWorkStation::SetFunReleaseScreen(m_objScreenLoader.GetFunReleaseScreen());
#endif

#ifdef _WIN32
    xScreen * pScreen = CreateCustomScreen(GetScreenDevice());

    xWorkStation::SetScreen(pScreen);
#else
    xWorkStation::CreateStationScreen();
#endif

    xWorkStation::InitScreen(strScreenConfigPath.c_str());

#if LINUX_SYSTEM
    if (NULL != m_pDev)
    {
        ConnectMsgReadHandler(m_pDev->GetKeypadHandle(), MSG_METHOD(this, CMainStation::OnReadKeyEvent));
    }
#endif

    InitModules();

    //初始化拓展台
    exp_InitExtFollowPhoneType();

    UIManager_Init();

    // 显示Idle界面
    UIManager_CreatePrivateDlg(DLG_IdleScreen);

    // mac地址后6位等于00-00-00，启动测试模式
    if (commonUnit_IsMatchTestMode())
    {
        // 进入测试模式
        AdapterBox_EnterTestMode(TST_NORMAL);
    }
    //是否自动启动zero sp touch, bEnable
    else
    {
#ifdef IF_SETTING_WIZARD
        // 设置向导
        SettingUI_EnterSetUpWizard();
#endif
        if (0 == configParser_GetConfigInt(kszForbidZeroSwitch)
                && !(1 == configParser_GetConfigInt(kszRedirectEnable)
                     && 1 == configParser_GetConfigInt(kszRPSPriority)))
        {
            SettingUILogic_EnterZero(true);
        }

        // 是否会自动进入rps
        if (1 == configParser_GetConfigInt(kszRedirectEnable) && netIsCableConnected())
        {
            yl::string strAutopPath = configParser_GetConfigString(kszAutoProvisionUrl);
            if (strAutopPath.empty() || 1 == configParser_GetConfigInt(kszRPSPriority))
            {
                int nWaitCount = 0;
                bool bEnable = netIsNetworkReady();
                NETWORK_STATUS netStatus = idleScreen_GetNetworkStatus(true);
                // 等待网络可用
                while (NS_OK != netStatus && RPS_WAIT_NETWORK_TIME > nWaitCount)
                {
                    netStatus = idleScreen_GetNetworkStatus(true);
                    bEnable = netIsNetworkReady();
                    etlSleep(RPS_WAIT_TIME_1S);
                    nWaitCount++;
                }
            }
        }
    }

#if IF_FEATURE_METASWITCH
    MTSW_Init();
#endif

    comminUnits_WriteInitInfo("InitTime_ModulesInitSucceed = 1");

    return TRUE;
}

#ifdef _WIN32
void CMainStation::Init(LPWSTR lpCmdLine, int nCmdShow)
{
    if (!g_MsgKitHandler.InitMsgKit())
    {
        APP_ERR("InitMsgKit fail!");
    }

    //初始化机型变量
    g_iPhoneType = devicelib_GetPhoneType();

    // 防止多线程构造xTimer出错http://10.2.1.199/Bug.php?BugID=55974
    etlGetTickCount();

    // 设置适配函数
    SetAdapterBox();

    // 必要资源的初始化（Zero和Screen都需要）
    if (PT_T40 == g_iPhoneType || DEVICELIB_IS_T42(devicelib_GetPhoneType())
            || DEVICELIB_IS_T27(devicelib_GetPhoneType()))
    {
        devicelib_Init();
    }
    else
    {
        devicelib_Init();
    }

    // http://192.168.1.99/Bug.php?BugID=37645
    TaskAction_InitDLConf();

    devicelib_startWatchDog();

    configParser_Init();

    //初始化配置加载模块
    m_pDev = GetPhoneDev(getRealPathByPhoneType(SYSTEM_KEYMAP_PATH, KEYMAP_CONFIG_PATH, RESPATHTYPE_KEYMAP));

    if (NULL != m_pDev)
    {
        g_iKeyDes = m_pDev->GetKeypadHandle();
    }

    if (PT_CP920 == g_iPhoneType)
    {
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_CP920);
    }
    else if (PT_T27 == g_iPhoneType
             || PT_T27S == g_iPhoneType)
    {
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_T27);
    }
    else if (PT_T52 == g_iPhoneType)
    {
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_T52);
    }
    else
    {
#if IS_COLOR
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_T46);
#else
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_BLACK);
#endif
    }
}

#if IF_CUSTOM_SCREEN_DEVICE
CWin32ScreenDevice * CMainStation::GetScreenDevice()
{
    if (NULL == m_pScreenDevice)
    {
        m_pScreenDevice = new CWin32ScreenDevice;
    }

    return m_pScreenDevice;
}
#endif
#else
void CMainStation::Init(int argc, char * argv[])
{
    ParseRunCommand(argc, argv);

    if (!g_MsgKitHandler.InitMsgKit())
    {
        APP_ERR("InitMsgKit fail!");
    }

    //初始化机型变量
    g_iPhoneType = devicelib_GetPhoneType();

    // 防止多线程构造xTimer出错http://10.2.1.199/Bug.php?BugID=55974
    etlGetTickCount();

    // 设置适配函数
    SetAdapterBox();

    // 必要资源的初始化（Zero和Screen都需要）
    if (PT_T40 == g_iPhoneType || DEVICELIB_IS_T42(devicelib_GetPhoneType())
            || DEVICELIB_IS_T27(devicelib_GetPhoneType()))
    {
#warning
//      devicelib_Init(true);
        devicelib_Init();
    }
    else
    {
#warning
//      devicelib_Init(false);
        devicelib_Init();
    }

#if !IS_COLOR
    devicelib_InitLcd();
#endif

    TaskAction_InitDLConf();

    devicelib_startWatchDog();

    configParser_Init();
    TaskAction_InitDLConf();

    //初始化配置加载模块
    m_pDev = GetPhoneDev(getRealPathByPhoneType(SYSTEM_KEYMAP_PATH, KEYMAP_CONFIG_PATH,
                         RESPATHTYPE_KEYMAP));

    if (NULL != m_pDev)
    {
        g_iKeyDes = m_pDev->GetKeypadHandle();
    }

    if (PT_CP920 == g_iPhoneType)
    {
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_CP920);
    }
    else if (PT_T27 == g_iPhoneType
             || PT_T27S == g_iPhoneType)
    {
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_T27);
    }
    else if (PT_T52 == g_iPhoneType)
    {
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_T52);
    }
    else
    {
#if IS_COLOR
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_T46);
#else
        xFont::SetDefaultSize(DEFAULT_FONT_SIZE_BLACK);
#endif
    }
}
#endif

void CMainStation::ParseRunCommand(int argc, char * argv[])
{
    dsklog_init(argc, argv, NULL);
    if (!dsklog_is_manual_init(argc, argv))
    {
        dsklog_setlog_level(7);
    }
}

void CMainStation::BeforeThreadShutdown()
{
    //DM_UnInitData();
    inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_PREHOOKBEGIN);
    inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_PREHOOKEND);

    xWorkStation::CloseStation();

    if (NULL != m_pDev)
    {
        DisconnectMsgReadHandler(m_pDev->GetKeypadHandle());
    }

    xWorkStation::SetFunCreateScreen(NULL);
    xWorkStation::SetFunReleaseScreen(NULL);
    //getPhoneStation().Stop();
#if !IF_SCREEN_PLUGIN_DEBUG && IF_SUPPORT_SCREEN_PLUGIN
    m_objScreenLoader.UnInit();
#endif

#if IF_USED_IMAGE_CACHE_MANAGER
    SetGetImageFunction(NULL);
#endif

    lang_UnRegister(this);
    lang_UnInit();
    //modTranSceiver_UnInit();

    //webd_unInit();
    //modDevManager_UnInit();
    //inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_PREHOOKBEGIN);
    //inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_PREHOOKEND);
}

#ifndef _WIN32
BOOL CMainStation::OnHandlerMsgKit(msgObject &)
{
    g_MsgKitHandler.OnMsgKitFDActive();

    return TRUE;
}
#endif

BOOL CMainStation::OnReadKeyEvent(msgObject &)
{
    if (NULL != m_pDev)
    {
        PhoneEvent xphoneEvent = m_pDev->readEvent();

        if (PET_KEY_EVENT == xphoneEvent.eEventType)
        {
#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
            inputHook_SetRawKey(xphoneEvent.wParam);
#endif
            int iKey = m_pDev->GetKey(xphoneEvent.wParam, xphoneEvent.lParam != 0);
            bool bPress = xphoneEvent.lParam != 0;

            OnSpecialKeyChange(iKey, bPress);

            dispatchKeypadEvent(iKey, bPress);
        }
    }

    return TRUE;
}

void CMainStation::OnSpecialKeyChange(int & nKey, bool & bPress)
{
    if (PHONE_KEY_HANDSET == nKey)
    {
        if (bPress)
        {
            nKey = PHONE_KEY_HANDSET_ON_HOOK;
        }
        else
        {
            nKey = PHONE_KEY_HANDSET_OFF_HOOK;
        }

        bPress = true;
    }
}

void CMainStation::dispatchKeypadEvent(int nKey, bool bPress)
{
#if IF_DEBUG_PAINTE_TIME
    XWIN_TRACE_TIME("OnKeyEvent key=%d press=%d", nKey, (bPress ? 1 : 0));
#endif

    if (!inputHook_ProcessKeyInput(nKey, bPress))
    {
        xWorkStation::dispatchKeypadEvent(nKey, bPress);
    }
}

void CMainStation::InitModules()
{
    TaskAction_Init(3, 20);

    Ime_Lang_Init();

    backlight_Init();
    powerled_Init();
    ledlamp_Init();

    // 初始化资源配置模块
    res_init();

    //语言翻译初始化
    lang_Init();
    lang_Register(this);

#ifdef IF_SUPPORT_WIFI
    WIFI_Init();
#endif

#if IF_FEATURE_EDK
    Edk_Init();
    EdkUI_Init();
#endif

    // 注册Dsskey的模块
    dsskey_Init();

    fwddnd_Init();

    // zero进入前需要初始化三级权限
    feature_Init();

    g_bAfterInit = true;

    // 初始化全局按键过滤
    inputHook_EnableHook(HT_KEYPAD);

    inputHook_RegisterKeyPadHookProc(CMainStation::PreKeyHookBegin, HOOK_MODULE_PREHOOKBEGIN);
    inputHook_RegisterKeyPadHookProc(CMainStation::PreKeyHookEnd, HOOK_MODULE_PREHOOKEND);

    BaseUI_Init();

#if IF_USED_IMAGE_CACHE_MANAGER
    SetGetImageFunction(Image_GetImage);

    Image_Init();
    Image_LoadImage(getRealPathByPhoneType(THEME_DEFAULT_PATH, "", RESPATHTYPE_PICS), IMG_CLASSIFY_THEME);
#endif

#if IS_COLOR
    xBrush brushBG;
    xPixmap bmBG;

    if (GetImageByPath(PIC_DEFAULT_THEME_BG, bmBG))
    {
        brushBG.setPixmap(bmBG);
    }
    else
    {
        brushBG.setColor(profile().m_clrViewDefaultBG);
    }

    setStationBackground(brushBG);
#else
    xBrush brushBG(profile().m_clrViewDefaultBG);

    setStationBackground(brushBG);
#endif

    // 加载图片完成设置环境变量
    comminUnits_WriteInitInfo("InitTime_ImageLoadSucceed = 1");

    talklogic_Init();
    talkuilogic_Init();
    talkui_Init();

    Dir_Init(CONFIG_DATA_PATH, TEMP_PATH);

    voice_Init();

    idleScreen_Init();

    WarningCenter_Init();

#if IS_COLOR || IF_FEATURE_XMLB_SCREENSAVER
    //初始化屏保模块
    ScreenSavers_Init();
#endif

    // 注册TestMode的模块
    testmode_Init();

    dsskeyUI_Init();
    IdleUI_Init();

    // CallCompletion 的初始化
    CallCompletion_Init();

    // autoset
    autoset_Init();

    acc_Init();

    // 初始化ActionURL功能,需在账号之后初始化
    actURL_ActionURLInit();

    // actionURI功能初始化
    actURI_Init();

    csta_Init();

#if IF_FEATURE_CTRIP_CTI
    ctrip_Init();
#endif

#if IF_FEATURE_EVENT_MODULE
    event_Init();
#endif

#ifndef _WIN32
#warning
#endif
//    fwd_dnd_Init();

    commonUnits_Init();

#if IF_SUPPORT_BLUETOOTH
    // 蓝牙模块初始化
    Bluetooth_Init();
#endif // IF_SUPPORT_BLUETOOTH

    // 初始化键盘锁功能
    keyGuard_Init();
    keyGuard_SetCheckUnlockPswFun(SettingUI_EnterUnlockCheck);
    keyGuard_SetEmergencyTipFun(SettingUI_ShowNoteBox);

    // SCA初始化
    bsSCA_Init();


    //注册消息
    CallCenter_Init();

#ifdef IF_FEATURE_HOTDESKING
    // Hotdesking功能初始化
    hotDesking_Init();
    //检查是否进入hotdesking loginwizard
    hotDesking_EnterLoginWizard();
#endif

    // XMLBrowser功能初始化
    xmlbrowser_Init();

    SuperSearch_Init();

    XSI_BaseHelp_Init();

    SettingUILogic_Init();

    SettingUI_Init();
    SettingUI_SetPageDataPath(getRealPathByPhoneType(RESOURCE_PATH, "", RESPATHTYPE_MENU));

    ZeroTouchUI_Init();

    DM_InitData();

    //Call Park 初始化
    callPark_Init();

    LDAP_Init();

    blf_Init();

#if FEATURE_BROADTOUCH_UC
    Presence_Init();
#endif

    //初始化xmlbrowser
    XmlBrowserUI_Init();

#ifdef IF_SUPPORT_LINE_DEVICE
    lineDevice_Init();
#endif

#ifdef IF_FEATURE_PSTN
    APP_INFO("PSTN compiled.");
    pstnadapter_Init(); // 需要在hotplug模块前初始化
#endif

#ifdef IF_USB_SUPPORT
    modhotplug_Init();
    modRecord_Init();
#endif

#ifdef _DEBUGMEM
    MemoryCheckInit();
#endif

#ifdef _TEST_PSTN
    pstntester_Init();
#endif // _TEST_PSTN

    // 延时3s刷新界面，等待账号注册完毕
#ifndef _WIN32
#warning m_iTimerDelayToShow
#endif
    //m_iTimerDelayToShow = 1000;
    //timerSetThreadTimer((UINT)&m_iTimerDelayToShow, m_iTimerDelayToShow);

#if IF_FEATURE_GENBAND_E911
    e911_Init();
#endif

    g_bAfterInit = false;
    PreProcesser();
    g_bAfterInit = true;
}

// 上电第一次各模块处理慢的问题
void CMainStation::PreProcesser()
{
    if (AdapterBox_IsTestMode())
    {
        return;
    }

    // 为防止第一次上电时来电界面显示和响铃声时间间隔较大，先预进入来电界面一次
    int iCallID = rand();
    sip_call_identify_info_t tCallin_Info;
    memset(&tCallin_Info, 0, sizeof(tCallin_Info));

    //http://10.3.5.199/Bug.php?BugID=63307
    tCallin_Info.force_no_forward = 1;

    etl_NotifyAppEx(true, SIP_CALL_INCOMING, 16, iCallID, sizeof(tCallin_Info), &tCallin_Info);

    etlSleep(100);

    // 避免第一次来电挂断产生呼叫记录
    sip_reason_info_t tCallRelease_reason;
    memset(&tCallRelease_reason, 0, sizeof(tCallRelease_reason));
    tCallRelease_reason.code = 1;

    etl_NotifyAppEx(true, SIP_CALL_RELEASED, 16, iCallID, sizeof(tCallRelease_reason), &tCallRelease_reason);
}

void CMainStation::SetAdapterBox()
{
    AdapterBox_SetReturnToIdle(UIManager_ReturnToIdle);
    AdapterBox_SetUpdateTopWnd(UIManager_UpdateTopWnd);
    AdapterBox_SetIsInIdle(UIManager_IsInIdle);
    AdapterBox_SetIsInTalk(UIManager_IsInTalk);
    AdapterBox_SetSendKey(inputHook_SendKey);

    AdapterBox_SetReboot(SettingUI_Reboot);
    AdapterBox_SetResetFactory(SettingUI_Reset);

#ifndef _WIN32
#warning
#endif
    AdapterBox_SetFunUploadConfig(SettingUI_UploadConfig);
    AdapterBox_SetFunUpdateFirmware(SettingUI_UpdateFirware);

    AdapterBox_SetZEROOpenDlg(ZeroTouchUI_EnterPageByType);
    AdapterBox_SetSettingUIEnterPage(SettingUI_EnterPageByType);
    AdapterBox_SetTalkUIClearSessionData(talkuilogic_ClearSessionData);
    AdapterBox_SetTalkUISessionExit(talkuilogic_SessionExit);
    AdapterBox_SetTalkUIGetTalkTimeBySessionId(talkuilogic_GetTalkTimeBySessionId);
    AdapterBox_SetTalkUIGetInputContent(talkuilogic_GetInputContent);
    AdapterBox_SetTalkUIGetInputContentEx(talkuilogic_GetInputContent);
    AdapterBox_SetTalkUIUpdateErrorInfo(talkuilogic_UpdateErrorInfo);

    AdapterBox_SetUpdateSoftKey(EdkUI_UpdateSoftkey);
    AdapterBox_SetShowEdkPrompt(EdkUI_ShowEdkPrompt);
    AdapterBox_SetEnterMenu(EdkUI_EnterMenu);
    AdapterBox_SetExecuteSoftkey(EdkUI_OnExcuteSoftkey);

    AdapterBox_SetShowVolumeBar(Volumebar_SetVolumeBarVisible);

    AdapterBox_SetTalkUIUpdateWnd(talkuilogic_UpdateWnd);

#if IS_COLOR || IF_FEATURE_XMLB_SCREENSAVER
    AdapterBox_SetExitScreenSaver(ScreenSavers_Exit);
#endif

    AdapterBox_SetIsTestMode(testmode_IsTestMode);
    AdapterBox_SetEnterTestMode(testmode_Enter);
    AdapterBox_SetExitTestMode(testmode_Exit);

    AdapterBox_SetXmlBrowserShowMessage(XmlBrowserUI_ShowMessage);
    AdapterBox_SetXmlBrowserDestroyMessage(XmlBrowserUI_DestroyMessage);
    AdapterBox_SetXmlBrowserUpdateWnd(XmlBrowserUI_UpdateWnd);
    AdapterBox_SetXmlBrowserExitXBDlg(XmlBrowserUI_ExitXBDlg);
    AdapterBox_SetXmlBrowserUpdate(IdleUL_UpdateXMLBrowser);
    AdapterBox_SetXmlBrowserCtrlLight(dsskeyUI_DsskeyLightCallback);

    AdapterBox_SetReloadIME(IME_Reload);

    AdapterBox_SetExitPickupMenu(talkui_ExitPickupMenu);

#ifndef _WIN32
#warning
#endif
//     AdapterBox_SetTotalUnReadMessage(SettingUILogic_GetTotalUnReadMessage);

    AdapterBox_SetDsskeyLongPress(dsskeyUI_OnDsskeyLongPress);
}
