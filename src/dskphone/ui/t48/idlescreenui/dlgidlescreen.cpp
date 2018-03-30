#include <QLabel>
#include <QLineEdit>

#include "dlgidlescreen.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
// #include "dsskeyuicommondefine.h"
#include "account/include/accountmessagedefine.h"

//mod-UIManager
#include "uimanager/uimanager_inc.h"
#include "uimanager/uimanager_header.h"

//mod-MainWnd
#include "mainwnd/mainwnd.h"
#include "mainwnd/autovolumebar.h"
#include "uikernel/qsoftkeybar.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qstatusbar.h"
#include "uikernel/quipushbutton.h"
#include "uikernel/qwidgetutility.h"
#include "baseui/fontcolor.h"
#include "baseui/transwidgetbgcontroler.h"

#include "picpath.h"
#include "imagemanager/modimagemanager.h"
#include "settingui/include/modsettingui.h"
// #include "commonunits/statusmanager.h"
#include "commonunits/modcommonunits.h"
#include "setting/include/common.h"
#include "keyguard/include/keyguardmessagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "keymap.h"
#include "voice/include/modvoice.h"
#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "bluetooth/btkit_api.h"
#include "extended/bluetoothmessagedefine.h"
// #include "define/phonestatemessagedefine.h"
#include "idle/tipmessagedefine.h"
// #include "settingui/include/modsettingui.h"
#include "setting/include/modsetting.h"
// #include "setting/voicemailcontroller.h"
// #include "setting/textmessagecontroller.h"
// #include "setting/resetandrebootcontroller.h"
// #include "menu/menucontroller.h"
#include "qtcommon/qmisc.h"
#include "talk/callcompletion/include/modcallcompletion.h"

// XML Module
#include "xmlbrowserui/xmlbrowseruiframe.h"
#include "xmlbrowserui/modxmlbrowserui.h"

// #include "dsskey/moddsskey.h"
#include "uimanager/moduimanager.h"
// #include "CommonUnits/AutoPCodePaser.h"

#include "idlecommonwidget.h"
#include "idlenotemanager.h"
#include "account/include/modaccount.h"

//CallCenter
#include "broadsoft/callcenter/include/ccmessagedefine.h"
#include "broadsoft/callcenter/include/modcallcenter.h"

// #include "CommonUnits/ProgrammableKeyManager.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
// #include "DssKey/ACDProcessor.h"

#include "hotplug/commondefine.h"

// mod-KeyGuard
#include "keyguard/include/modkeyguard.h"

#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "lamp/backlight/include/modbacklight.h"

// dsskey Logic
#include "dsskey/include/moddsskey.h"

// DsskeyUI
#include "dsskeyui/src/dsskeyframe.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "uimanager/rectdefine.h"

// 图片ICON宏定义
#include "baseui/t4xpicpath.h"
#include "setting/include/displaymessagedefine.h"
#include "directoryui/include/moddirectoryui.h"
#include "idlecommonwidget.h"
#include "setting_page_define.h"
#include "setting/include/modmenu.h"
//lcm
#include "uicommon/messagebox/modmessagebox.h"
#include "setting/src/displaycontroller.h"
#include "adapterbox/include/modadapterbox.h"
#include "settingui/src/networksettingmanager.h"

#include "popupboxmsgdefine.h"
#include "dsskeyui/src/moddsskeyperformance.h"
#include "feature/include/modfeature.h"

#include "mainwndhelp/keyguardhelp.h"

#include "screensavers/include/modscreensavers.h"
#include "uicommon/globalinputhook/modinputhook.h"

#ifdef IF_FEATURE_GETBALANCE
#include "account/include/getbalance.h"
#endif
#include "blf/include/blflistmsgdefine.h"

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/recordmessagedefine.h"
#include "record/include/modrecord.h"
#endif

#include "blf/include/modblf.h"

#include "autopcontrol/include/modautopcontrol.h"
#include "autopcontrol/include/autopcontrollmsgdefine.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
#include "screensavers/src/screensavercontroller.h"
#endif

#include "customtheme/modcustomtheme.h"

static const yl::string OPEN_STATUS_ACTION("OpenMenu(status_list.xml)");

static const yl::string OPEN_DIRECTORY_ACTION("OpenDirectoryPage(local_contact_list.xml)");

// 界面控件位置定义

static const QRect RECT_ACD_NOTE(106, 293, 268, 50);
static const QRect RECT_ACD_QUESTAT(12, 10, 458, 50);
static const QRect RECT_DEFAULT_ACCOUNT(106, 293, 268, 50);

static const bool        TEST_MODE = false;

// Idle界面位置
const QRect RECT_IDLE(160, TITLE_HEIGHT, PHONE_WIDTH - 320, 347);

#define DSSKEY_IDLE_LEFT_OFFSET_L   1
#define DSSKEY_IDLE_TOP_OFFSET      0
#define DSSKEY_IDLE_WIDTH           156
#define DSSKEY_IDLE_HEIGHT          55
#define DSSKEY_IDLE_V_DISTANCE      57
#define DSSKEY_IDLE_H_DISTANCE      161
const QRect RECT_IDLE_DSSKEYFRAME(0, 7, 480, 340);// Dsskey悬浮框的位置
#define WEB2PHONE_QUERY_DSKPHONE_RUNUP  0x30004

#define MAX_BUF_LEN         512

#define ACTION_REDIAL "Redial"

enum tagIdleMsgbox
{
    IDLE_MSGBOX_TYPE_CANCEL = 0,
    IDLE_MSGBOX_TYPE_OK,
    IDLE_MSGBOX_TYPE_PASSWORD,
    IDLE_MSGBOX_TYPE_NETWORKCHANGE,
    IDLE_MSGBOX_TYPE_BALANCE_LOADING,
    IDLE_MSGBOX_TYPE_HOTELING_START,
    IDLE_MSGBOX_TYPE_DOWNLOAD_CONFIG,
    IDLE_MSGBOX_TYPE_HOTELING_LOGOUT,
    IDLE_MSGBOX_TYPE_HOTELING_UNLOCK,
    IDLE_MSGBOX_TYPE_DSSKEY_LOCK_PASSWORD,
    IDLE_MSGBOX_TYPE_UPDATE_CONFIG
};
#define ST_POPUP_VOICE_MAIL     (0x01 << 0)
#define ST_POPUP_MISSED_CALL    (0x01 << 1)
#define ST_POPUP_FORWARD_CALL   (0x01 << 2)
#define ST_POPUP_TEXT_MSG       (0x01 << 3)

//默认进入屏保的最小时间
#define ENTER_SCREEN_MIN_TIME           15

// AutoP提示时间,单位毫秒
#define AUTOP_RESULT_HINT_SHOW_TIME     10000
#if IF_FEATURE_XMLBROWSER_HOTELING
enum XMLBrowserHotelingConfigType
{
    XB_HOTELING_SERVER = 0,
    XB_HOTELING_LOGIN_STATUS = 1,
    XB_HOTELING_LOGOUT_PROMPT = 2,
    XB_HOTELING_LOGOUT_DURATION = 3,
    XB_HOTELING_LOGOUT_USER = 4,
};
#endif


#include "dsskeyui/include/dsskeybtndisplaydelegate.h"

static DsskeyBtnDisplayDelegete * pDKBtnDel = NULL;
////////////////Dsskey按键自定义显示代理的基类/////////////////////////
class IdleDsskeyDelegete : public DsskeyBtnDisplayDelegete
{
public:
    explicit IdleDsskeyDelegete()
    {

    }
    virtual ~ IdleDsskeyDelegete()
    {

    }

    virtual void PaintDsskeyBtn(QStylePainter & stylePainter, const DsskeyDisplayData & sData,
                                const QWidget * pWidget)
    {
        if (NULL == pWidget)
        {
            DSSKEYUI_INFO("You cann't paint a null pWidget.");
        }
        else
        {
            // 画按钮上的文本, 如果已经画了ICON, 则需要进行相应的偏移
            QRect rcText = pWidget->rect();
            QPalette pal = pWidget->palette();
            QPen penNormal = stylePainter.pen();
            penNormal.setColor(qRgb(0, 0, 0));
            stylePainter.setPen(penNormal);
            QString strText = toQString(sData.strFirstRowText.c_str());
            stylePainter.drawItemText(rcText, Qt::AlignCenter, pal, pWidget->isEnabled(), strText);
        }
    }
protected:

private:

};

class CIdleDsskeyFrame : public qDsskeyFrame
{
public:
    CIdleDsskeyFrame(QWidget * parent);
    virtual ~CIdleDsskeyFrame();

    void RefreshDsskeyFrame();

protected:
    //每次值变化进行重画
    void paintEvent(QPaintEvent * pPaintEvent);
private :
    QPixmap m_pixBg;
};

CIdleDsskeyFrame::CIdleDsskeyFrame(QWidget * parent)
    : qDsskeyFrame(parent)
{
    RefreshDsskeyFrame();
}

void CIdleDsskeyFrame::RefreshDsskeyFrame()
{
    // 绘制背景图片
    QPixmap pmFrameBG(480, 340);
    pmFrameBG.fill(QColor(0, 0, 0, 0));
    QPixmap pmBtnBg = THEME_GET_BMP(g_TransWidgetBgControler.GetNormalDsskeyBg().c_str());
    QPainter painter(&pmFrameBG);

    int i;
    // 设置按钮
    for (i = 0; i < 18; ++i)
    {
        QRect rcArea;
        if (i < 6)
        {
            // 第1排Dsskey按钮
            rcArea.setRect(DSSKEY_IDLE_LEFT_OFFSET_L, DSSKEY_IDLE_TOP_OFFSET + i * DSSKEY_IDLE_V_DISTANCE,
                           DSSKEY_IDLE_WIDTH, DSSKEY_IDLE_HEIGHT);
        }
        else if (i < 12)
        {
            // 第2排Dsskey按钮
            rcArea.setRect(DSSKEY_IDLE_LEFT_OFFSET_L + DSSKEY_IDLE_H_DISTANCE,
                           DSSKEY_IDLE_TOP_OFFSET + (i - 6) * DSSKEY_IDLE_V_DISTANCE, DSSKEY_IDLE_WIDTH, DSSKEY_IDLE_HEIGHT);
        }
        else
        {
            // 第3排Dsskey按钮
            rcArea.setRect(DSSKEY_IDLE_LEFT_OFFSET_L + 2 * DSSKEY_IDLE_H_DISTANCE,
                           DSSKEY_IDLE_TOP_OFFSET + (i - 12) * DSSKEY_IDLE_V_DISTANCE, DSSKEY_IDLE_WIDTH, DSSKEY_IDLE_HEIGHT);
        }

        painter.drawPixmap(rcArea, pmBtnBg);
    }

    m_pixBg = pmFrameBG;
    update();
}


CIdleDsskeyFrame::~CIdleDsskeyFrame()
{

}

void CIdleDsskeyFrame::paintEvent(QPaintEvent * pPaintEvent)
{
    IDLEUI_INFO("CIdleDsskeyFrame::paintEvent");
    QRect rc = rect();
    QStylePainter p(this);
    if (!m_pixBg.isNull())
    {
        p.drawPixmap(rc, m_pixBg);
    }
    IDLEUI_INFO("CIdleDsskeyFrame::paintEvent  Ending.");
}

bool IdleScreen_ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (UIManager_IsInIdle() && !bPress)
    {
        g_pIdleScreenUI->StartScreensaverTimer();
    }

    return false;
}

bool IdleScreen_MouseEvent(QObject * pObject, QMouseEvent * pEvent)
{
    if (UIManager_IsInIdle() && NULL != pEvent && pEvent->type() == QEvent::MouseButtonRelease)
    {
        g_pIdleScreenUI->StartScreensaverTimer();
    }

    return false;
}

//
static LRESULT OnIdleScreenMsg(msgObject & objMessage)
{
    IDLEUI_INFO("DlgIdleScren OnIdleScreenMsg 0x[%x] 0x[%x]", objMessage.message, objMessage.wParam);
    IDLEUI_INFO("AUTOPROVISION_UPDATE_NOTICE= 0x[%x]", AUTOPROVISION_UPDATE_NOTICE);
    switch (objMessage.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            // 处理配置改变
            switch (objMessage.wParam)
            {
            case ST_PROGRAMABLE_KEY:
                {
                    g_pIdleScreenUI->OnProgramableKeyConfigChange();
                }
                break;
#if IF_BUG_24048_RTCP_XR
            case ST_VQ_REPORT_ACTIVE:
                {
                    g_pIdleScreenUI->OnRTPActiveChange();
                }
                break;
#endif
            case ST_VOICEMAIL_POPUP:
                {
                    /*#define ST_POPUP_VOICE_MAIL       (0x01 << 0)
                     #define ST_POPUP_MISSED_CALL   (0x01 << 1)
                     #define ST_POPUP_FORWARD_CALL  (0x01 << 2)
                     #define ST_POPUP_TEXT_MSG      (0x01 << 3)*/
                    if (ST_POPUP_VOICE_MAIL & objMessage.lParam)
                    {
                        g_pIdleScreenUI->UpdateVoiceMailNotify();
                    }
                    if (ST_POPUP_MISSED_CALL & objMessage.lParam)
                    {
                        g_pIdleScreenUI->UpdateMissCallNotify();
                    }
                    if (ST_POPUP_FORWARD_CALL & objMessage.lParam)
                    {

                        g_pIdleScreenUI->UpdateCallForwardNotify();
                    }
                    if (ST_POPUP_TEXT_MSG & objMessage.lParam)
                    {
                        g_pIdleScreenUI->UpdateTextMsgNotify();
                    }
                    if (0 == objMessage.lParam) //autop发过来的0
                    {
                        g_pIdleScreenUI->UpdateTextMsgNotify();
                        g_pIdleScreenUI->UpdateCallForwardNotify();
                        g_pIdleScreenUI->UpdateMissCallNotify();
                        g_pIdleScreenUI->UpdateVoiceMailNotify();
                    }
                }
                break;
            case ST_SCREEN_SAVER_TIME:
                {
                    g_pIdleScreenUI->OnScreensaverWaitTimeChange();
                }
                break;
            case ST_SCREEN_SAVER_MODE:
                {
                    //在idle时如果屏保定时器没运行则直接进屏保。
                    g_pIdleScreenUI->ProcessScreensaverTimeOut();
                }
                break;
#if IF_FEATURE_DND_LARGE_ICON
            case ST_DND:
                {
                    g_pIdleScreenUI->RefreshStatus(idleScreen_GetStatusList());
                }
                break;
#endif
#if IF_BUG_33812
            case ST_FONALITY_LOGO:
                {
                    g_pIdleScreenUI->InitWallpaperLogo();
                    g_pIdleScreenUI->CheckWallpaperLogo();
                }
                break;
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
            case ST_XMLBROWSER_HOTELING:
                {
                    g_pIdleScreenUI->UpdateXMLBrowserHotelingStatus(objMessage.lParam);
                }
                break;
#endif
            case ST_FEATURE_SOFTKEY:
                {
                    g_pIdleScreenUI->UpdateFeatureSoftkey();
                }
                break;
            default:
                break;
            }
        }
        break;
    case ACCOUNT_DEFAULT_CHANGED:
        {
#if !IF_FEATURE_SHOW_DEFAULT_ACCOUNT
            // 刷新账号信息
            g_pIdleScreenUI->UpdateDefaultAccountLabel();
#endif
            // 刷新账号相关的xmlbrowser信息
            g_pIdleScreenUI->RefreshCustomData();
        }
        break;
    case WEB2PHONE_QUERY_DSKPHONE_RUNUP:
        {
            //objMessage.ReplyMessage(1);
        }
        break;
    case ACD_MESSAGE_STATUS_CHANGE:
    case HOTELING_MESSAGE_STATUS_CHANGE:
    case HOTELING_MESSAGE_REQ_TIMEOUT:
    case ACD_MESSAGE_QUEUE_STATUS:
#if IF_BUG_24363
    case ACD_MESSAGE_LOGIN_TIMEOUT:
#endif
    case ACD_MESSAGE_STATUS_UPDATE:
        {
            return g_pIdleScreenUI->OnACDHotelingMsg(objMessage);
        }
        break;
    case KEYGUARD_LOCK_CHANGED:
        {
            if ((bool)objMessage.wParam)
            {
                idleScreen_SetStatus(PS_STATE_LOCK, "");
                // 上锁给予提示,CKeyGuardHelper::OnMessage中做
            }
            else
            {
                idleScreen_DropStatus(PS_STATE_LOCK);

                // 解锁则需要重新开始计时
                g_pIdleScreenUI->ResetKeypadLock();
            }
        }
        break;
    case KEYGUARD_ENABLE_CHANGED:
    case KEYGUARD_AUTOLOCK_TIME_CHANGED:
        {
            g_pIdleScreenUI->ResetKeypadLock();
        }
        break;
    case DISPLAY_CURRENT_WALLPAPER_CHANGED:
        {
            Image_ClearImageByImageType(DIT_NOTTHEME);
#if IF_FEATURE_WALLPAPER_MODE
            g_pIdleScreenUI->CheckWallpaperChange();
#endif
            if (UIManager_IsInIdle())
            {
                _MainWND.UpdateBackGround();
            }
        }
        break;
    case MULTIFWD_ENABLED_NOTICE:
        {
            char * pKeys = (char *)objMessage.GetExtraData();
            char szTemp[MAX_BUF_LEN] = {0};
            memcpy(szTemp, pKeys, objMessage.GetExtraSize());
            szTemp[MAX_BUF_LEN - 1] = '\0';
            QString fwdTarget = toQString(szTemp);

            QString strTip = LANG_TRANSLATE(TRID_ALWAYS_FORWARD_TO) + " " + fwdTarget;
            //弹出保存成功的提示框
            MessageBox_ShowCommonMsgBox(NULL, strTip, MESSAGEBOX_NOTE, 1000);
        }
        break;
    case COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG:
        {
            g_pIdleScreenUI->ProcessFlexibleMsg(objMessage);
        }
        break;
#ifdef IF_FEATURE_GETBALANCE
    case GET_BALANCE_MESSAGE_GET_RESULT:
    case GET_BALANCE_MESSAGE_LOADING:
    case GET_BALANCE_MESSAGE_LOAD_ERROR:
    case GET_BALANCE_MESSAGE_CLEAN_TIPS:
        {
            g_pIdleScreenUI->ProcessBalanceMsg(QVariant::fromValue(objMessage.message));
        }
        break;
#endif
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case RECORD_STORAGE_STATE_CHANGE:
        {
            if (1 == objMessage.wParam)
            {
                // 显示连接上
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTING);
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTFAILED);
                idleScreen_SetStatus(PS_STATE_USBDEVICE);

                if (!AdapterBox_IsTestMode())
                {
                    yl::string strNote = fromQString(LANG_TRANSLATE(TRID_USB_DEVICE_CONNECTED));
                    if (0 != configParser_GetConfigInt(kszUSBRecordEnable)
                            && talklogic_SessionExist())
                    {
                        strNote += "\n";
                        strNote += fromQString(LANG_TRANSLATE(TRID_RECORD_TIME));
                        strNote += ":";
                        yl::string strTime = modRecord_GetAvailableRecordingTime();
                        strNote.append(strTime.c_str());
                    }

                    MessageBox_ShowCommonMsgBox(NULL, toQString(strNote), MESSAGEBOX_NOTE, 2000);
                }
            }
            else if (2 == objMessage.wParam)
            {
                // 显示正在检测
                idleScreen_DropStatus(PS_STATE_USBDEVICE);
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTFAILED);
                idleScreen_SetStatus(PS_STATE_USBDEVICE_DETECTING);
            }
            else if (3 == objMessage.wParam)
            {
                // 显示无法识别
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTING);
                idleScreen_DropStatus(PS_STATE_USBDEVICE);
                idleScreen_SetStatus(PS_STATE_USBDEVICE_DETECTFAILED);

                if (!AdapterBox_IsTestMode())
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_USB_DEVICE_CONNECT_FAIL), MESSAGEBOX_NOTE,
                                                2000);
                }
            }
            else if (4 == objMessage.wParam)
            {
                // 识别出是其他非u盘USB设备，如蓝牙，wifi的dongle
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTING);
                idleScreen_DropStatus(PS_STATE_USBDEVICE);
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTFAILED);
            }
            else
            {
                // 取消显示图标
                idleScreen_DropStatus(PS_STATE_USBDEVICE);
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTING);
                idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTFAILED);

                if (!AdapterBox_IsTestMode()
                        && STATE_CONNECT == objMessage.lParam)
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_USB_DEVICE_REMOVED), MESSAGEBOX_NOTE, 2000);
                }
            }
        }
        break;
    case RECORD_STORAGE_RECORD_RESULT:
        {
            if (objMessage.wParam == 1
                    && objMessage.lParam == 1)
            {
                MessageBox_ShowCommonMsgBox(NULL,
                                            LANG_TRANSLATE(TRID_THE_RECORDINGS_HAVE_BEEN_PRESERVED_IN_THE_USB_DEVICE), MESSAGEBOX_NOTE, 2000);
                //idleScreen_SetStatus(PS_STATE_SAVE_RECORD_FILE);
            }
            else if (objMessage.wParam == 0
                     && objMessage.lParam == 0)
            {
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_USB_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
            }
        }
        break;
    case DEV_MSG_EXP_KEYDOWN:
        {
            g_pIdleScreenUI->StartScreensaverTimer();
        }
        break;
#endif
    case BLF_MESSAGE_FLUSH:
        {
            if (configParser_GetConfigInt(kszBLFActiveBackLight) == 1)
            {
                g_pIdleScreenUI->StartScreensaverTimer();
            }
        }
        break;
    case BLUETOOTH_MSG_CONNECT_STATE:
        {
            if (BT_PHONE == objMessage.lParam && 0 == objMessage.wParam)
            {
                idleScreen_DropStatus(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY);
            }
        }
        break;
    case AUTOP_AUTOP_STATUS:
        {
            int nWaitTimes = 0;

            switch (objMessage.wParam)
            {
            case AUT_STATUS_UPDATING:
                {
                    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_FAILED);
                    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS);

                    idleScreen_SetStatus(PS_STATE_AUTO_PROVISION_UPDATING);
                }
                break;
            case AUT_STATUS_SUCCESS:
                {
                    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATING);
                    idleScreen_SetStatus(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS);

                    nWaitTimes = AUTOP_RESULT_HINT_SHOW_TIME;
                }
                break;
            case AUT_STATUS_FAILED:
                {
                    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATING);
                    idleScreen_SetStatus(PS_STATE_AUTO_PROVISION_UPDATE_FAILED);

                    nWaitTimes = AUTOP_RESULT_HINT_SHOW_TIME;
                }
                break;
            case AUT_STATUS_AUTH:
                {
                    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATING);
                    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_FAILED);
                    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS);

                    AdapterBox_ZeroOpenDlg(ZERO_DLG_AUTOP_AUTHENTICATION);
                }
            }

            g_pIdleScreenUI->ResetAutoPWaitTime(nWaitTimes);
        }
        break;

#if IF_FEATURE_FAVORITE_DIR
    case IDLE_MSG_NUMBER_EMPTY:
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_PHONE_NUMBER_IS_EMPTY), MESSAGEBOX_NOTE, 1500);

        }
        break;
#endif

    default:
        break;
    }
    return 0;
}

#if IF_FEATURE_DND_LARGE_ICON
void IdleScreen_StatusNotifyCallBackFunction(const YLList<IdleStatusItem> & listStatusList)
{
    g_pIdleScreenUI->RefreshStatus(listStatusList);
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
CDlgIdleScreen * CDlgIdleScreen::m_pToMyself = NULL;

CDlgIdleScreen::CDlgIdleScreen()
    : m_bIsTimeOut(false)
    , m_pXMLFrame(NULL)
    , m_pNoteLabel(NULL)
#if !IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    , m_pLabDefaultAccount(NULL)
#endif
    , m_pLabACDQueStat(NULL)
    , m_bIdleDsskey(false)
    , m_pIdleDsskey(NULL)
    , m_bXMLActive(false)
    , m_bIsPressed(false)
    , m_nScreenSaverWaitTime(ENTER_SCREEN_MIN_TIME)
    , m_pmBG(PHONE_WIDTH, PHONE_HEIGHT)
#if IF_BUG_33812
    , m_pWallPaperLogo(NULL)
#endif
{
    //设置为顶层窗口
    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_IDLE);

    //设置不返回Idle
    SetRejectReturnToIdle();

    // 初始化数据
    InitData();

    // softkey初始化为托盘图标类型
    SetSoftkeyType(STYLE_TYPE_ICONTRAY);

    UIManager_InstallTransChangeSignalSlot(this, SLOT(OnTransparencyChange()));
}

CDlgIdleScreen::~CDlgIdleScreen()
{
    m_timerPound.stop();
    m_timerOK.stop();
    m_timerCancel.stop();


    etl_UnregisterMsgHandle(MULTIFWD_ENABLED_NOTICE, MULTIFWD_ENABLED_NOTICE, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(DISPLAY_CURRENT_WALLPAPER_CHANGED, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                            OnIdleScreenMsg);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(WEB2PHONE_QUERY_DSKPHONE_RUNUP, WEB2PHONE_QUERY_DSKPHONE_RUNUP,
                            OnIdleScreenMsg);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                            OnIdleScreenMsg);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_REQ_TIMEOUT, HOTELING_MESSAGE_REQ_TIMEOUT,
                            OnIdleScreenMsg);
    etl_UnregisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(KEYGUARD_AUTOLOCK_TIME_CHANGED, KEYGUARD_AUTOLOCK_TIME_CHANGED,
                            OnIdleScreenMsg);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE, OnIdleScreenMsg);
#if IF_BUG_24363
    etl_UnregisterMsgHandle(ACD_MESSAGE_LOGIN_TIMEOUT, ACD_MESSAGE_LOGIN_TIMEOUT, OnIdleScreenMsg);
#endif
    etl_UnregisterMsgHandle(COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG,
                            COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG, OnIdleScreenMsg);

#ifdef IF_FEATURE_GETBALANCE
    etl_UnregisterMsgHandle(GET_BALANCE_MESSAGE_BEGIN, GET_BALANCE_MESSAGE_END, OnIdleScreenMsg);
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(RECORD_STORAGE_RECORD_RESULT, RECORD_STORAGE_RECORD_RESULT,
                            OnIdleScreenMsg);
#endif
    etl_UnregisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(BLF_MESSAGE_FLUSH, BLF_MESSAGE_FLUSH, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(AUTOP_AUTOP_STATUS, AUTOP_AUTOP_STATUS, OnIdleScreenMsg);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, OnIdleScreenMsg);

    UIManager_UnInstallTransChangeSignalSlot(this, SLOT(OnTransparencyChange()));

#if IF_FEATURE_FAVORITE_DIR
    etl_UnregisterMsgHandle(IDLE_MSG_NUMBER_EMPTY, IDLE_MSG_NUMBER_EMPTY, OnIdleScreenMsg);
#endif

}

void CDlgIdleScreen::Init()
{
    if (_MainWND.GetVolumeBar() != NULL)
    {
        connect(_MainWND.GetVolumeBar(), SIGNAL(visibleChanged(bool)), this,
                SLOT(OnVolumeBarVisibleChanged(bool)));
        connect(_MainWND.GetVolumeBar(), SIGNAL(valueChanged(int)), this,
                SLOT(OnVolumeBarValueChanged(int)));
    }

    GetScreensaverWaitTime();
}

void CDlgIdleScreen::Uninit()
{
    m_timerOK.stop();
    m_timerCancel.stop();
    m_timerPound.stop();

    if (_MainWND.GetVolumeBar() != NULL)
    {
        disconnect(_MainWND.GetVolumeBar(), SIGNAL(valueChanged(int)), this,
                   SLOT(OnVolumeBarValueChanged(int)));
        disconnect(_MainWND.GetVolumeBar(), SIGNAL(visibleChanged(bool)), this,
                   SLOT(OnVolumeBarVisibleChanged(bool)));
        _MainWND.SetVolumeBarVisible(false);
    }
}

void CDlgIdleScreen::InitData()
{
    // 初始化计时器
    connect(&m_timerOK, SIGNAL(timeout()), this, SLOT(OnRecoverLongPress()));
    connect(&m_timerCancel, SIGNAL(timeout()), this, SLOT(OnCancelLongPress()));
    connect(&m_timerPound, SIGNAL(timeout()), this, SLOT(OnPoundLongPress()));
    connect(&m_timerRebootWeb, SIGNAL(timeout()), this, SLOT(OnRebootWeb()));
    connect(&m_timerScreensaver, SIGNAL(timeout()), this, SLOT(OnEnterScreensaver()));
    connect(&m_timerAutoPHint, SIGNAL(timeout()), this, SLOT(OnAutoPHintTimeout()));
    connect(&m_timerUpdateConfig, SIGNAL(timeout()), this, SLOT(OnUpdateConfigTimeout()));

    // 安装过滤器
    this->installEventFilter(this);

    // 注册消息
    etl_RegisterMsgHandle(MULTIFWD_ENABLED_NOTICE, MULTIFWD_ENABLED_NOTICE, OnIdleScreenMsg);
    etl_RegisterMsgHandle(DISPLAY_CURRENT_WALLPAPER_CHANGED, DISPLAY_CURRENT_WALLPAPER_CHANGED,
                          OnIdleScreenMsg);

    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED, OnIdleScreenMsg);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnIdleScreenMsg);
    etl_RegisterMsgHandle(WEB2PHONE_QUERY_DSKPHONE_RUNUP, WEB2PHONE_QUERY_DSKPHONE_RUNUP,
                          OnIdleScreenMsg);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE, OnIdleScreenMsg);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                          OnIdleScreenMsg);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_REQ_TIMEOUT, HOTELING_MESSAGE_REQ_TIMEOUT, OnIdleScreenMsg);
    etl_RegisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS, OnIdleScreenMsg);
    etl_RegisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED, OnIdleScreenMsg);
    etl_RegisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED, OnIdleScreenMsg);
    etl_RegisterMsgHandle(KEYGUARD_AUTOLOCK_TIME_CHANGED, KEYGUARD_AUTOLOCK_TIME_CHANGED,
                          OnIdleScreenMsg);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE, OnIdleScreenMsg);

#if IF_BUG_24363
    etl_RegisterMsgHandle(ACD_MESSAGE_LOGIN_TIMEOUT, ACD_MESSAGE_LOGIN_TIMEOUT, OnIdleScreenMsg);
#endif

    etl_RegisterMsgHandle(COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG,
                          COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG, OnIdleScreenMsg);

#ifdef IF_FEATURE_GETBALANCE
    etl_RegisterMsgHandle(GET_BALANCE_MESSAGE_BEGIN, GET_BALANCE_MESSAGE_END, OnIdleScreenMsg);
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE, OnIdleScreenMsg);
    etl_RegisterMsgHandle(RECORD_STORAGE_RECORD_RESULT, RECORD_STORAGE_RECORD_RESULT, OnIdleScreenMsg);
#endif

    etl_RegisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, OnIdleScreenMsg);
    etl_RegisterMsgHandle(BLF_MESSAGE_FLUSH, BLF_MESSAGE_FLUSH, OnIdleScreenMsg);
    etl_RegisterMsgHandle(AUTOP_AUTOP_STATUS, AUTOP_AUTOP_STATUS, OnIdleScreenMsg);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, OnIdleScreenMsg);

#if IF_FEATURE_FAVORITE_DIR
    etl_RegisterMsgHandle(IDLE_MSG_NUMBER_EMPTY, IDLE_MSG_NUMBER_EMPTY, OnIdleScreenMsg);
#endif

    //初始化Idle Dsskey按钮
    InitDsskeyFrame();

    // 初始化控件

    m_pXMLFrame = new CXMLBrowserUIFrame(this);
    if (NULL != m_pXMLFrame)
    {
        m_pXMLFrame->setGeometry(RECT_XMLBrowser);
        m_pXMLFrame->hide();
    }

    m_pNoteLabel = new CNoteLabel(this);
    if (m_pNoteLabel != NULL)
    {
        SetWgtFontColor(m_pNoteLabel, FONT_TYPE_TEXT, Qt::white);
        m_pNoteLabel->setGeometry(RECT_ACD_NOTE);
        m_pNoteLabel->setAlignment(Qt::AlignCenter);

        // 设置字体大小
        CustomTheme_SetWidgetFontProperty(m_pNoteLabel, CUSTOM_FONT_ID_IDLE_LABEL, true, QFont::Bold);
    }

#if !IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    m_pLabDefaultAccount = new QLabel(this);
    if (m_pLabDefaultAccount != NULL)
    {
        SetWgtFontColor(m_pLabDefaultAccount, FONT_TYPE_TEXT, Qt::white);
        m_pLabDefaultAccount->setGeometry(RECT_DEFAULT_ACCOUNT);
        m_pLabDefaultAccount->setAlignment(Qt::AlignCenter);

        // 设置字体大小
        CustomTheme_SetWidgetFontProperty(m_pLabDefaultAccount, CUSTOM_FONT_ID_IDLE_LABEL, true,
                                          QFont::Bold);
        configParser_GetConfigInt(kszEnableIdleDefaultAccount) == 1
        ? m_pLabDefaultAccount->show() : m_pLabDefaultAccount->hide();
    }
#endif

    m_pLabACDQueStat = new QLabel(this);
    if (NULL != m_pLabACDQueStat)
    {
        SetWgtFontColor(m_pLabACDQueStat, FONT_TYPE_TEXT, Qt::white);
        m_pLabACDQueStat->setText("");
        m_pLabACDQueStat->setGeometry(RECT_ACD_QUESTAT);
        m_pLabACDQueStat->setAlignment(Qt::AlignCenter);
        SetACDQueStatActivate(false);
        // 设置字体大小
        CustomTheme_SetWidgetFontProperty(m_pLabACDQueStat, CUSTOM_FONT_ID_IDLE_ACD_STATUS, true,
                                          QFont::Bold);
    }

#if IF_FEATURE_DND_LARGE_ICON
    //DND 大图标
    m_pDNDIcon = new QLabel(this);
    m_pDNDLabel = new QLabel(this);
    if (m_pDNDIcon != NULL && m_pDNDLabel != NULL)
    {
        m_pDNDIcon->setGeometry(QRect(51, 97, 152, 152));
        m_pDNDIcon->setScaledContents(true);
        m_pDNDIcon->setPixmap(THEME_GET_BMP(PIC_ICON_DND_LARGE));
        m_pDNDIcon->lower();
        m_pDNDIcon->hide();

        // 高度使用152，实际会偏向下，不完全居中，因此改成146
        m_pDNDLabel->setGeometry(QRect(229, 97, 228, 146));
        /*QPalette pal;
        pal.setColor(QPalette::WindowText, qRgb(236, 49, 49));
        QFont font;
        font.setPointSize(100);
        font.setBold(true);

        m_pDNDLabel->setPalette(pal);
        m_pDNDLabel->setFont(font);*/
        CustomTheme_SetWidgetFontProperty(m_pDNDLabel, CUSTOM_FONT_ID_IDLE_DND, true);
        m_pDNDLabel->setText(TRID_DND);
        m_pDNDLabel->lower();
        m_pDNDLabel->hide();

        //注册状态回调函数
        idleScreen_SetNotifyFunByCount(IdleScreen_StatusNotifyCallBackFunction, -1);
    }
#endif

#if IF_BUG_33812
    InitWallpaperLogo();
#endif

    m_listCustomData.clear();

#if IF_FEATURE_WALLPAPER_MODE
    m_eWallpaperMode = (WALLPAPAER_MODE)configParser_GetConfigInt(kszWallpaperMode);
    yl::string strBackgroundColor = configParser_GetConfigString(kszWallpaperBackgroundColor);
    SetBackgroundColor(strBackgroundColor);
#endif

#if IF_FEATURE_XMLBROWSER_HOTELING
    connect(&m_timerHotelingLogoutPrompt, SIGNAL(timeout()), this, SLOT(OnHotelingLogoutPrompt()));
    ResetHotelingLogoutPromptTime();
#endif
}

#if IF_FEATURE_DND_LARGE_ICON
// 刷新状态
void CDlgIdleScreen::RefreshStatus(const YLList<IdleStatusItem> & listStatusItem)
{
    m_bShowDND = false;
    for (YLList<IdleStatusItem>::const_iterator iter = listStatusItem.begin();
            iter != listStatusItem.end(); ++ iter)
    {
        const IdleStatusItem& StatusItem = *iter;
        PhoneState ePhoneState = (PhoneState)StatusItem.m_nId;
        if (ePhoneState == PS_STATE_DND)
        {
            if (configParser_GetConfigInt(kszShowDNDLargeIcon) == 1)
            {
                m_bShowDND = true;
            }
            break;
        }
    }

    CheckDNDStatus();
}

void CDlgIdleScreen::CheckDNDStatus()
{
    if (m_bShowDND)
    {
        if (m_bIdleDsskey)
        {
            OnDssKeyPageAction();
        }
        m_pDNDIcon->show();
        m_pDNDLabel->show();
    }
    else
    {
        m_pDNDIcon->hide();
        m_pDNDLabel->hide();
    }
}
#endif

//初始化Dsskey按钮
void CDlgIdleScreen::InitDsskeyFrame()
{
    // Dsskey部分初始化
    m_pIdleDsskey = new CIdleDsskeyFrame(this);

    int i = 0;
    qDsskeyButton * pDsskeyBtn = NULL;
    qUIPushButton * pPushBtn = NULL;
    CAbstractDsskeyBtnAction * pAction = NULL;
    //
    if (m_pIdleDsskey != NULL)
    {
        m_pIdleDsskey->setGeometry(RECT_IDLE_DSSKEYFRAME);
        m_pIdleDsskey->setObjectName("IdleDsskey");
        // 设置按钮
        for (i = 0; i < 18; ++i)
        {
            pDsskeyBtn = new qDsskeyButton(i + 11, m_pIdleDsskey);
            if (pDsskeyBtn != NULL)
            {
                pDsskeyBtn->setObjectName(QString("DsskeyBtnCenter%1").arg(i));
                m_pIdleDsskey->AddDsskeyBtn(pDsskeyBtn);
                if (i < 6)
                {
                    // 第1排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_IDLE_LEFT_OFFSET_L,
                                            DSSKEY_IDLE_TOP_OFFSET + i * DSSKEY_IDLE_V_DISTANCE, DSSKEY_IDLE_WIDTH, DSSKEY_IDLE_HEIGHT);
                }
                else if (i < 12)
                {
                    // 第2排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_IDLE_LEFT_OFFSET_L + DSSKEY_IDLE_H_DISTANCE,
                                            DSSKEY_IDLE_TOP_OFFSET + (i - 6) * DSSKEY_IDLE_V_DISTANCE, DSSKEY_IDLE_WIDTH, DSSKEY_IDLE_HEIGHT);
                }
                else
                {
                    // 第3排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_IDLE_LEFT_OFFSET_L + 2 * DSSKEY_IDLE_H_DISTANCE,
                                            DSSKEY_IDLE_TOP_OFFSET + (i - 12) * DSSKEY_IDLE_V_DISTANCE, DSSKEY_IDLE_WIDTH, DSSKEY_IDLE_HEIGHT);
                }

                pDsskeyBtn->setIconPicAlign(Qt::AlignLeft);
                pDsskeyBtn->setTextColor(Qt::black);
            }
            else
            {
                IDLE_WARN("qDsskeyButton is NULL!.\n");
            }
        }

        // 设置按钮动作
        // 设置点击按钮动作
        pAction = new CDsskeyFeatureAction;
        if (pAction != NULL)
        {
            m_pIdleDsskey->SetBtnClickAction(pAction);
        }
        else
        {
            MAINWND_WARN("pClickAction is NULL!.\n");
        }
        // 设置长按按钮动作
        pAction = NULL;
        pAction = new CDsskeyLongClickAction;
        if (pAction != NULL)
        {
            m_pIdleDsskey->SetBtnLongPressAction(pAction);
        }
        else
        {
            MAINWND_WARN("pLongAction is NULL!.\n");
        }
    }
    else
    {
        IDLE_WARN("m_pIdleDsskey is NULL!.\n");
    }

    m_pIdleDsskey->hide();


}

#if !IF_FEATURE_SHOW_DEFAULT_ACCOUNT
void CDlgIdleScreen::UpdateDefaultAccountLabel()
{
    if (m_pLabDefaultAccount != NULL)
    {
        yl::string strLabel = acc_GetLabel(acc_GetDefaultAccount());
        m_pLabDefaultAccount->setText(!strLabel.empty() ? toQString(strLabel.c_str()) : LANG_TRANSLATE(
                                          TRID_NO_SERVICE));

        //处理重叠控件的显示
        CheckNoteLabAndAccountLab();
    }
}
#endif

void CDlgIdleScreen::CheckNoteLabAndAccountLab()
{
#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    if (m_pNoteLabel == NULL)
    {
        return;
    }
    //ACD优先级高于DefaultAccount
    if (!(m_pNoteLabel->GetLabelText().isEmpty()))
    {
        m_pNoteLabel->show();
    }
    else
    {
        m_pNoteLabel->hide();
    }
#else
    if (m_pNoteLabel == NULL || m_pLabDefaultAccount == NULL)
    {
        return;
    }
    //ACD优先级高于DefaultAccount
    if (!(m_pNoteLabel->GetLabelText().isEmpty()))
    {
        m_pNoteLabel->show();
        m_pLabDefaultAccount->hide();
    }
    else
    {
        m_pNoteLabel->hide();
        if (configParser_GetConfigInt(kszEnableIdleDefaultAccount) == 1)
        {
            m_pLabDefaultAccount->show();
        }
    }
#endif
}

void CDlgIdleScreen::GetScreensaverWaitTime()
{
    int nScreenWaitTimes = configParser_GetConfigInt(kszScreenSaverWaitTime);

    if (nScreenWaitTimes <= ENTER_SCREEN_MIN_TIME)
    {
        m_nScreenSaverWaitTime = ENTER_SCREEN_MIN_TIME;
    }
    else
    {
        m_nScreenSaverWaitTime = nScreenWaitTimes;
    }

    // 换算成毫秒
    m_nScreenSaverWaitTime *= 1000;
}

yl::string CDlgIdleScreen::GetCurrentBackground()
{
    // 如果背景图片未变化则不更新
    yl::string strBkTmp = "Default:Default.png";

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
#if IF_CUSTOMER_SUPPORT_98_NFON//IF_FEATURE_DND_LARGE_ICON
    if (m_bShowDND)
    {
        strBkTmp = g_pDisplayController->GetDefaultWallpaper();
    }
    else
#endif
        if (m_bIdleDsskey)
        {
            strBkTmp = g_pDisplayController->GetDsskeyWallpaper();
            if (strcasecmp(strBkTmp.c_str(), "auto") == 0)
            {
                strBkTmp = g_pDisplayController->GetCurrentWallpaper();
            }
        }
        else
        {
            strBkTmp = g_pDisplayController->GetCurrentWallpaper();
        }
#else
    strBkTmp = g_pDisplayController->GetCurrentWallpaper();
#endif

    return strBkTmp;
}

void CDlgIdleScreen::DrawDsskeyBtnOnBackground()
{
#if IF_FEATURE_WALLPAPER_MODE
    m_pmBG.fill(m_BackgroundColor);
    if (m_pmBG.size().width() != PHONE_WIDTH
            || m_pmBG.size().height() != PHONE_HEIGHT)
    {
        m_pmBG = m_pmBG.scaled(PHONE_WIDTH, PHONE_HEIGHT);
    }

    QPixmap pBackGround;
    if (!m_strBGPicPath.empty())
    {
        pBackGround = STOCK_GET_BMP(m_strBGPicPath.c_str());
    }

    if (pBackGround.isNull())
    {
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
        if (m_bIdleDsskey)
        {
            m_strBGPicPath = g_pDisplayController->GetCurrentWallpaper();
            pBackGround = STOCK_GET_BMP(m_strBGPicPath.c_str());
        }

        if (pBackGround.isNull())
        {
            pBackGround = THEME_GET_BMP(PIC_THEME1_BACKGROUND);
        }
#else
        pBackGround = THEME_GET_BMP(PIC_THEME1_BACKGROUND);
#endif
    }

    if (pBackGround.size().width() == PHONE_WIDTH
            && pBackGround.size().height() == PHONE_HEIGHT)
    {
        m_pmBG = pBackGround;
    }
    else
    {
        DrawWallpaper(pBackGround);
    }
#else
    if (!m_strBGPicPath.empty())
    {
        m_pmBG = STOCK_GET_BMP(m_strBGPicPath.c_str());
    }

    if (m_pmBG.isNull())
    {
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
        if (m_bIdleDsskey)
        {
            m_strBGPicPath = g_pDisplayController->GetCurrentWallpaper();
            m_pmBG = STOCK_GET_BMP(m_strBGPicPath.c_str());
        }

        if (m_pmBG.isNull())
        {
            m_pmBG = THEME_GET_BMP(PIC_THEME1_BACKGROUND);
        }
#else
        m_pmBG = THEME_GET_BMP(PIC_THEME1_BACKGROUND);
#endif
    }

    if (m_pmBG.size().width() != PHONE_WIDTH
            || m_pmBG.size().height() != PHONE_HEIGHT)
    {
        IDLEUI_INFO("IDLE BackGround width[%d], height[%d]",
                    m_pmBG.size().width(), m_pmBG.size().height());

        m_pmBG = m_pmBG.scaled(PHONE_WIDTH, PHONE_HEIGHT);
    }
#endif
    QPainter painter(&m_pmBG);

    // 绘制Title背景
    QPixmap pmTitleBg = THEME_GET_BMP(g_TransWidgetBgControler.GetTitleBarBg().c_str());
    painter.drawPixmap(QRect(0, 0, PHONE_WIDTH, TITLE_HEIGHT), pmTitleBg);

    // 绘制Dsskey图标
    QPixmap pmBtnBg = THEME_GET_BMP(g_TransWidgetBgControler.GetNormalDsskeyBg().c_str());

    int i = 0;
    for (i = 0 ; i < 6; ++i)
    {
        painter.drawPixmap(0, 50 + 57 * i, pmBtnBg);
    }

    for (i = 0; i < 6; ++i)
    {
        painter.drawPixmap(644, 50 + 57 * i, pmBtnBg);
    }
}

bool CDlgIdleScreen::IsShowTitle(TitleLayoutInfo & objInfo)
{
    objInfo.eMidType = TITLE_MODE_STATUS_BAR;
    return true;
}

void CDlgIdleScreen::OnFocusChanged(bool bFocused)
{
    if (bFocused)
    {
        DM_ExitDirectory();
    }
    else
    {
        if (m_timerOK.isActive())
        {
            m_timerOK.stop();
        }

        if (m_timerCancel.isActive())
        {
            m_timerCancel.stop();
        }
    }
}

void CDlgIdleScreen::UpdateXMLBrowser(const XMLLogicToUIData & objData)
{
    if (NULL != m_pXMLFrame)
    {
        m_pXMLFrame->UpdateUI(objData);
        if (m_bIdleDsskey)
        {
            HideWidgets();
            return;
        }
    }
}

// XMLBrowser
void CDlgIdleScreen::ActivateXMLBrowser(bool bActivate)
{
    m_bXMLActive = bActivate;
    checkXMLframeAndACDQueStat();
}


void CDlgIdleScreen::OnRecoverLongPress()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    //如果上锁则不允许该操作
    if (keyGuard_IsEnable() && keyGuard_IsLock())
    {
        return;
    }

    m_bIsTimeOut = true;

    m_timerOK.stop();
    MessageBox_ShowCommonMsgBox(this, /*LANG_TRANSLATE(TRID_WARNING),*/
                                LANG_TRANSLATE(TRID_IF_RESET_TO_FACTORY_SETTING), MESSAGEBOX_OkCancel, -1, "", "",
                                IDLE_MSGBOX_TYPE_OK);
    m_bIsTimeOut = false;
}

void CDlgIdleScreen::OnCancelLongPress()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    m_bIsTimeOut = true;
    m_timerCancel.stop();

    MessageBox_ShowCommonMsgBox(this,/* LANG_TRANSLATE(TRID_NOTE), */LANG_TRANSLATE(
                                    TRID_IF_REBOOT_THE_PHONE), \
                                MESSAGEBOX_OkCancel, -1, "", "", IDLE_MSGBOX_TYPE_CANCEL);
}
void CDlgIdleScreen::OnRebootWeb()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    m_bIsTimeOut = true;
    //m_showingRebootWeb = true;
    m_timerRebootWeb.stop();
    //MessageBox_ShowCommonMsgBox(this,LANG_TRANSLATE("Note"), LANG_TRANSLATE("Reboot the web process?"),MESSAGEBOX_OkCancel,-1,"","",3);
    system(PHONE_SCRIPTS_PATH "reset-httpd.sh &");
}

void CDlgIdleScreen::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    IDLEUI_INFO("CDlgIdleScreen::MessageBoxCallBack Popbox ID %d ,Result is %d.", pMessageBox->GetID(),
                pMessageBox->GetResult());
    if (pMessageBox == NULL)
    {
        return;
    }
    switch (pMessageBox->GetID())
    {
    case IDLE_MSGBOX_TYPE_CANCEL://代表长按cancel
        {
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                // 重启话机
                AdapterBox_Reboot(0);
            }
            m_bIsTimeOut = false;
        }
        break;
    case IDLE_MSGBOX_TYPE_UPDATE_CONFIG:// 更新配置功能
        {
            if (MessageBox_OK == pMessageBox->GetResult())
            {
                // 更新配置 AUTOP TO DO :
                Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 0, 1);
            }
            m_bIsTimeOut = false;
        }
        break;
    case IDLE_MSGBOX_TYPE_HOTELING_LOGOUT://hoteling 登出功能
        {
            // 响应OK键的处理
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                // ACD和HotelIng登出
                CallCenter_Logout();
            }
        }
        break;
    case IDLE_MSGBOX_TYPE_HOTELING_UNLOCK://hoteling 解锁
        {
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                CallCenter_GuestLock(false);
                UpdateSoftKeyCache();

                // 只有在Idle界面才需要立即刷新softkey
                if (UIManager_IsInIdle())
                {
                    _MainWND.DrawSoftkey(this);
                }
            }
        }
        break;
    case IDLE_MSGBOX_TYPE_OK://代表长按ok
        {
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                //恢复出厂是否需要权限
                int iNeedPwd = configParser_GetConfigInt(kszIdleResetNeedPwd);
                if (iNeedPwd == 1)//需要权限
                {
                    yl::string strAdminName = feature_UserModeGetUserName(UT_ADMIN);
                    //需要权限时询问密码
                    MessageBox_ShowPasswordBox(this, strAdminName.c_str(), 0, IDLE_MSGBOX_TYPE_PASSWORD);
                }
                else//不需要权限
                {
                    // IDLE界面的恢复出厂设置需要全部恢复
                    IDLEUI_INFO("Before AdapterBox_ResetFactory.");
                    // 恢复出厂配置
                    AdapterBox_ResetFactory(RESET_OPTION_ALL);
                }

            }
            m_bIsTimeOut = false;
        }
        break;
    case IDLE_MSGBOX_TYPE_PASSWORD: //密码框 只针对ok的弹出框
        {
            if (pMessageBox->GetResult() != MessageBox_OK)//
            {
                m_bIsTimeOut = false;
                return;
            }
            else//密码验证ok
            {
                // IDLE界面的恢复出厂设置需要全部恢复
#if IF_BUG_20555
                if (acc_LogoutBeforeReset())
#endif  //IF_BUG_20555
                {
                    // 恢复出厂配置
                    AdapterBox_ResetFactory(RESET_OPTION_ALL);
                }
                m_bIsTimeOut = false;
            }
        }
        break;
    case IDLE_MSGBOX_TYPE_NETWORKCHANGE: //网络变更重启提示
        {
            int nResult = pMessageBox->GetResult();
            switch (nResult)
            {
            case MessageBox_OK:
                {
                    //弹出重启提示后，无论点击是否，都将标记重置
                    _NetworkChangeFlagManager.ClearChangeFlag();
                    AdapterBox_Reboot(0);//点击确认，重启机器，使得网络配置生效
                }
                break;
            case MessageBox_Cancel:
                {
                    //弹出重启提示后，无论点击是否，都将标记重置
                    _NetworkChangeFlagManager.ClearChangeFlag();
                }
                break;
            default:
                break;
            }
        }
        break;
    case IDLE_MSGBOX_TYPE_BALANCE_LOADING:
#ifdef IF_FEATURE_GETBALANCE
        {
            //对正在下载余额信息的弹出框被取消进行处理
            if (pMessageBox->GetResult() == MessageBox_Cancel)
            {
                IDLEUI_INFO("CDlgIdleScreen::MessageBoxCallBack Loading Balance Canceled");
                //取消下载
                acc_CancelLoadBalance();
            }
        }
#endif
        break;
    case IDLE_MSGBOX_TYPE_DSSKEY_LOCK_PASSWORD:
        {
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                dsskey_OnDsskeyClick(pMessageBox->GetExtraData().value<int>(), 0, false);
            }
        }
        break;
    default:
        break;
    }
}

// 重置键盘锁的计时器
void CDlgIdleScreen::ResetKeypadLock()
{
    g_pKeyGuardHelper->ResetKeypadLock();
}

void CDlgIdleScreen::SetCustomData(YLList<CustomNotify> & listNotifyData)
{
    // set custom data
    m_listCustomData.clear();
    m_listCustomData = listNotifyData;

    // refresh custom data
    RefreshCustomData();
}

void CDlgIdleScreen::RefreshCustomData()
{
    yl::string strAccountInfo = "";
    int nDefAccountId = acc_GetDefaultAccount();
    if (acc_IsAccountIDValid(nDefAccountId))
    {
        strAccountInfo = commonAPI_FormatString("%s@%s",
                                                acc_GetRegisterName(nDefAccountId).c_str(),
                                                acc_GetServerName(nDefAccountId).c_str());
    }

    IDLEUI_INFO("XMLBrowser AccountInfo:%s", strAccountInfo.c_str());

    // 拼装刷新数据
    YLList<CustomNotify> listRefreshData;
    YLList<CustomNotify>::iterator it = m_listCustomData.begin();
    for (; it != m_listCustomData.end(); it++)
    {
        if ((*it).strAccount.empty()
                || (*it).strAccount == strAccountInfo)
        {
            listRefreshData.push_back(*it);
        }
    }

    // 设置激活状态
    g_pIdleScreenUI->ActivateXMLBrowser(listRefreshData.size() != 0);

    // 刷新控件
    if (listRefreshData.size() != 0)
    {
        XMLLogicToUIData objXMLData;
        objXMLData.eElementType = XMT_STATUS;
        objXMLData.nElementID = INVALID_ELEMENT_ID;
        objXMLData.pElementData = (void *)&listRefreshData;
        g_pIdleScreenUI->UpdateXMLBrowser(objXMLData);
    }
}

void CDlgIdleScreen::OnScreensaverWaitTimeChange()
{
    StopScreensaverTimer();

    GetScreensaverWaitTime();

    // 首次设置允许设置，避免一直改配置导致一直进不了屏保
    if (UIManager_IsInIdle())
    {
        StartScreensaverTimer();
    }
}

// XMLBrowser是否激活
bool CDlgIdleScreen::IsXMLBrowserActivate()
{
    return m_bXMLActive;
}

void CDlgIdleScreen::checkXMLframeAndACDQueStat()
{
    //XMLFrame优先级高于ACDQueStat
    if (m_pXMLFrame == NULL || m_pLabACDQueStat == NULL)
    {
        return;
    }

    if (m_bIdleDsskey)
    {
        HideWidgets();
        return;
    }

    if (m_bXMLActive)
    {

        m_pXMLFrame->show();
        m_pLabACDQueStat->hide();
        //m_pLabACDQueStat->clear();
#if IF_BUG_33812
        CheckWallpaperLogo(false);
#endif
    }
    else
    {
        m_pXMLFrame->hide();

        if (CallCenter_IsEnableQueueStatus()
                && AS_IDLE != ACD_GetAcdStatus())
        {
            m_pLabACDQueStat->show();
        }
#if IF_BUG_33812
        CheckWallpaperLogo(true);
#endif
    }
}

// 是否开启ACD队列信息
void CDlgIdleScreen::SetACDQueStatActivate(bool bActivate)
{
    if (NULL == m_pLabACDQueStat)
    {
        return ;
    }

    if (m_bIdleDsskey)
    {
        HideWidgets();
        return;
    }

    if (!IsXMLBrowserActivate() && bActivate && CallCenter_IsEnableQueueStatus())
    {
        m_pLabACDQueStat->show();
    }
    else
    {
        m_pLabACDQueStat->hide();
    }
}

void CDlgIdleScreen::OnPoundLongPress()
{
    // 如果不在Idle界面不进行处理
    if (!UIManager_IsInIdle())
    {
        return;
    }

    m_timerPound.stop();

    if (keyGuard_IsEnable())
    {
        keyGuard_Lock(true);
    }
}

//
bool CDlgIdleScreen::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                     QRect & rtVolume, yl::string & strBGPath) const
{
    iVolumeType = VT_RING;
    iMin = 0;

    iMax = 15;
    return true;
}

// 是否显示Dsskey，默认认为显示
bool CDlgIdleScreen::IsShowDsskey(bool & bExpend)
{
    bExpend = m_bIdleDsskey;
    return true;
}

bool CDlgIdleScreen::OnTransparencyChange()
{
    if (NULL != m_pIdleDsskey)
    {
        CIdleDsskeyFrame * pIdleDsskeyFrame = (CIdleDsskeyFrame *)m_pIdleDsskey;
        pIdleDsskeyFrame->RefreshDsskeyFrame();
    }

    DrawDsskeyBtnOnBackground();

    return true;
}

void CDlgIdleScreen::SetData(void * pData/* = NULL*/)
{
    m_eWndType = eWT_Idle;
    SetSoftkeyType(STYLE_TYPE_ICONTRAY);
    UpdateSoftKeyCache();
}

void CDlgIdleScreen::OnVolumeBarVisibleChanged(bool bVisible)
{
    if (UIManager_IsInIdle())
    {
        // http://10.2.1.199/Bug.php?BugID=108588
        if (!blf_IsBlfIdleRing())
        {
            if (bVisible)
            {
                IDLEUI_INFO("IdleScreen Start Play Default Ring");
                voice_PlayDefaultRing();
                //voice_PlayRing(GetCurrentRingPath().c_str());
            }
            else
            {
                IDLEUI_INFO("IdleScreen Stop Play Default Ring");
                voice_StopRingInMenu();
            }
        }
    }

    if (!bVisible)
    {
        //////////////////////////////
        //http://bugfree.yealink.com/Bug.php?BugID=138667
        if (!talklogic_SessionExist()
                && !UIManager_GetPrivateDlg(DLG_DlgPickupMenu)
                && !blf_IsBlfIdleRing())
        {
            voice_StopRing();
        }

        // 调节音量中, 有paging则将铃声停止
        if (talklogic_SessionExist()
                && talklogic_IsMulticastPage())
        {
            voice_StopRing();
        }
    }
}

void CDlgIdleScreen::OnVolumeBarValueChanged(int iValue)
{
    if (voice_GetCurrentChannel() == CHANNEL_RING)
    {
        if (iValue == 0)
        {
            idleScreen_SetStatus(PS_STATE_RING_MUTE, "");
        }
        else
        {
            idleScreen_DropStatus(PS_STATE_RING_MUTE);
        }
    }
}

void CDlgIdleScreen::UpdateSoftKeyCache()
{
    InitSoftKeyData();

    int nDssKeyType = DT_NA;
    yl::string strAction, strName;
    bool bNeedTranslate = false;
    for (int i = 0; i < 4; i++)
    {
        int iDsskeyID = dsskey_GetDsskeyID(DMT_PROGRAMEKEY_MODULE, 0, i);
        DssKeyLogicData DKlogicData;
        dsskey_GetData(iDsskeyID, DKlogicData);
        strName = DKlogicData.strLabel;
        nDssKeyType = DKlogicData.eDKType;
        if (strName.empty())
        {
            strName = dsskey_GetDefaultLabel(DKlogicData.eDKType);
            bNeedTranslate = true;
        }
        char temp[32] = {0};
        sprintf(temp, "Programablekey%d", i);
        m_SoftkeyData[i].m_strSoftkeyAction = temp;
        if (bNeedTranslate)
        {
            m_SoftkeyData[i].m_strSoftkeyTitle = LANG_TRANSLATE(strName.c_str());
        }
        else
        {
            m_SoftkeyData[i].m_strSoftkeyTitle = toQString(strName.c_str());
        }

        UpdateSoftKeyIcon(nDssKeyType, m_SoftkeyData[i].m_strIconPath);
        bNeedTranslate = false;
    }

    CSoftKeyBarData objSoftkeyData;
    objSoftkeyData.m_pReceiver = this;
    objSoftkeyData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));

#if IF_FEATURE_XMLBROWSER_HOTELING
    yl::string strHotelingServer = configParser_GetConfigString(kszHotelingServer);
    if (!strHotelingServer.empty())
    {
        if (configParser_GetConfigInt(kszHotelingLoginStatus) == 1)
        {
            objSoftkeyData.m_strSoftkeyAction = "XMLBrowserHotelingLogout";
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGOUT;
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_LOGOUT);
        }
        else
        {
            objSoftkeyData.m_strSoftkeyAction = "XMLBrowserHotelingLogin";
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGIN;
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_LOGIN);
        }

        //m_SoftkeyData.insert(m_SoftkeyData.begin(), objSoftkeyData);
        InsertSoftkey(1, objSoftkeyData);
        return;
    }
#endif

#if IF_FEATURE_32591
    return;
#endif

    // 开启了ACD
    if (CallCenter_IsACDOn() && 0 != configParser_GetConfigInt(kszSoftkeyIdleACDEnable))
    {
        ACD_STATUS eAcdState = ACD_GetAcdStatus();

#if IF_FEATURE_START2START_ACD
        if (Get_ACDType() == ACD_START2START)
        {
            objSoftkeyData.m_strSoftkeyAction = "AcdRefresh";
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_S2SACD_REFRESH);
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_TRACE;
            m_SoftkeyData.push_back(objSoftkeyData);

            objSoftkeyData.m_strSoftkeyAction = "AcdLoginOut";
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_S2SACD_LOGIN);
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGIN;
            //InsertSoftkeyAbsolute(1, objSoftkeyData);
            InsertSoftkey(1, objSoftkeyData);

            if (eAcdState == AS_AVAILABLE)
            {
                objSoftkeyData.m_strSoftkeyAction = "AcdUnvail";
                objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_S2SACD_UNAVAIL);
                objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_UNAVAILABLE;

                InsertSoftkey(0, objSoftkeyData);
            }
            else if (eAcdState == AS_UNAVAILABLE)
            {
                objSoftkeyData.m_strSoftkeyAction = "AcdAvail";
                objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_S2SACD_AVAIL);
                objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_AVAILABLE;

                InsertSoftkey(0, objSoftkeyData);
            }

            return;
        }
#endif
#if IF_FEATURE_METASWITCH_ACD
        if (Get_ACDType() == ACD_METASWITCH)
        {
            if (AS_IDLE != eAcdState
                    && MTSWACD_IsEnableShowState())
            {
                objSoftkeyData.m_strSoftkeyAction = "acdstate";
                objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_STATE);
                objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_MTSWACD_MYSTATE;
                InsertSoftkey(0, objSoftkeyData);
            }
            return;
        }
#endif
        if (!CallCenter_IsHotelingOn())
        {
            SetACDSoftkey();
        }
        // 两者都开的情况
        else
        {
            SetHotelingAndACDSoftkey();
        }
    }
    else if (CallCenter_IsHotelingOn()
             && configParser_GetConfigInt(kszBroadsoftActive) != 0
             && 0 != configParser_GetConfigInt(kszSoftkeyIdleHotelingEnable))
    {
        SetHotelingSoftkey();
    }

#if IF_BUG_24048_RTCP_XR
    if (0 != configParser_GetConfigInt(kszDisplayVQReportOnUI)
            && 0 != configParser_GetConfigInt(kszSessionReportSwitch))
    {
        objSoftkeyData.m_strSoftkeyAction = "RTPStatus";
        objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_RTP_STATUS);
        InsertSoftkey(m_SoftkeyData.size(), objSoftkeyData);
    }
#endif
}

void CDlgIdleScreen::UpdateSoftKeyIcon(int nDssKeyType, QString & strIcon)
{
    switch (nDssKeyType)
    {
    case DT_IDLE_CALLLIST:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_HISTORY);
        }
        break;
    case DT_DND:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DND);
        }
        break;
    case DT_IDLE_MENU:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_MENU);
        }
        break;
    case DT_INTERCOM:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_INTERCOM);
        }
        break;
    case DT_PAGING:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_PAGING);
        }
        break;
    case DT_IDLE_SWITCHACCOUNT:
    case DT_IDLE_SWITCHACCOUNT_DOWN:
    case DT_IDLE_SWITCHACCOUNT_UP:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_SWITCH_ACCOUNT);
        }
        break;
    case DT_CALLRETURN:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_CALL_RETURN);
        }
        break;
    case DT_SMS:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_SMS);
        }
        break;
    case DT_PICKUP:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTED_PICKUP);
        }
        break;
    case DT_IDLE_NEWSMS:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_NEW_SMS);
        }
        break;
    case DT_IDLE_STATUS:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_STATUS);
        }
        break;
    case DT_FWD:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_FORWARD);
        }
        break;
    case DT_GROUPPICKUP:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_GROUP_PICKUP);
        }
        break;
#if IF_BUG_25055
    case DT_BSFT_RETRIEVE_PARK:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_RETRIEVE);
        }
        break;
#endif
    case DT_SPEEDDIAL:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_SPEED_DIAL);
        }
        break;
    case DT_XMLGROUP:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_IDLE_REMOTE:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_IDLE_CONTACTS:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_LOCALDIR:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_LOCALGROUP:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_BSGROUP:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
#if IF_BUG_PLCMDIR
    case DT_PLCM_PHONEBOOK:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
#endif
    case DT_BSDIR:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_XMLBROWSER:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_XML_BROWSER);
        }
        break;
    case DT_KEYPAD_LOCK:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_KEYPAD_LOCK);
        }
        break;
    case DT_IDLE_NETWORKCALLLOG:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_HISTORY);
        }
        break;
    case DT_LDAP:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_PREFIX:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_PTT);
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_MEET_ME
    case DT_MEET_ME_CONFERENCE:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_MEET_ME_CONFERENCE);
        }
        break;
#endif
    case DT_MY_STATUS:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_MYSTATUS);
        }
        break;
    case DT_BUDDIES:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;

    case DT_FAVORITES:
    case DT_NETWORK_FAVORITE:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
    case DT_HOTDESKING:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_HOST_DESKING);
        }
        break;
    case DT_ZERO:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_ZERO);
        }
        break;
    case DT_PAGING_LIST:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_PAGINGLIST);
        }
        break;
    case DT_CUSTOM_KEY:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_EDK);
        }
        break;
    case DT_REDIAL:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_TALK_REDIAL);
        }
#if IF_FEATURE_METASWITCH
    case DT_MTSW_LOGIN:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_ACD_LOGIN);
        }
        break;
    case DT_MTSW_LOGOUT:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_ACD_LOGOUT);
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DT_MTSWDIR:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_DIRECTORY);
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLPARK
    case DT_MTSW_CALLPARK:
        {
            strIcon = QLatin1String(PIC_WIDGET_CALL_PARK);
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
    case DT_MESSAGE_LIST:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_SMS);
        }
        break;
#endif
    case DT_BSFT_RETRIEVE_PARK:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_IDLE_RETRIEVE);
        }
        break;
    case DT_PRIVATEHOLD:
        {
            strIcon = QLatin1String(PIC_SOFTKEY_PRIVATE_HOLD);
        }
        break;
    default:
        break;
    }
}

// 插入Softkey
bool CDlgIdleScreen::InsertSoftkey(int iIndex, const CSoftKeyBarData & softKeyData)
{
    if (iIndex > m_SoftkeyData.size())
    {
        m_SoftkeyData.push_back(softKeyData);
    }
    // 插入key
    else if (m_SoftkeyData[iIndex].m_strSoftkeyAction != softKeyData.m_strSoftkeyAction)
    {
        m_SoftkeyData.push_back(m_SoftkeyData[iIndex]);
        m_SoftkeyData[iIndex] = softKeyData;
    }

    return true;
}

bool CDlgIdleScreen::OnAction(const QString & strAction)
{
    IDLEUI_INFO("strAction = %s", strAction.toUtf8().data());
    int iDsskeyID = -1;
    if (strAction == "Programablekey0")
    {
        iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(PHONE_KEY_FN1);
        dsskey_OnDsskeyClick(iDsskeyID);
    }
    else if (strAction == "Programablekey1")
    {
        iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(PHONE_KEY_FN2);
        dsskey_OnDsskeyClick(iDsskeyID);
    }
    else if (strAction == "Programablekey2")
    {
        iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(PHONE_KEY_FN3);
        dsskey_OnDsskeyClick(iDsskeyID);
    }
    else if (strAction == "Programablekey3")
    {
        iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(PHONE_KEY_FN4);
        dsskey_OnDsskeyClick(iDsskeyID);
    }
    else if (strAction == "AcdLogin")
    {
        IDLEUI_INFO("CallCenter_IsHotelingOn() = %d", CallCenter_IsHotelingOn());
        if (CallCenter_IsHotelingOn())
        {
            //开启了hoteling，进行登录界面
            SettingUI_OpenPage("hoteling_login");
        }
        //如果没有开启hoteling直接登录
        else
        {
            ACD_TYPE eType = Get_ACDType();
            if (ACD_BROADSOFT == eType)
            {
                //未登录按键登陆
                ACD_ReqLogin();
            }
            else if (ACD_COSMOCOM == eType
                     || ACD_GENESYS == eType)
            {
                //进入登录改变界面
                SettingUI_OpenPage("hoteling_login");
            }
        }
    }
    else if (strAction == "AcdAvail")
    {
        ACD_ReqChangeACDStatus(AA_AVAIL);
    }
    else if (strAction == "AcdUnvail")
    {
        if (CallCenter_IsUnavailReasonCodeOn())
        {
            SettingUI_OpenPage("unavailable_reason");
        }
        else
        {
            //设置ACD为Unavailable
            ACD_ReqChangeACDStatus(AA_UNAVAIL);
        }
    }
    else if (strAction == "AcdLogOut")
    {
        // 产品部新需求：Hoteling或FlexibleSeating有开, 退出时都需要弹窗
        if (CallCenter_IsHotelingOn() || CallCenter_IsFlexibleSeatingOn())
        {
            idleScreen_SetStatus(PS_STATE_HOTELING_GUESTOUT);
        }
        else
        {
            // ACD登出
            CallCenter_Logout();
        }
    }
    else if (strAction == "AcdDispCode")
    {
        if (CallCenter_IsDispCodeOn()
                && configParser_GetConfigInt(kszBroadsoftActive) != 0)
        {
            //进入Disposition code界面
            SettingUI_OpenPage("disposition_code");
        }
    }
    else if (strAction == "AcdTrace")
    {
        if (CallCenter_IsTraceOn()
                && configParser_GetConfigInt(kszBroadsoftActive) != 0)
        {
            //发起上一路来电追踪
            CallCenter_OriginatedTrace();
        }
    }
#if IF_BUG_24048_RTCP_XR
    else if (strAction == "RTPStatus")
    {
        SettingUI_OpenPage("status_rtp");
    }
#endif
    else if (strAction == ACTION_REDIAL)
    {
        if (BsftLog_IsEnable()
                && configParser_GetConfigInt(kszBWCallLogandDir) == 1)
        {
            //UIManager_ReturnToIdle();
            Menu_EnterPageByType(MENU_PAGE_BROADSOFT_CALLLOG, kszPlacedLogId);
        }
#if IF_FEATURE_METASWITCH_CALLLOG
        else if (MTSWLog_IsEnable() && !LocalLog_IsEnable())
        {
            Menu_EnterPageByType(MENU_PAGE_METASWITCH_CALLLOG, kszPlacedLogId);
        }
#endif
        else
        {
            //先返回idle,再打开CallLog界面
            //UIManager_ReturnToIdle();
            Menu_EnterPageByType(MENU_PAGE_LOCAL_CALLLOG, kszPlacedLogId);

        }

        return true;
    }
#if IF_FEATURE_START2START_ACD
    else if (strAction == "AcdLoginOut")
    {
        if (ACD_START2START == Get_ACDType())
        {
            ACD_ReqLogin();
        }
    }
    else if (strAction == "AcdRefresh")
    {
        if (ACD_START2START == Get_ACDType())
        {
            ACD_ReqChangeACDStatus(AA_REFRESH);
        }
    }
#endif
    else if (strAction == "GuestLock")
    {
        // 启用键盘锁
        if (!CallCenter_IsGuestLock())
        {
            CallCenter_GuestLock(true);

            UpdateSoftKeyCache();

            // 只有在Idle界面才需要立即刷新softkey
            if (UIManager_IsInIdle())
            {
                _MainWND.DrawSoftkey(this);
            }
        }
    }
    else if (strAction == "Unlock")
    {
        // 关闭键盘锁
        if (CallCenter_IsGuestLock())
        {
            MessageBox_ShowGuestUnlockBox(this, IDLE_MSGBOX_TYPE_HOTELING_UNLOCK);
        }
    }
#if IF_FEATURE_METASWITCH_ACD
    else if (strAction == "acdstate")
    {
        SettingUI_OpenPage("mtswacdmystate");
    }
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
    else if (strAction == "XMLBrowserHotelingLogin")
    {
        SettingUI_OpenPage("xmlbrowser_hoteling_login");
    }
    else if (strAction == "XMLBrowserHotelingLogout")
    {
        //xmlbrowser_HotelingRequest(false);
        idleScreen_SetStatus(PS_STATE_HOTELING_LOGOUT_PROMPT);
    }
#endif
    else
    {
        return false;
    }

    return true;
}

yl::string CDlgIdleScreen::GetCurrentRingPath()
{
    yl::string       strRingPath;
    strRingPath = configParser_GetConfigString(kszRingtype);
    commonUnits_Hide2Real(strRingPath, PHFILE_TYPE_RING_FILE);

    // 如果配置中的铃声不存在，则用第一个铃声
    if (strRingPath.empty() || !pathFileExist(strRingPath.c_str()))
    {
        //http://bugfree.yealink.com/Bug.php?BugID=133372
        //该处理已经不是现在的逻辑了，并且现在逻辑层统一处理
        strRingPath = "";
        return strRingPath;
    }

    return strRingPath;
}

CBaseDialog * CDlgIdleScreen::CreateInstance()
{
    if (m_pToMyself  == NULL)
    {
        m_pToMyself = new CDlgIdleScreen();
    }
    return m_pToMyself;
}

void CDlgIdleScreen::OnDelayCheckPopBox()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

#ifdef IF_FEATURE_GETBALANCE
    if (MessageBox_GetMessageBoxCount() != 0)
    {
        QTimer::singleShot(1000, this, SLOT(OnDelayCheckPopBox()));
        return;
    }
#endif

    _IdleNoteManager.CheckPopbox();
}

void CDlgIdleScreen::OnDelayChecknNetworkChange()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    if (_NetworkChangeFlagManager.GetNetworkChangeFlag())
    {
        //Setting界面自动跳回，且网络配置有改变
        MessageBox_ShowCommonMsgBox(this, /*LANG_TRANSLATE(TRID_WARNING),*/
                                    LANG_TRANSLATE(TRID_NETWORK_CHANGED_IF_TAKE_EFFECT_AFTER_RESTART), MESSAGEBOX_OkCancel, -1, "", "",
                                    IDLE_MSGBOX_TYPE_NETWORKCHANGE);
    }
}

void CDlgIdleScreen::OnEnterScreensaver()
{
    StopScreensaverTimer();

    if (UIManager_IsInIdle())
    {
        ScreenSavers_Enter();
    }
}

void CDlgIdleScreen::OnAutoPHintTimeout()
{
    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_FAILED);
    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS);
    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATING);
}

void CDlgIdleScreen::OnUpdateConfigTimeout()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    //如果上锁则不允许该操作
    if (keyGuard_IsEnable() && keyGuard_IsLock())
    {
        return;
    }

    m_bIsTimeOut = true;
    m_timerUpdateConfig.stop();
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_IF_PROVISION_NOW),
                                MESSAGEBOX_OkCancel, -1, "", "", IDLE_MSGBOX_TYPE_UPDATE_CONFIG);
}

void CDlgIdleScreen::StartScreensaverTimer()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    if (m_nScreenSaverWaitTime <= ENTER_SCREEN_MIN_TIME)
    {
        return;
    }

    IDLEUI_INFO("Start Screensaver Timer In IdleScreen.");
    StopScreensaverTimer();
    m_timerScreensaver.start(m_nScreenSaverWaitTime);
}

void CDlgIdleScreen::ResetAutoPWaitTime(int nTimes)
{
    if (m_timerAutoPHint.isActive())
    {
        m_timerAutoPHint.stop();
    }

    if (nTimes > 0)
    {
        m_timerAutoPHint.start(nTimes);
    }
}

void CDlgIdleScreen::StopScreensaverTimer()
{
    if (m_timerScreensaver.isActive())
    {
        m_timerScreensaver.stop();
    }
}

void CDlgIdleScreen::UpdateFeatureSoftkey()
{
    UpdateSoftKeyCache();
    // 只有在Idle界面才需要立即刷新softkey
    if (UIManager_IsInIdle())
    {
        _MainWND.DrawSoftkey(this);
    }
}

bool CDlgIdleScreen::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    // 是否需要响应 case QEvent::Resize:
    QEvent::Type eEventType = pEvent->type();
    switch (eEventType)
    {
    case QEvent::Show:
        {
            IDLEUI_INFO("Check Tip && NetworkChange");

            QTimer::singleShot(20, this, SLOT(OnDelayChecknNetworkChange()));

            QTimer::singleShot(20, this, SLOT(OnDelayCheckPopBox()));
            //_IdleNoteManager.CheckPopbox();

#ifdef IF_FEATURE_GETBALANCE
            ProcessOnFocusEvent();
#endif
            StartScreensaverTimer();

            inputHook_RegisterKeyPadHookProc(IdleScreen_ProcessKeyEvent, HOOK_MODULE_SCREENSAVER);
            inputHook_RegisterMouseHookProc(IdleScreen_MouseEvent, HOOK_MODULE_SCREENSAVER);
        }
        break;
    case QEvent::Hide:
        {
            IDLEUI_INFO("QEvent::Hide");
            //_IdleNoteManager.ClosePopupBox(MessageBox_Destory);

            m_bIsPressed = false;

            if (_MainWND.GetVolumeBar() != NULL
                    && !_MainWND.GetVolumeBar()->isHidden())
            {
                voice_StopRingInMenu();
                _MainWND.SetVolumeBarVisible(false);
            }

            inputHook_UnRegisterKeyPadHookProc(IdleScreen_ProcessKeyEvent);
            inputHook_UnRegisterMouseHookProc(IdleScreen_MouseEvent);

            StopScreensaverTimer();
        }
        break;
    case QEvent::KeyPress:
        {
            m_bIsPressed = true;

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_0:
            case PHONE_KEY_1:
            case PHONE_KEY_2:
            case PHONE_KEY_3:
            case PHONE_KEY_4:
            case PHONE_KEY_5:
            case PHONE_KEY_6:
            case PHONE_KEY_7:
            case PHONE_KEY_8:
            case PHONE_KEY_9:
            case PHONE_KEY_STAR:
                {
                    talklogic_EnterPredialUI(pKeyEvent->key());
                    return true;
                }
                break;
            case PHONE_KEY_POUND:
                {
                    // 如果键盘锁没有开启,则点击相应#号加快进入预拨号界面的速度
                    bool bResult = false;
                    if (!keyGuard_IsEnable())
                    {
                        talklogic_EnterPredialUI(pKeyEvent->key());
                        bResult = true;
                    }
                    else
                    {

                        //如果键盘锁开启并且已经上锁，则按下#就可以直接进入预拨号界面
                        if (keyGuard_IsLock())
                        {
                            talklogic_EnterPredialUI(pKeyEvent->key());
                            bResult = true;
                        }
                        else if (MessageBox_GetMessageBoxCount() == 0)
                        {
                            m_timerPound.start(LONG_PRESS_TIME);
                            bResult = true;
                        }
                    }
                    return bResult;
                }
                break;
            case PHONE_KEY_REDIAL:
                {
                    m_timerUpdateConfig.start(LONG_PRESS_TIME);
                    return true;
                }
                break;
            case PHONE_KEY_OK:
                {
                    if (MessageBox_GetMessageBoxCount() == 0)
                    {
                        if (configParser_GetConfigInt(kszResetByLongPress) == 0
                                || !feature_UserModeConfigItemShow("reset_to_factory"))
                        {
                            //停止计时器
                            m_timerOK.stop();

                            if (UIManager_IsInIdle())
                            {
                                int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(pKeyEvent->key());
                                DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
                                if (DMT_PROGRAMEKEY_MODULE == eModuleType)
                                {
                                    // 只有可编程按键才在这边处理
                                    dsskey_OnDsskeyClick(iDsskeyID);
                                    return true;
                                }
                            }
                        }
                        else
                        {
                            m_timerOK.start(OK_LONG_PRESS_TIME);
                        }

                        return true;
                    }
                }
                break;
            case PHONE_KEY_CANCEL:
                {
                    if (MessageBox_GetMessageBoxCount() == 0)
                    {
                        m_timerCancel.start(LONG_PRESS_TIME);
                        return true;
                    }
                }
                break;
            case PHONE_KEY_FN1:
            case PHONE_KEY_FN2:
            case PHONE_KEY_FN3:
            case PHONE_KEY_FN4:
                {
                    // softkey不走Programablekey流程由自己控制
                    return false;
                }
                break;
            case PHONE_KEY_MESSAGE:
                {
                    if (MessageBox_GetMessageBoxCount() == 0)
                    {
                        m_timerRebootWeb.start(LONG_PRESS_TIME);
                        return true;
                    }
                }
                break;
//        case PHONE_KEY_MUTE:
//            {
//                if (NULL != pDKBtnDel)
//                {
//                    delete pDKBtnDel;
//                    pDKBtnDel = NULL;
//                }
//                pDKBtnDel = new IdleDsskeyDelegete();
//                bool bRet = dsskeyUI_SetCustomBtnDisplay(3, pDKBtnDel);
//                if (!bRet)
//                {
//                    IDLEUI_INFO("Set IdleDsskeyDelegete failed. ");
//                }
//            }
//            break;
//        case PHONE_KEY_HOLD:
//            {
//                dsskeyUI_DebindCustomBtnDisplay(3);
//            }
//            break;
            default:
                {
                    int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(pKeyEvent->key());
                    DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
                    if (DMT_PROGRAMEKEY_MODULE == eModuleType)
                    {
                        // 只有可编程按键才在这边处理
                        if (dsskey_OnDsskeyClick(iDsskeyID))
                        {
                            return true;
                        }
                    }
                }
                break;
            }
        }
        break;
    case QEvent::KeyRelease:
        {
            //没有接收到pressed的release事件不处理，一对一对处理
            if (!m_bIsPressed)
            {
                IDLEUI_INFO(" KeyRelease event is not begin from idle");
                return false;
            }

            m_bIsPressed = false;

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                    if (m_timerOK.isActive())
                    {
                        //停止计时器
                        m_timerOK.stop();

                        if (MessageBox_GetMessageBoxCount() == 0 && UIManager_IsInIdle())
                        {
                            int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(pKeyEvent->key());
                            dsskey_OnDsskeyClick(iDsskeyID);
                        }
                    }
                }
                break;
            case PHONE_KEY_CANCEL:
                {
                    //停止计时器
                    if (m_timerCancel.isActive())
                    {
                        m_timerCancel.stop();

                        if (MessageBox_GetMessageBoxCount() == 0 && UIManager_IsInIdle())
                        {
                            int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(pKeyEvent->key());
                            dsskey_OnDsskeyClick(iDsskeyID);
                        }
                    }
                }
                break;
            case PHONE_KEY_POUND:
                {
                    //停止计时器
                    m_timerPound.stop();

                    //如果键盘锁开启并且已经上锁，则不响应#抬起事件，进入拨号#的事件在按下时处理了
                    if (keyGuard_IsLock())
                    {
                        return true;
                    }
                    else if (MessageBox_GetMessageBoxCount() == 0 && keyGuard_IsEnable())
                    {
                        talklogic_EnterPredialUI(pKeyEvent->key());
                        return true;
                    }
                }
                break;
            case PHONE_KEY_REDIAL:
                {
                    //停止计时器
                    if (m_timerUpdateConfig.isActive())
                    {
                        m_timerUpdateConfig.stop();

                        // 有定时器代表redial在Idle按下，这时才抬起才响应功能
                        if (UIManager_IsInIdle())
                        {
                            int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(pKeyEvent->key());
                            DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
                            if (DMT_PROGRAMEKEY_MODULE == eModuleType)
                            {
                                // 只有可编程按键才在这边处理
                                if (!dsskey_OnDsskeyClick(iDsskeyID))
                                {
                                    OnAction(ACTION_REDIAL);
                                }
                                return true;
                            }
                        }
                    }
                }
                break;
            case PHONE_KEY_MESSAGE://短按处理
                {
                    if (m_timerRebootWeb.isActive())
                    {
                        m_timerRebootWeb.stop();
                    }

                    if (UIManager_IsInIdle())   //这次按键为长按就不响应短按了
                    {
                        int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(pKeyEvent->key());
                        DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
                        if (DMT_PROGRAMEKEY_MODULE == eModuleType)
                        {
                            // 只有可编程按键才在这边处理
                            if (!dsskey_OnDsskeyClick(iDsskeyID))
                            {
#if IF_FEATURE_BROADSOFT_VOICEMAIL
                                if (configParser_GetConfigInt(kszVoiceMailMessageKeyMode) == 1)
                                {
                                    Setting_ConnectVoiceMail();
                                }
                                else
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
                                    if (MTSWVM_IsMessagesListEnable())
                                    {
                                        SettingUI_OpenPage(SubMenuData("OpenMenu(message_list.xml)", 1), true);
                                    }
                                    else
#endif
                                    {
                                        int iAccountId = acc_GetLastVMailAccountId();
                                        if (iAccountId < 0)
                                        {
                                            iAccountId = acc_GetDefaultAccount();
                                        }
                                        //呼出配置的默认语音帐号号码（如果帐号未配置，则进入默认账号的语音信箱设置）
                                        Setting_ConnectVoiceMail(iAccountId);
                                    }

                            }
                            return true;
                        }
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

// 处理ProgramableKey配置改变
void CDlgIdleScreen::OnProgramableKeyConfigChange()
{
    IDLEUI_INFO("CDlgIdleScreen::OnProgramableKeyConfigChange");
    // 重新读取ProgramableKey配置

    UpdateSoftKeyCache();

    // 只有在Idle界面才需要立即刷新softkey
    if (UIManager_IsInIdle())
    {
        _MainWND.DrawSoftkey(this);
    }
}

void CDlgIdleScreen::OnRTPActiveChange()
{
    IDLEUI_INFO("CDlgIdleScreen::OnRTPActiveChange");
    UpdateSoftKeyCache();

    // 只有在Idle界面才需要立即刷新softkey
    if (UIManager_IsInIdle())
    {
        _MainWND.DrawSoftkey(this);
    }
}
BOOL CDlgIdleScreen::OnACDHotelingMsg(msgObject & objMessage)
{
    IDLE_INFO("ACDHotelingmsg change %d", objMessage.message);
    QString strAcdTip;
    switch (objMessage.message)
    {
    case ACD_MESSAGE_STATUS_CHANGE:
    case ACD_MESSAGE_STATUS_UPDATE:
        {
            //更新softkey
            UpdateSoftKeyCache();

#if IF_FEATURE_START2START_ACD
            if (Get_ACDType() == ACD_START2START)
            {
                break;
            }
#endif
            //更新状态提示信息
            switch (objMessage.lParam)
            {
            case AS_AVAILABLE:
                {
                    strAcdTip = LANG_TRANSLATE(TRID_AVAILABLE);

                    // GENESYS类型
                    if (ACD_GENESYS == Get_ACDType())
                    {
                        strAcdTip = ACD_GetACDLoginName().c_str();
                    }
                }
                break;
            case AS_UNAVAILABLE:
                {
                    yl::string strUnavailCode;
                    //获取Unavailable Code
                    ACD_GetUnavailCode(strUnavailCode);
                    //如果有code就显示Code，否则显示Unavailable
                    if (strUnavailCode.empty())
                    {
                        strAcdTip = LANG_TRANSLATE(TRID_UNAVAILABLE);

                        // GENESYS类型
                        if (ACD_GENESYS == Get_ACDType())
                        {
                            strAcdTip = ACD_GetACDLoginName().c_str();
                        }
                    }
                    else
                    {
                        strAcdTip = toQString(strUnavailCode, true);
                    }
                }
                break;
            case AS_WRAPUP:
                {
                    strAcdTip = LANG_TRANSLATE(TRID_WRAP_UP);

                    // GENESYS类型
                    if (ACD_GENESYS == Get_ACDType())
                    {
                        strAcdTip = ACD_GetACDLoginName().c_str();
                    }
#if IF_FEATURE_METASWITCH_ACD
                    else if (ACD_METASWITCH == Get_ACDType())
                    {
                        yl::string strWrapup = MTSWACD_GetLastAnseredMHLG();
                        if (!strWrapup.empty())
                        {
                            strAcdTip.append('(');
                            strAcdTip.append(toQString(strWrapup.c_str()));
                            strAcdTip.append(')');
                        }
                    }
#endif
                }
                break;
            case AS_IDLE:
            default:
                {
                    SetACDQueStatActivate(false);
                }
                break;
            }

#if IF_FEATURE_METASWITCH_ACD
            if (Get_ACDType() == ACD_METASWITCH
                    && !MTSWACD_IsEnableShowState())
            {
                strAcdTip = "";
                SetACDQueStatActivate(false);
            }
#endif

            SetNoteContent("", strAcdTip);
        }
        break;
    case HOTELING_MESSAGE_STATUS_CHANGE:
        {
            //更新softkey
            UpdateSoftKeyCache();
            //更新linekey状态
            //-- todo
        }
        break;
    case HOTELING_MESSAGE_REQ_TIMEOUT:
        {
            //退出ACD登录窗口
            //http://10.2.1.199/Bug.php?BugID=43754
            //UIManager_ReturnToIdle();
        }
        break;
#if IF_BUG_24363
    case ACD_MESSAGE_LOGIN_TIMEOUT:
        {
            if (UIManager_IsInIdle())
            {
                if (ACD_GetAcdStatus() == AS_IDLE)
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOGIN_FAILED), MESSAGEBOX_NOTE, 1500);
                }
                else
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOGOUT_FAILED), MESSAGEBOX_NOTE, 1500);
                }
            }
        }
        break;
#endif
    case ACD_MESSAGE_QUEUE_STATUS:
        {
            if (!CallCenter_IsEnableQueueStatus())
            {
                // 未开启Queue Status，隐藏控件
                SetACDQueStatActivate(false);
                break;
            }

            //更新queute status
            const CallCenterQueueStatus & qst = ACD_GetQueueStatus();
            QString strQest = QString::fromUtf8(qst.m_strName.c_str());
            strQest += ": ";
            switch (qst.m_eStatus)
            {
            //empty
            case QUEUE_EMPTY:
                {
                    strQest += LANG_TRANSLATE(TRID_EMPTY);
                    SetACDQueStatActivate(true);
                }
                break;
            //normal
            case QUEUE_NORMAL:
                {
                    strQest += LANG_TRANSLATE(TRID_QUEUEING);
                    SetACDQueStatActivate(true);
                }
                break;
            //Threshold Exceeded
            case QUEUE_THRESHOLD_EXCEEDED:
                {
                    strQest += LANG_TRANSLATE(TRID_ALERT);
                    SetACDQueStatActivate(true);
                }
                break;
            default:
                {
                    SetACDQueStatActivate(false);
                    strQest.clear();
                }
                break;
            }

            //更新队列信息状态提示
            if (m_pLabACDQueStat != NULL)
            {
                m_pLabACDQueStat->setText(strQest);
                checkXMLframeAndACDQueStat();
            }
        }
        break;
    default:
        break;
    }

    //刷新界面
    UIManager_UpdateWnd(this);
    return FALSE;
}

void CDlgIdleScreen::SetNoteContent(const QString & strIconPath, const QString & strNoteText)
{
    if (m_pNoteLabel != NULL)
    {
        m_pNoteLabel->SetLabelIcon(strIconPath);
        m_pNoteLabel->SetLabelText(strNoteText);

        if (m_bIdleDsskey)
        {
            HideWidgets();
            return;
        }

        if (!IsXMLBrowserActivate())
        {
            CheckNoteLabAndAccountLab();
        }
        else
        {
            m_pNoteLabel->hide();
        }

    }
}

// 更新界面背景
void CDlgIdleScreen::RefreshBackGroundPic()
{
    // 如果背景图片未变化则不更新
    yl::string strBkTmp = GetCurrentBackground();

    if (m_strBGPicPath == strBkTmp)
    {
        return;
    }

    // 获取背景图片
    m_strBGPicPath = strBkTmp;

    DrawDsskeyBtnOnBackground();
}

QPixmap CDlgIdleScreen::GetCustomBackGround()
{
    RefreshBackGroundPic();

    return m_pmBG;
}

// 切换Dsskey页面
bool CDlgIdleScreen::OnDssKeyPageAction()
{
    if (m_bIdleDsskey == true)
    {
        m_bIdleDsskey = false;

        m_pIdleDsskey->lower();
        m_pIdleDsskey->hide();

        ShowWidgets();
    }
    else
    {
        HideWidgets();
        m_bIdleDsskey = true;

        m_pIdleDsskey->raise();
        m_pIdleDsskey->show();

    }

    UIManager_UpdateWnd(this);
    return m_bIdleDsskey;
}

//展开dsskeyframe时隐藏dsskeyframe位置上的控件
void CDlgIdleScreen::HideWidgets()
{
    if (m_pNoteLabel != NULL)
    {
        m_pNoteLabel->hide();
    }

    if (m_pXMLFrame != NULL)
    {
        m_pXMLFrame->hide();
    }

    if (m_pLabACDQueStat != NULL)
    {
        m_pLabACDQueStat->hide();
    }

#if !IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    if (m_pLabDefaultAccount != NULL)
    {
        m_pLabDefaultAccount->hide();
    }
#endif
}

//收起dsskeyframe时重新显示dsskeyframe位置上的其它控件
void CDlgIdleScreen::ShowWidgets()
{
    if (m_pNoteLabel != NULL)
    {
        m_pNoteLabel->show();
    }

    checkXMLframeAndACDQueStat();
    CheckNoteLabAndAccountLab();
}
void CDlgIdleScreen::UpdateVoiceMailNotify()
{
    //设置是否弹出消息提示框
    bool IsPopWind = !configParser_GetConfigInt(kszStatusChangeIsNotPopWind); //状态改变
    bool IsPopNotify = IsPopWind;
    //判断VoiceMailWind开关
    if (IsPopNotify)
    {
        yl::string strPopValue = configParser_GetConfigString(kszDisplayVoiceMailPopup);
        if (!strPopValue.empty())
        {
            IsPopNotify = atoi(strPopValue.c_str());
        }
        //找不到配置时，按中性处理
    }
    PopupWindowType windType = IsPopNotify ? POPUP_WINDOW_POPUP : POPUP_WINDOW_NO_POPUP;
    // 刷新Dsskey内容
    idleScreen_RegisterStatus(PS_STATE_VOICEMAIL, windType, 0);
    if (!IsPopNotify)
    {
        idleScreen_ClearPopupFlag(PS_STATE_VOICEMAIL);

    }
    idleScreen_TurnStatusNotify(true);
}

void CDlgIdleScreen::UpdateMissCallNotify()
{
    //设置是否弹出消息提示框
    bool IsPopWind = !configParser_GetConfigInt(kszStatusChangeIsNotPopWind); //状态改变
    bool IsPopNotify = IsPopWind;
    //判断VoiceMailWind开关
    if (IsPopNotify)
    {
        yl::string strPopValue = configParser_GetConfigString(kszDisplayMissedCallPopup);
        if (!strPopValue.empty())
        {
            IsPopNotify = atoi(strPopValue.c_str());
        }
        //找不到配置时，按中性处理
    }
    PopupWindowType windType = IsPopNotify ? POPUP_WINDOW_POPUP : POPUP_WINDOW_NO_POPUP;
    // 刷新Dsskey内容
    idleScreen_RegisterStatus(PS_STATE_MISSCALL, windType, 0);

    if (!IsPopNotify)
    {
        idleScreen_ClearPopupFlag(PS_STATE_MISSCALL);

    }
    idleScreen_TurnStatusNotify(true);
}

void CDlgIdleScreen::UpdateTextMsgNotify()
{
    //设置是否弹出消息提示框
    bool IsPopWind = !configParser_GetConfigInt(kszStatusChangeIsNotPopWind); //状态改变
    bool IsPopNotify = IsPopWind;
    //判断VoiceMailWind开关
    if (IsPopNotify)
    {
        yl::string strPopValue = configParser_GetConfigString(kszDisplayTextMessagePopup);
        if (!strPopValue.empty())
        {
            IsPopNotify = atoi(strPopValue.c_str());
        }
        //找不到配置时，按中性处理
    }
    PopupWindowType windType = IsPopNotify ? POPUP_WINDOW_POPUP : POPUP_WINDOW_NO_POPUP;
    // 刷新Dsskey内容
    idleScreen_RegisterStatus(PS_STATE_TEXTMESSAGE, windType, 0);
    if (!IsPopNotify)
    {
        idleScreen_ClearPopupFlag(PS_STATE_TEXTMESSAGE);

    }
    idleScreen_TurnStatusNotify(true);
}

void CDlgIdleScreen::UpdateCallForwardNotify()
{
    //设置是否弹出消息提示框
    bool IsPopWind = !configParser_GetConfigInt(kszStatusChangeIsNotPopWind); //状态改变
    bool IsPopNotify = IsPopWind;
    //判断VoiceMailWind开关
    if (IsPopNotify)
    {
        yl::string strPopValue = configParser_GetConfigString(kszDisplayForwardCallPopup);
        if (!strPopValue.empty())
        {
            IsPopNotify = atoi(strPopValue.c_str());
        }
        //找不到配置时，按中性处理
    }
    PopupWindowType windType = IsPopNotify ? POPUP_WINDOW_POPUP : POPUP_WINDOW_NO_POPUP;
    // 刷新Dsskey内容
    idleScreen_RegisterStatus(PS_STATE_CALLFORWARD, windType, 0);
    if (!IsPopNotify)
    {
        idleScreen_ClearPopupFlag(PS_STATE_CALLFORWARD);

    }
    idleScreen_TurnStatusNotify(true);
}

void CDlgIdleScreen::ProcessFlexibleMsg(msgObject & objMessage)
{

    IDLEUI_INFO("CDlgIdleScreen::ProcessFlexibleMsg.[%d][%d][%d]",
                objMessage.message, objMessage.wParam, objMessage.lParam);

    switch (objMessage.message)
    {
    case COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG:
        {
            CallCenter_ClearFlexibleSeating();
            MessageBox_RemoveMessageBox(this, MessageBox_Destory, IDLE_MSGBOX_TYPE_HOTELING_START);
        }
        break;
    default:
        break;
    }
}

void CDlgIdleScreen::ProcessBalanceMsg(const QVariant & objData)
{
#ifdef IF_FEATURE_GETBALANCE
    IDLEUI_INFO("CDlgIdleScreen::ProcessBalanceMsg.");

    // 先退出当前弹窗
    MessageBox_RemoveTopMessageBox(MessageBox_Destory);

    if (!objData.canConvert<UINT>()
            || !UIManager_IsInIdle())
    {
        return;
    }

    UINT msg = objData.value<UINT>();
    IDLEUI_INFO("ProcessBalanceMsg msg (%d)", msg);
    switch (msg)
    {
    case GET_BALANCE_MESSAGE_GET_RESULT:
        {
            Balance_Info_t balanceInfo;
            memset(&balanceInfo, 0, sizeof(balanceInfo));
            // 获取余额信息
            acc_GetBalanceInfo(balanceInfo);

            if (balanceInfo.iResult == 0)
            {
                SettingUI_OpenPage("get_balance");
            }
            else
            {
                MessageBox_ShowCommonMsgBox(NULL,
                                            toQString(balanceInfo.szBalance), MESSAGEBOX_Cancel, BALANCE_MSG_BOX_SHOW_NORMAL);
            }
        }
        break;
    case GET_BALANCE_MESSAGE_LOADING:
        {
            // 正在下载弹窗
            MessageBox_ShowCommonMsgBox(this,  LANG_TRANSLATE(TRID_GETTING_BALENCE),
                                        MESSAGEBOX_Cancel, BALANCE_MSG_BOX_SHOW_LOADING, "", "", IDLE_MSGBOX_TYPE_BALANCE_LOADING);
        }
        break;
    case GET_BALANCE_MESSAGE_LOAD_ERROR:
        {
            // 下载过程失败
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_GET_BALANCE_FAILED),
                                        MESSAGEBOX_Cancel, BALANCE_MSG_BOX_SHOW_NORMAL);
        }
        break;
    default:
        break;
    }
#endif
}

void CDlgIdleScreen::ProcessOnFocusEvent()
{
#ifdef IF_FEATURE_GETBALANCE
    IDLEUI_INFO("ProcessOnFocusEvent");
    if (UIManager_IsInIdle())
    {
        acc_LoadBalance();
    }
#endif
}

//只开启Hoteling的Softkey
void CDlgIdleScreen::SetHotelingSoftkey()
{
    CSoftKeyBarData objSoftkeyData;
    objSoftkeyData.m_pReceiver = this;
    objSoftkeyData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));

    if (HOTELING_STATUS_LOGIN == CallCenter_GetHotelingStatus())
    {
        int iIndex = 1;
        objSoftkeyData.m_strSoftkeyAction = "AcdLogOut";
        objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_GUEST_OUT);
        objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGOUT;
        SetLogoutAndLockSoftkey(iIndex, objSoftkeyData);
    }
    else
    {
        objSoftkeyData.m_strSoftkeyAction = "AcdLogin";
        objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_GUEST_IN);
        objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGIN;
        InsertSoftkey(1, objSoftkeyData);
    }
}

// 只开启ACD的Softkey
void CDlgIdleScreen::SetACDSoftkey(bool bShowLock/* = false*/)
{
    CSoftKeyBarData objSoftkeyData;
    objSoftkeyData.m_pReceiver = this;
    objSoftkeyData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));

    if (AS_IDLE != ACD_GetAcdStatus())
    {
        // 标识到哪个Softkey
        int nSoftKeyIndex = 0;
        bool bAvailabilityState = configParser_GetCfgItemIntValue(kszAccountACDAvailabilityState,
                                  ACD_GetAccountId());

        // 判断是否开启ACD的登录登出
        if (bAvailabilityState)
        {
            if (AS_AVAILABLE == ACD_GetAcdStatus())
            {
                objSoftkeyData.m_strSoftkeyAction = "AcdUnvail";
                objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_UNAVAIL);
                objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_UNAVAILABLE;
                InsertSoftkey(nSoftKeyIndex++, objSoftkeyData);
            }
            else if (AS_UNAVAILABLE == ACD_GetAcdStatus()
                     || AS_WRAPUP == ACD_GetAcdStatus())
            {
                objSoftkeyData.m_strSoftkeyAction = "AcdAvail";
                objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_AVAIL);
                objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_AVAILABLE;
                InsertSoftkey(nSoftKeyIndex++, objSoftkeyData);
            }
        }

        if (bShowLock
                && configParser_GetConfigInt(kszSoftkeyIdleHotelingEnable) != 0)
        {
            objSoftkeyData.m_strSoftkeyAction = "AcdLogOut";
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_LOGOUT);
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGOUT;
            SetLogoutAndLockSoftkey(nSoftKeyIndex, objSoftkeyData);
            ++nSoftKeyIndex;
        }
        else
        {
            objSoftkeyData.m_strSoftkeyAction = "AcdLogOut";
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_LOGOUT);
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGOUT;
            InsertSoftkey(nSoftKeyIndex++, objSoftkeyData);
        }

        //如果Disposition Code功能没开启，不显示Softkey
        if (CallCenter_IsDispCodeOn()
                && ACD_BROADSOFT == Get_ACDType()
                && configParser_GetConfigInt(kszBroadsoftActive) != 0)
        {
            objSoftkeyData.m_strSoftkeyAction = "AcdDispCode";
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ACD_DISPOSITION_CODE_SHORT);
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_DISPCODE;
            InsertSoftkey(nSoftKeyIndex++, objSoftkeyData);
        }
        //如果Originated Trace功能没开启，不显示Softkey
        if (CallCenter_IsTraceOn()
                && ACD_BROADSOFT == Get_ACDType()
                && configParser_GetConfigInt(kszBroadsoftActive) != 0)
        {
            objSoftkeyData.m_strSoftkeyAction = "AcdTrace";
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_TRACE;
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ACD_TRACE);
            InsertSoftkey(nSoftKeyIndex++, objSoftkeyData);
        }
    }
    else
    {
        objSoftkeyData.m_strSoftkeyAction = "AcdLogin";
        objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_LOGIN);
        objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGIN;
        InsertSoftkey(1, objSoftkeyData);
    }
}

// 两者都开时的Softkey
void CDlgIdleScreen::SetHotelingAndACDSoftkey()
{
    CSoftKeyBarData objSoftkeyData;
    objSoftkeyData.m_pReceiver = this;
    objSoftkeyData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));

    if (AS_IDLE == ACD_GetAcdStatus())
    {
        // Hoteling还是登录状态, 则显示Hoteling
        if (HOTELING_STATUS_LOGIN == CallCenter_GetHotelingStatus()
                && 0 != configParser_GetConfigInt(kszSoftkeyIdleHotelingEnable))
        {
            SetHotelingSoftkey();
        }
        else
        {
            objSoftkeyData.m_strSoftkeyAction = "AcdLogin";
            objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_LOGIN);
            objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_ACD_LOGIN;
            InsertSoftkey(1, objSoftkeyData);
        }
    }
    else
    {
        SetACDSoftkey(true);
    }
}

void CDlgIdleScreen::SetLogoutAndLockSoftkey(int & iIndex, const CSoftKeyBarData & softKeyData)
{
    CSoftKeyBarData objSoftkeyData;
    objSoftkeyData.m_pReceiver = this;
    objSoftkeyData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));

    if (!CallCenter_EnableGuestLock())
    {
        InsertSoftkey(iIndex, softKeyData);
    }

    else if (!CallCenter_IsGuestLock())
    {
        InsertSoftkey(iIndex++, softKeyData);
        objSoftkeyData.m_strSoftkeyAction = "GuestLock";
        objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_GUEST_LOCK);
        objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_GUEST_LOCK;
        InsertSoftkey(iIndex, objSoftkeyData);
    }
    else
    {
        objSoftkeyData.m_strSoftkeyAction = "Unlock";
        objSoftkeyData.m_strSoftkeyTitle = LANG_TRANSLATE(TRID_UNLOCK);
        objSoftkeyData.m_strIconPath = PIC_SOFTKEY_IDLE_UNLOCK;
        InsertSoftkey(iIndex, objSoftkeyData);
    }
}

void CDlgIdleScreen::ShowDsskeyLockPassword(int iDssKeyID)
{
    MessageBox_ShowDsskeyLockPasswordBox(this, QVariant::fromValue(iDssKeyID),
                                         IDLE_MSGBOX_TYPE_DSSKEY_LOCK_PASSWORD);
}

#if IF_BUG_33812
void CDlgIdleScreen::InitWallpaperLogo()
{
    if (configParser_GetConfigInt(kszIsShowLogo) != 1
            || m_pWallPaperLogo != NULL)
    {
        return;
    }

    m_pWallPaperLogo = new QLabel(this);
    if (m_pWallPaperLogo != NULL)
    {
        m_pWallPaperLogo->setGeometry(QRect(80, 68, 340, 255));
        m_pWallPaperLogo->setScaledContents(false);
        m_pWallPaperLogo->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        m_pWallPaperLogo->setPixmap(THEME_GET_BMP(PIC_ICON_WALLPAPER_LOGO));
        m_pWallPaperLogo->lower();
        CheckWallpaperLogo();
    }
}

void CDlgIdleScreen::CheckWallpaperLogo(bool bShow/* = true*/)
{
    if (m_pWallPaperLogo == NULL)
    {
        return;
    }

    if (configParser_GetConfigInt(kszIsShowLogo) == 1
            && bShow)
    {
        m_pWallPaperLogo->show();
    }
    else
    {
        m_pWallPaperLogo->hide();
    }
}
#endif

#if IF_FEATURE_WALLPAPER_MODE
void CDlgIdleScreen::CheckWallpaperChange()
{
    WALLPAPAER_MODE eWallpaperMode = (WALLPAPAER_MODE)configParser_GetConfigInt(kszWallpaperMode);
    yl::string strBackgroundColor = configParser_GetConfigString(kszWallpaperBackgroundColor);
    if (SetBackgroundColor(strBackgroundColor)
            || m_eWallpaperMode != eWallpaperMode)
    {
        m_eWallpaperMode = eWallpaperMode;
        DrawDsskeyBtnOnBackground();
    }
}

bool CDlgIdleScreen::SetBackgroundColor(yl::string & strBackgroundColor)
{
    int iRed, iGreen, iBlue;
    strBackgroundColor.trim_both();
    strBackgroundColor.to_upper();

    if (strBackgroundColor.empty()
            || (sscanf(strBackgroundColor.c_str(), "%d,%d,%d", &iRed, &iGreen, &iBlue) != 3
                && sscanf(strBackgroundColor.c_str(), "RGB(%d,%d,%d)", &iRed, &iGreen, &iBlue) != 3))
    {
        iRed = 28;
        iGreen = 95;
        iBlue = 184;
    }

    if (m_BackgroundColor.red() != iRed
            || m_BackgroundColor.green() != iGreen
            || m_BackgroundColor.blue() != iBlue)
    {
        m_BackgroundColor.setRed(iRed);
        m_BackgroundColor.setGreen(iGreen);
        m_BackgroundColor.setBlue(iBlue);
        return true;
    }

    return false;
}

void CDlgIdleScreen::DrawWallpaper(QPixmap & pBackGround)
{
    QRect rcWallpaper(0, 0, PHONE_WIDTH, PHONE_HEIGHT);
    bool bDrawWallpaper = true;
    switch (m_eWallpaperMode)
    {
    case WALLPAPAER_MODE_CENTER:
        {
            int iX = (PHONE_WIDTH - pBackGround.width()) / 2;
            int iY = (PHONE_HEIGHT - pBackGround.height()) / 2;
            rcWallpaper = QRect(iX, iY, pBackGround.width(), pBackGround.height());
        }
        break;
    case WALLPAPAER_MODE_FILL:
    case WALLPAPAER_MODE_FIT:
        {
            float fX = (float)PHONE_WIDTH / (float)pBackGround.width();
            float fY = (float)PHONE_HEIGHT / (float)pBackGround.height();
            if ((fY > fX && m_eWallpaperMode == WALLPAPAER_MODE_FILL)
                    || (fY < fX && m_eWallpaperMode == WALLPAPAER_MODE_FIT))
            {
                int iWidth = pBackGround.width() * fY;
                pBackGround = pBackGround.scaled(iWidth, PHONE_HEIGHT);
                rcWallpaper = QRect((PHONE_WIDTH - iWidth) / 2, 0, iWidth, PHONE_HEIGHT);
            }
            else
            {
                int iHeight = pBackGround.height() * fX;
                pBackGround = pBackGround.scaled(PHONE_WIDTH, iHeight);
                rcWallpaper = QRect(0, (PHONE_HEIGHT - iHeight) / 2, PHONE_WIDTH, iHeight);
            }
        }
        break;
    case WALLPAPAER_MODE_TILE:
        {
            bDrawWallpaper = false;
            QPainter painterWallpaper(&m_pmBG);
            for (int iX = 0; iX < PHONE_WIDTH; iX = iX + pBackGround.width())
            {
                for (int iY = 0; iY < PHONE_HEIGHT; iY = iY + pBackGround.height())
                {
                    painterWallpaper.drawPixmap(iX, iY, pBackGround.width(), pBackGround.height(), pBackGround);
                }
            }
        }
        break;
    default:
        {
            bDrawWallpaper = false;
            m_pmBG = pBackGround.scaled(PHONE_WIDTH, PHONE_HEIGHT);
        }
    }

    if (bDrawWallpaper)
    {
        QPainter painterWallpaper(&m_pmBG);
        painterWallpaper.drawPixmap(rcWallpaper, pBackGround);
    }
}
#endif

#if IF_FEATURE_XMLBROWSER_HOTELING
bool Compare(int & lCount, int & rCount)
{
    return lCount < rCount;
}

void CDlgIdleScreen::UpdateXMLBrowserHotelingStatus(int iType)
{
    IDLEUI_INFO("CDlgIdleScreen::UpdateXMLBrowserHotelingStatus(%d) HotelingLoginStatus[%d]", iType,
                configParser_GetConfigInt(kszHotelingLoginStatus));

    switch (iType)
    {
    case XB_HOTELING_LOGIN_STATUS:
        {
            ResetHotelingLogoutPromptTime();
            if (configParser_GetConfigInt(kszHotelingLoginStatus) == 1)
            {
                //退出登录界面
                UIManager_ReturnToIdle();

                MessageBox_RemoveAllMessageBox(MessageBox_Destory);
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_TIP_LOGIN_SUCCESSFUL), MESSAGEBOX_Cancel, -1,
                                            LANG_TRANSLATE(TRID_EXIT));
            }
            else
            {
                //清空用户名密码
                configParser_SetConfigString(kszHotelingUserName, "", CONFIG_LEVEL_PHONE);
                configParser_SetConfigString(kszHotelingPassword, "", CONFIG_LEVEL_PHONE);

                MessageBox_RemoveAllMessageBox(MessageBox_Destory);
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_TIP_LOGOUT_SUCCESSFUL), MESSAGEBOX_Cancel, -1,
                                            LANG_TRANSLATE(TRID_EXIT));
            }
        }
    case XB_HOTELING_SERVER:
        {
            ResetHotelingLogoutPromptTime();
            //更新softkey
            UpdateSoftKeyCache();
            if (UIManager_IsInIdle())
            {
                //刷新界面
                UIManager_UpdateWnd(this);
            }
        }
        break;
    case XB_HOTELING_LOGOUT_PROMPT:
        {
            ResetHotelingLogoutPromptTime();
        }
        break;
    case XB_HOTELING_LOGOUT_DURATION:
        {

        }
        break;
    default:
        break;
    }
}

void CDlgIdleScreen::ResetHotelingLogoutPromptTime()
{
    idleScreen_DropStatus(PS_STATE_HOTELING_LOGOUT_PROMPT);
    if (m_timerHotelingLogoutPrompt.isActive())
    {
        m_timerHotelingLogoutPrompt.stop();
    }

    vecLogoutPrompt.clear();
    yl::string strHotelingLogOutPromptTime = configParser_GetConfigString(kszHotelingLogOutPromptTime);
    if (strHotelingLogOutPromptTime.empty())
    {
        return;
    }

    YLList<yl::string> listPromptTime;
    if (commonAPI_splitStringBySeparator(strHotelingLogOutPromptTime, ',', listPromptTime)
            && listPromptTime.size() > 0)
    {
        YLList<yl::string>::iterator it = listPromptTime.begin();
        for (; it != listPromptTime.end(); it++)
        {
            yl::string strPromptTime = *it;
            if (!strPromptTime.empty()
                    && strPromptTime.length() < 5
                    && commonAPI_IsDigital(strPromptTime))
            {
                int iPromptTime = atoi(strPromptTime.c_str());
                vecLogoutPrompt.push_back(iPromptTime);
            }
        }
        Sort(vecLogoutPrompt, Compare);
    }

    StartHotelingLogoutTime();
}

void CDlgIdleScreen::StartHotelingLogoutTime()
{
    if (m_timerHotelingLogoutPrompt.isActive())
    {
        m_timerHotelingLogoutPrompt.stop();
    }

    yl::string strHotelingServer = configParser_GetConfigString(kszHotelingServer);
    if (strHotelingServer.empty())
    {
        return;
    }

    if (vecLogoutPrompt.size() == 0
            || configParser_GetConfigInt(kszHotelingLoginStatus) != 1)
    {
        return;
    }

    while (vecLogoutPrompt.size() != 0)
    {
        if (vecLogoutPrompt[0] > 0)
        {
            m_timerHotelingLogoutPrompt.start(vecLogoutPrompt[0] * 1000 * 60);
            return;
        }
        else
        {
            vecLogoutPrompt.removeAt(0);
        }
    }
}

void CDlgIdleScreen::OnHotelingLogoutPrompt()
{
    if (vecLogoutPrompt.size() == 0
            || configParser_GetConfigInt(kszHotelingLoginStatus) != 1)
    {
        return;
    }

    idleScreen_SetStatus(PS_STATE_HOTELING_LOGOUT_PROMPT);

    for (int i = vecLogoutPrompt.size() - 1; i > 0; i--)
    {
        vecLogoutPrompt[i] = vecLogoutPrompt[i] - vecLogoutPrompt[0];
    }

    vecLogoutPrompt.removeAt(0);

    StartHotelingLogoutTime();
}
#else
void CDlgIdleScreen::OnHotelingLogoutPrompt()
{
}
#endif

// 处理屏保定时器到时场景
void CDlgIdleScreen::ProcessScreensaverTimeOut()
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    if (!m_timerScreensaver.isActive())
    {
        //定时器已经停止则直接进屏保
        OnEnterScreensaver();
    }
}
