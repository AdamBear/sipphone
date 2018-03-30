#include "qtcommon/qmisc.h"
#if LINUX_SYSTEM
#include <qwindowsystem_qws.h>
#endif

// mod-UIManager
#include "uimanager/uimanager_header.h"
#include "uimanager/uimanager_inc.h"
#include "uimanager/basedialog.h"
#include "uimanager/rectdefine.h"
#include "baseui/transwidgetbgcontroler.h"
#include "uikernel/qcomboboxutility.h"
// mod-this(MainWnd)
#include "autovolumebar.h"
#include "mainwnd.h"

// mod-globalInputHook
#include "keymap.h"

// mod-imageManager
#include "picpath.h"
#include "imagemanager/modimagemanager.h"

// mod-UIKernel
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "baseui/maskframe.h"
#include "uicommon.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/languagehelper.h"
#include "baseui/t4xpicpath.h"

// mod-UI Manager
#include "uimanager/modlogicuimanager.h"

// mod-phoneDevice
#include "devicelib/phonedevice.h"

// 背光灯模块
#include "lamp/backlight/include/modbacklight.h"

#include "interfacedefine.h"
#include "mainwndwgtdelegatebase.h"
#include "uicommon/messagebox/modmessagebox.h"

#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "dsskeyui/src/dsskeybtnbase.h"

#include "edkui/modedkui.h"
#include "inputmethod/inputmethod.h"

Q_DECLARE_METATYPE(TitleLayoutInfo);

// 测试模式的开关
static const bool TEST_MODE = false;
static const int s_ncheckStateTime = 60 * 1000;

namespace mainwnd
{
CBackGroundFrame::CBackGroundFrame()
{
    setObjectName("CBackGroundFrame");
}

CBackGroundFrame::~CBackGroundFrame()
{

}

void CBackGroundFrame::SetBackGroundPixMap(const QPixmap & pixmapBack)
{
    m_pixmapBack = pixmapBack;
}


void CBackGroundFrame::paintEvent(QPaintEvent * pEvent)
{
    QPainter painter(this);

    painter.fillRect(rect(), QBrush(m_pixmapBack));
}
}

// 获取全局唯一的主窗口对象
CMainWnd & GetMainWndInstance()
{
    // 用函数内static方式控制全局对象的构造顺序
    static CMainWnd s_MainWnd;
    return s_MainWnd;
}

void CMainWnd::OnTopWndChangeCallbackFun(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop)
{
    // 顶层窗口改变，则隐藏blackframe
    _MainWND.HideBlackFrame();
    _MainWND.ResetDownSoftkey();
    if (FullKeyboardEnabled())
    {
        _MainWND.HideFullKeyboard();
    }
}

////////////////////////////////////////////////////////////////////////////////
CMainWnd::CMainWnd()
    : CBaseMainWnd()
    , m_pVolumeBar(NULL)
    , m_pMaskFrame(NULL)
    , m_pTitleDelegate(NULL)
    , m_pSoftkeyDelegate(NULL)
    , m_pDsskeyDelegate(NULL)
    , m_pHomeAndBackBtnDelegate(NULL)
    , m_pBlackFrame(NULL)
    , m_bIsTimerLock(false)
{
    this->setObjectName("CMainWnd");
#ifdef WIN32
    this->setWindowTitle("dskPhone");
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif
    // 初始化控件对象
    InitWidget();

    // 设置窗口位置和大小
    SetWndSize();

    m_pCheckTimer = new QTimer(this);
    if (NULL != m_pCheckTimer)
    {
        connect(m_pCheckTimer, SIGNAL(timeout()), this, SLOT(CheckStateTimer()));
    }

    UIManager_RegisterTopWndChangeCallBack(CMainWnd::OnTopWndChangeCallbackFun);

    if (FullKeyboardEnabled())
    {
        inputMethod_RegisterKeyBoardShowHookProc(CMainWnd::MainWndKeyBoardShowCallbackFun,
                KEYBOARD_SHOW_HOOK_MODULE_MAINWND);
    }
}

CMainWnd::~CMainWnd()
{
    UIManager_UnregisterTopWndChangeCallBack(CMainWnd::OnTopWndChangeCallbackFun);

    if (NULL != m_pBlackFrame)
    {
        m_pBlackFrame->deleteLater();
        m_pBlackFrame = NULL;
    }
}

bool CMainWnd::MainWndKeyBoardShowCallbackFun(bool bKeyBoardShow)
{
    if (bKeyBoardShow)
    {
        return _MainWND.OnKeyboardShow();
    }
    else
    {
        return _MainWND.OnKeyboardHide();
    }
}

void CMainWnd::CheckStateTimer()
{
    CBaseDialog * pDialog = UIManager_GetTopWindow();
    if (NULL != pDialog
            && pDialog->IsRejectReturnToIdle(RETURN_IDLE_TYPE_TIME_OUT))
    {
        return;
    }

    UIManager_ReturnToIdle(RETURN_IDLE_TYPE_TIME_OUT);
}

void CMainWnd::UpdateWnd(CBaseDialog * pDialog)
{
    MAINWND_INFO("CMainWnd::UpdateWnd");
    if (pDialog == NULL)
    {
        return;
    }

    // stop/start the timer
    ////---- 整改CallManager的时候处理
    SetCheckTimerActive(eWT_CallManager == pDialog->GetWindowType()
                        ? true
                        : !pDialog->IsRejectReturnToIdle());

    IterUpdateWndHook it = m_listUpdateWndHook.begin();
    for (; it != m_listUpdateWndHook.end(); ++it)
    {
        if (NULL != *it)
        {
            (*it)(pDialog);
        }
    }

    // 设置是否使用配置透明度
    g_TransWidgetBgControler.SetCurDlgUseTransparency(pDialog->IsUseTransparency());

    // 画softkey
    DrawSoftkey(pDialog);

    // 画Title
    DrawTitle(pDialog);

    // 画小窗口
    DrawPrivateDlg(pDialog);

    // 绘制背景图片
    DrawBackGround(pDialog);

    // 画linekey
    DrawLinekey(pDialog);

    // 绘制HomeButton
    DrawHomeAndBackBtn(pDialog);

    //画音量条
    DrawVolumeBar(pDialog);


    // 刷新窗口
    pDialog->UpdateWnd();

    //当前界面刷新时，灰色遮罩再次置顶
    if (NULL != m_pMaskFrame
            && !m_pMaskFrame->isHidden())
    {
        m_pMaskFrame->raise();
    }

    // 刷新全键盘
    qRaiseFullKeyboard();
}

void CMainWnd::InitWidget()
{
    // 初始化背景图片
    m_frmBackGround.setParent(this);
    m_frmBackGround.setGeometry(0, 0, RECT_FULL_SCREEN.width(), RECT_FULL_SCREEN.height());

    m_frmBackGround.lower();

    if (NULL == m_pMaskFrame)
    {
        m_pMaskFrame = new CMaskFrame(this);

        m_pMaskFrame->setGeometry(0, 0, PHONE_WIDTH, PHONE_HEIGHT);
        m_pMaskFrame->hide();
    }

    InitVolumeBar();

    m_pBlackFrame = new QFrame();

    if (NULL != m_pBlackFrame)
    {
        m_pBlackFrame->setWindowFlags(Qt::Tool);

        HideBlackFrame();

        m_pBlackFrame->setGeometry(0, 0, PHONE_WIDTH, PHONE_HEIGHT);

        m_pBlackFrame->setAutoFillBackground(true);
        QPalette palette = m_pBlackFrame->palette();
        palette.setBrush(QPalette::Background, QBrush(Qt::black));
        m_pBlackFrame->setPalette(palette);
    }
}

void CMainWnd::InitVolumeBar()
{
    if (m_pVolumeBar == NULL)
    {
        //创建volume实例，并且初始化参数设置
        //创建音量调节控件
        m_pVolumeBar  = new CAutoVolumeBar(this);

        //指针判空
        if (m_pVolumeBar != NULL)
        {
            //设置位置
            m_pVolumeBar->setGeometry(RECT_VOLUMEBAR);

            //设置音量条隐藏
            SetVolumeBarVisible(false);
        }
    }
}

bool CMainWnd::GetVolumeBarInfo()
{
    VOLUME_TYPE eVolueType = VT_AUTO;
    int iMin = 0;
    int iMax = 10;
    QRect rtVolumeBar = RECT_VOLUMEBAR;
    yl::string strVolumeBarBGPath = "";//默认不需要背景图片

    // http://10.2.1.199/Bug.php?BugID=31055
    // 去除只在Idle界面显示音量条的限制
    //  if (CDlgBasePopupBox::GetModalDialogTotal() == 0
    //      && m_pVolumeBar != NULL
    if (m_pVolumeBar != NULL)
    {
        CBaseDialog * pDialog = UIManager_GetTopWindow();
        if (pDialog != NULL
                && pDialog->IsShowVolumeBar((int &)eVolueType, iMin, iMax, rtVolumeBar, strVolumeBarBGPath))
        {
            m_pVolumeBar->SetRange(iMin, iMax);
            m_pVolumeBar->SetVolumeValue(eVolueType);
            m_pVolumeBar->SetVolumeBarIcon(eVolueType);
            m_pVolumeBar->setGeometry(rtVolumeBar);
            m_pVolumeBar->SetVolumeBackGround(strVolumeBarBGPath);
            return true;
        }
    }

    return false;
}

void CMainWnd::DrawVolumeBar(CBaseDialog * pDialog)
{
    if (m_pVolumeBar != NULL)
    {
        //将控件放置在界面最顶
        m_pVolumeBar->raise();
    }
}

void CMainWnd::ShowBlackFrame()
{
    if (NULL != m_pBlackFrame)
    {
        if (m_pBlackFrame->isHidden())
        {
            MAINWND_INFO("Show BlackFrame In CMainWnd.");
            m_pBlackFrame->show();
        }

        m_pBlackFrame->raise();
    }
}

void CMainWnd::HideBlackFrame()
{
    if (NULL != m_pBlackFrame && !m_pBlackFrame->isHidden())
    {
        MAINWND_INFO("Hide BlackFrame In CMainWnd.");
        m_pBlackFrame->hide();
    }
}

void CMainWnd::HideFullKeyboard()
{
    qSetHideKeyboard();
}

const CAutoVolumeBar * CMainWnd::GetVolumeBar()
{
    return m_pVolumeBar;
}

void CMainWnd::SetVolumeBarVisible(bool bVisible)
{
    if (m_pVolumeBar != NULL)
    {
        if (bVisible && GetVolumeBarInfo())
        {
            // 显示音量条
            m_pVolumeBar->SetVolumeBarVisible(true);
            m_pVolumeBar->raise();
        }
        else
        {
            //隐藏音量条
            m_pVolumeBar->SetVolumeBarVisible(false);
        }
    }
}

void CMainWnd::RefreshVolumeBarIconByChannelType(int nChannelType)
{
    if (NULL != m_pVolumeBar && nChannelType >= CHANNEL_AUTO && nChannelType < CHANNEL_NULL)
    {
        m_pVolumeBar->SetVolumeBarIconByChannelType(CHANNEL_MODE(nChannelType));
    }
}

void CMainWnd::SetWndSize()
{
    //设置窗口大小
#ifdef WIN32
    resize(RECT_FULL_SCREEN.width(), RECT_FULL_SCREEN.height());
#else
    setGeometry(RECT_FULL_SCREEN);
#endif
}

void CMainWnd::SetTitle(const QString & strTitle)
{
    if (NULL != m_pTitleDelegate)
    {
        m_pTitleDelegate->Operator(TITLE_BAR_SET_TITLE,
                                   QVariant::fromValue(strTitle));
    }
}

void CMainWnd::SetTitleAccountDisplayName(const QString & strText)
{
    if (NULL != m_pTitleDelegate)
    {
        m_pTitleDelegate->Operator(TITLE_BAR_SET_LEFT_AREA_TEXT,
                                   QVariant::fromValue(strText));
    }
}

void CMainWnd::SetTitleAccountIcon(const QString & strIcon)
{
    if (NULL != m_pTitleDelegate)
    {
        m_pTitleDelegate->Operator(TITLE_BAR_SET_LEFT_AREA_ICON,
                                   QVariant::fromValue(strIcon));
    }
}

void CMainWnd::SetTitleSelectAccountID(int nAccountID)
{
    if (NULL != m_pTitleDelegate)
    {
        m_pTitleDelegate->Operator(TITLE_BAR_SET_SELECT_ACCOUNT_ID,
                                   QVariant::fromValue(nAccountID));
    }
}

void CMainWnd::SetTitleLayoutInfo(const TitleLayoutInfo & objTitleInfo)
{
    if (NULL != m_pTitleDelegate)
    {
        m_pTitleDelegate->Operator(TITLE_BAR_SET_TITLE_INFO,
                                   QVariant::fromValue(objTitleInfo));
    }
}

QWidget * CMainWnd::GetTitle() const
{
    return m_pTitleDelegate == NULL ? NULL : m_pTitleDelegate->GetWidget();
}

//获取灰色遮罩
CMaskFrame * CMainWnd::GetMaskFrame()
{
    return m_pMaskFrame;
}

//获取灰色遮罩，原接口先保留
CMaskFrame * CMainWnd::GetGrayWidget()
{
    return m_pMaskFrame;
}

//设置灰色遮罩是否显示
void CMainWnd::SetGrayShow(bool bShow)
{
    if (NULL == m_pMaskFrame)
    {
        return ;
    }

    m_pMaskFrame->setVisible(bShow);
    m_pMaskFrame->raise();
}

// 返回Idle前的预操作,由UIManager调用
////----ReturnToIdle等能否转化为事件
void CMainWnd::PreReturnToIdle()
{
    // 销毁所有的弹出框
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);
}

bool CMainWnd::OnKeyboardShow()
{
    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();
    if (NULL == pBaseDlg)
    {
        return false;
    }

    DrawPrivateDlg(pBaseDlg);
    pBaseDlg->OnKeyboardShow();
    return true;
}

bool CMainWnd::OnKeyboardHide()
{
    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();
    if (NULL == pBaseDlg)
    {
        return false;
    }

    DrawPrivateDlg(pBaseDlg);
    pBaseDlg->OnKeyboardHide();
    return true;
}

void CMainWnd::DrawLinekey(CBaseDialog * pDialog)
{
    if (NULL != m_pDsskeyDelegate)
    {
        m_pDsskeyDelegate->DrawWidget(pDialog);
    }
}

void CMainWnd::paintEvent(QPaintEvent * pEvent)
{
    return ;
}

void CMainWnd::DrawSoftkey(CBaseDialog * pDialog)
{
    if (NULL != m_pSoftkeyDelegate)
    {
        m_pSoftkeyDelegate->DrawWidget(pDialog);
    }
}

void CMainWnd::DisplayIMESoftkey(QWidget * pWidget)
{
    if (NULL != m_pSoftkeyDelegate)
    {
        m_pSoftkeyDelegate->Operator(SOFTKEY_BAR_DISPLAY_IME,
                                     QVariant::fromValue(pWidget));
    }
}

void CMainWnd::ResetDownSoftkey()
{
    if (NULL != m_pSoftkeyDelegate)
    {
        m_pSoftkeyDelegate->Operator(SOFTKEY_BAR_RESET_DOWN,
                                     QVariant());
    }
}

void CMainWnd::SwitchNextIMEName(QWidget * pFocusWidget)
{
    if (NULL != m_pSoftkeyDelegate)
    {
        m_pSoftkeyDelegate->Operator(SOFTKEY_BAR_SWITCH_IME,
                                     QVariant::fromValue(pFocusWidget));
    }
}

void CMainWnd::DrawPrivateDlg(CBaseDialog * pDialog)
{
    // 判断小窗口的指针
    if (pDialog != NULL)
    {
        //以下做防抖处理，避免频繁操作，效率过低
        if (pDialog->parent() != this)
        {
            pDialog->setParent(this);
        }

        if (!pDialog->isVisible())
        {
            pDialog->show();
        }

        if (!pDialog->isActiveWindow())
        {
            pDialog->raise();
            pDialog->activateWindow();
        }

        // 根据窗体类型设置小窗口的区域
        pDialog->SetRect(pDialog->GetDlgGeometry());
    }
}

void CMainWnd::DrawHomeAndBackBtn(CBaseDialog * pDialog)
{
    if (NULL != m_pHomeAndBackBtnDelegate)
    {
        m_pHomeAndBackBtnDelegate->DrawWidget(pDialog);
    }
}

void CMainWnd::DrawBackGround(CBaseDialog * pDialog)
{
    // 入口参数检查
    if (pDialog == NULL)
    {
        return;
    }

    // 获取相关页面的背景图片
    QPixmap pixmapBg = pDialog->GetCustomBackGround();

#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    TitleLayoutInfo objInfo;
    if (pDialog->IsShowTitle(objInfo)
            && (objInfo.eLeftType == TITLE_MODE_ACCOUNT_INFO
                || objInfo.eLeftType == TITLE_MODE_CALL_MANAGER))
    {
        QPainter painter(&pixmapBg);
        //绘制titlebar
        QPixmap pmTitle;
#if IF_BUG_26252
        pmTitle = THEME_GET_BMP(PIC_THEME1_TITLEBAR_WITHOUT_ACCOUNT);
#else
        if (objInfo.eLeftType == TITLE_MODE_ACCOUNT_INFO
                && configParser_GetConfigInt(kszEnableIdleDefaultAccount) == 0)
        {
            pmTitle = THEME_GET_BMP(PIC_THEME1_TITLEBAR_WITHOUT_ACCOUNT);
        }
        else
        {
            pmTitle = THEME_GET_BMP(PIC_THEME1_TITLELITTLE);
        }
#endif

        painter.drawPixmap(QRect(0, 0, PHONE_WIDTH, TITLE_HEIGHT), pmTitle);
    }
#endif

    m_frmBackGround.SetBackGroundPixMap(pixmapBg);

    update();
}

void CMainWnd::UpdateBackGround()
{
    DrawBackGround(UIManager_GetTopWindow());
}

void CMainWnd::RegisterUpdateWndHook(ON_UPDATEWND_HOOK pFun)
{
    IterUpdateWndHook it = m_listUpdateWndHook.begin();
    for (; it != m_listUpdateWndHook.end(); ++it)
    {
        if (*it == pFun)
        {
            break;
        }
    }

    if (it == m_listUpdateWndHook.end())
    {
        m_listUpdateWndHook.push_back(pFun);
    }
}

void CMainWnd::UnregisterUpdateWndHook(ON_UPDATEWND_HOOK pFun)
{
    IterUpdateWndHook it = m_listUpdateWndHook.begin();
    for (; it != m_listUpdateWndHook.end(); ++it)
    {
        if (*it == pFun)
        {
            break;
        }
    }

    if (it != m_listUpdateWndHook.end())
    {
        m_listUpdateWndHook.erase(it);
    }
}

void CMainWnd::SetSoftkeyDelegate(CMainWndWgtDelegateBase * pDelegate)
{
    if (NULL != pDelegate)
    {
        m_pSoftkeyDelegate = pDelegate;
        m_pSoftkeyDelegate->SetParent(this);
    }
}

void CMainWnd::SetTitleDelegate(CMainWndWgtDelegateBase * pDelegate)
{
    if (NULL != pDelegate)
    {
        m_pTitleDelegate = pDelegate;
        m_pTitleDelegate->SetParent(this);
    }
}

void CMainWnd::SetDsskeyDelegate(CMainWndWgtDelegateBase * pDelegate)
{
    if (NULL != pDelegate)
    {
        m_pDsskeyDelegate = pDelegate;
        m_pDsskeyDelegate->SetParent(this);
    }
}

void CMainWnd::SetHomeAndBackBtnDelegate(CMainWndWgtDelegateBase * pDelegate)
{
    if (NULL != pDelegate)
    {
        m_pHomeAndBackBtnDelegate = pDelegate;
        m_pHomeAndBackBtnDelegate->SetParent(this);
    }
}

void CMainWnd::DrawTitle(CBaseDialog * pDialog)
{
    if (NULL != m_pTitleDelegate)
    {
        m_pTitleDelegate->DrawWidget(pDialog);
    }
}

bool CMainWnd::OnLineKeyClick(int iIndex)
{
    if (NULL != m_pDsskeyDelegate)
    {
        return m_pDsskeyDelegate->Operator(DSSKEY_FRAME_CLICK_DSSKEY,
                                           QVariant::fromValue(iIndex));
    }

    return false;
}

bool CMainWnd::ProcessVolumeKey(int nKeyCode, bool bPress)
{
    if (nKeyCode != PHONE_KEY_VOLUME_INCREASE && nKeyCode != PHONE_KEY_VOLUME_DECREASE)
    {
        return false;
    }

    if (!bPress)
    {
        // EDK 关闭时 释放音量键+ 事件不传递（按下时才下传）
        if (PHONE_KEY_VOLUME_DECREASE == nKeyCode
                || (PHONE_KEY_VOLUME_INCREASE == nKeyCode && !EDKUI_IsEdkSwitchEnable()))
        {
            return false;
        }
    }

    // EDK 开启时 按下音量键+ 事件不传递（释放时才下传）
    if (bPress && PHONE_KEY_VOLUME_INCREASE == nKeyCode && EDKUI_IsEdkSwitchEnable())
    {
        return false;
    }

    if (!voice_IsVolumeCanChange())
    {
        return true;
    }

    // 显示音量条
    if (m_pVolumeBar != NULL
            && !m_pVolumeBar->isVisible()
            && GetVolumeBarInfo())
    {
        //音量条不可见情况，若为左右键或音量键，询问子窗口，若需要显示则直接显示
        m_pVolumeBar->SetVolumeBarVisible(true);
        m_pVolumeBar->raise();
        return true;
    }

    return false;
}

bool CMainWnd::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    CBaseDialog * pDialog = UIManager_GetTopWindow();
    if (NULL != pDialog)
    {
        // 重新计时
        SetCheckTimerActive(eWT_CallManager == pDialog->GetWindowType()
                            ? true
                            : !pDialog->IsRejectReturnToIdle());
    }

    switch (pEvent->type())
    {
    case QEvent::MouseButtonPress:
        {
#ifdef _WIN32
            if (pObject->inherits("QComboBox"))
            {
                qComboBoxUtility::SwitchItem((QComboBox *)pObject, false);
                return true;
            }
#endif
            if (NULL != m_pVolumeBar)
            {
                m_pVolumeBar->eventFilter(pObject, pEvent);
            }

            if (NULL != pDialog && pDialog->eventFilter(pObject, pEvent))
            {
                return true;
            }
        }
        break;
    case QEvent::KeyPress:
        {
            if (m_pVolumeBar != NULL
                    && m_pVolumeBar->eventFilter(pObject, pEvent))
            {
                return true;
            }

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();

            //传递给顶层私有小窗体处理
            if (pDialog != NULL && pDialog->eventFilter(pObject, pEvent))
            {
                return true;
            }

            switch (nKey)
            {
            case PHONE_KEY_FN1:
            case PHONE_KEY_FN2:
            case PHONE_KEY_FN3:
            case PHONE_KEY_FN4:
                {
                    //热键栏按键传递给SoftkeyBar处理
                    if (NULL != m_pSoftkeyDelegate
                            && m_pSoftkeyDelegate->ProcessKeyEvent(pObject, pKeyEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_VOLUME_INCREASE:
            case PHONE_KEY_VOLUME_DECREASE:
                {
                    if (ProcessVolumeKey(nKey, true))
                    {
                        return true;
                    }
                }
                break;
#if (defined(_T4X) && !defined(_T48))
            case PHONE_KEY_MEMORY1:
            case PHONE_KEY_MEMORY2:
            case PHONE_KEY_MEMORY3:
            case PHONE_KEY_MEMORY4:
            case PHONE_KEY_MEMORY5:
            case PHONE_KEY_MEMORY6:
            case PHONE_KEY_MEMORY7:
            case PHONE_KEY_MEMORY8:
            case PHONE_KEY_MEMORY9:
            case PHONE_KEY_MEMORY10:
                {
                    if (NULL != m_pDsskeyDelegate)
                    {
                        m_pDsskeyDelegate->ProcessKeyEvent(pObject, pKeyEvent);
                    }
                }
                break;
#endif
            default:
                break;
            }
        }
        break;
    case QEvent::KeyRelease:
        {
            // 传递给顶层私有小窗体处理
            if (pDialog != NULL && pDialog->eventFilter(pObject, pEvent))
            {
                return true;
            }

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_FN1:
            case PHONE_KEY_FN2:
            case PHONE_KEY_FN3:
            case PHONE_KEY_FN4:
                {
                    //热键栏按键传递给SoftkeyBar处理
                    if (NULL != m_pSoftkeyDelegate
                            && m_pSoftkeyDelegate->ProcessKeyEvent(pObject, pKeyEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_VOLUME_INCREASE:
            case PHONE_KEY_VOLUME_DECREASE:
                {
                    if (ProcessVolumeKey(nKey, false))
                    {
                        return true;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        {
            // 传递给顶层私有小窗体处理
            if (pDialog != NULL && pDialog->eventFilter(pObject, pEvent))
            {
                // 鼠标弹起事件继续交给QT处理
                return false;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

bool CMainWnd::ProcessVolumeBarKeyEvent(int nKeyCode, bool bPress)
{
    //音量条可见情况，交由音量条处理
    if (m_pVolumeBar != NULL
            && m_pVolumeBar->ProcessKeyEvent(nKeyCode, bPress))
    {
        return true;
    }

    return false;
}

void CMainWnd::ShowRebootMessage(const QString & strText)
{
    // 提示正在升级
#ifdef _WIN32
    QLabel * pLabel = new QLabel(this);
#else
    static QLabel * pLabel = new QLabel();
#endif

    if (NULL == pLabel)
    {
        return;
    }

    //清除所有弹窗
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    // 隐藏全键盘
    qSetHideKeyboard();

    // 设置位置
    pLabel->setGeometry(0, 0, PHONE_WIDTH, PHONE_HEIGHT);

    // 设置黑色背景,白色文字
    pLabel->setAutoFillBackground(true);
    QPalette palette = pLabel->palette();
    palette.setBrush(QPalette::Window, QBrush(Qt::black));
    palette.setColor(QPalette::WindowText, Qt::white);
    pLabel->setPalette(palette);

    // 设置文字属性
    qWidgetUtility::setFontStyle(pLabel, 30, QFont::Bold);
    pLabel->setAlignment(Qt::AlignCenter);
    pLabel->setText(LANG_PARSE(strText));

    // 显示Label
    pLabel->raise();
    pLabel->show();
    pLabel->repaint();

    // 开启背光灯
    backlight_Reset();

    // 处理所有消息,防止重绘消息没有生效
    QApplication::processEvents();
    MAINWND_INFO("Show Reboot QLabel");
}

void CMainWnd::SetTimerLock(bool bLock)
{
    m_bIsTimerLock = bLock;

    if (m_pCheckTimer == NULL)
    {
        return;
    }

    if (m_bIsTimerLock)
    {
        m_pCheckTimer->stop(); // 上锁后停止计时器
    }
    else
    {
        m_pCheckTimer->start(s_ncheckStateTime);// 解锁后启动计时器
    }
}

void CMainWnd::SetCheckTimerActive(bool bActive)
{
    if (m_pCheckTimer == NULL)
    {
        return;
    }

    if (bActive && !m_bIsTimerLock)
    {
        m_pCheckTimer->start(s_ncheckStateTime);
    }
    else
    {
        m_pCheckTimer->stop();
    }
}
