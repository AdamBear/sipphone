#include "screensavers.h"
#include "screensaverpainter.h"
#include "uimanager/rectdefine.h"
#include "uimanager/moduimanager.h"
#include "imagemanager/modimagemanager.h"
#include "idlescreenui/popupboxproxyfactory.h"
#include "mainwnd/mainwnd.h"
#include "setting/include/modsetting.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "uimanager/moduimanager.h"
#include "keymap.h"
#include "globalinputhook/modinputhook.h"
#include "uikernel/languagehelper.h"
#include "lamp/backlight/include/modbacklight.h"
#include "dsskey/include/moddsskey.h"
#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "blf/include/blflistmsgdefine.h"
#include "dsklog/log.h"
#include "lamp/powersaving/include/modpowersaving.h"
#if IF_FEATURE_XMLB_SCREENSAVER
#include "uikernel/qwidgetutility.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "xmlbrowserui/uicommon.h"
#include "modscreensavers.h"
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/recordmessagedefine.h"
#endif

namespace
{
#if SCREEN_SAVE_FEATRUE_CLOCK_MOVING
//时钟连续移动的时间间隔
#define SCREENSAVER_MOVE_TIME_INTERVAL              3
//时钟连续移动的单步距离
#define SCREENSAVER_FRAME_MOVE_STEP                 10
#endif

//背景过渡时变换透明度的时间间隔
#define SCREENSAVER_OPACITY_CHANGE_INTERVAL         2

//如果正在切换图片，此时又有删除用户图片导致的背景变化，
//则等待此时间后，就认为上一次切换已经完成
// #define SCREENSAVER_OPACITY_CHANGE_WAIT_TIME        2500

//刚刚进入屏保时，时钟和状态图标整体的初始位置
#define SCREENSAVER_DEFAULT_FRAME_RECT              QRect(SCREENSAVER_RECT_LEFT, SCREENSAVER_RECT_TOP, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)

//时钟和状态图标整体的左上角可以使用的值的范围
#define SCREENSAVER_MIN_FRAME_LEFT                  -28
#define SCREENSAVER_MAX_FRAME_LEFT                  PHONE_WIDTH - SCREENSAVER_RECT_WIDTH - SCREENSAVER_MIN_FRAME_LEFT
#define SCREENSAVER_MIN_FRAME_TOP                   30
#define SCREENSAVER_MAX_FRAME_TOP                   PHONE_HEIGHT - SCREENSAVER_RECT_HEIGHT - SCREENSAVER_MIN_FRAME_TOP

//时钟和状态图标整体内部的控件位置
//时钟位置
#define SCREENSAVER_CLOCK_RECT                      QRect(0, 0, SCREENSAVER_RECT_WIDTH, 116)
//状态图标位置
#define SCREENSAVER_TITLEICONS_RECT                 QRect(0, 116, SCREENSAVER_RECT_WIDTH, 43)
//错误提示位置
#define SCREENSAVER_PIC_ERROR_RECT                  QRect(0, 0, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)

#define SCREENSAVER_LOADERROR_TEXT_COLOR            QColor(26, 157, 213)
#define SCREENSAVER_LOADERROR_TEXT_FONT_SIZE        34

#define SCREENSAVER_CLOCK_HM_FONT_SIZE              60
#define SCREENSAVER_CLOCK_AMPM_FONT_SIZE            20
#define SCREENSAVER_CLOCK_DATE_FONT_SIZE            20
#define SCREENSAVER_CLOCK_HM_AND_DATE_INTTERVAL     15

// push xml消息
#define PHONE_MSG_PUSH_XML 0x60d08

//XML类型屏保时frame初始位置
#define SCREENSAVER_XML_FRAME_RECT              QRect(0, 0, PHONE_WIDTH, PHONE_HEIGHT)

}

LRESULT CDlgScreenSavers::OnMessage(msgObject & refMessage)
{
    IDLEUI_INFO("CDlgScreenSavers::OnMessage MessageId[%d], wParam[%d]", refMessage.message,
                refMessage.wParam);

    CDlgScreenSavers * pDlgScreensavers = (CDlgScreenSavers *)UIManager_GetPrivateDlg(
            DLG_CDlgScreenSavers);

    if (NULL != pDlgScreensavers && pDlgScreensavers == UIManager_GetTopWindow())
    {
        if (BACKLIGHT_LEVEL_CHANGED_MSG == refMessage.message)
        {
            if (refMessage.wParam > 0)
            {
                pDlgScreensavers->SetShowBackGround(true);
            }
            else
            {
                pDlgScreensavers->SetShowBackGround(false);
            }
        }
        else if (POWERSAVINGMEG_SCREEN_CLOSE == refMessage.message)
        {
            if (POWERSAVING_MODE_CLOSE == refMessage.wParam)
            {
                pDlgScreensavers->SetShowBackGround(false);
            }
        }
        else if (POWERSAVINGMEG_SCREEN_AWAKE == refMessage.message)
        {
            pDlgScreensavers->SetShowBackGround(true);
        }
        else if (DEV_MSG_EXP_KEYDOWN == refMessage.message)
        {
            UIManager_PutPrivateDlg(pDlgScreensavers);
        }
        // 需求变更，不存在屏保关闭的场景
//      else if (CONFIG_MSG_BCAST_CHANGED == refMessage.message)
//      {
//          if (ST_SCREEN_SAVER_TIME == refMessage.wParam)
//          {
//              if (configParser_GetConfigInt(kszScreenSaverWaitTime) <= 0)
//              {
//                  UIManager_PutPrivateDlg(pDlgScreensavers);
//              }
//          }
//      }
        else if (BLF_MESSAGE_FLUSH == refMessage.message)
        {
            if (configParser_GetConfigInt(kszBLFActiveBackLight) == 1)
            {
                UIManager_PutPrivateDlg(pDlgScreensavers);
            }
        }
        else if (PHONE_MSG_PUSH_XML == refMessage.message)
        {
            UIManager_PutPrivateDlg(pDlgScreensavers);
        }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
        else if (RECORD_STORAGE_STATE_CHANGE == refMessage.message)
        {
            if (0 == refMessage.wParam || 2 == refMessage.wParam || 3 == refMessage.wParam)
            {
                UIManager_PutPrivateDlg(pDlgScreensavers);
            }
        }
#endif
    }

    return FALSE;
}

void CDlgScreenSavers::StatusChangeCallback(STATUS_ACTION eAction)
{
    switch (eAction)
    {
    case STATUS_ACTION_ADD:
    case STATUS_ACTION_UPDATE:
    {
        CBaseDialog * pDlgScreensaver = UIManager_GetPrivateDlg(DLG_CDlgScreenSavers);

        if (NULL != pDlgScreensaver)
        {
            UIManager_PutPrivateDlg(pDlgScreensaver);
        }
    }
    break;
    default:
        break;
    }
}

CBaseDialog * CDlgScreenSavers::CreateInstance()
{
    return new CDlgScreenSavers();
}

void CDlgScreenSavers::StatusNotifyFunction(const YLList<IdleStatusItem> & listStatusList)
{
    IDLEUI_INFO("CDlgScreenSavers::StatueNotifyFunction Change Count:%d.", listStatusList.size());

    CDlgScreenSavers * pDlgScreenSavers = (CDlgScreenSavers *)UIManager_GetPrivateDlg(
            DLG_CDlgScreenSavers);

    if (NULL != pDlgScreenSavers && pDlgScreenSavers == UIManager_GetTopWindow())
    {
        pDlgScreenSavers->OnStatusChange(listStatusList);
    }
}

CDlgScreenSavers::CDlgScreenSavers(QWidget * parent)
    : CBaseDialog(parent)
    , m_ClockLab(this)
    , m_TitleIcons(this)
    , m_labelLoadError(this)
    , m_timerMove(this)
    , m_timerChangeBG(this)
    , m_timerBGOpacity(this)
    , m_strBGPic("")
    , m_strOldBGPic("")
    , m_bFirstSetBGChangeTime(true)
    , m_bFirstSetClockMoveTime(true)
    , m_bShowBackGround(true)
#if SCREEN_SAVE_FEATRUE_CLOCK_MOVING
    , m_bRightwards(true)
    , m_bUpwards(true)
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
    , m_XMLDateTime(this)
    , m_XMLTitleIcons(this)
    , m_labelImage(this)
    , m_bEnterXMLSS(false)
#endif

{
    SetTopWnd();
    SetDlgGeometry(RECT_FULL_SCREEN);
    SetWindowType(eWT_FullScreen);
    setAttribute(Qt::WA_AcceptTouchEvents, true);

    qsrand(time(NULL));

    //RegisterMsg();

    //初始化获取图片
    m_strBGPic = g_pScreenSaverController->GetNextScreenSaverPicPath();

#if SCREEN_SAVE_FEATRUE_CLOCK_MOVING
    //随机设定启动时时钟移动方向
    int nRandomBool = g_pScreenSaverController->GetRandomNumber(SCREENSAVER_BOOL_BOUNDRY);
    if (0 == nRandomBool)
    {
        m_bRightwards = true;
    }
    else
    {
        m_bRightwards = false;
    }

    nRandomBool = g_pScreenSaverController->GetRandomNumber(SCREENSAVER_BOOL_BOUNDRY);
    if (0 == nRandomBool)
    {
        m_bUpwards = true;
    }
    else
    {
        m_bUpwards = false;
    }
#endif

#if IF_FEATURE_XMLB_SCREENSAVER
    m_XMLDateTime.hide();
    m_XMLTitleIcons.hide();
    m_labelImage.hide();
#endif

    InitWigdet();
}

CDlgScreenSavers::~CDlgScreenSavers()
{
#if IF_FEATURE_XMLB_SCREENSAVER
    //如果有进过xml屏保，退出时需要清除数据
    if (m_bEnterXMLSS == true)
    {
        xmlbrowser_Exit();
    }
#endif

    //UnRegisterMsg();
}

void CDlgScreenSavers::InitWigdet()
{
    m_frmBG.setParent(this);
    m_frmBG.setGeometry(SCREENSAVER_DEFAULT_FRAME_RECT);
    m_frmBG.setObjectName("frmBG");

    m_ClockLab.setParent(&m_frmBG);
    m_ClockLab.setGeometry(SCREENSAVER_CLOCK_RECT);
    m_ClockLab.SetNeedShadow(true);

    QFont ClockHMfont;
    ClockHMfont.setPointSize(SCREENSAVER_CLOCK_HM_FONT_SIZE);
    ClockHMfont.setWeight(QFont::Light);
    ClockHMfont.setBold(true);
    m_ClockLab.SetHMFont(ClockHMfont);

    QFont ClockDatefont;
    ClockDatefont.setPointSize(SCREENSAVER_CLOCK_DATE_FONT_SIZE);
    ClockDatefont.setWeight(QFont::Light);
    ClockDatefont.setBold(true);
    m_ClockLab.SetDateFont(ClockDatefont);

    QFont ClockAmPmFont;
    ClockAmPmFont.setPointSize(SCREENSAVER_CLOCK_AMPM_FONT_SIZE);
    ClockAmPmFont.setWeight(QFont::Light);
    ClockAmPmFont.setBold(true);
    m_ClockLab.SetAmPmFont(ClockAmPmFont);

    m_ClockLab.SetIntervalOfHMAndDate(SCREENSAVER_CLOCK_HM_AND_DATE_INTTERVAL);

    ResetClockTimeFmt();

    m_TitleIcons.setParent(&m_frmBG);
    m_TitleIcons.setAttribute(Qt::WA_AcceptTouchEvents, false);
    m_TitleIcons.setObjectName("TitleIcons");

    m_TitleIcons.SetIsAllowTitleStatusClick(false);
    m_TitleIcons.SetGeometry(SCREENSAVER_TITLEICONS_RECT);

    QPalette paletteTitleIcons = m_TitleIcons.palette();
    paletteTitleIcons.setColor(QPalette::WindowText, Qt::white);
    m_TitleIcons.setPalette(paletteTitleIcons);

    m_TitleIcons.RefreshStatusIcon();

    m_labelLoadError.setParent(&m_frmBG);
    m_labelLoadError.setObjectName("labelLoadError");
    m_labelLoadError.setGeometry(SCREENSAVER_PIC_ERROR_RECT);
    m_labelLoadError.setAlignment(Qt::AlignCenter);
    m_labelLoadError.hide();

    QFont fontLoadError;
    fontLoadError.setPointSize(SCREENSAVER_LOADERROR_TEXT_FONT_SIZE);
    m_labelLoadError.setFont(fontLoadError);

    QPalette palettelabelLoadError = m_labelLoadError.palette();
    palettelabelLoadError.setColor(QPalette::WindowText, SCREENSAVER_LOADERROR_TEXT_COLOR);
    m_labelLoadError.setPalette(palettelabelLoadError);

    m_labelLoadError.setText(LANG_TRANSLATE(TRID_IMAGE_LOAD_ERROR));

    connect(&m_timerChangeBG, SIGNAL(timeout()), this, SLOT(OnChangeBGTimeOut()));
    connect(&m_timerMove, SIGNAL(timeout()), this, SLOT(OnMoveTimeOut()));
    connect(&m_timerBGOpacity, SIGNAL(timeout()), this, SLOT(OnChange()));
}

void CDlgScreenSavers::Init()
{
    RegisterMsg();
    connect(g_pScreenSaverController, SIGNAL(ScreenSaverPicChange(bool)), this,
            SLOT(OnScreenSaverPicChange(bool)));
    connect(g_pScreenSaverController, SIGNAL(ScreenSaverClockChange()), this, SLOT(CheckShowClock()));
    connect(g_pScreenSaverController, SIGNAL(ScreenSaverClockFormatChange()), this,
            SLOT(OnClockFormatChange()));
    connect(g_pScreenSaverController, SIGNAL(signalPicIntervalChange()), this,
            SLOT(OnBgIntervalChange()));
    connect(g_pScreenSaverController, SIGNAL(signalClockMoveIntervalChange()), this,
            SLOT(OnMoveTimeIntervalChange()));
    connect(g_pScreenSaverPainter, SIGNAL(ScreenSaverReallyChange()), this, SLOT(OnReallyChangeBG()));
    connect(g_pScreenSaverPainter, SIGNAL(ScreenSaverChangeEnd()), this, SLOT(StopChanging()));
    connect(g_pScreenSaverController, SIGNAL(ScreenSaverModeOrURLChange()), this,
            SLOT(OnScreenSaverModeOrURLChange()));

    if (backlight_GetCurrenLightLevel() <= 0)
    {
        m_bShowBackGround = false;
        m_frmBG.hide();
#if IF_FEATURE_XMLB_SCREENSAVER
        m_frmXML.hide();
#endif
        update();
    }

    idleScreen_SetStatusChangeCallback(PS_STATE_PICKUP_MENU, &CDlgScreenSavers::StatusChangeCallback);
}

void CDlgScreenSavers::Uninit()
{
    UnRegisterMsg();
    disconnect(g_pScreenSaverController, SIGNAL(ScreenSaverPicChange(bool)), this,
               SLOT(OnScreenSaverPicChange(bool)));
    disconnect(g_pScreenSaverController, SIGNAL(ScreenSaverClockChange()), this,
               SLOT(CheckShowClock()));
    disconnect(g_pScreenSaverController, SIGNAL(ScreenSaverClockFormatChange()), this,
               SLOT(OnClockFormatChange()));
    disconnect(g_pScreenSaverController, SIGNAL(signalPicIntervalChange()), this,
               SLOT(OnBgIntervalChange()));
    disconnect(g_pScreenSaverController, SIGNAL(signalClockMoveIntervalChange()), this,
               SLOT(OnMoveTimeIntervalChange()));
    disconnect(g_pScreenSaverPainter, SIGNAL(ScreenSaverReallyChange()), this,
               SLOT(OnReallyChangeBG()));
    disconnect(g_pScreenSaverPainter, SIGNAL(ScreenSaverChangeEnd()), this, SLOT(StopChanging()));
    disconnect(g_pScreenSaverController, SIGNAL(ScreenSaverModeOrURLChange()), this,
               SLOT(OnScreenSaverModeOrURLChange()));

    _MainWND.SetTimerLock(false);

    g_pScreenSaverPainter->ResetChanging();

    idleScreen_SetStatusChangeCallback(PS_STATE_PICKUP_MENU, NULL);
}

void CDlgScreenSavers::showEvent(QShowEvent * e)
{
    _MainWND.SetTimerLock(true);

    if (IsStartChangePic())
    {
        StartBGChangeTimer();
    }
    else
    {
        StopBGChangeTimer();
    }

    CheckShowClock();

    if (IsShowClock())
    {
        StartMoveTimer();
    }
    else
    {
        StopMoveTimer();
    }
}

void CDlgScreenSavers::hideEvent(QHideEvent * e)
{
    _MainWND.SetTimerLock(false);
    StopBGChangeTimer();
    StopMoveTimer();
    StopChanging();
}

void CDlgScreenSavers::StartBGChangeTimer()
{
    StopBGChangeTimer();
    m_timerChangeBG.start(g_pScreenSaverController->GetBGChangeTime());
}

void CDlgScreenSavers::StopBGChangeTimer()
{
    if (m_timerChangeBG.isActive())
    {
        m_timerChangeBG.stop();
    }
}

void CDlgScreenSavers::StartMoveTimer()
{
    StopMoveTimer();
    m_timerMove.start(g_pScreenSaverController->GetClockMoveTime());
}

void CDlgScreenSavers::StopMoveTimer()
{
    if (m_timerMove.isActive())
    {
        m_timerMove.stop();
    }
}

bool CDlgScreenSavers::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL || pEvent == NULL)
    {
        return false;
    }

    QEvent::Type eEventType = pEvent->type();
    switch (eEventType)
    {
    case QEvent::KeyPress:
    {
        QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);

        int nkey = pKeyEvent->key();
        IDLEUI_INFO("CDlgScreenSavers::eventFilter, nkey = %d", nkey);

        //当按键为数字键和*#时需要进入拨号界面
        if ((PHONE_KEY_0 <= nkey && nkey <= PHONE_KEY_9)
                || PHONE_KEY_STAR == nkey
                || PHONE_KEY_POUND == nkey
                //免提和手柄处理在全局处，比这里优先
//              || PHONE_KEY_HANDFREE == nkey
//              || PHONE_KEY_HANDSET_OFF_HOOK == nkey
                //耳机也在全局有处理，因此导致在屏保界面按耳机，耳机灯会亮，和其他按键行为不统一
           )
        {
            pKeyEvent->accept();
            UIManager_ReturnToIdle();
            inputHook_ApplicationSendKey(pKeyEvent->key(), true);
            return true;
        }
        // 产品部：挂机键也是按键，与节电统一，退出当前界面
//          else if (PHONE_KEY_HANDSET_ON_HOOK == nkey)
//          {
//              return true;
//          }
#if SCREEN_SAVE_FEATURE_TEST
        //测试用，按redial进行强行切换
        else if (PHONE_KEY_REDIAL == pKeyEvent->key())
        {
            OnMoveTimeOut();
            OnScreenSaverPicChange(false);
            return true;
        }
#endif
        else
        {
            UIManager_PutPrivateDlg(this);
            return true;
        }
    }
    break;
    case QEvent::MouseButtonPress:
    {
        IDLEUI_INFO("CDlgScreenSavers::eventFilter, MouseButtonPress");
        UIManager_PutPrivateDlg(this);
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}

void CDlgScreenSavers::OnStatusChange(YLList<IdleStatusItem> listStatusList)
{
    //XML屏保：即使没弹框，账号、DND、FWD等状态变化也退出屏保
#if 0//IF_FEATURE_XMLB_SCREENSAVER
    CBaseDialog * pScreenSavers = UIManager_GetPrivateDlg(DLG_CDlgScreenSavers);
    if (NULL != pScreenSavers)
    {
        UIManager_ReturnToIdle();
        return;
    }
#endif

#if IF_FEATURE_XMLB_SCREENSAVER
    m_XMLTitleIcons.RefreshStatusIcon(listStatusList);
#endif
    m_TitleIcons.RefreshStatusIcon(listStatusList);
}

QPixmap CDlgScreenSavers::GetCustomBackGround()
{
    RefreshBackGroundPic();

    if (m_pmBG.isNull())
    {
        IDLEUI_INFO("CDlgScreenSavers::GetCustomBackGround fail, show LoadError text.");
#if IF_FEATURE_XMLB_SCREENSAVER
        m_frmXML.hide();
        m_frmBG.show();
#endif
        m_labelLoadError.show();
        m_ClockLab.hide();
        m_TitleIcons.hide();
    }
    else
    {
#if IF_FEATURE_XMLB_SCREENSAVER
        SCREENSAVER_MODE_CHANGE_TYPE eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
                kszScreenSaverType);

        if (eSSType == SCREENSAVER_MODE_CHANGE_TYPE_XMLB)
        {
            if (!m_bEnterXMLSS)
            {
                //容错，处于屏保模式时xml的URL为空则不显示。
                m_frmXML.hide();
                m_frmBG.show();
            }
            else if (!m_bShowBackGround)
            {
                IDLEUI_INFO("CDlgScreenSavers::GetCustomBackGround m_frmXML.hide()\n");
                m_frmXML.hide();
                m_frmBG.hide();
            }
            else
            {
                IDLEUI_INFO("CDlgScreenSavers::GetCustomBackGround m_frmXML.show()\n");
                m_frmXML.show();
                m_frmBG.hide();
            }
        }
        else
#endif
        {
#if IF_FEATURE_XMLB_SCREENSAVER
            m_frmXML.hide();
#endif
            m_labelLoadError.hide();
            m_ClockLab.show();
            m_TitleIcons.show();
        }

    }


    return m_pmBG;
}

void CDlgScreenSavers::OnClockFormatChange()
{
    ResetClockTimeFmt();
}

void CDlgScreenSavers::OnMoveTimeOut()
{
    if (!IsShowClock())
    {
        return;
    }

    m_bFirstSetClockMoveTime = true;

#if !SCREEN_SAVE_FEATRUE_CLOCK_MOVING
    ClockChangeToRandomRect();
#else
    //根据现在移动方向计算下一次的位置，如果超出边缘限制就把方向置反
    QRect rctfrmBG = m_frmBG.geometry();
    int nLeft = rctfrmBG.left();
    int nTop = rctfrmBG.top();

    if (m_bRightwards)
    {
        if (nLeft >= SCREENSAVER_MAX_FRAME_LEFT)
        {
            nLeft = SCREENSAVER_MAX_FRAME_LEFT;
            m_bRightwards = false;
        }
        else
        {
            nLeft += SCREENSAVER_FRAME_MOVE_STEP;
        }
    }
    else
    {
        if (nLeft <= SCREENSAVER_MIN_FRAME_LEFT)
        {
            nLeft = SCREENSAVER_MIN_FRAME_LEFT;
            m_bRightwards = true;
        }
        else
        {
            nLeft -= SCREENSAVER_FRAME_MOVE_STEP;
        }
    }

    if (!m_bUpwards)
    {
        if (nTop >= SCREENSAVER_MAX_FRAME_TOP)
        {
            nTop = SCREENSAVER_MAX_FRAME_TOP;
            m_bUpwards = true;
        }
        else
        {
            nTop += SCREENSAVER_FRAME_MOVE_STEP;
        }
    }
    else
    {
        if (nTop <= SCREENSAVER_MIN_FRAME_TOP)
        {
            nTop = SCREENSAVER_MIN_FRAME_TOP;
            m_bUpwards = false;
        }
        else
        {
            nTop -= SCREENSAVER_FRAME_MOVE_STEP;
        }
    }

    rctfrmBG.moveTo(nLeft, nTop);
    m_frmBG.setGeometry(rctfrmBG);
#endif
}

void CDlgScreenSavers::ClockChangeToRandomRect()
{
    if (!IsShowClock())
    {
        return;
    }

    QRect rctfrmBG;

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
    //从固定的数个位置中选择一个
    rctfrmBG = setGeometry(g_pScreenSaverController->GetNextScreenSaverRect());
#else
    //从随机位置中选择一个
    rctfrmBG = m_frmBG.geometry();
    int nLeft = SCREENSAVER_MIN_FRAME_LEFT + g_pScreenSaverController->GetRandomNumber(
                    SCREENSAVER_MAX_FRAME_LEFT - SCREENSAVER_MIN_FRAME_LEFT);
    int nTop = SCREENSAVER_MIN_FRAME_TOP + g_pScreenSaverController->GetRandomNumber(
                   SCREENSAVER_MAX_FRAME_TOP - SCREENSAVER_MIN_FRAME_TOP);

    IDLEUI_INFO("CDlgScreenSavers::ClockChangeToRandomRect, nLeft = %d, nTop = %d", nLeft, nTop);

    rctfrmBG.moveTo(nLeft, nTop);
#endif

    m_frmBG.setGeometry(rctfrmBG);

    StartMoveTimer();
}

void CDlgScreenSavers::OnChangeBGTimeOut()
{
    m_bFirstSetBGChangeTime = true;

    //如果正在进行切换则不允许再次切换
    if (!g_pScreenSaverPainter->IsBeChanging())
    {
        IDLEUI_INFO("CDlgScreenSavers::OnChangeBGTimeOut, start change!");
        StartChanging();
    }
    else
    {
        IDLEUI_INFO("CDlgScreenSavers::OnChangeBGTimeOut, is in changing!");
    }
}

void CDlgScreenSavers::OnReallyChangeBG()
{
    IDLEUI_INFO("CDlgScreenSavers::OnReallyChangeBG");

    m_strOldBGPic = m_strBGPic;

    //到control模块去获取下一张背景图片
    m_strBGPic = g_pScreenSaverController->GetNextScreenSaverPicPath();

    UIManager_UpdateWnd(UIManager_GetTopWindow());
}

void CDlgScreenSavers::OnBgIntervalChange()
{
    StartBGChangeTimer();
}

void CDlgScreenSavers::OnMoveTimeIntervalChange()
{
    StartMoveTimer();
}

void CDlgScreenSavers::OnScreenSaverPicChange(bool bStopTimer)
{
    IDLEUI_INFO("CDlgScreenSavers::OnScreenSaverPicChange");

    if (bStopTimer)
    {
        StopBGChangeTimer();
    }
    else
    {
        StartBGChangeTimer();
    }

    //当配置改变时，需要强制改变背景，所以强行调用背景变换时间到的槽
    //如果已经在进行透明度变化
    if (g_pScreenSaverPainter->IsBeChanging())
    {
        IDLEUI_INFO("CDlgScreenSavers::OnScreenSaverPicChange, IsBeChanging");
        //如果正处于透明度增加的状态，说明还没有去获取图片
        //等到透明度到达最大时就会去获取下一张图片，因此此处不需要进行任何操作
        if (g_pScreenSaverPainter->IsBeEntering())
        {
            IDLEUI_INFO("CDlgScreenSavers::OnScreenSaverPicChange, IsBeEntering");
        }
        //如果处于透明度减少的状态
        //即图片已经切换过了，则切换到Intering状态继续变化
        else
        {
            IDLEUI_INFO("CDlgScreenSavers::OnScreenSaverPicChange, not Entering and Change to Entering.");
            g_pScreenSaverPainter->SetToEntering(true);
        }
    }
    //如果没有正在进行透明度变化，则可以直接进行背景切换
    else
    {
        IDLEUI_INFO("CDlgScreenSavers::OnScreenSaverPicChange, change now");
        OnChangeBGTimeOut();
    }
}

void CDlgScreenSavers::SetShowBackGround(bool bShow)
{
    if (bShow == m_bShowBackGround)
    {
        return;
    }

    m_bShowBackGround = bShow;

    CheckShowClock();

    update();
}

void CDlgScreenSavers::paintEvent(QPaintEvent * p)
{
    QPainter qbgpainter(this);

    // 如果背光灯关闭，则绘制黑色图片
    if (!m_bShowBackGround)
    {
        qbgpainter.fillRect(0, 0, PHONE_WIDTH, PHONE_HEIGHT, Qt::black);
    }
    else if (g_pScreenSaverPainter->IsBeChanging())
    {
        g_pScreenSaverPainter->PaintScreenSaverChanging(qbgpainter);
    }
}

void CDlgScreenSavers::StopChanging()
{
    if (m_timerBGOpacity.isActive())
    {
        m_timerBGOpacity.stop();
    }
}

void CDlgScreenSavers::StartChanging()
{
    SCREENSAVER_MODE_CHANGE_TYPE eSSPicType = g_pScreenSaverController->GetSSPicType();

    IDLEUI_INFO("CDlgScreenSavers::StartChanging, nUseSysPic = %d", eSSPicType);

    //根据产品部意见，当使用系统图片的时候，又只有一张图片，不进行“切换”行为
    if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == eSSPicType)
    {
        StopBGChangeTimer();
        return;
    }
    else
    {
        //重置计时器
        StartBGChangeTimer();
    }

    //开始变化
    StopChanging();
    m_timerBGOpacity.start(SCREENSAVER_OPACITY_CHANGE_INTERVAL);

    g_pScreenSaverPainter->StartChanging();

    update();
}

void CDlgScreenSavers::OnChange()
{
    update();
}

void CDlgScreenSavers::OnScreenSaverPicChangeByFirst()
{
    if (m_bFirstSetBGChangeTime)
    {
        OnScreenSaverPicChange(false);
        m_bFirstSetBGChangeTime = false;
    }
}

void CDlgScreenSavers::OnScreenSaverClockMoveByFirst()
{
    if (m_bFirstSetClockMoveTime)
    {
        ClockChangeToRandomRect();
        m_bFirstSetClockMoveTime = false;
    }
}

void CDlgScreenSavers::CheckShowClock()
{
#if IF_FEATURE_XMLB_SCREENSAVER
    SCREENSAVER_MODE_CHANGE_TYPE eUseSysPic = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
                kszScreenSaverType);
    if (SCREENSAVER_MODE_CHANGE_TYPE_XMLB == eUseSysPic)
    {
        IDLEUI_INFO("CDlgScreenSavers::CheckShowClock m_bEnterXMLSS[%d] m_bShowBackGround[%d]\n",
                    m_bEnterXMLSS, m_bShowBackGround);

        if (m_bEnterXMLSS && m_bShowBackGround)
        {
            m_frmBG.hide();
            m_frmXML.show();
        }
        else if (!m_bShowBackGround)
        {
            //防止点亮屏保时闪现屏保界面
            m_frmXML.hide();
        }
        else
        {
            yl::string strURL = configParser_GetConfigString(kszXmlBrowserScreenSaverURL);
            if (!strURL.empty())
            {
                xmlbrowser_ScreenSaverEnter(strURL.c_str());
            }
        }
    }
    else if (m_bShowBackGround && IsShowClock())
    {
        m_frmBG.show();
        m_frmXML.hide();
    }
    else
    {
        m_frmBG.hide();
        m_frmXML.hide();
    }
#else
    if (IsShowClock() && m_bShowBackGround)
    {
        m_frmBG.show();
    }
    else
    {
        m_frmBG.hide();
    }
#endif
}

bool CDlgScreenSavers::IsShowClock()
{
#if IF_FEATURE_XMLB_SCREENSAVER
    SCREENSAVER_MODE_CHANGE_TYPE eUseSysPic = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
                kszScreenSaverType);
    if (SCREENSAVER_MODE_CHANGE_TYPE_XMLB == eUseSysPic)
    {
        return false;
    }
#endif
    return g_pScreenSaverController->IsShowScreensaverClock();
}

bool CDlgScreenSavers::IsStartChangePic()
{
    bool bResult = false;

    // 根据产品部意见，当使用系统图片时，只有一张图片,不进行“切换”行为
#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    SCREENSAVER_MODE_CHANGE_TYPE eUseSysPic = g_pScreenSaverController->GetSSPicType();

    if (SCREENSAVER_MODE_CHANGE_TYPE_SYS != eUseSysPic
            && SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC != eUseSysPic)
    {
        bResult = true;
    }
#endif

    return bResult;
}

void CDlgScreenSavers::ResetClockTimeFmt()
{
#if IF_FEATURE_XMLB_SCREENSAVER
    m_XMLDateTime.SetTimeFmtType(Setting_GetCurDateFormatType(), Setting_GetCurTimeFormatType());
#endif

    m_ClockLab.SetTimeFmtType(Setting_GetCurDateFormatType(), Setting_GetCurTimeFormatType());
}

void CDlgScreenSavers::RegisterMsg()
{
    // 需求变更，不存在屏保关闭的场景
//  etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CDlgScreenSavers::OnMessage);
    etl_RegisterMsgHandle(BACKLIGHT_LEVEL_CHANGED_MSG, BACKLIGHT_LEVEL_CHANGED_MSG,
                          &CDlgScreenSavers::OnMessage);
    etl_RegisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, &CDlgScreenSavers::OnMessage);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE,
                          &CDlgScreenSavers::OnMessage);
#endif
    etl_RegisterMsgHandle(BLF_MESSAGE_FLUSH, BLF_MESSAGE_FLUSH, &CDlgScreenSavers::OnMessage);
    etl_RegisterMsgHandle(PHONE_MSG_PUSH_XML, PHONE_MSG_PUSH_XML, &CDlgScreenSavers::OnMessage);
    etl_RegisterMsgHandle(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVINGMEG_SCREEN_AWAKE, &CDlgScreenSavers::OnMessage);

}

void CDlgScreenSavers::UnRegisterMsg()
{
//  etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CDlgScreenSavers::OnMessage);
    etl_UnregisterMsgHandle(BACKLIGHT_LEVEL_CHANGED_MSG, BACKLIGHT_LEVEL_CHANGED_MSG,
                            &CDlgScreenSavers::OnMessage);
    etl_UnregisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, &CDlgScreenSavers::OnMessage);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE,
                            &CDlgScreenSavers::OnMessage);
#endif
    etl_UnregisterMsgHandle(BLF_MESSAGE_FLUSH, BLF_MESSAGE_FLUSH, &CDlgScreenSavers::OnMessage);
    etl_UnregisterMsgHandle(PHONE_MSG_PUSH_XML, PHONE_MSG_PUSH_XML, &CDlgScreenSavers::OnMessage);
    etl_UnregisterMsgHandle(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVINGMEG_SCREEN_AWAKE, &CDlgScreenSavers::OnMessage);
}

void CDlgScreenSavers::RefreshBackGroundPic()
{
    if (m_strBGPic.empty() || m_strBGPic == m_strOldBGPic)
    {
        return;
    }

    IDLEUI_INFO("CDlgScreenSavers::GetCustomBackGround, m_strBGPic = %s", m_strBGPic.c_str());

    Image_LoadPixmapByPath(m_pmBG, m_strBGPic.c_str());

    if (m_pmBG.width() != PHONE_WIDTH || m_pmBG.height() != PHONE_HEIGHT)
    {
        m_pmBG = m_pmBG.scaled(PHONE_WIDTH, PHONE_HEIGHT);
    }
}
#if IF_FEATURE_XMLB_SCREENSAVER
//初始化xmlbrowser下载的图片
bool CDlgScreenSavers::InitXmlImage(XBScreenSaverData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    m_bEnterXMLSS = true;

    m_frmXML.setParent(this);
    m_frmXML.setObjectName("frmXML");
    m_frmXML.setGeometry(SCREENSAVER_XML_FRAME_RECT);

    int iInserImageLineNum = pData->stAttr.iInserImageLineNum;

    ImageItem * pImageItem = pData->listImage.front();
    if (pImageItem == NULL)
    {
        return false;
    }

    int iX = 0; //图片X坐标
    int iY = 0; //图片Y坐标
    //初始化line
    int iInsertImageY = InitLineAlone(pData, iInserImageLineNum, pImageItem->uHeight);
    //设置时间及状态图标
    InitTimeIcon(pData);

    if (pImageItem->eVertical == IMAGE_VERTICAL_ALIGN_CENTER)
    {
        iY = PHONE_HEIGHT / 2 - pImageItem->uHeight / 2;
    }
    else if (pImageItem->eVertical == IMAGE_VERTICAL_ALIGN_BOTTOM)
    {
        iY = PHONE_HEIGHT - pImageItem->uHeight;
    }

    if (pImageItem->eHorizontal == IMAGE_HORIZONTAL_ALIGN_CENTER)
    {
        iX = PHONE_WIDTH / 2 - pImageItem->uWidth / 2;
    }
    else if (pImageItem->eHorizontal == IMAGE_HORIZONTAL_ALIGN_RIGHT)
    {
        iX = PHONE_WIDTH - pImageItem->uWidth;
    }

    if (0 != iInserImageLineNum)
    {
        //图片插入第几行文字之上重新获取过Y坐标
        iY = iInsertImageY;
    }

    m_labelImage.setParent(&m_frmXML);
    m_labelImage.setObjectName("labelImage");
    m_labelImage.setGeometry(QRect(iX, iY, pImageItem->uWidth, pImageItem->uHeight));
    m_labelImage.setAlignment(Qt::AlignCenter);

    m_labelImage.setScaledContents(true);

    QPixmap pixCurrent;
    pixCurrent.load(pImageItem->strImagePath.c_str());
    m_labelImage.setPixmap(pixCurrent);

    m_labelImage.show();
    m_labelImage.lower();

    return true;
}

//初始化xmlbrowser模式相关控件
bool CDlgScreenSavers::InitXmlWigdet(void * pData)
{
    XBScreenSaverData * pNewData = static_cast<XBScreenSaverData *>(pData);
    if (pNewData == NULL)
    {
        return false;
    }
    m_frmXML.hide();
    //文字label清空，防止line数据为空时直接在屏保切换还显示之前line问题
    ClearLineList();

    //存在图片的处理
    if (pNewData->listImage.size() != 0)
    {
        ImageItem * pImageItem = pNewData->listImage.front();
        if (pImageItem != NULL)
        {
            switch (pImageItem->eLoadStatus)
            {
            case ILS_DOWNLOAD:
            case ILS_COMPRESS:
            {
                //下载和解压则返回不处理等下载完成再统一加载界面内容
                return false;
            }
            case ILS_LOAD_OK:
            {
                return InitXmlImage(pNewData);
            }
            default:
                // 其他情况则接下去处理，不加载图片
                break;
            }
        }
    }

    m_bEnterXMLSS = true;

    m_frmXML.setParent(this);
    m_frmXML.setGeometry(SCREENSAVER_XML_FRAME_RECT);

    //设置时间及状态图标
    InitTimeIcon(pNewData);
    //文字设置：
    InitLineAlone(pNewData);

    //无图片则隐藏图片控件
    //http://bugfree.yealink.com/Bug.php?BugID=116336
    m_labelImage.hide();

    return true;
}
bool CDlgScreenSavers::InitTimeIcon(XBScreenSaverData * pSSData)
{
    if (pSSData == NULL)
    {
        return false;
    }

    m_XMLDateTime.hide();
    m_XMLTitleIcons.hide();
    //显示时间
    if (pSSData->stSysTime.bShow)
    {
        // 时间设置
        m_XMLDateTime.setParent(&m_frmXML);
        m_XMLDateTime.setObjectName("XMLDateTime");
        m_XMLDateTime.setGeometry(GetXMLTimeRect(pSSData->stSysTime.eVertical,
                                  pSSData->stSysTime.eHorizontal));
        int iFontSize = TypeToFontPointSize(pSSData->stSysTime.eFont);
        //最大字体是否会出现抖动问题,需要降低一级显示
        qWidgetUtility::setFontStyle(&m_XMLDateTime, iFontSize > 24 ? 24 : iFontSize);
        // 设置lab上的字的颜色
        QPalette pal = palette();
        pal = m_XMLDateTime.palette();
        if (!pSSData->stSysTime.strRGB.empty())
        {
            pal.setColor(QPalette::WindowText, GetChangeToRGB(pSSData->stSysTime.strRGB));
        }
        else
        {
            pal.setColor(QPalette::WindowText, TypeToColor(pSSData->stSysTime.eColor));
        }
        m_XMLDateTime.setPalette(pal);
        SetXMLTimeAlignment(pSSData->stSysTime.eHorizontal);
        m_XMLDateTime.show();
    }

    //显示状态图标
    if (pSSData->stStatusIcon.bShow)
    {
        // 状态图标设置
        m_XMLTitleIcons.setParent(&m_frmXML);
        m_XMLTitleIcons.setAttribute(Qt::WA_AcceptTouchEvents, false);

        m_XMLTitleIcons.SetGeometry(GetXMLStatusIconRect(pSSData->stStatusIcon.eVertical,
                                    pSSData->stStatusIcon.eHorizontal));

        QPalette paletteTitleIcons = m_XMLTitleIcons.palette();
        paletteTitleIcons.setColor(QPalette::WindowText, Qt::white);
        m_XMLTitleIcons.setPalette(paletteTitleIcons);
        SetXMLStatusIconAlignment(pSSData->stStatusIcon.eHorizontal);
        m_XMLTitleIcons.RefreshStatusIcon();
        m_XMLTitleIcons.show();
    }

    return true;
}

int CDlgScreenSavers::InitLineAlone(XBScreenSaverData * pData, int iImageInsertLine/* = 0*/,
                                    int iImageHeight/* = 0*/)
{
    if (pData == NULL)
    {
        return 0;
    }
    if (pData->listLine.size() == 0)
    {
        return 0;
    }

    int iLineSpacing = pData->stAttr.iLineSpacing; //行间距
    int iLineH = TITLE_HEIGHT;//行高

    bool bTopLineUsed = false; //屏幕第一行是否占用
    bool bBottomLineUsed = false; //屏幕最后一行是否占用

    if (pData->stSysTime.bShow)
    {
        if (pData->stSysTime.eVertical == IMAGE_VERTICAL_ALIGN_TOP)
        {
            bTopLineUsed = true;
        }
        else
        {
            bBottomLineUsed = true;
        }
    }

    if (pData->stStatusIcon.bShow)
    {
        if (pData->stStatusIcon.eVertical == IMAGE_VERTICAL_ALIGN_TOP)
        {
            bTopLineUsed = true;
        }
        else
        {
            bBottomLineUsed = true;
        }
    }

    int iLastY = bBottomLineUsed ? PHONE_HEIGHT - iLineH : PHONE_HEIGHT;//最大Y坐标
    int iStartY = bTopLineUsed ? (iLineH + iLineSpacing) : 0;//起始Y坐标
    int iImageY = 0;
    bool bInserImage = false;

    List_Line_Zone::iterator itLine = pData->listLine.begin();
    for (int iCount = 0; itLine != pData->listLine.end(); ++itLine, ++iCount)
    {
        LineData * pLineData = (*itLine);
        if (pLineData == NULL)
        {
            continue;
        }

        if (iImageInsertLine == iCount + 1)
        {
            iImageY = (iCount == 0) ? iStartY : iStartY + iLineSpacing + iLineH;
            bInserImage = true;
        }

        //Y轴坐标为上个坐标加是行高和行间距
        if (iCount != 0 && !bInserImage)
        {
            iStartY = iStartY + iLineSpacing + iLineH;
        }
        else if (bInserImage)
        {
            bInserImage = false;
            iStartY = iImageY + iLineSpacing + iImageHeight;
        }

        if (iStartY + iLineH > iLastY)
        {
            //已经显示不下新的一行则退出
            break;
        }

        //配置line文字
        QLabel * pTempLine = new QLabel(this);
        pTempLine->setObjectName(QString("TempLine%1").arg(iCount));
        pTempLine->setParent(&m_frmXML);
        pTempLine->setAlignment(TypeToAlignmentFlag(pLineData->iAlign));
        qWidgetUtility::setFontStyle(pTempLine, TypeToFontPointSize(pLineData->eFont));
        // 设置lab上的字的颜色
        QPalette pal = pTempLine->palette();
        if (!pLineData->strRGB.empty())
        {
            pal.setBrush(QPalette::WindowText, QBrush(GetChangeToRGB(pLineData->strRGB)));
        }
        else
        {
            pal.setBrush(QPalette::WindowText, QBrush(TypeToColor(pLineData->eColor)));
        }
        pTempLine->setPalette(pal);
        pTempLine->setGeometry(QRect(5, iStartY, PHONE_WIDTH - 10, iLineH));
        pTempLine->setText(QString::fromUtf8(pLineData->strText.c_str()));
        pTempLine->show();
        m_listLine.push_back(pTempLine);
    }

    //如果插入的行数大于实际最大行数则图片放在最后
    if (iImageInsertLine > pData->listLine.size())
    {
        iImageY = iStartY + iLineSpacing + iLineH;
    }

    return iImageY;

}

QRect CDlgScreenSavers::GetXMLTimeRect(IMAGE_VERTICAL_ALIGN eVertical,
                                       IMAGE_HORIZONTAL_ALIGN eHorizontal)
{
    int iLineH = TITLE_HEIGHT;//行高
    int iWith = 300;//行宽
    int iX = 0;
    int iY = IMAGE_VERTICAL_ALIGN_BOTTOM == eVertical ? PHONE_HEIGHT - iLineH : 0;

    if (IMAGE_HORIZONTAL_ALIGN_RIGHT == eHorizontal)
    {
        iX = PHONE_WIDTH - iWith;
    }
    else if (IMAGE_HORIZONTAL_ALIGN_CENTER == eHorizontal)
    {
        iX = PHONE_WIDTH / 2 - iWith / 2;
    }

    return QRect(iX, iY, iWith, iLineH);
}
QRect CDlgScreenSavers::GetXMLStatusIconRect(IMAGE_VERTICAL_ALIGN eVertical,
        IMAGE_HORIZONTAL_ALIGN eHorizontal)
{
    int iLineH = TITLE_HEIGHT;//行高
    int iWith = PHONE_WIDTH / 2; //行宽
    int iX = 0;
    int iY = IMAGE_VERTICAL_ALIGN_BOTTOM == eVertical ? PHONE_HEIGHT - iLineH : 0;

    if (IMAGE_HORIZONTAL_ALIGN_RIGHT == eHorizontal)
    {
        iX = PHONE_WIDTH - iWith;
    }
    else if (IMAGE_HORIZONTAL_ALIGN_CENTER == eHorizontal)
    {
        iX = PHONE_WIDTH / 2 - iWith / 2;
    }

    return QRect(iX, iY, iWith, iLineH);
}

void CDlgScreenSavers::SetXMLTimeAlignment(IMAGE_HORIZONTAL_ALIGN eHorizontal)
{
    if (IMAGE_HORIZONTAL_ALIGN_RIGHT == eHorizontal)
    {
        m_XMLDateTime.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    else if (IMAGE_HORIZONTAL_ALIGN_CENTER == eHorizontal)
    {
        m_XMLDateTime.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    else
    {
        m_XMLDateTime.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
}

void CDlgScreenSavers::SetXMLStatusIconAlignment(IMAGE_HORIZONTAL_ALIGN eHorizontal)
{
    if (IMAGE_HORIZONTAL_ALIGN_RIGHT == eHorizontal)
    {
        m_XMLTitleIcons.SetIconAlign(CStatusBtnBar::STATUS_BTN_ALIGN_RIGHT);
    }
    else if (IMAGE_HORIZONTAL_ALIGN_CENTER == eHorizontal)
    {
        m_XMLTitleIcons.SetIconAlign(CStatusBtnBar::STATUS_BTN_ALIGN_CENTER);
    }
    else
    {
        m_XMLTitleIcons.SetIconAlign(CStatusBtnBar::STATUS_BTN_ALIGN_LEFT);
    }
}

//清空m_listLine内容
void CDlgScreenSavers::ClearLineList()
{
    if (m_listLine.size() > 0)
    {
        YLList<QLabel *>::iterator iter = m_listLine.begin();
        for (int i = 0; iter != m_listLine.end() && m_listLine.size() > 0; ++iter, ++i)
        {
            QLabel * pTempLine = (*iter);
            if (pTempLine != NULL)
            {
                pTempLine->deleteLater();
                pTempLine = NULL;
            }
        }
        m_listLine.clear();
    }
}

QColor CDlgScreenSavers::GetChangeToRGB(yl::string strRGB)
{
    int iRed, iGreen, iBlue;
    strRGB.trim_both();
    strRGB.to_upper();
    if (sscanf(strRGB.c_str(), "%d,%d,%d", &iRed, &iGreen, &iBlue) == 3)
    {
        return QColor(iRed, iGreen, iBlue);
    }
    else if (sscanf(strRGB.c_str(), "RGB(%d,%d,%d)", &iRed, &iGreen, &iBlue) == 3)
    {
        return QColor(iRed, iGreen, iBlue);
    }
    else
    {
        return QColor(255, 255, 255);
    }
}

#endif //#if IF_FEATURE_XMLB_SCREENSAVER    

void CDlgScreenSavers::OnScreenSaverModeOrURLChange()
{
#if IF_FEATURE_XMLB_SCREENSAVER
    if (m_bEnterXMLSS)
    {
        m_bEnterXMLSS = false;
    }

    CDlgScreenSavers * pDlgScreensavers = (CDlgScreenSavers *)UIManager_GetPrivateDlg(
            DLG_CDlgScreenSavers);

    if (NULL != pDlgScreensavers && pDlgScreensavers == UIManager_GetTopWindow())
    {
        yl::string strURL = configParser_GetConfigString(kszXmlBrowserScreenSaverURL);
        SCREENSAVER_MODE_CHANGE_TYPE eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
                kszScreenSaverType);
        if (SCREENSAVER_MODE_CHANGE_TYPE_XMLB == eSSType && strURL.empty())
        {
            //将屏保模式设置为system模式
            configParser_SetConfigInt(kszScreenType, 0, CONFIG_LEVEL_PHONE);
            etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SCREEN_SAVER_MODE, 0);

        }
    }
#endif
}
