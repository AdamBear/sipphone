#include "idlescreenui.h"
#include "baseui/include/xdatetimeview.h"
#include "directoryui/include/moddirectoryui.h"

#include "settingui/include/modsettingui.h"
#include "settingui/include/pageiddefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "uimanager/moduimanager.h"
#include "uimanager/modvolumebar.h"
#include "idlescreen/include/modidlescreen.h"
#include "messagebox/modmessagebox.h"
#include "xwindow/xControls.h"
#include "uilogicmanager.h"
#include "dsskeyui/moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "edkui/include/modedkui.h"
#include "talkui/include/modtalkui.h"
#include "languagehelper/modlangtranslate.h"
#include "keyguard/include/modkeyguard.h"
#include "screensavers/include/logomanager.h"

#include "baseui/include/t2xpicpath.h"
#include "baseui/include/baseuicommon.h"
#include "iconlist.h"
#include "idleuicommon.h"
#include "xwindow/xSoftkeyBar.h"
#include "dsskeyui/clinekeyui.h"
#include "edk/include/modedk.h"
#include "idleuilogic/include/modidleul.h"
#ifdef IF_OLD_BLUETOOTH
#include "wireless/bluetoothOld/include/modbluetooth.h"
#else
#include "wireless/bluetoothNew/include/modbluetooth.h"
#endif // IF_OLD_BLUETOOTH

#include "xwindow/xWindowManager.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"
#include "imagecache/include/modimagecache.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "settingui/include/modsettingui.h"
#include "settinguilogic/include/displaymessagedefine.h"
#include "xwindow/xListView.h"
#include "xmlbrowserui/src/textstatusadapter.h"


namespace
{
const char* IDLE_UI_LAYOUT_FILE = "idlescreen/idleui.xml";
const char* IDLE_SCREEN_NAME_DATE_TIME = "datetime";
const char* IDLE_SCREEN_NAME_ACCOUNT_PIC = "picAccount";
const char* IDLE_SCREEN_NAME_ACCOUNT_TEXT = "textAccount";
const char* IDLE_SCREEN_NAME_ICON_LIST = "statusIconList";
const char* IDLE_SCREEN_NAME_FRAME_IDLE = "frameIdle";
const char* IDLE_SCREEN_NAME_LOGO_PIC = "picLogo";
const char* IDLE_SCREEN_NAME_LAYOUT_LARGE_DND = "boxLargeDND";

const char* IDLE_SCREEN_NAME_NOTIFY_LAYOUT = "boxNotify";
const char* IDLE_SCREEN_NAME_NOTIFY_PIC = "picNotify";
const char* IDLE_SCREEN_NAME_NOTIFY_TEXT = "textNotify";
const char* IDLE_SCREEN_NAME_ACCOUNT_LAYOUT = "boxAccount";
const char * IDLE_SCREEN_NAME_TEXT_STATUS = "listView";

enum
{
    VALUE_HANDLE_TYPE_IDLE_FRAME = VIEW_HANDLE_TYPE_VALUE | 0x000A0000,
};

#if IF_SUPPORT_TEXT_NOTIFY
const char* IDLE_SCREEN_NAME_PROMPT_LAYOUT = "boxPrompt";
const char* IDLE_SCREEN_NAME_PROMPT_ANIMATE = "animatePrompt";
const char* IDLE_SCREEN_NAME_PROMPT_FLASHBOX = "flashPrompt";
const char* IDLE_SCREEN_NAME_PROMPT_TEXT = "textPrompt";
#define TITLE_PROMPT_FLASHING_TIME    500
#endif

#if IF_SUPPORT_CENTER_ACD_STATUS
const char* IDLE_SCREEN_CENTER_NOTE_TEXT = "textCenterNote";
#endif

#if IF_FEATURE_STATION_NAME
const char* IDLE_SCREEN_STATION_TEXT = "textStation";
#endif

#if IF_SUPPORT_EXTRA_IDLE_NOTE
const char* IDLE_SCREEN_EXTRA_NOTE_TEXT = "textExtraNote";
#endif

#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
const char* IDLE_SCREEN_NAME_MIDDLE_DATE_TIME = "datetimeMiddle";
#endif

#if IF_IDLE_HIDE_DATE_BY_ICON
#define DATE_TIME_WIDTH_SHOR_TIME           52
#endif

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET

#define  NORMAL_RECT_ID_DATE_TIME       "android::rcNormalDateTime"
#define  SHORT_RECT_ID_DATE_TIME        "android::rcShortDateTime"

#define  NORMAL_RECT_ID_ACCOUNT         "android::rcNormalAccount"
#define  SHORT_RECT_ID_ACCOUNT          "android::rcShortAccount"

#define  NORMAL_RECT_ID_CUSTOM_NOTIFY   "android::rcNormalCustomNotify"
#define  SHORT_RECT_ID_CUSTOM_NOTIFY    "android::rcShortCustomNotify"

#if IF_FEATURE_DND_LARGE_ICON
#define  NORMAL_RECT_ID_LARGE_DND       "android::rcNormalLargeDND"
#define  SHORT_RECT_ID_LARGE_DND        "android::rcShortLargeDND"
#endif

#if IF_SUPPORT_EXTRA_IDLE_NOTE
#define  IDLE_NOTE_ID_DATE_TIME_Y_OFFSET "android::nDateTimeOffsetY"
#endif

#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
#define  IDLE_ID_STATUS_ICON_RECT_RIGHT "android::rcSatusIconRight"
#define  IDLE_ID_STATUS_ICON_RECT_CENTER "android::rcSatusIconCenter"
#endif

#endif

#if IF_SUPPORT_ICON_PARAMETER
#define  IDLE_PARAMETER_ID_ICON_SIZE        "android::nIconSize"
#define  IDLE_PARAMETER_ID_ICON_DIRECTION   "android::bIconLeft2Right"
#endif

#define RING_MIN_VALUE      0
#define RING_MAX_VALUE      15
#define STATUS_ICON_MAX_AMOUNT  6

}

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET || IF_SUPPORT_ICON_PARAMETER || IF_SUPPORT_EXTRA_IDLE_NOTE
////////////////////////////////////////////////////////////////////////////////////////////
// xIdleFrameHandle
EXPORT_VIEW_HANDLER_CLASS(xIdleFrameHandle);
xIdleFrameHandle::xIdleFrameHandle()
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    : m_nOffsetYDateTime(0)
#endif
{
    m_nType = VALUE_HANDLE_TYPE_IDLE_FRAME;
}

xIdleFrameHandle::~xIdleFrameHandle()
{

}

void xIdleFrameHandle::loadAttributes(xml_node& node)
{
    xViewHandler::loadAttributes(node);
#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
    PopupAndroidRect(node, NORMAL_RECT_ID_DATE_TIME, m_stWidgetRectNormal.rcDateTime);
    PopupAndroidRect(node, NORMAL_RECT_ID_ACCOUNT, m_stWidgetRectNormal.rcAccount);
    PopupAndroidRect(node, NORMAL_RECT_ID_CUSTOM_NOTIFY, m_stWidgetRectNormal.rcCustomNotify);
    PopupAndroidRect(node, SHORT_RECT_ID_DATE_TIME, m_stWidgetRectShort.rcDateTime);
    PopupAndroidRect(node, SHORT_RECT_ID_ACCOUNT, m_stWidgetRectShort.rcAccount);
    PopupAndroidRect(node, SHORT_RECT_ID_CUSTOM_NOTIFY, m_stWidgetRectShort.rcCustomNotify);
#if IF_FEATURE_DND_LARGE_ICON
    PopupAndroidRect(node, NORMAL_RECT_ID_LARGE_DND, m_stWidgetRectNormal.rcLargeDND);
    PopupAndroidRect(node, SHORT_RECT_ID_LARGE_DND, m_stWidgetRectShort.rcLargeDND);
#endif
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    PopupAndroidInt(node, IDLE_NOTE_ID_DATE_TIME_Y_OFFSET, m_nOffsetYDateTime);
#endif
#if IF_SUPPORT_ICON_PARAMETER
    PopupAndroidInt(node, IDLE_PARAMETER_ID_ICON_SIZE, m_stIconParam.nIconSize);
    PopupAndroidBool(node, IDLE_PARAMETER_ID_ICON_DIRECTION, m_stIconParam.bLeft2Right);
#endif
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    PopupAndroidRect(node, IDLE_ID_STATUS_ICON_RECT_RIGHT, m_rcIconStatusRight);
    PopupAndroidRect(node, IDLE_ID_STATUS_ICON_RECT_CENTER, m_rcIconStatusCenter);
#endif
}

xIdleFrameHandle* xIdleFrameHandle::GetIdleFrameHandle(xViewHandler* pHandle)
{
    if (NULL == pHandle || VALUE_HANDLE_TYPE_IDLE_FRAME != pHandle->m_nType)
    {
        return NULL;
    }

    return static_cast<xIdleFrameHandle*>(pHandle);
}

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
void xIdleFrameHandle::GetIdleWidgetRect(ST_IDLE_WIDGET_RECT& stWidgetRect, bool bNormal)
{
    if (bNormal)
    {
        stWidgetRect = m_stWidgetRectNormal;
    }
    else
    {
        stWidgetRect = m_stWidgetRectShort;
    }
}
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
void xIdleFrameHandle::GetStatusIconRect(chRect& rcIcon, bool bRight)
{
    if (bRight)
    {
        rcIcon = m_rcIconStatusRight;
    }
    else
    {
        rcIcon = m_rcIconStatusCenter;
    }
}
#endif
#endif
#endif

CBaseDialog* CIdleScreenUI::CreateInstance()
{
    CIdleScreenUI* pIdleScreenUI = new CIdleScreenUI();

    if (NULL != pIdleScreenUI)
    {
        pIdleScreenUI->loadContent(IDLE_UI_LAYOUT_FILE);
    }

    return pIdleScreenUI;
}

CIdleScreenUI* CIdleScreenUI::GetInstance()
{
    static CIdleScreenUI* pIdleScreenUI = static_cast<CIdleScreenUI*>(UIManager_GetPrivateDlg(
            DLG_IdleScreen));
    if (NULL == pIdleScreenUI)
    {
        pIdleScreenUI = static_cast<CIdleScreenUI*>(UIManager_CreatePrivateDlg(DLG_IdleScreen));
    }

    return pIdleScreenUI;
}

CIdleScreenUI::CIdleScreenUI()
    : CBaseDialog(this, eWT_Idle)
    , m_pDateTimeView(NULL)
    , m_pIconList(NULL)
#if IF_SHOW_ACOUNT_ICON
    , m_pAccountPic(NULL)
#endif
    , m_pAccountText(NULL)
    , m_pListXMLStatus(NULL)
    , m_pAdapterXMLStatus(NULL)
#if IF_FEATURE_STATION_NAME
    , m_pStationText(NULL)
#endif
    , m_pFrameIdle(NULL)
#if IF_IDLE_SUPPORT_LOGO
    , m_pLogoPic(NULL)
#endif
#if IF_FEATURE_DND_LARGE_ICON
    , m_pLargeDND(NULL)
#endif
    , m_pNotifyLayout(NULL)
    , m_pNotifyPic(NULL)
    , m_pNotifyText(NULL)
#if IF_SUPPORT_TEXT_NOTIFY
    , m_pPromptLayout(NULL)
    , m_pPromptAnimationBox(NULL)
    , m_pPromptFlashBox(NULL)
    , m_pPromptText(NULL)
#endif
#if IF_SUPPORT_CENTER_ACD_STATUS
    , m_pCenterNote(NULL)
#endif
    , m_nLayoutShowFlag(IDLEUI_LAYOUT_SHOW_NONE)
    , m_nTimeShowType(xDateTimeView::TIME_SHOW_FORMAT_NONE)
    , m_nDateTimeType(xDateTimeView::DATE_TIME_SHOW_FORMAT_NONE)
#if IF_IDLE_HIDE_DATE_BY_ICON
    , m_nDateTimeWidth(0)
#endif
    , m_pAccountLayout(NULL)
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    , m_pExtraNote(NULL)
#endif
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    , m_pMiddleDateTimeView(NULL)
#endif
{
#if IF_SUPPORT_WALLPAPER
    etl_RegisterMsgHandle(DISPLAY_CURRENT_WALLPAPER_CHANGED, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                          &CIdleScreenUI::OnIdleScreenMsg);
#endif
}

CIdleScreenUI::~CIdleScreenUI()
{
#if IF_SUPPORT_WALLPAPER
    etl_UnregisterMsgHandle(DISPLAY_CURRENT_WALLPAPER_CHANGED, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                            &CIdleScreenUI::OnIdleScreenMsg);
#endif
}

#if IF_SUPPORT_WALLPAPER
LRESULT CIdleScreenUI::OnIdleScreenMsg(msgObject& msg)
{
    if (DISPLAY_CURRENT_WALLPAPER_CHANGED != msg.message)
    {
        return FALSE;
    }
    CBaseDialog* pDlg = UIManager_GetPrivateDlg(DLG_IdleScreen);
    if (NULL == pDlg)
    {
        return FALSE;
    }

    if (UIManager_IsInIdle())
    {
        UIManager_UpdateTopWnd();
        pDlg->DialogUpdate();
    }
    return TRUE;
}
#endif

void CIdleScreenUI::Init()
{
#if IF_SUPPORT_WALLPAPER
    UpdateBackGroundBrush();
#endif
    SetDefaultShowDsskey(true);

    m_SoftkeyData.clear();

    CSoftKeyBarData objSoftkeyData;

    objSoftkeyData.m_iType = ST_DIRECTORY;
    objSoftkeyData.m_strSoftkeyTitle = "Directory";
    m_SoftkeyData.push_back(objSoftkeyData);

    objSoftkeyData.m_iType = ST_HISTORY;
    objSoftkeyData.m_strSoftkeyTitle = "History";
    m_SoftkeyData.push_back(objSoftkeyData);

    objSoftkeyData.m_iType = ST_EMPTY;
    objSoftkeyData.m_strSoftkeyTitle = "DND";
    m_SoftkeyData.push_back(objSoftkeyData);

    objSoftkeyData.m_iType = ST_ENTER;
    objSoftkeyData.m_strSoftkeyTitle = "Menu";
    m_SoftkeyData.push_back(objSoftkeyData);

    SetSoftKey(m_SoftkeyData);

    //创建初始化UILogic
    _UiIdleLogicManager.Init(this);
#if IF_IDLE_SUPPORT_LOGO
    _LogoManager.InitLogo();
#endif

    Volumebar_ConnectVisibleChangeSlot(SLOT_METHOD(this, CIdleScreenUI::OnVolumeVisibleChanged));
    Volumebar_ConnectVolumeChangeSlot(SLOT_METHOD(this, CIdleScreenUI::OnVolumeChanged));
    MessageBox_ConnettShowMessageSlot(SLOT_METHOD(this, CIdleScreenUI::OnShowMessageBox));
}

void CIdleScreenUI::Uninit()
{
    Volumebar_DisconnectVisibleChangeSlot(SLOT_METHOD(this, CIdleScreenUI::OnVolumeVisibleChanged));
    Volumebar_DisconnectVolumeChangeSlot(SLOT_METHOD(this, CIdleScreenUI::OnVolumeChanged));
    MessageBox_DisconnectShowMessageSlot(SLOT_METHOD(this, CIdleScreenUI::OnShowMessageBox));
    if (m_pAdapterXMLStatus != NULL)
    {
        delete m_pAdapterXMLStatus;
        m_pAdapterXMLStatus = NULL;
    }
}

void CIdleScreenUI::loadChildrenElements(xml_node& node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pDateTimeView = static_cast<xDateTimeView*>(getViewById(IDLE_SCREEN_NAME_DATE_TIME));

    if (NULL != m_pDateTimeView)
    {
        m_pDateTimeView->Init();
        m_nDateTimeType = m_pDateTimeView->GetDateTimeShowFormat();
        m_nTimeShowType = m_pDateTimeView->GetTimeShowFormat();
    }

#if IF_SHOW_ACOUNT_ICON
    m_pAccountPic = static_cast<xImageView*>(getViewById(IDLE_SCREEN_NAME_ACCOUNT_PIC));
#endif
    m_pAccountText = static_cast<xTextView*>(getViewById(IDLE_SCREEN_NAME_ACCOUNT_TEXT));
    m_pIconList = static_cast<xLinearLayout*>(getViewById(IDLE_SCREEN_NAME_ICON_LIST));
    if (!DEVICELIB_IS_T42(devicelib_GetPhoneType()))
    {
        g_pIconList->InitLayoutView(m_pIconList);
    }

    m_pFrameIdle = static_cast<xFrameLayout*>(getViewById(IDLE_SCREEN_NAME_FRAME_IDLE));
#if IF_IDLE_SUPPORT_LOGO
    m_pLogoPic = static_cast<xImageView*>(getViewById(IDLE_SCREEN_NAME_LOGO_PIC));
#endif

#if IF_FEATURE_DND_LARGE_ICON
    m_pLargeDND = static_cast<xLinearLayout*>(getViewById(IDLE_SCREEN_NAME_LAYOUT_LARGE_DND));
#endif

    m_pNotifyLayout = static_cast<xLinearLayout*>(getViewById(IDLE_SCREEN_NAME_NOTIFY_LAYOUT));
    m_pNotifyPic = static_cast<xImageView*>(getViewById(IDLE_SCREEN_NAME_NOTIFY_PIC));
    m_pNotifyText = static_cast<xTextView*>(getViewById(IDLE_SCREEN_NAME_NOTIFY_TEXT));
    m_pListXMLStatus = static_cast<xListView *>(getViewById(IDLE_SCREEN_NAME_TEXT_STATUS));
    if (NULL != m_pListXMLStatus)
    {
        m_pAdapterXMLStatus = new CTextStatusAdapter;
        if (NULL != m_pAdapterXMLStatus)
        {
            m_pListXMLStatus->SetAdapter(m_pAdapterXMLStatus);
        }
    }

#if IF_SUPPORT_TEXT_NOTIFY
    m_pPromptLayout = static_cast<xLinearLayout*>(getViewById(IDLE_SCREEN_NAME_PROMPT_LAYOUT));
    m_pPromptAnimationBox = static_cast<xAnimationBox*>(getViewById(IDLE_SCREEN_NAME_PROMPT_ANIMATE));
    m_pPromptFlashBox = static_cast<xFlashingBox*>(getViewById(IDLE_SCREEN_NAME_PROMPT_FLASHBOX));
    m_pPromptText = static_cast<xTextView*>(getViewById(IDLE_SCREEN_NAME_PROMPT_TEXT));
#endif

#if IF_SUPPORT_CENTER_ACD_STATUS
    m_pCenterNote = static_cast<xTextView*>(getViewById(IDLE_SCREEN_CENTER_NOTE_TEXT));
#endif

#if IF_FEATURE_STATION_NAME
    m_pStationText = static_cast<xTextView*>(getViewById(IDLE_SCREEN_STATION_TEXT));
#endif

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
    m_pAccountLayout = static_cast<xLinearLayout*>(getViewById(IDLE_SCREEN_NAME_ACCOUNT_LAYOUT));
#endif

#if IF_SUPPORT_EXTRA_IDLE_NOTE
    m_pExtraNote = static_cast<xTextView*>(getViewById(IDLE_SCREEN_EXTRA_NOTE_TEXT));
#endif

#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    m_pMiddleDateTimeView = static_cast<xDateTimeView*>(getViewById(IDLE_SCREEN_NAME_MIDDLE_DATE_TIME));
    if (NULL != m_pMiddleDateTimeView)
    {
        m_pMiddleDateTimeView->Init();
    }
#endif

#if IF_IDLE_HIDE_DATE_BY_ICON
    if (NULL != m_pDateTimeView)
    {
        m_nDateTimeWidth = m_pDateTimeView->layout_width;
    }
#endif

}

#if IF_SUPPORT_ICON_PARAMETER
void CIdleScreenUI::loadAttributes(xml_node& node)
{
    xFrameLayout::loadAttributes(node);
    xIdleFrameHandle* pIdleHandle = xIdleFrameHandle::GetIdleFrameHandle(m_pHandler);

    if (NULL == pIdleHandle)
    {
        return;
    }

    ST_ICON_PARAMETER stIconParam;
    pIdleHandle->GetIconParam(stIconParam);
    g_pIconList->SetIconMax(stIconParam.nIconSize);
    g_pIconList->SetLayoutDirection(stIconParam.bLeft2Right);
}
#endif

//用于处理需要单独处理的配置，彩屏黑白屏有不同的地方
LRESULT CIdleScreenUI::OnConfigChangeMsg(msgObject& msg)
{
    switch (msg.wParam)
    {
#if IF_FEATURE_DND_LARGE_ICON
    case ST_DND:
        {
            LIST_IDLE_STATUS_ITEM listStatus = idleScreen_GetStatusList(SNT_ICON);
            _UiIdleLogicManager.IdleLogicIconCallBack(listStatus);
        }
        break;
#endif
#if IF_FEATURE_STATION_NAME
    case ST_STATION_NAME:
        {
            idleScreen_SetStatus(PS_STATE_ACCOUNT);
        }
        break;
#endif
#if IF_FEATURE_STATION_NAME
    case ST_IDLE_CLOCK:
        {
            CBaseDialog* pTopWindow = UIManager_GetTopWindow();
            if (NULL != pTopWindow && UIManager_IsInIdle())
            {
                UIManager_UpdateWnd(pTopWindow);
            };
        }
        break;
#endif
    case ST_FEATURE_SOFTKEY:
        {
            LIST_IDLE_STATUS_ITEM listStatus = idleScreen_GetStatusList(SNT_SOFTKEY);
            _UiIdleLogicManager.IdleLogicSoftkeyCallBack(listStatus);
        }
        break;
    case ST_DSSKEY_LENGHT:
    case ST_DSSKEY_LENGTH_SHORTEN:
        {
            IDLEUI_INFO(" ====== OnConfigChangeMsg %d====== ", msg.wParam);
            CBaseDialog* pTopWindow = UIManager_GetTopWindow();

            if (pTopWindow != NULL)
            {
                pTopWindow->OnDsskeyLengthChange();
                UIManager_UpdateWnd(pTopWindow);
            };
        }
        break;
#if IF_SUPPORT_WALLPAPER
    case ST_WALLPAPER:
        {
            if (NULL != g_pIdleScreenUI)
            {
                g_pIdleScreenUI->UpdateBackGroundBrush();
            }
        }
#endif
    default:
        break;
    }
    return TRUE;
}

bool CIdleScreenUI::onKeyPressedEvent(int key)
{
    bool bRet = _UiIdleLogicManager.OnKeyPress((PHONE_KEY_CODE)key);
#if IF_IDLE_SUPPORT_LOGO
    _LogoManager.ResetLogo();
#endif
    return bRet;
}

bool CIdleScreenUI::onKeyReleasedEvent(int key)
{
    return _UiIdleLogicManager.OnKeyRelease((PHONE_KEY_CODE)key);
}

bool CIdleScreenUI::onKeyLongPressedEvent(int key)
{
    bool bRet = _UiIdleLogicManager.OnKeyLongPress((PHONE_KEY_CODE)key);
#if IF_IDLE_SUPPORT_LOGO
    _LogoManager.ResetLogo();
#endif
    return bRet;
}

bool CIdleScreenUI::IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix)
{
    iVolumeType = VT_RING;
    iMin = RING_MIN_VALUE;
    iMax = RING_MAX_VALUE;
    return true;
}

void CIdleScreenUI::OnVolumeVisibleChanged(xSignal* sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    if (UIManager_IsInIdle())
    {
        if (wParam)
        {
#if IF_SUPPORT_BLUETOOTH
#ifndef _WIN32
#warning
#endif
//          if (!Bluetooth_IsMusicPlaying())
            {
#endif
                voice_PlayDefaultRing();
#if IF_SUPPORT_BLUETOOTH
            }
#endif
        }
        else
        {
            voice_StopRingInMenu();
        }
    }
}

void CIdleScreenUI::OnVolumeChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                    LPVOID pData)
{
    if (CHANNEL_RING == voice_GetCurrentChannel())
    {
        if (0 == (int)wParam)
        {
            // 音量为0，就显示静音状态
            idleScreen_SetStatus(PS_STATE_RING_MUTE);
        }
        else
        {
            // 音量改变，就撤销静音状态
            idleScreen_DropStatus(PS_STATE_RING_MUTE);
        }
    }
}

bool CIdleScreenUI::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (!bLongPress)
    {
        _UiIdleLogicManager.OnSoftkeyAction((SoftKey_TYPE)iType);
    }

    return false;
}

bool CIdleScreenUI::UpdateSoftkey(VEC_SOFTKET_TYPE& softkey)
{
    VEC_SOFTKET_TYPE::iterator it = softkey.begin();

    m_SoftkeyData.clear();
    CSoftKeyBarData objSoftkeyData;
    for (; it != softkey.end(); ++it)
    {
        objSoftkeyData.m_iType = (*it);
        MapSoftkeyString((SoftKey_TYPE)objSoftkeyData.m_iType, objSoftkeyData.m_strSoftkeyTitle);
        m_SoftkeyData.push_back(objSoftkeyData);
    }

    SetSoftKey(m_SoftkeyData);
    return true;
}

#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
yl::string CIdleScreenUI::GetSoftkeyBG()
{
    return PIC_THEME1_SOFTKEY_TRANSPARENT_BG;
}
#endif

#if IF_SUPPORT_TEXT_NOTIFY
bool CIdleScreenUI::UpdatePromptText(const ST_TextNotifyData& stNotifyText)
{
    if (NULL == m_pPromptAnimationBox || NULL == m_pPromptFlashBox
            || NULL == m_pPromptText || NULL == m_pDateTimeView)
    {
        return false;
    }

    if (stNotifyText.m_strNote.empty())
    {
        m_nLayoutShowFlag &= ~IDLEUI_LAYOUT_SHOW_PROMPT_TEXT;
        UpdateDynamicContent();
        return true;
    }

    if (stNotifyText.m_bFlashIcon)
    {
        LIST_STRING listIconPath;
        yl::string strIcon;
        LIST_ICON_TYPE::const_iterator it = stNotifyText.m_listIconType.begin();

        for (; it != stNotifyText.m_listIconType.end(); ++it)
        {
            ICON_TYPE eIconType = *it;
            if (IconType2PicPath(eIconType, strIcon))
            {
                listIconPath.push_back(strIcon);
            }
        }

        m_pPromptAnimationBox->show();
        m_pPromptAnimationBox->SetAnimation(listIconPath);
    }
    else
    {
        m_pPromptAnimationBox->hide(true);
    }

    if (stNotifyText.m_bFlashNote)
    {
        m_pPromptFlashBox->setupFlashTime(TITLE_PROMPT_FLASHING_TIME, TITLE_PROMPT_FLASHING_TIME);
    }
    else
    {
        m_pPromptFlashBox->setupFlashTime(-1, -1);
    }

    m_pPromptText->SetText(stNotifyText.m_strNote);
    m_nLayoutShowFlag |= IDLEUI_LAYOUT_SHOW_PROMPT_TEXT;
    UpdateDynamicContent();

    return true;
}
#endif

bool CIdleScreenUI::ClearIcon()
{
    g_pIconList->RemoveAll();
#if IF_FEATURE_DND_LARGE_ICON
    m_nLayoutShowFlag &= ~IDLEUI_LAYOUT_SHOW_LARGE_DND;
#endif
    return true;
}

void CIdleScreenUI::UpdateDynamicContent()
{
    if (NULL == m_pNotifyLayout || NULL == m_pDateTimeView
#if IF_SUPPORT_TEXT_NOTIFY
            || NULL == m_pPromptLayout
#endif
#if IF_FEATURE_DND_LARGE_ICON
            || NULL == m_pLargeDND
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
            || NULL == m_pExtraNote
#endif
       )
    {
        return;
    }

    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    if (PT_T54S == ePhoneType && NULL == m_pMiddleDateTimeView)
    {
        return;
    }
#endif

#if IF_SUPPORT_TEXT_NOTIFY
    m_pPromptLayout->hide();
#endif

#if IF_FEATURE_DND_LARGE_ICON
    m_pLargeDND->hide();
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    m_pExtraNote->hide();
#endif
    m_pNotifyLayout->hide();
    m_pDateTimeView->SetTimeShowFormat(m_nTimeShowType);
    m_pDateTimeView->SetDateTimeShowFormat(m_nDateTimeType);

    if (IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT))
    {
        m_pNotifyLayout->show();
#if IF_IDLE_SHOW_TIME_SMALL
        m_pDateTimeView->SetTimeShowFormat(xDateTimeView::TIME_SHOW_FORMAT_SMALL_HH_MM_SS);
#endif
    }
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    else if (IDLEUI_LAYOUT_SHOW_EXTRA_TEXT == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_EXTRA_TEXT))
    {
        m_pExtraNote->show();
    }
#endif

#if IF_SUPPORT_TEXT_NOTIFY
    if (IDLEUI_LAYOUT_SHOW_PROMPT_TEXT == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_PROMPT_TEXT))
    {
        m_pPromptLayout->show();
        m_pDateTimeView->SetDateTimeShowFormat(xDateTimeView::DATE_TIME_SHOW_FORMAT_TIME);
        return;
    }
#endif

#if IF_FEATURE_DND_LARGE_ICON
    if (IDLEUI_LAYOUT_SHOW_LARGE_DND == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_LARGE_DND))
    {
#if IF_IDLE_SHOW_STATUS_ROWS
        if (IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT != (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT))
#endif
        {
            m_pLargeDND->show();
        }

#if IF_SHOW_MIDDLE_ACOUNT
        m_pDateTimeView->SetDateTimeShowFormat(xDateTimeView::DATE_TIME_SHOW_FORMAT_TIME);
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
        m_pExtraNote->hide();
#endif
    }
#endif

#if IF_FEATURE_STATION_NAME
    RefreshStationName();
#endif

#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    if (PT_T54S == ePhoneType)
    {
        if (IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT)
#if IF_FEATURE_DND_LARGE_ICON
                || (IDLEUI_LAYOUT_SHOW_LARGE_DND == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_LARGE_DND))
#endif
                || 1 == configParser_GetConfigInt(kszDsskeyLength)
                || 0 == configParser_GetConfigInt(kszIdleClockDisplay)
           )
        {
            m_pMiddleDateTimeView->hide();
            m_pDateTimeView->show();
            UpdateStatusIconLayout(true);
        }
        else
        {
            m_pMiddleDateTimeView->show();
            m_pDateTimeView->hide();
            UpdateStatusIconLayout(false);
        }
    }
#endif

    ChangeDsskeyDispalyMode(true);
#if IF_IDLE_HIDE_DATE_BY_ICON
    if (PT_T52 == ePhoneType)
    {
        LIST_IDLE_STATUS_ITEM listStatus = idleScreen_GetStatusList(SNT_ICON);
        if (listStatus.size() >= 1)
        {
            m_pDateTimeView->layout_width = DATE_TIME_WIDTH_SHOR_TIME;
            m_pDateTimeView->SetDateTimeShowFormat(xDateTimeView::DATE_TIME_SHOW_FORMAT_TIME);
        }
        else
        {
            m_pDateTimeView->layout_width = m_nDateTimeWidth;
        }
        ownerStation().asyncRelayoutViewBound(m_pDateTimeView);
    }
#endif
}

bool CIdleScreenUI::UpdateIcon(const VEC_ICON_INFO& Statuslist)
{
    if (!UIManager_IsInIdle())
    {
        return true;
    }

    ClearIcon();
    for (int i = 0; i < Statuslist.size() && i < STATUS_ICON_MAX_AMOUNT; ++i)
    {
        const IdleIconInfo& iconInfo = Statuslist[i];
        g_pIconList->AddIcon((ICON_TYPE)iconInfo.m_eIconType, iconInfo.m_bFlash, iconInfo.m_strHint);
#if IF_FEATURE_DND_LARGE_ICON
        if (IT_STATE_DND == iconInfo.m_eIconType && 1 == configParser_GetConfigInt(kszShowDNDLargeIcon) && NULL != m_pLargeDND)
        {
            m_nLayoutShowFlag |= IDLEUI_LAYOUT_SHOW_LARGE_DND;
        }
#endif
    }

#if IF_IDLE_HIDE_DATE_BY_ICON || IF_FEATURE_DND_LARGE_ICON
    UpdateDynamicContent();
#endif

    return true;
}

bool CIdleScreenUI::UpdateXmlBrowser(VEC_CUSTOM_NOTIFY& listRefreshData)
{
    return true;
}

bool CIdleScreenUI::CreateMessageBox(const PopupBoxInfo& popData)
{
#if IF_IDLE_SUPPORT_LOGO
    _LogoManager.CancelLogo();
#endif

    // 删除同样类型的状态弹出框，为了更新数据，如连续两个misscall
    if (-1 != popData.m_nId)
    {
        IDLEUI_INFO("CreateMessageBox removemsg id[%d].", popData.m_nId);
        MessageBox_RemoveMessageBox(this, MessageBox_Close, popData.m_nId);
    }

    //弹出提示类型
    if (IDLE_MSGBOX_TYPE_NOTE == popData.m_iType || IDLE_MSGBOX_TYPE_WARNING == popData.m_iType)
    {
        MSGBOX_INFO infoMsgBox;
        infoMsgBox.eType = MESSAGEBOX_NOTE;
        infoMsgBox.pAction = this;
        if (-1 == popData.m_nId)
        {
            infoMsgBox.iID = MSGBOX_ID_NOTE;
            MessageBox_RemoveMessageBox(this, MessageBox_Close, MSGBOX_ID_NOTE);
        }
        else
        {
            infoMsgBox.iID = popData.m_nId;
        }
        infoMsgBox.strNote = popData.m_strNote;
        infoMsgBox.nMsTime = popData.m_nTimes;
        MessageBox_ShowCommonMsgBox(infoMsgBox);
    }
    else if (IDLE_MSGBOX_TYPE_POPUPNORMAL == popData.m_iType
#if IF_SUPPORT_MIDDLE_POPBOX
             || IDLE_MSGBOX_TYPE_POPUP_MIDDLE == popData.m_iType
#endif
            )
    {
        MSGBOX_INFO infoMsgBox;
#if IF_SUPPORT_MIDDLE_POPBOX
        infoMsgBox.eType = MESSAGEBOX_MIDDLE;
#else
        infoMsgBox.eType = MESSAGEBOX_OkCancel;
#endif
        infoMsgBox.pAction = this;
        infoMsgBox.iID = popData.m_nId;
        infoMsgBox.strTitle = _LANG(TRID_NOTE);
        infoMsgBox.strNote = popData.m_strNote;
        infoMsgBox.nMsTime = popData.m_nTimes;
        MsgBoxSoftkey stSoftInfo;
        SoftKey_TYPE eSoftType;
        for (int i = 0; i < popData.m_vecSoftkey.size(); ++i)
        {
            eSoftType = popData.m_vecSoftkey[i];
            MapSoftkeyString(eSoftType, stSoftInfo.m_title);
            stSoftInfo.m_result = SoftkeyType2ResultType(eSoftType);
            infoMsgBox.vecSoftkey.push_back(stSoftInfo);
        }
        MessageBox_ShowCommonMsgBox(infoMsgBox);
    }
    else if (IDLE_MSGBOX_TYPE_PICKUP_PAGE == popData.m_iType)
    {
        talkui_EnterPickupMenu();
    }
    return true;
}

bool CIdleScreenUI::RemoveIdleMessageBox(int nID)
{
    MessageBox_RemoveMessageBox(this, MessageBox_Close, nID);
#if IF_IDLE_SUPPORT_LOGO
    if (UIManager_IsInIdle())
    {
        _LogoManager.ResetLogo();
    }
#endif
    return true;
}

bool CIdleScreenUI::UpdateAccountInfo(const yl::string& strName, ICON_TYPE eIcon)
{
    if (NULL == m_pAccountText
#if IF_SHOW_ACOUNT_ICON
            || NULL == m_pAccountPic
#endif
       )
    {
        return false;
    }

#if IF_SHOW_ACOUNT_ICON
    yl::string strIcon;
    if (IconType2PicPath(eIcon, strIcon))
    {
        m_pAccountPic->setPixmap(strIcon);
    }
#endif

    if (!strName.empty())
    {
        m_pAccountText->SetText(strName);
    }

#if IF_FEATURE_STATION_NAME
    RefreshStationName();
#endif

    return true;
}

BOOL CIdleScreenUI::TransSoftkeyToKeyCode(SoftKey_TYPE eSoftKey, PHONE_KEY_CODE& nKeyCode)
{
    if (NULL == nKeyCode)
    {
        return FALSE;
    }

    BOOL bRet = TRUE;
    switch (eSoftKey)
    {
    case ST_IDLE_HOTKEY1:
        nKeyCode = PHONE_KEY_FN1;
        break;
    case ST_IDLE_HOTKEY2:
        nKeyCode = PHONE_KEY_FN2;
        break;
    case ST_IDLE_HOTKEY3:
        nKeyCode = PHONE_KEY_FN3;
        break;
    case ST_IDLE_HOTKEY4:
        nKeyCode = PHONE_KEY_FN4;
        break;
    default:
        bRet = FALSE;
        break;
    }

    return bRet;
}

void CIdleScreenUI::MapSoftkeyString(SoftKey_TYPE stype, yl::string& strSoftkey)
{
    switch (stype)
    {
    case ST_EXIT:
        {
            strSoftkey = _LANG(TRID_EXIT);
        }
        break;
    case ST_VIEW:
        {
            strSoftkey = _LANG(TRID_VIEW);
        }
        break;
    case ST_CANCEL:
        {
            strSoftkey = _LANG(TRID_CANCEL);
        }
        break;
    case ST_OK:
        {
            strSoftkey = _LANG(TRID_OK);
        }
        break;
    case ST_CONNECT:
        {
            strSoftkey = _LANG(TRID_CONNECT);
        }
        break;
    case ST_BLOCK:
        {
            strSoftkey = _LANG(TRID_BLOCK);
        }
        break;
    case ST_EMPTY:
        {
            strSoftkey = "";
        }
        break;
    case ST_HOTELING_GUESTIN:
        {
            strSoftkey = _LANG(TRID_GUEST_IN);
        }
        break;
    case ST_HOTELING_GUESTOUT:
        {
            strSoftkey = _LANG(TRID_GUEST_OUT);
        }
        break;
    case ST_UNLOCK:
        {
            strSoftkey = _LANG(TRID_UNLOCK);
        }
        break;
    case ST_GUEST_LOCK:
        {
            strSoftkey = _LANG(TRID_GUEST_LOCK);
        }
        break;
    case ST_ACD_LOGOUT:
        {
            strSoftkey = _LANG(TRID_LOGOUT);
        }
        break;
    case ST_ACD_LOGIN:
        {
            strSoftkey = _LANG(TRID_LOGIN);
        }
        break;
    case ST_ACD_UNAVAILABLE:
        {
            strSoftkey = _LANG(TRID_UNAVAIL);
        }
        break;
    case ST_ACD_AVAILABLE:
        {
            strSoftkey = _LANG(TRID_AVAIL);
        }
        break;
    case ST_ACD_DISPCODE:
        {
            strSoftkey = _LANG(TRID_ACD_DISPOSITION_CODE_SHORT);
        }
        break;
    case ST_ACD_TRACE:
        {
            strSoftkey = _LANG(TRID_ACD_TRACE);
        }
        break;
#if (defined IF_USB_RECORD)&& (defined IF_USB_SUPPORT)
    case ST_STOP_RECORD:
        {
            strSoftkey = _LANG(TRID_STOP_RECORD);
        }
        break;
    case ST_PAUSE_RECORD:
        {
            strSoftkey = _LANG(TRID_PAUSE_RECORD);
        }
        break;
    case ST_RESUME_RECORD:
        {
            strSoftkey = _LANG(TRID_RESUME_RECORD);
        }
        break;
#endif
    case ST_IDLE_HOTKEY1:
    case ST_IDLE_HOTKEY2:
    case ST_IDLE_HOTKEY3:
    case ST_IDLE_HOTKEY4:
        {
            PHONE_KEY_CODE nKeyCode = PHONE_KEY_NONE;
            TransSoftkeyToKeyCode(stype, nKeyCode);
            int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(nKeyCode);
            DssKeyLogicData DKlogicData;
            dsskey_GetData(iDsskeyID, DKlogicData);
            strSoftkey = DKlogicData.strLabel;

            // 先获取提示名称(EDK提示模式下有效)
            yl::string strTipName;
#if IF_FEATURE_EDK
            if (EdkUI_GetKeyTipName(DKlogicData.eDKType, strTipName, TIPNAME_PROGRAMKEY))
            {
                strSoftkey = strTipName;
            }
            else if (DT_CUSTOM_KEY == DKlogicData.eDKType)
            {
                strSoftkey = Edk_GetDsskeyData(DKlogicData.iDssKeyID, EDK_LABEL);;
            }
#endif
            if (strSoftkey.empty())
            {
                strSoftkey = _LANG(dsskey_GetDefaultLabel(DKlogicData.eDKType).c_str());
            }
        }
        break;
    default:
        break;
    }
}

MESSAGEBOX_RESULT CIdleScreenUI::SoftkeyType2ResultType(SoftKey_TYPE softtype)
{
    switch (softtype)
    {
    case ST_OK:
        {
            return MessageBox_OK;
        }
        break;
    case ST_VIEW:
        {
            return MessageBox_View;
        }
        break;
    case ST_CANCEL:
        {
            return MessageBox_Cancel;
        }
        break;
    case ST_EXIT:
        {
            return MessageBox_Cancel;
        }
        break;
    case ST_CONNECT:
        {
            return MessageBox_Connect;
        }
        break;
    case ST_BLOCK:
        {
            return MessageBox_Cancel;
        }
        break;
    }
    return MessageBox_ERROR;
}

SoftKey_TYPE CIdleScreenUI::ResultType2SoftkeyType(MESSAGEBOX_RESULT resultType)
{
    switch (resultType)
    {
    case MessageBox_OK:
        {
            return ST_OK;
        }
        break;
    case MessageBox_View:
        {
            return ST_VIEW;
        }
        break;
    case MessageBox_Cancel:
        {
            return ST_CANCEL;
        }
        break;
    case MessageBox_TimeOut:
        {
            return ST_CANCEL;
        }
        break;
    case MessageBox_Connect:
        {
            return ST_CONNECT;
        }
        break;
    }
    return ST_EMPTY;
}

void CIdleScreenUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    _UiIdleLogicManager.UpdateStatus();

    ChangeDsskeyDispalyMode(true);
    UpdateDynamicContent();
}

#if IF_IDLE_SUPPORT_LOGO
void CIdleScreenUI::ShowLogo(bool bShow, const yl::string& strPic)
{
    if (NULL == m_pFrameIdle || NULL == m_pLogoPic)
    {
        return;
    }
    if (bShow)
    {
        m_pFrameIdle->hide();
        currentSoftKeyBar().hide();
        xPixmap pixmap;
        pixmap.load(strPic);
        m_pLogoPic->setPixmap(pixmap);
        m_pLogoPic->show();
    }
    else
    {
        if (!_LogoManager.IsNeedToShowLogo())
        {
            xPixmap pixmap;
            m_pLogoPic->setPixmap(pixmap);
        }
        m_pLogoPic->hide();
        currentSoftKeyBar().show();
        m_pFrameIdle->show();
    }
}
#endif

#if IF_SUPPORT_WALLPAPER
void CIdleScreenUI::UpdateBackGroundBrush()
{
    yl::string strWallPath = SettingUILogic_GetWallpaperPath(WALLPAPER_Type_CURRENT);

    Image_GetImage(m_pmBkGround, strWallPath, IMG_CLASSIFY_WALLPAPER);
    Image_RemoveImage(strWallPath, IMG_CLASSIFY_WALLPAPER);

    if (m_pmBkGround.isNull())
    {
        strWallPath = SettingUILogic_GetWallpaperPath(WALLPAPER_Type_DEFAULT);
        Image_GetImage(m_pmBkGround, strWallPath, IMG_CLASSIFY_WALLPAPER);
        Image_RemoveImage(strWallPath, IMG_CLASSIFY_WALLPAPER);
    }

    if (!m_pmBkGround.isNull())
    {
        m_pmBkGround.convertDefaultFormat();
    }
}

xBrush CIdleScreenUI::GetBackGroundBrush()
{
    xBrush brushBk;
    brushBk.setPixmap(m_pmBkGround);
    return brushBk;
}
#endif

#if IF_FEATURE_STATION_NAME
void CIdleScreenUI::RefreshStationName()
{
    if (NULL == m_pAccountLayout
            || NULL == m_pStationText
       )
    {
        return;
    }

    yl::string strStationName = configParser_GetConfigString(kszStationName).c_str();
    if (strStationName.empty())
    {
        m_pAccountLayout->show();
        m_pStationText->hide();
    }
    else
    {
        // 主动更新账号信息
        m_pAccountLayout->hide();
        m_pStationText->show();
        m_pStationText->SetText(strStationName);
        if (0 != configParser_GetConfigInt(kszStationNameScrolling))
        {
            m_pStationText->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pStationText->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

#if IF_FEATURE_DND_LARGE_ICON
    if (IDLEUI_LAYOUT_SHOW_LARGE_DND == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_LARGE_DND))
    {
#if IF_SHOW_MIDDLE_ACOUNT
        m_pStationText->hide();
        m_pAccountLayout->hide();
#endif
    }
#endif
}
#endif

void CIdleScreenUI::OnFocusChanged(bool bFocused)
{
    if (bFocused)
    {
        // idle下重新设置定时器
        _UiIdleLogicManager.ResetKeypadLock();
#if IF_IDLE_SUPPORT_LOGO
        _LogoManager.ResetLogo();
#endif
    }
    else
    {
        _UiIdleLogicManager.StopKeypadLockTimer();
#if IF_IDLE_SUPPORT_LOGO
        _LogoManager.CancelLogo();
#endif
    }
}

void CIdleScreenUI::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    int nID = pMessageBox->GetID();
    IDLEUI_INFO("CIdleScreenUI::MessageBoxCallBack nResult is %d, nID is %d.", nResult, nID);
    if (MessageBox_Close == nResult || MessageBox_Destory == nResult || MessageBox_ERROR == nResult)
    {
        return;
    }

    SoftKey_TYPE eSoftkeyType = ResultType2SoftkeyType((MESSAGEBOX_RESULT)nResult);

    _UiIdleLogicManager.OnMessageKeyAction(eSoftkeyType, PHONE_KEY_NONE, nID);
    //关闭一个弹出框之后，重新check一次，只有状态弹出框才去check，
    if (MSGBOX_ID_STATUS == nID)
    {
        LIST_IDLE_STATUS_ITEM temp;
        _UiIdleLogicManager.IdleLogicPopupCallBack(temp);
    }
    else if (MSGBOX_ID_NOTE == nID)
    {
#if IF_IDLE_SUPPORT_LOGO
        _LogoManager.ResetLogo();
#endif
        // idle下重新设置定时器
        _UiIdleLogicManager.ResetKeypadLock();
    }
}

bool CIdleScreenUI::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    // 键盘锁开启的时候才进行判断
    if (!keyGuard_IsLock())
    {
        return true;
    }
    //如果有弹出框则先考虑弹出框
    CMessageBoxBase* pMessageBox = MessageBox_GetTopMessageBox();
    if (NULL != pMessageBox)
    {
        switch (pMessageBox->GetType())
        {
        //如果是提示框则给提示框处理
        case MESSAGEBOX_NOTE:
            {
                return true;
            }
            break;
        case MESSAGEBOX_OkCancel:
            {
                //menukey下不锁
                if (KG_MENU_KEY == keyGuard_GetKeyGuardType())
                {
                    break;
                }
                else
                {
                    //不允许okcancel
                    if (PHONE_KEY_OK == nKeyCode
                            || PHONE_KEY_CANCEL == nKeyCode
                            || PHONE_KEY_FN1 == nKeyCode
                            || PHONE_KEY_FN2 == nKeyCode
                            || PHONE_KEY_FN3 == nKeyCode
                            || PHONE_KEY_FN4 == nKeyCode)
                    {
                        // 不管是否解锁成功都拦截该按键
                        return false;
                    }
                    break;
                }
            }
            break;
        default:
            {
                //其他所有类型弹出框给界面处理
            }
            break;
        }
    }
    return CBaseDialog::AllowKeyPadEventOnKeyLock(nKeyCode, bPress);
}

void CIdleScreenUI::RefreshSoftkey()
{
    IdleUL_UpdateSoftkey();
}

bool CIdleScreenUI::IsSupportDsskeyShorten()
{
    if (PT_T52 == devicelib_GetPhoneType())
    {
        return true;
    }

    return false;
}

#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
bool CIdleScreenUI::IsShowDefaultAccount()
{
    // http://10.2.1.199/Bug.php?BugID=90716
    if (PT_T69 == devicelib_GetPhoneType() || PT_T69S == devicelib_GetPhoneType())
    {
        return 1 == configParser_GetConfigInt(kszEnableIdleDefaultAccount);
    }

    return false;
}
#endif

bool CIdleScreenUI::IsIdleContentShow()
{
    return (
               //m_bXMLActive ||
               (NULL != m_pListXMLStatus && m_pListXMLStatus->GetTotalItemCount())
#if IF_FEATURE_DND_LARGE_ICON
               || (IDLEUI_LAYOUT_SHOW_LARGE_DND == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_LARGE_DND))
#endif
#if IF_SUPPORT_CENTER_ACD_STATUS
               || (NULL != m_pCenterNote && m_pCenterNote->GetText().trim_both().empty())
#endif
#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
               || (NULL != m_pAccountText && !m_pAccountText->GetText().trim_both().empty()
                   && IsShowDefaultAccount())
#endif
           );
}

bool CIdleScreenUI::OnDsskeyLengthChange()
{
    ChangeDsskeyDispalyMode(false);

    UpdateDynamicContent();

    return true;
}

DSSKEY_DISPLAY_MODE CIdleScreenUI::GetDsskeyDisplayMode()
{
    return m_eDsskeyDisplayMode;
}

void CIdleScreenUI::ChangeDsskeyDispalyMode(bool bUpdate /* = false*/)
{
    if (IsSupportDsskeyShorten() && IsIdleContentShow())
    {
        if (1 == configParser_GetConfigInt(kszDsskeyLengthShorten))
        {
            m_eDsskeyDisplayMode = DDM_TWO_CHARS;
        }
        else
        {
            m_eDsskeyDisplayMode = DDM_NO_LABEL;
        }
    }
    else
    {
#if IF_SUPPORT_FULL_DSSKEY_LEGNTH

        if (0 != configParser_GetConfigInt(kszEnableACDAutoAvailable))
        {
            //当ACD开启的时候,Linekey加长的功能强制不生效
            m_eDsskeyDisplayMode = DDM_NORMAL;
        }

#if IF_FEATURE_T46_LINEKEY_LABEL
#if IF_SUPPORT_EXTERN_DSSKEY_LENGTH
        else if (devicelib_GetPhoneType() != PT_T52
                 && 2 == configParser_GetConfigInt(kszDsskeyLength))
        {
            m_eDsskeyDisplayMode = DDM_EXTEND;
        }
#endif
#endif
        else
        {
            if (!IsIdleContentShow() && 1 == configParser_GetConfigInt(kszDsskeyLength))
            {
                m_eDsskeyDisplayMode = DDM_FULL;
            }
            else
            {
                m_eDsskeyDisplayMode = DDM_NORMAL;
            }
        }
#else
        if (1 == configParser_GetConfigInt(kszDsskeyLength))
        {
            m_eDsskeyDisplayMode = DDM_EXTEND;
        }
        else
        {
            m_eDsskeyDisplayMode = DDM_NORMAL;
        }
#endif

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
        ChangeContentSizeByMode(m_eDsskeyDisplayMode);
#endif
    }

    if (bUpdate)
    {
        UpdateDsskey(true);
    }
}

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
void CIdleScreenUI::ChangeContentSizeByMode(DSSKEY_DISPLAY_MODE eDsskeyMode)
{
    xIdleFrameHandle* pIdleHandle = xIdleFrameHandle::GetIdleFrameHandle(m_pHandler);

    if (NULL == pIdleHandle)
    {
        return;
    }

    switch (eDsskeyMode)
    {
    case DDM_EXTEND:
        {
            ST_IDLE_WIDGET_RECT rcWidgetRect;
            pIdleHandle->GetIdleWidgetRect(rcWidgetRect, false);
            UpdateIdleContent(rcWidgetRect);
        }
        break;
    case DDM_NORMAL:
        {
            ST_IDLE_WIDGET_RECT rcWidgetRect;
            pIdleHandle->GetIdleWidgetRect(rcWidgetRect, true);
            UpdateIdleContent(rcWidgetRect);
        }
        break;
    default:
        break;
    }
}

void CIdleScreenUI::UpdateIdleContent(const ST_IDLE_WIDGET_RECT& stWidgetRect)
{
    if (NULL == m_pAccountLayout || NULL == m_pDateTimeView
            || NULL == m_pNotifyLayout
#if IF_FEATURE_DND_LARGE_ICON
            || NULL == m_pLargeDND
#endif
#if IF_FEATURE_STATION_NAME
            || NULL == m_pStationText
#endif
#if IF_SUPPORT_CENTER_ACD_STATUS
            || NULL == m_pCenterNote
#endif
       )
    {
        return;
    }

#if IF_SUPPORT_EXTRA_IDLE_NOTE
    if (IDLEUI_LAYOUT_SHOW_EXTRA_TEXT == (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_EXTRA_TEXT)
            && IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT != (m_nLayoutShowFlag & IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT))
    {
        xIdleFrameHandle* pIdleHandle = xIdleFrameHandle::GetIdleFrameHandle(m_pHandler);
        if (NULL == pIdleHandle)
        {
            return;
        }

        int nOffsetY = pIdleHandle->GetDateTimeOffsetY();
        stWidgetRect.rcDateTime.OffsetRect(0, -nOffsetY);
    }
#endif

    UpdateWidgetLayoutInfo(m_pDateTimeView, stWidgetRect.rcDateTime);
#if IF_SHOW_MIDDLE_ACOUNT
    UpdateWidgetLayoutInfo(m_pAccountLayout, stWidgetRect.rcAccount);
#if IF_FEATURE_STATION_NAME
    UpdateWidgetLayoutInfo(m_pStationText, stWidgetRect.rcAccount);
#endif
#endif

#if IF_SUPPORT_CENTER_ACD_STATUS
    UpdateWidgetLayoutInfo(m_pCenterNote, stWidgetRect.rcAccount);
#endif

    UpdateWidgetLayoutInfo(m_pNotifyLayout, stWidgetRect.rcCustomNotify);
#if IF_FEATURE_DND_LARGE_ICON
    UpdateWidgetLayoutInfo(m_pLargeDND, stWidgetRect.rcLargeDND);
#endif

}

void CIdleScreenUI::UpdateWidgetLayoutInfo(xView* pView, const chRect& rcWidget)
{
    if (NULL == pView || rcWidget.IsEmpty())
    {
        return;
    }

    pView->layout_marginLeft = rcWidget.left;
    pView->layout_marginTop = rcWidget.top;
    pView->layout_width = rcWidget.right - rcWidget.left;
    pView->layout_height = rcWidget.bottom - rcWidget.top;

    ownerStation().asyncRelayoutViewBound(pView);
}
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
void CIdleScreenUI::UpdateStatusIconLayout(bool bRight)
{
    xIdleFrameHandle* pIdleHandle = xIdleFrameHandle::GetIdleFrameHandle(m_pHandler);
    if (NULL == pIdleHandle || NULL == m_pIconList)
    {
        return;
    }

    if (bRight)
    {
        m_pIconList->gravity = gravity_y_center | align_right;
    }
    else
    {
        m_pIconList->gravity = align_center;
    }

    chRect rcIcon;
    pIdleHandle->GetStatusIconRect(rcIcon, bRight);
    UpdateWidgetLayoutInfo(m_pIconList, rcIcon);
}
#endif
#endif

bool CIdleScreenUI::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (MessageBox_GetMessageBoxCount() > 0 && bPress)
    {
        bool bDigitProcess = false;
        IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_POPUP);
        switch (phonestate.m_nId)
        {
        case PS_STATE_MISSCALL:
        case PS_STATE_TEXTMESSAGE:
        case PS_STATE_CALLFORWARD:
        case PS_STATE_VOICEMAIL:
        case PS_STATE_NETWORK_OK_BUT_CONFLICT:
        case PS_STATE_NETWORK_FAILED:
#ifdef IF_FEATURE_PSTN
        case PS_STATE_PSTN_ACCOUNT_NOTE:
#endif
#if (defined IF_USB_RECORD)&& (defined IF_USB_SUPPORT)
        case PS_STATE_USBDEVICE_DETECTFAILED:
#warning PS_STATE_USBDEVICE_NOT_SUPPORT
            //case PS_STATE_USBDEVICE_NOT_SUPPORT:
#warning PS_STATE_USB_CURRENT_OVERLOAD
            //case PS_STATE_USB_CURRENT_OVERLOAD:
#endif // IF_USB
            {
                bDigitProcess = true;
            }
            break;
        default:
            break;
        }

        if ((bDigitProcess && ((PHONE_KEY_0 <= nKeyCode && nKeyCode <= PHONE_KEY_9)
                               || PHONE_KEY_STAR == nKeyCode || PHONE_KEY_POUND == nKeyCode))
                || PHONE_KEY_HANDFREE == nKeyCode || PHONE_KEY_HANDSET_OFF_HOOK == nKeyCode)
        {
            return _UiIdleLogicManager.OnKeyPress((PHONE_KEY_CODE)nKeyCode);
        }

        CMessageBoxBase* pMsgBox = MessageBox_GetTopMessageBox();
        if (NULL == pMsgBox)
        {
            return false;
        }

        int nID = pMsgBox->GetID();
        if (MSGBOX_ID_STATUS == nID && PHONE_KEY_HANDSET_ON_HOOK == nKeyCode)
        {
            bool bRet = _UiIdleLogicManager.OnMessageKeyAction(ST_EMPTY, PHONE_KEY_CANCEL, MSGBOX_ID_STATUS);
            if (bRet)
            {
                LIST_IDLE_STATUS_ITEM statusList;
                _UiIdleLogicManager.IdleLogicPopupCallBack(statusList);
            }
            return true;
        }
        else if (MSGBOX_ID_NOTE == nID)
        {
            MessageBox_RemoveMessageBox(this, MessageBox_Close, MSGBOX_ID_NOTE);
            return true;
        }
        else if (MESSAGEBOX_OkCancel == pMsgBox->GetType() && PHONE_KEY_HANDSET_ON_HOOK == nKeyCode)
        {
            MessageBox_RemoveMessageBox(this, MessageBox_Cancel, nID);
            return true;
        }
    }

    if (MessageBox_GetMessageBoxCount() > 0
            && (PHONE_KEY_MESSAGE == nKeyCode || PHONE_KEY_REDIAL == nKeyCode))
    {
        if (bPress)
        {
            _UiIdleLogicManager.OnKeyPress((PHONE_KEY_CODE)nKeyCode);
        }
        else
        {
            _UiIdleLogicManager.OnKeyRelease((PHONE_KEY_CODE)nKeyCode);
        }
    }

    return false;
}

bool CIdleScreenUI::UpdateCustomNotify(const VEC_STATUS_ITEM& refCusNotify, bool bCusNotify)
{
    if (NULL == m_pNotifyLayout)
    {
        return false;
    }
    if (bCusNotify)
    {
        if (m_pAdapterXMLStatus != NULL)
        {
            m_pNotifyLayout->show(); //m_pNotifyLayout为show时才能刷新列表
            m_pAdapterXMLStatus->SetData(refCusNotify);
            m_pAdapterXMLStatus->RefreshListView();
        }

        m_nLayoutShowFlag |= IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT;
    }
    else
    {
        m_nLayoutShowFlag &= ~IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT;
    }
    UpdateDynamicContent();

    return true;
}

void CIdleScreenUI::EnterPasswordCheckPage()
{
    SettingUI_EnterPage(kszPageIDIdleResetNeedPwd);
}

void CIdleScreenUI::SetLongPressTimeByKey(int nKey, int nTime)
{
    g_WorkStation.SetLongPressTimeByKey(nKey, nTime);
}

void CIdleScreenUI::ResetLongPressTimeByKey(int nKey)
{
    g_WorkStation.SetLongPressTimeByKey(nKey);
}

void CIdleScreenUI::OnShowMessageBox(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), POWERSAVIGNMEG_NOTIFY_OPERATE, 0, 0);
}

bool CIdleScreenUI::IsExistMessageBox()
{
    return (0 != MessageBox_GetMessageBoxCount());
}

int CIdleScreenUI::GetDsskeyIDByKeyCode(int nKey)
{
    return dsskeyUI_GetDsskeyIDByKeycode(nKey);
}

void CIdleScreenUI::EnterPageByType(IDLE_ENTER_PAGE_TYPE_E eEnterPageType)
{
    EnterPage(eEnterPageType);
}

#if IF_SUPPORT_CENTER_ACD_STATUS
void CIdleScreenUI::UpdateCenterNote(const yl::string& strNote)
{
    if (NULL == m_pCenterNote)
    {
        return;
    }

    if (strNote.empty())
    {
        m_pCenterNote->hide();
    }
    else
    {
        m_pCenterNote->show();
        m_pCenterNote->SetText(strNote);
    }
}
#endif

#if IF_SUPPORT_EXTRA_IDLE_NOTE
void CIdleScreenUI::UpdateExtraNote(const yl::string& strNote)
{
    if (NULL == m_pExtraNote)
    {
        return;
    }

    if (strNote.empty())
    {
        m_nLayoutShowFlag &= ~IDLEUI_LAYOUT_SHOW_EXTRA_TEXT;
    }
    else
    {
        m_nLayoutShowFlag |= IDLEUI_LAYOUT_SHOW_EXTRA_TEXT;
        m_pExtraNote->SetText(strNote);
    }
    UpdateDynamicContent();
}
#endif


