//mod-base
#include "talkui_define.h"
#include "dlgtalkuiframe.h"

#include "devicelib/networkserver.h"

//mod-Talk
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/uiandlogic_common.h"

#include "talk/talklogic/include/commonfunctions.h"
// #include "talk/talklogic_inc.h"
// #include "talk/talklogicmanager.h"

//mod-ImageManager
#include "imagemanager/modimagemanager.h"
#include "picpath.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "baseui/transwidgetbgcontroler.h"
#include "uikernel/qscrolltextlabel.h"

//mod-globalInputHook
#include "keymap.h"
#include "globalinputhook/modinputhook.h"

//mod-MainWnd
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include "mainwnd/autovolumebar.h"
#include "mainwnd/modmainwnd.h"

//mod-config
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

//mod-this(TalkUI)
#include "talkuimanager.h"
#include "talkuidelegate.h"
#include "talkuiframemanager.h"
#include "commonfunction.h"
#include "commonwidget.h"
#include "dlgdialingui.h"

#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qwidgetutility.h"
#include "talksessiontab.h"

#include "talkuicommon.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"


#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif
#include "commonfunction.h"

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/modrecord.h"
#endif
#include "application/include/modapplication.h"
#include "messagebox/modmessagebox.h"
#include "broadsoft/callrecording/include/modcallrecording.h"

#include "talk/fwd_dnd/include/modfwddnd.h"
#include "voice/include/modvoice.h"

const QRect TOPBAR_GEOMETRY(0, 0, PHONE_WIDTH, TITLE_HEIGHT);
const QRect CC_STATUS_LABEL_GEOMETRY(212, 338, 378, 48);
const QRect DTMF_LINE_GEOMETRY(164, 338, 474, 47);//DTMF控件位置
const QRect INCOMINGCALL_NOTIFY_GEMETRY(164, 338, 429, 53);     //来电提醒的位置
const QRect NETWORK_NOTE_GEOMETRY(162, 338, 478, 51);//网络异常提示的默认位置
const QRect TEB_MANAGER_GEOMETRY(158, 48, 486, 350);
const int   ERROR_SESSION_ID(-1);// 用来标示错误的SessionId

//不显示头像时 topbar 的位置
const QRect TALKUI_FRAME_TOPBAR_GEOMETRY(305, 10, 120, 25);

//构造函数
CDlgTalkUIFrame::CDlgTalkUIFrame(QWidget * pParent /* = 0 */)
    : CBaseDialog()
    , m_pTalkUI(NULL)
    , m_pLabNetworkNote(NULL)
    , m_bIsCableConnect(true)
    , m_strRecordIconPath("")
    , m_pTabManager(NULL)
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    , m_pCCStatusLabel(NULL)
#endif
#ifdef IF_FEATURE_ENTRANCE_GUARD
    , m_pLabEntranceGuard(NULL)
#endif
{
    //设置为顶层窗口
    SetTopWnd();

    SetWindowType(eWT_Talk);

    // 设置窗体位置
    SetDlgGeometry(RECT_TALK_FRAME);

    //设置不返回Idle
    SetRejectReturnToIdle();

    //设置DTMF输入框内容
    m_pDtmfLine = new QDtmfLine(this);
    if (m_pDtmfLine != NULL)
    {
        m_pDtmfLine->setObjectName("DtmfLine");
        m_pDtmfLine->hide();

        //设置位置
        m_pDtmfLine->setGeometry(DTMF_LINE_GEOMETRY);
        qLineEditUtility::SetLineEditTransparent(m_pDtmfLine);

        //设置背景
        QPalette pl = m_pDtmfLine->palette();
        m_pDtmfLine->setAutoFillBackground(true);
        QPixmap pix = STOCK_GET_BMP(PIC_DTMF_BACKGROUND);
        pl.setBrush(QPalette::Base, QBrush(pix));
        m_pDtmfLine->setPalette(pl);
    }

    m_pLabNetworkNote = new QLabel(this);
    if (m_pLabNetworkNote != NULL)
    {
        m_pLabNetworkNote->setObjectName("LabNetworkNote");
        m_pLabNetworkNote->setGeometry(NETWORK_NOTE_GEOMETRY);
        m_pLabNetworkNote->setAlignment(Qt::AlignCenter);
        m_pLabNetworkNote->setText(LANG_TRANSLATE(TRID_NETWORK_UNAVAILABLE));
        SetWgtFontColor(m_pLabNetworkNote, FONT_TYPE_TEXT, Qt::red);
        m_pLabNetworkNote->hide();
    }

    //设置来电提醒相关信息
    m_qIncomingCallNotify.setObjectName("IncomingCallNotify");
    m_qIncomingCallNotify.setParent(this);

//  m_qIncomingCallNotify.set3StatePic(PIC_THEME1_TAB_CLICK_BOTTON, PIC_THEME1_TAB_CLICK_BOTTON, PIC_THEME1_TAB_CLICK_BOTTON);
    m_qIncomingCallNotify.set3StatePic(PIC_THEME1_TAB_NORMAL_BOTTON, PIC_THEME1_TAB_CLICK_BOTTON,
                                       PIC_THEME1_TAB_NORMAL_BOTTON);

    m_qIncomingCallNotify.SetEnableTextColor(Qt::white);
    m_qIncomingCallNotify.SetDisableTextColor(Qt::black);

    m_qIncomingCallNotify.SetTime(0, 0, 0);
    m_qIncomingCallNotify.ActiveTimer(false);
    //连接槽函数
    connect(&m_qIncomingCallNotify, SIGNAL(clicked()), this, SLOT(OnIncomingCallNotifyClick()));


    m_pTabManager = new CTalkTabManager(this);
    if (NULL != m_pTabManager)
    {
        m_pTabManager->setObjectName("TabManager");
        m_pTabManager->setGeometry(TEB_MANAGER_GEOMETRY);
        connect(m_pTabManager, SIGNAL(FocusUIGeometryChange(bool, const QRect &)),
                this, SLOT(OnFocusUIGeometryChange(bool, const QRect &)));
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    m_pCCStatusLabel = new qScrollTextLabel(this);
    if (NULL != m_pCCStatusLabel)
    {
        m_pCCStatusLabel->setObjectName("CCStatusLabel");
        m_pCCStatusLabel->setGeometry(CC_STATUS_LABEL_GEOMETRY);
        m_pCCStatusLabel->setAlignment(Qt::AlignCenter);
        SetWgtFontColor(m_pCCStatusLabel, FONT_TYPE_TALK_TEXT);
    }
#endif

    DrawDsskeyBtnOnBackground();

    //设置父窗口
    m_qTopBar.setObjectName("TopBar");
    m_qTopBar.setParent(this);
    m_qTopBar.setGeometry(TALKUI_FRAME_TOPBAR_GEOMETRY);

#ifdef IF_FEATURE_ENTRANCE_GUARD
    m_pLabEntranceGuard = new QLabel(this);
    m_pLabEntranceGuard->setObjectName("LabEntranceGuard");
    m_pLabEntranceGuard->setGeometry(TEB_MANAGER_GEOMETRY);

    m_pLabEntranceGuard->setAlignment(Qt::AlignCenter);
    m_pLabEntranceGuard->setScaledContents(true);
    m_pLabEntranceGuard->setAutoFillBackground(true);
    SetWgtFontColor(m_pLabEntranceGuard, FONT_TYPE_TALK_TEXT, Qt::red);

    QFont objFont;
    objFont.setPointSize(20);
    m_pLabEntranceGuard->setFont(objFont);

    m_pLabEntranceGuard->installEventFilter(this);
    m_pLabEntranceGuard->lower();
    m_pLabEntranceGuard->hide();
#endif
}

//析构函数
CDlgTalkUIFrame::~CDlgTalkUIFrame()
{

}

//取得实例
CBaseDialog * CDlgTalkUIFrame::CreateInstance()
{
    return new CDlgTalkUIFrame();
}

// 初始化
void CDlgTalkUIFrame::Init()
{
    //更新通道图标
    //这边统一在  SetData 的时候做操作
//  _MainWND.SetTitleAccountIcon(::GetChannelIcon());

    if (m_pDtmfLine != NULL)
    {
        connect(m_pDtmfLine, SIGNAL(visibleChanged(bool)), this, SLOT(OnDTMFLineVisableChanged(bool)));
    }

    if (NULL != m_pTabManager)
    {
        connect(m_pTabManager, SIGNAL(SigNotifyScroll()), this, SLOT(OnNotifyScroll()));
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (NULL != m_pCCStatusLabel)
    {
        // 默认隐藏
        m_pCCStatusLabel->clear();
        m_pCCStatusLabel->hide();
    }
#endif

    //不显示联系人头像 那么topbar 要show 出来
    CDlgBaseTalkUI * pCurrentUI = GetTalkUI();
    if (NULL != pCurrentUI && !(pCurrentUI->HasDisplayContactImage()))
    {
        m_qTopBar.show();
    }
    else    //显示联系人头像  topbar 要隐藏
    {
        if (configParser_GetConfigInt(kszDisplayContactPhoto) == 1)
        {
            m_qTopBar.hide();
        }
        else
        {
            m_qTopBar.show();
        }
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgTalkUIFrame::Uninit()
{
    //恢复时，设置TalkUI指针为空
    SetTalkUI(NULL);

    //断开volumeBar的信号槽
    //!!!!!,MainWnd提供C接口
    if (_MainWND.GetVolumeBar() != NULL)
    {
        // 隐藏音量条!!!!
        _MainWND.SetVolumeBarVisible(false);
    }

    // 断开Dtmf的信号槽
    if (m_pDtmfLine != NULL)
    {
        disconnect(m_pDtmfLine, SIGNAL(visibleChanged(bool)), this, SLOT(OnDTMFLineVisableChanged(bool)));
    }

    // 清楚Tab的数据
    if (NULL != m_pTabManager)
    {
        disconnect(m_pTabManager, SIGNAL(SigNotifyScroll()), this, SLOT(OnNotifyScroll()));
        m_pTabManager->ClearData();
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (NULL != m_pCCStatusLabel)
    {
        // 默认隐藏
        m_pCCStatusLabel->clear();
        m_pCCStatusLabel->hide();
    }
#endif

    //图标在SetData 的时候完成就行了
//  DeleteBlueToothIcon();

#ifdef IF_FEATURE_ENTRANCE_GUARD
    RefreshDoorLine(false);
#endif
}

// 设置小窗口数据
void CDlgTalkUIFrame::SetData(void * pData /* = NULL */)
{
    TALKUI_INFO("CDlgTalkUIFrame::SetData");

    //更新通道图标 因为当摘机来电界面显示的是免提图标 或者 耳麦图标   而接起来电的时候显示的应该是手柄通道图标  所以这边需要刷新一下图标
//  UpdateChannelIcon();
    _TalkUIDelegate.UpdateChannelIcon();

    //先删除所有图标
    DeleteAllIcon();

    //加上通话中支持的DND图标
    if (!configParser_GetConfigInt(kszKeepDNDAfterTalk))
    {
        RefreshDNDIcon();
    }

    //加上蓝牙图标
    RefreshBlueToothIcon();

    //初始化设置为CommonTalk
    m_eWndType = eWT_Talk;

    if (m_pTalkUI != NULL)
    {
        if (!m_pTalkUI->isVisible())
        {
            //显示小窗口
            m_pTalkUI->show();
            m_pTalkUI->raise();
            //RefreshBlueToothIcon();
        }

        // 每次切换小窗口都要先关闭弹出框
        if (_TalkUIDelegate.GetMessageBoxCount() > 0)
        {
            _TalkUIDelegate.RemoveAllMessageBox();
        }

        // 载入页面的数据
        DataLogic2UI * pLogic2UIData = (DataLogic2UI *)(pData);
        if (pLogic2UIData != NULL)
        {
            //统一使用 该接口 设置数据
            m_pTalkUI->SetLogicData(*pLogic2UIData);

            //直接调用获取talkuisoftkey的函数， 在TalkUI 设置数据的时候  就应该先把 softkey的数据先准备。
            //刷新softkey有两种方式  一种是在刷新窗口的时候 mainwnd会主动调用刷新， 另一种是 talkuiframe 调用mainwnd函数刷新
            //所以在 子窗口的 SetTalkData 函数的时候 不需要调用talkuiframe 刷新 ， 因为 紧接着 talkuiframe 会主动调用
            GetTalkUISoftkey();

            //设置录音图标
            SetRecordTip(m_pTalkUI->GetRecordIconType());
        }
        else
        {
            //获取Softkey数据
            GetTalkUISoftkey();
            //  m_pTalkUI->GetSoftkeyData(m_SoftkeyData);

            ////隐藏加密图标
            //SetEncryptTip(false);

            ////隐藏录音图标
            //SetRecordTip(IT_UNKNOWN);
        }

        if (pLogic2UIData != NULL)
        {
            if (m_pTabManager != NULL)
            {
                if (!m_pTalkUI->inherits("CDlgDialingUI"))
                {
                    // 刷新Tab内容
                    ////---- 需进行防抖
                    m_pTabManager->show();
                    m_pTabManager->RefreshContent();
                }
            }

            // to do 换个位置
            _MainWND.SetTitleAccountDisplayName("");
        }

        //设置窗口类型，并相应设置小窗口位置
        m_eWndType = SetTalkUIWindowType();
    }

    //刷新数据时检查网络是否正常
    CheckNetwork();

    // 每次设置页面都需要刷新下TIP的区域
    SetTipRegionVisiblePolicy();

#ifdef IF_FEATURE_ENTRANCE_GUARD
    RefreshDoorLine();
#endif
}

//是否显示音量条
bool CDlgTalkUIFrame::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                      QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = TalkUIlogic_IsAllowSilenceDuringCall() ? 0 : 1;
    iMax = 15;
    if (m_pTalkUI != NULL)
    {
        return m_pTalkUI->IsShowVolumeBar(iVolumeType, iMin, iMax, rtVolume, strBGPath);
    }

    //默认显示
    return true;
}

// 是否显示Title
bool CDlgTalkUIFrame::IsShowTitle(TitleLayoutInfo & objInfo)
{
    objInfo.eLeftType = TITLE_MODE_TALK_INFO;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    if (modRecord_IsRecordPause(RECORD_AUDIO)
            || modRecord_IsRecording(RECORD_AUDIO))
    {
        objInfo.eMidType = TITLE_MODE_USB_RECORDING;
    }
    else
    {
        objInfo.eMidType  = TITLE_MODE_TITLE;
    }
#else
    objInfo.eMidType  = TITLE_MODE_TITLE;
#endif
    objInfo.eRightType = TITLE_MODE_TIME;
    return true;
}

void CDlgTalkUIFrame::AfterShowTitle()
{
    _TalkUIDelegate.UpdateChannelIcon();
//  _MainWND.SetTitleAccountIcon(::GetChannelIcon());
}

// 是否显示Dsskey，默认认为显示
bool CDlgTalkUIFrame::IsShowDsskey(bool & bExpend)
{
    bExpend = false;
    return true;
}

// 焦点改变.
void CDlgTalkUIFrame::OnFocusChanged(bool bFocused)
{
    //因为v80 通道改变 逻辑层不再调用 ui 层的 通道改变接口 所以 在其他界面如（账号选择，dir）改变声道在退回到通话界面 就需要自己手动刷新通道
    if (bFocused)
    {
        _TalkUIManager.UpdateChannelIcon();
    }

    //这边不通过 TalkUIManager调用了  直接自己内部调用
    //_TalkUIManager.SendFocusChangeAction(bFocused);
    SendFocusChangeAction(bFocused);
}

QPixmap CDlgTalkUIFrame::GetCustomBackGround()
{
    return m_pmBG;
}

// 通道键处理
bool CDlgTalkUIFrame::OnChannelKeyPress(QObject * pObject, QEvent * pEvent, int iKey)
{
    BOOL bHandled = false;

    if (m_pTalkUI != NULL)
    {
#ifdef IF_FEATURE_HEADSETKEY_IN_TALK
        //是否拨号界面在Park 状态
        bool bIsDialParkStatus = false;

        //如果是在拨号界面
        if (m_pTalkUI->inherits(DLG_Dial))
        {
            CDlgDialingUI * pDlgDialingUI = (CDlgDialingUI *)m_pTalkUI;

            //如果UI类型是CallPark  那么才是
            if (DIAL_UI_CALLPARK == pDlgDialingUI->m_eDialUIType)
            {
                bIsDialParkStatus = true;
            }
        }
        // http://192.168.1.99/Bug.php?BugID=19141
        // 焦点是通话界面，且耳麦打开的情况下不再处理channel按键
        if (configParser_GetConfigInt(kszHeadSetKeyInTalkSwitch) == 0
                && voice_GetHeadsetStatus()
                && (m_pTalkUI->inherits(DLG_Talking)                     //普通通话
                    || m_pTalkUI->inherits(DLG_Conference)                  //会议
                    || m_pTalkUI->inherits(DLG_ConfManagerUI)               //会议管理
                    || m_pTalkUI->inherits(DLG_RTPStatusUI)                 //RTP状态下
                    || bIsDialParkStatus
                   ))
        {
            //更新通道图标
            _TalkUIDelegate.UpdateChannelIcon();

            return FALSE;
        }
#endif
    }

    // 这边不能再交给voice 处理了  因为在qT46Application::PreKeyHookBegin 函数中会处理一次！！！！！！
    //  bHandled = voice_OnKeyPress(iKey);

    // 双耳麦模式，摘挂机只更改通道，不再继续往下传
    if (voice_IsCallCenterMode()
            && (iKey == PHONE_KEY_HANDSET_OFF_HOOK
                || iKey == PHONE_KEY_HANDSET_ON_HOOK))
    {
        bHandled = true;
    }
    else
    {
        if (NULL != m_pTalkUI)
        {
            bHandled |= m_pTalkUI->OnChannelKeyPress(iKey);
        }
    }

    //这边强制 更新
    //if(bHandled)
    //{
    //更新通道图标
    _TalkUIDelegate.UpdateChannelIcon();
    //}

    //这个要放在 通话处理之后， 因为这个时候可能 已经退出了通话界面了， 避免退到Idle后还出现音量条
    //需要放在更新通道 图标之后 因为 更新通道图标会把m_bIsShowVolume 的状态值 刷新
    if (PHONE_KEY_HANDFREE == iKey
            || PHONE_KEY_HANDSET == iKey
            || PHONE_KEY_HANDSET_OFF_HOOK == iKey
            || PHONE_KEY_HANDSET_ON_HOOK == iKey
            || PHONE_KEY_HEADSET_CTRL == iKey
       )
    {
        if (UIManager_IsInTalk() && m_pTalkUI != NULL)
        {
            TALK_UI_TYPE eType = m_pTalkUI->GetTalkUIType();
            //摘手柄或者按免提导致声道切换，需要显示切换后声道的音量
            //通过模拟按键显示音量条
            if (eType != TALK_UI_RING)
            {
                if (_TalkUIFrameMananger.GetIsShowVolume())
                {
                    inputHook_SendKey(PHONE_KEY_VOLUME_DECREASE, true);

                    _TalkUIFrameMananger.SetShowVolumeStatus(false);
                }
            }
        }
    }

    return bHandled;
}

bool CDlgTalkUIFrame::OnFunckeyPress(QObject * pObject, QEvent * pEvent, int iKey, bool bEXP)
{
    if (NULL == m_pTalkUI)
    {
        return false;
    }

    switch (iKey)
    {
    case PHONE_KEY_UP:
    {
        if (NULL != m_pTalkUI && m_pTalkUI->IsChangeSession())
        {
            if (NULL != m_pTabManager)
            {
                if (m_pTabManager->GetPageSize() > 1)
                {
                    m_pTabManager->OnNotifyScrollPrevious();
                }
                else
                {
                    m_pTalkUI->OnFunckeyPress(iKey, bEXP);
                    ChangePrevSession(); // 会刷新m_pTalkUI, 要先处理上下键
                    return true;
                }
            }
        }
    }
    break;
    case PHONE_KEY_DOWN:
    {
        if (NULL != m_pTalkUI && m_pTalkUI->IsChangeSession())
        {
            if (NULL != m_pTabManager)
            {
                if (m_pTabManager->GetPageSize() > 1)
                {
                    m_pTabManager->OnNotifyScrollNext();
                }
                else
                {
                    m_pTalkUI->OnFunckeyPress(iKey, bEXP);
                    ChangeNextSession();
                    return true;
                }
            }
        }
    }
    break;
    case PHONE_KEY_HOLD:
    {
        //http://ts.yealink.com/Bug.php?BugID=40668
        if (configParser_GetConfigInt(kszEnableHoldFunKeyInTalk) == 0)
        {
            return true;
        }
    }
    break;
    default:
        break;
    }

    //播放按键音
    if (NULL != m_pTalkUI)
    {
        if (m_pTalkUI->IsNeedToPlayKeyTone(iKey))
        {
            _TalkUIDelegate.PlayKeyTone(iKey);
        }
    }

    if (m_pTalkUI != NULL
            && m_pTalkUI->OnFunckeyPress(iKey, bEXP))
    {
        return true;
    }

    return false;
}

bool CDlgTalkUIFrame::OnDigitkeyPress(QObject * pObject, QEvent * pEvent, int iKey)
{
    //播放按键音
    if (NULL != m_pTalkUI)
    {
        if (m_pTalkUI->IsNeedToPlayKeyTone(iKey))
        {
            _TalkUIDelegate.PlayKeyTone(iKey);
        }
    }

    //交给子通话模块处理
    if (m_pTalkUI != NULL
            && m_pTalkUI->OnDigitkeyPress(iKey))
    {
        return true;
    }

    return false;
}

// 设置子窗口指针
void CDlgTalkUIFrame::SetTalkUI(CDlgBaseTalkUI * pBaseTalkUI, void * pData /* = NULL */)
{
    //窗口不一样，才进行切换
    if (m_pTalkUI == pBaseTalkUI)
    {
        return;
    }

    if (m_pTalkUI != NULL)
    {
        // 画新的窗口前,旧窗口处理焦点失去事件
        m_pTalkUI->OnFocusChanged(false);

        // 断开相关的信号槽
        disconnect(m_pTalkUI, SIGNAL(RequestRefreshSoftkey()), this, SLOT(RefreshSoftkey()));
        disconnect(m_pTalkUI, SIGNAL(SwitchSession()), this, SLOT(OnSwitchSession()));
        disconnect(m_pTalkUI, SIGNAL(LoadPhotoMarkChange()), this,
                   SLOT(SlotResetWidgetByContactPhotoVisibleChange()));

        // softkey的槽是连接在子页面上的清空子页面的时候需要清除
        m_SoftkeyData.clear();

        // 隐藏旧界面
        m_pTalkUI->setParent(NULL);
        m_pTalkUI->hide();
    }

    //窗口指针赋值
    m_pTalkUI = pBaseTalkUI;

    // 通话窗口变化的话要隐藏音量条!!!!
    // !!!!!, MainWnd提供C接口
    _MainWND.SetVolumeBarVisible(false);
    // 通话窗口变化的话要隐藏DTMF条，并清空DTMF内容!!!!
    if (m_pDtmfLine != NULL)
    {
        m_pDtmfLine->clear();
    }
    SetDTMFLineVisible(false);

    ResetWidgetByContactPhotoVisibleChange(false);

    if (m_pTalkUI != NULL
            && m_pTalkUI->parent() != this)
    {
        //设置小窗口的父窗口为自身
        m_pTalkUI->setParent(this);

        //小窗口获取焦点
        //修正bug http://10.2.1.199/Bug.php?BugID=31728 使焦点在通过窗体上
        //这个bug 因为在TalkUIFrame中已经不判断是否子窗口了  所以这个设置焦点的可以去除
        //m_pTalkUI->setFocus();

        connect(m_pTalkUI, SIGNAL(RequestRefreshSoftkey()), this, SLOT(RefreshSoftkey()));
        connect(m_pTalkUI, SIGNAL(SwitchSession()), this, SLOT(OnSwitchSession()));
        //异步连接信号槽 通话界面刷新嵌套带来的位置问题
        connect(m_pTalkUI, SIGNAL(LoadPhotoMarkChange()), this, \
                SLOT(SlotResetWidgetByContactPhotoVisibleChange()), Qt::QueuedConnection);
    }

#ifdef IF_FEATURE_ENTRANCE_GUARD
    RefreshDoorLine();
#endif
}

// 获取子窗口指针
CDlgBaseTalkUI * CDlgTalkUIFrame::GetTalkUI() const
{
    return m_pTalkUI;
}

// 发送焦点变化事件
void CDlgTalkUIFrame::SendFocusChangeAction(bool bFocus)
{
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->OnFocusChanged(bFocus);
        m_pTalkUI->SendUIRequestAction(TALK_ACTION_WND_FOCUSE_CHANGE, (int)bFocus);
    }
}

// !!!!!,设置当前通话窗口类型，包括设置窗口大小等
EWindowType CDlgTalkUIFrame::SetTalkUIWindowType()
{
    //初始化设置为CommonTalk
    m_eWndType = eWT_Talk;

    if (m_pTalkUI != NULL
            && m_pTalkUI->inherits("CDlgDialingUI"))
    {
        //设置背景
        m_eWndType = eWT_Talk_Dial;
    }

    return m_eWndType;
}

//设置加密图标提示
void CDlgTalkUIFrame::SetEncryptTip(bool bEncrypt)
{
    // 隐藏或者显示加密图标
    OnAddTitleIcon(PIC_ICON_ENCRYPT, bEncrypt);
}

//设置录音图标提示
void CDlgTalkUIFrame::SetRecordTip(ICON_TYPE eIconType)
{
    // 先去旧有的录音图标
    if (!m_strRecordIconPath.empty())
    {
        OnAddTitleIcon(m_strRecordIconPath, false);
    }

    // 显示录音图标
    m_strRecordIconPath = GetIconPath(eIconType);
    if (!m_strRecordIconPath.empty())
    {
        OnAddTitleIcon(m_strRecordIconPath, true);
    }
}

LPCSTR CDlgTalkUIFrame::GetIconPath(ICON_TYPE eIconType)
{
    switch (eIconType)
    {
    case IT_RECORD:
        return PIC_ICON_RECORD;
    case IT_CANNOT_BE_RECORDED:
        return PIC_CANNOT_RECORD;
    case IT_CANNOT_BE_STOPPED:
        return PIC_CANNOT_STOPPED_RECORD;
    case IT_CANNOT_BE_STARTED:
        return PIC_CANNOT_STARTED_RECORD;
    case IT_RECORD_NORMAL:
        return PIC_RECORD_NORMAL;
    case IT_RECORD_PAUSE:
        return PIC_RECORD_PAUSE;
    case IT_RECORD_FULL:
        return PIC_RECORD_FULL;
    default:
        return "";
    }

    return "";
}

//删除所有图标
void CDlgTalkUIFrame::DeleteAllIcon()
{
    //topbar 也要清除所有图标
    m_qTopBar.ClearAllIcon();

    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->DeleteAllIcon();
    }
}

void CDlgTalkUIFrame::DeleteBlueToothIcon()
{
    if (!IsBluetoothEnable())
    {
        return;
    }

    OnAddTitleIcon(PIC_ICON_TALK_BT_UNCONNECTED, false);
    OnAddTitleIcon(PIC_ICON_TALK_BT_CONNECTED, false);
}

// 刷新DND图标
void CDlgTalkUIFrame::RefreshDNDIcon()
{

    OnAddTitleIcon(PIC_ICON_DND_SMALL, dnd_IsEnable(acc_GetDefaultAccount()));

}


// 刷新蓝牙图标
void CDlgTalkUIFrame::RefreshBlueToothIcon()
{
    if (!IsBluetoothEnable())
    {
        return;
    }

    DeleteBlueToothIcon();

    Bluetooth_IsConnected() ? OnAddTitleIcon(PIC_ICON_TALK_BT_CONNECTED, true)
    : OnAddTitleIcon(PIC_ICON_TALK_BT_UNCONNECTED, true);

}

void CDlgTalkUIFrame::VisibleCCStatusLabel(bool bVisible)
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (NULL != m_pCCStatusLabel)
    {
        if (bVisible)
        {
            QRect rect = CC_STATUS_LABEL_GEOMETRY;
            if (NULL != m_pTalkUI)
            {
                rect.moveTo(rect.x(), m_pTalkUI->geometry().bottom() - CC_STATUS_LABEL_GEOMETRY.height());
            }

            m_pCCStatusLabel->setGeometry(rect);
            m_pCCStatusLabel->raise();
            m_pCCStatusLabel->show();
        }
        else
        {
            m_pCCStatusLabel->hide();
        }
    }
#endif
}

//显示 来电提醒
void CDlgTalkUIFrame::ShowIncomingCallNotify()
{
    QString strNotify;
    yl::string strNumber;

    talklogic_GetIncomingCallNum(strNumber);

    strNotify = LANG_TRANSLATE(TRID_INCOMING_CALL) + QString(" : ");
    //http://10.2.1.199/Bug.php?BugID=98121 qstring的类型不能直接与yl::string类型相加返回qstring
    strNotify += toQString(strNumber);
    m_qIncomingCallNotify.SetTabText(strNotify);

    QRect rect = INCOMINGCALL_NOTIFY_GEMETRY;

    if (NULL != m_pTalkUI)
    {
        if (m_pTalkUI->isVisible() || !m_pTalkUI->isHidden())
        {
            rect.moveTo(rect.x() - 1, m_pTalkUI->geometry().bottom() - INCOMINGCALL_NOTIFY_GEMETRY.height() +
                        1);
        }
        else
        {
            rect.moveTo(rect.x() - 1, this->geometry().bottom() - INCOMINGCALL_NOTIFY_GEMETRY.height() - 1);
        }

        //在底边  7 只是取一个大概的数
        if (this->geometry().bottom() - rect.bottom() < 7)
        {
            m_qIncomingCallNotify.set3StatePic(PIC_THEME1_TAB_NORMAL_BOTTON, PIC_THEME1_TAB_CLICK_BOTTON,
                                               PIC_THEME1_TAB_NORMAL_BOTTON);

        }
        else
        {
            //  rect.setHeight(rect.height() - 2);
            rect.moveTo(rect.x(), rect.y() + 1);

            m_qIncomingCallNotify.set3StatePic(PIC_THEME1_TAB_NORMAL_MID, PIC_THEME1_TAB_CLICK_MID,
                                               PIC_THEME1_TAB_NORMAL_MID);

        }

        m_qIncomingCallNotify.setGeometry(rect);

        m_qIncomingCallNotify.show();
        m_qIncomingCallNotify.raise();
    }


}

//隐藏来电提醒
void CDlgTalkUIFrame::HideIncomingCallNotify()
{
    m_qIncomingCallNotify.hide();
}

void CDlgTalkUIFrame::DrawDsskeyBtnOnBackground()
{
    // 绘制背景图片
    m_pmBG = THEME_GET_BMP(PIC_THEME1_OTHER_BACKGROUND);

    QPixmap pmBtnBg = THEME_GET_BMP(PIC_DSS_BTN_NORMAL);
    QPainter painter(&m_pmBG);
    int i = 0;
    for (i = 0 ; i < 6; ++i)
    {
        painter.drawPixmap(0, 50 + 57 * i, pmBtnBg);
    }

    for (i = 0; i < 6; ++i)
    {
        painter.drawPixmap(644, 50 + 57 * i, pmBtnBg);
    }

    painter.drawPixmap(158, 48, THEME_GET_BMP(PIC_GENERAL_TALKBACKGROUND));
}

bool CDlgTalkUIFrame::IsBluetoothEnable() const
{
    return Bluetooth_GetDeviceState() != DS_NO_DONGLE && Bluetooth_IsEnable();
}

//改变Title
void CDlgTalkUIFrame::OnChangeTitle(const yl::string & strTitle)
{
    _MainWND.SetTitle(strTitle.c_str());
}

// 刷新Softkey数据
void CDlgTalkUIFrame::RefreshSoftkey()
{
    if (NULL == m_pTalkUI)
    {
        return;
    }
    //m_pTalkUI->GetSoftkeyData(m_SoftkeyData);
    //获取子窗口的softkey
    GetTalkUISoftkey();

    // !!!!!,MainWnd提供C接口
    _MainWND.DrawSoftkey(this);

    // 刷新IME信息
    _MainWND.DisplayIMESoftkey(focusWidget());
}

void CDlgTalkUIFrame::OnAddTitleIcon(const yl::string & strIconPath, bool bAdd)
{
    if (NULL == m_pTalkUI)
    {
        return;
    }

    //不显示联系人头像的时候 需要在 topbar 上显示图标
    if (!(m_pTalkUI->HasDisplayContactImage()))
    {
        m_qTopBar.AddOrDeleteIcon(strIconPath, bAdd);
    }
    else
    {
        m_pTalkUI->AddOrDeleteIcon(strIconPath.c_str(), bAdd);
    }
}

void CDlgTalkUIFrame::OnBackButtonClick()
{
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SendUIRequestAction(TALK_ACTION_CANCEL);
    }
}

// 切换话路
void CDlgTalkUIFrame::OnSwitchSession()
{
    ChangeNextSession();
}

void CDlgTalkUIFrame::OnFocusUIGeometryChange(bool bShow, const QRect & geometry)
{
    if (NULL != m_pTalkUI && !m_pTalkUI->inherits("CDlgDialingUI"))
    {
        if (bShow && geometry.isValid())
        {
            TALKUI_INFO("TalkUIRect[x:%d,y:%d]", geometry.x(), geometry.y());
            TALKUI_INFO("TalkUIRect[w:%d,h:%d]", geometry.width(), geometry.height());

            m_pTalkUI->SetTalkUIRect(geometry);

            if (m_pTalkUI->isHidden())
            {
                m_pTalkUI->raise();
                m_pTalkUI->show();
                RefreshBlueToothIcon();
            }

#ifdef IF_FEATURE_ENTRANCE_GUARD
            if (m_RectDoorLineLast != geometry)
            {
                m_RectDoorLineLast = geometry;
                m_pLabEntranceGuard->setGeometry(geometry);

                RefreshDoorLine();
            }
#endif
        }
        else
        {
            m_pTalkUI->hide();
        }
    }
}

//切页操作
void CDlgTalkUIFrame::OnNotifyScroll()
{
    SetTipRegionVisiblePolicy();
}

//来电提醒提示条点击
void CDlgTalkUIFrame::OnIncomingCallNotifyClick()
{
    if (NULL != m_pTalkUI)
    {
        //当前显示的窗口是 来电窗口  且来电窗口这个时候是隐藏的  那么刷新来电窗口就可以了
        if (m_pTalkUI->GetSessionId() == talklogic_GetIncomingCallSessionId() && !(m_pTalkUI->isVisible()
                || !m_pTalkUI->isHidden()))
        {
            talklogic_UpdateWnd();

            return ;
        }
    }

    //不是的话 就要切换窗口
    //调用Logic层的改变焦点事件
    talklogic_UIEvent(talklogic_GetIncomingCallSessionId(), TALK_ACTION_FOCUSE_CHANGE,
                      talklogic_GetIncomingCallSessionId(), 0);

}

void CDlgTalkUIFrame::OnDTMFLineVisableChanged(bool bVisible)
{
    if (UIManager_IsInTalk())
    {
        if (bVisible)
        {
            //隐藏音量条
            // !!!!!,MainWnd提供C接口
            _MainWND.SetVolumeBarVisible(false);

            VisibleCCStatusLabel(false);

            ProcessTalkTipType(TALK_TIP_DTMF);
        }
        else
        {
            SetTipRegionVisiblePolicy();
        }
    }
}

//设置DTMF条是否显示
void CDlgTalkUIFrame::SetDTMFLineVisible(bool bVisible)
{
    if (m_pDtmfLine != NULL)
    {
        if (bVisible)
        {
            QRect rect = DTMF_LINE_GEOMETRY;
            if (NULL != m_pTalkUI)
            {
                rect.moveTo(rect.x(), m_pTalkUI->geometry().bottom() - DTMF_LINE_GEOMETRY.height() - 1);
            }
            m_pDtmfLine->setGeometry(rect);
            m_pDtmfLine->SetDtmfLineVisible(bVisible);
            m_pDtmfLine->raise();
        }

        m_pDtmfLine->SetDtmfLineVisible(bVisible);
    }
}

//检测网络情况
void CDlgTalkUIFrame::CheckNetwork()
{
    if (m_pLabNetworkNote == NULL)
    {
        return;
    }

    //先判断网络是否正常
    if (!netIsNetworkReady()
            && m_pTalkUI != NULL)
    {
        bool bIsFinishUI = false;
        if (m_pTalkUI != NULL)
        {
            bIsFinishUI = (m_pTalkUI->GetTalkUIType() == TALK_UI_FINISHED);
        }

        //如果上次的网络状态也异常，不需要再进行刷新
        if (!m_bIsCableConnect)
        {
            // http://10.3.5.199/Bug.php?BugID=63498
            //这边要提到上层显示
            if (NULL != m_pLabNetworkNote)
            {
                //FinishUI界面已有网络连接异常提示，这里不需要再提示，否则会重叠
                if (!bIsFinishUI)
                {
                    m_pLabNetworkNote->show();
                    m_pLabNetworkNote->raise();
                }
                else
                {
                    m_pLabNetworkNote->hide();
                }
            }
            return;
        }
        //改变标记
        m_bIsCableConnect = false;

        //FinishUI界面已有网络连接异常提示，这里不需要再提示，否则会重叠
        if (!bIsFinishUI)
        {
            //网络连接异常，显示提示
            QRect rect = NETWORK_NOTE_GEOMETRY;
            rect.moveTo(rect.x(), m_pTalkUI->geometry().bottom() - NETWORK_NOTE_GEOMETRY.height());
            m_pLabNetworkNote->setGeometry(rect);
            m_pLabNetworkNote->show();
            m_pLabNetworkNote->raise();
        }
        else
        {
            m_pLabNetworkNote->hide();
        }

        VisibleCCStatusLabel(false);

        ProcessTalkTipType(TALK_TIP_NETWORK);
    }
    else
    {
        //如果上次的网络状态也正常，不需要再进行刷新
        if (m_bIsCableConnect)
        {
            return;
        }

        m_bIsCableConnect = true;

        m_pLabNetworkNote->hide();

        SetTipRegionVisiblePolicy();
    }
}

// 切换到下一个Session
void CDlgTalkUIFrame::ChangeNextSession()
{
    TALKUI_INFO("CDlgTalkUIFrame::ChangeNextSession");
    //直接使用逻辑层的接口
    talklogic_SwitchFocusSession(true);
}

// 切换到上一个Session
void CDlgTalkUIFrame::ChangePrevSession()
{
    TALKUI_INFO("CDlgTalkUIFrame::ChangePrevSession");
    //直接使用逻辑层的接口
    talklogic_SwitchFocusSession(false);
}

//获取当前是否要显示 新来电提醒
bool CDlgTalkUIFrame::IsNeedShowIncomingCallNotify()
{
    //对方的信息
    yl::string strIncomingCallNum = "";

    //获取对方的信息  如果没有获取到 直接返回false
    if (!talklogic_GetIncomingCallNum(strIncomingCallNum))
    {
        return false;
    }

    //当前有显示sessionID  那么返回false
    if (NULL != m_pTabManager && true == m_pTabManager->GetIsShowIncomingCallSession())
    {
        return false;
    }

    if (NULL != m_pTalkUI)
    {
        //焦点界面是隐藏的 那么需要显示
        if (!m_pTalkUI->isVisible() || m_pTalkUI->isHidden())
        {
            return true;
        }
        else if (m_pTalkUI->inherits(DLG_Talking)
                 || m_pTalkUI->inherits(DLG_Conference))   //焦点界面是通话或者会议 那么需要显示
        {
            return true;
        }
    }

    return false;
}

//处理各种类型的通话提示类型
void CDlgTalkUIFrame::ProcessTalkTipType(TALK_TIP_TYPE eType)
{
    if (m_pTalkUI != NULL)
    {
        m_pTalkUI->OnTipChange(eType);
    }
}

//获取子窗口的 softkey
void CDlgTalkUIFrame::GetTalkUISoftkey()
{
    //直接调用basedialog的 获取softkey 的函数就可以了
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->GetSoftkey(m_SoftkeyData);
    }
}

//设置提示区域的显示策略
void CDlgTalkUIFrame::SetTipRegionVisiblePolicy()
{
    //暂时先隐藏来电提醒
    HideIncomingCallNotify();

    //优先级定义如下：
    //1.用户或环境(如网络、Transfer Failed等)主动触发的控件处于顶层，该部分策略写入各事件触发函数中
    //2.若顶层控件自动消失后，下层控件则按照Network-->DTMF-->VolumeBar-->Custom优先级
    bool bShowCCStatusLab = false;
    if (m_pLabNetworkNote != NULL
            && m_pLabNetworkNote->isVisible())
    {
        //网络提示可见
        ProcessTalkTipType(TALK_TIP_NETWORK);
    }
    else if (m_pDtmfLine != NULL
             && m_pDtmfLine->isVisible())
    {
        //DTMF提示可见
        ProcessTalkTipType(TALK_TIP_DTMF);
    }
    else if (_MainWND.GetVolumeBar() != NULL
             && _MainWND.GetVolumeBar()->isVisible())
    {
        //音量条可见
        ProcessTalkTipType(TALK_TIP_VOLUMEBAR);
    }
    else if (IsNeedShowIncomingCallNotify())    //是否显示当前来电提醒
    {
        //显示来电提醒
        ShowIncomingCallNotify();
        //音量条可见
        ProcessTalkTipType(TALK_TIP_VOLUMEBAR);     //暂时当是跟音量条一个等级的
    }
    else if (NULL != m_pTalkUI)
    {
        // 子页面没有提示信息的时候才显示状态信息
        ProcessTalkTipType(TALK_TIP_CUSTOM);

        if (!m_pTalkUI->IsShowTip())
        {
            // 子页面没有提示信息的时候才显示状态信息
            ProcessTalkTipType(TALK_TIP_CCSTATUS);
            bShowCCStatusLab = true;
        }
    }

    // 显示状态和当前不一致的时候才需变化状态
    VisibleCCStatusLabel(bShowCCStatusLab);

}

// 是否允许键盘事件通过
bool CDlgTalkUIFrame::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    return (NULL != m_pTalkUI) ?
           m_pTalkUI->AllowKeyPadEventOnKeyLock(nKeyCode, bPress) :
           true;
}

// 是否允许鼠标事件通过
bool CDlgTalkUIFrame::AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent)
{
    return (NULL != m_pTalkUI) ?
           m_pTalkUI->AllowMouseEventOnKeyLock(pObject, pEvent) :
           true;
}

//事件过滤器!!!!
bool CDlgTalkUIFrame::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    QEvent::Type eEventType = pEvent->type();

    if (UIManager_IsInTalk())//栈顶是否为通话窗口
    {
        switch (eEventType)
        {
        case QEvent::KeyPress:
        {
            //http://10.3.5.199/Bug.php?BugID=65290
            //把删除弹出框清除
            //if (_TalkUIDelegate.GetMessageBoxCount() > 0)
            //{
            //  _TalkUIDelegate.RemoveAllMessageBox();
            //}

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();

            //如果是数字键  先要判断是否需要显示dtmf 条
            if (IsDigitKey(nKey))
            {
                //数字键或ABCD键
                if (m_pDtmfLine != NULL
                        && m_pDtmfLine->isHidden())
                {
                    //Dtmf条不可见情况，若为数字键，询问子窗口，若需要显示则直接显示
                    if (m_pTalkUI != NULL
                            && m_pTalkUI->IsShowDtmfLine())
                    {
                        SetDTMFLineVisible(true);
                    }
                }
            }

            //把dtmf条的处理放到这边来  因为 所有的key按键dtmf条都需要处理的， 数字键需要输入， 非数字键 需要隐藏
            //而在数字键的处理函数中
            //Dtmf条可见情况，交由Dtmf条处理
            if (m_pDtmfLine != NULL
                    && !m_pDtmfLine->isHidden()
                    && m_pDtmfLine->eventFilter(pObject, pEvent))
            {
                //dtmf 处理完成后 不返回   因为 还是要传给 talk 中进行处理的， 现在 dtmf 都是有 对话框  传给 逻辑层
                //  return true;
            }

            //将按键事件传递给具体的Logic层处理
            //将按键值传递给Logic
            if (m_pTalkUI != NULL)
            {
                yl::string strSoftkeyAction = "";
                switch (nKey)
                {
                case PHONE_KEY_FN1:
                case PHONE_KEY_FN2:
                case PHONE_KEY_FN3:
                case PHONE_KEY_FN4:
                {
                    //获取热键栏操作
                    strSoftkeyAction = m_SoftkeyData[nKey - PHONE_KEY_FN1].m_strSoftkeyAction;

                    //响按键音
                    if (NULL != m_pTalkUI)
                    {
                        if (m_pTalkUI->IsSoftkeyNeedToPlayKeyTone(strSoftkeyAction))
                        {
                            int iKey = -1;
                            if (TALK_UI_ACTION_SEND == strSoftkeyAction)
                            {
                                iKey = 1;
                            }
                            else if (TALK_UI_ACTION_DELETE == strSoftkeyAction)
                            {
                                iKey = 3;
                            }

                            if (-1 != iKey)
                            {
                                _TalkUIDelegate.PlayKeyTone(iKey);
                            }

                        }
                    }

                    // 判断Logic是否已处理过按键且要求不再继续往下传递
                    //if(m_pTalkUI->SendUIRequestAction(TALK_ACTION_KEYPRESS, nKey, 0, &strSoftkeyAction))
                    //{
                    //  return true;
                    //}
                    //这边统一交给逻辑层处理一次  就返回 false  因为返回true 会影响softkey的执行
                    m_pTalkUI->SendUIRequestAction(TALK_ACTION_KEYPRESS, nKey, 0, &strSoftkeyAction);

                    //http://10.3.5.199/Bug.php?BugID=54770
                    //由于该BUG 把事件传给子界面处理一下
                    //这边判断TalkUI 是否为空是因为 防止发送了 TALK_ACTION_KEYPRESS 事件后，导致TalkUI变成空值
                    if (NULL != m_pTalkUI)
                    {
                        m_pTalkUI->OnFunckeyPress(nKey);
                    }
                    //这边要始终返回false 以免影响 softkey 的使用
                    return false;

                    //// 判断Logic是否已处理过按键且要求不再继续往下传递
                    //if (m_pTalkUI->SendUIRequestAction(TALK_ACTION_KEYPRESS, nKey, 0, &strSoftkeyAction))
                    //{
                    //  return true;
                    //}
                }
                break;
                default:
                    break;
                }

                //// 判断Logic是否已处理过按键且要求不再继续往下传递
                //if (m_pTalkUI->SendUIRequestAction(TALK_ACTION_KEYPRESS, nKey, 0, &strSoftkeyAction))
                //{
                //  return true;
                //}
            }

            //所有的通知事件都放在这边 在子函数中就不需要再传了
            if (NULL != m_pTalkUI)
            {
                m_pTalkUI->SendUIRequestAction(TALK_ACTION_KEYPRESS, nKey, 0, NULL);
            }


            //判断是否是通道图标 如果是通道图标处理完后直接返回true 不需要继续了
            if (voice_IsChannelKey(nKey))
            {
                return OnChannelKeyPress(pObject, pKeyEvent, nKey);
                //如果子对话框有处理 直接返回true 没有处理 返回false
                //if(true == OnChannelKeyPress(nKey))
                //{
                //  return true;
                //}

                //return false;
            }

            //如果是数字键 交给数字键处理函数处理
            if (IsDigitKey(nKey))
            {
                return OnDigitkeyPress(pObject, pEvent, nKey);
            }

            //其他的交给功能键函数处理
            return OnFunckeyPress(pObject, pEvent, nKey);


            //  switch(nKey)
            //  {
            //  case PHONE_KEY_1:
            //  case PHONE_KEY_2:
            //  case PHONE_KEY_3:
            //  case PHONE_KEY_4:
            //  case PHONE_KEY_5:
            //  case PHONE_KEY_6:
            //  case PHONE_KEY_7:
            //  case PHONE_KEY_8:
            //  case PHONE_KEY_9:
            //  case PHONE_KEY_0:
            //  case PHONE_KEY_POUND:
            //  case PHONE_KEY_STAR:
            //  case PHONE_KEY_A:
            //  case PHONE_KEY_B:
            //  case PHONE_KEY_C:
            //  case PHONE_KEY_D:
            //      {
            //          //数字键或ABCD键
            //          if (m_pDtmfLine != NULL
            //              && m_pDtmfLine->isHidden())
            //          {
            //              //Dtmf条不可见情况，若为数字键，询问子窗口，若需要显示则直接显示
            //              if (m_pTalkUI != NULL
            //                  && m_pTalkUI->IsShowDtmfLine())
            //              {
            //                  SetDTMFLineVisible(true);
            //              }
            //          }
            //      }
            //      break;
            //  case PHONE_KEY_CANCEL:
            //      {
            //          TALKUI_INFO("PHONE_KEY_CANCEL");
            //          //Cancel按键
            //          if (m_pTalkUI != NULL)
            //          {
            //              m_pTalkUI->OnUIAction(TALK_UI_ACTION_CANCEL);
            //          }
            //          return true;
            //      }
            //      break;
            //  case PHONE_KEY_UP:
            //  case PHONE_KEY_DOWN:
            //      {
            //          if (m_pTalkUI != NULL
            //              && m_pTalkUI->eventFilter(pObject, pEvent))
            //          {
            //              return true;
            //          }

            //          if (NULL != m_pTabManager)
            //          {
            //              (nKey == PHONE_KEY_UP) ?
            //              m_pTabManager->OnNotifyPrePage() :
            //              m_pTabManager->OnNotifyNextPage();
            //          }
            //      }
            //      break;
            //  default:
            //      break;
            //  }

            //             if (m_pTalkUI != NULL
            //      && m_pTalkUI->eventFilter(pObject, pEvent))
            //             {
            //      return true;
            //             }

            //  //Dtmf条可见情况，交由Dtmf条处理
            //  if (m_pDtmfLine != NULL
            //      && !m_pDtmfLine->isHidden()
            //      && m_pDtmfLine->eventFilter(pObject, pEvent))
            //  {
            //      return true;
            //  }
        }
        break;
        case QEvent::MouseButtonPress:
        {
            //将点击事件传递给具体的Logic层处理
            //http://bugfree.yealink.com/Bug.php?BugID=94901 添加MessageBox_GetMessageBoxCount == 0的条件
            if (m_pTalkUI != NULL && MessageBox_GetMessageBoxCount() == 0)
            {
                m_pTalkUI->SendUIRequestAction(TALK_ACTION_SCREEN_CLICK);
            }

            if (m_pTalkUI != NULL
                    //  && !m_pDtmfLine->isHidden()
                    && m_pTalkUI->eventFilter(pObject, pEvent))
            {
                return true;
            }

            //Dtmf条可见情况，交由Dtmf条处理
            if (m_pDtmfLine != NULL
                    && m_pDtmfLine->eventFilter(pObject, pEvent))
            {
                return true;
            }
        }
        break;
        case QEvent::MouseButtonRelease:
        case QEvent::KeyRelease:
        {
            if (m_pTalkUI != NULL)
            {
                return m_pTalkUI->eventFilter(pObject, pEvent);
            }
        }
        break;
        default:
            break;
        }
    }
    return false;
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
// 更新Queue状态
void CDlgTalkUIFrame::UpdateQueueStatus(const yl::string & strQueueStatus)
{
    if (m_pCCStatusLabel != NULL)
    {
        m_pCCStatusLabel->setText(strQueueStatus.c_str());
    }
}
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
void CDlgTalkUIFrame::USBRecordStateChange()
{
    if (m_pTalkUI != NULL)
    {
        //m_pTalkUI->AddUSBRecordSoftkey(m_SoftkeyData);
        //m_pTalkUI->BindSoftkeyAction(m_SoftkeyData);
        m_pTalkUI->RefreshRecordSoftKey(m_SoftkeyData);
    }

    if (!modRecord_IsRecordEnable(RECORD_AUDIO))
    {
        //设置bsft录音图标
        SetRecordTip(m_pTalkUI->GetRecordIconType());
        MainWnd_UpdateSoftkey();
    }
    else
    {
        DeleteBSFTRecordIcon();
    }
}
#endif

bool CDlgTalkUIFrame::OnAction(const QString & strAction)
{
    return m_pTalkUI->OnUIAction(strAction);
}

bool CDlgTalkUIFrame::OnTransparencyChange()
{
    DrawDsskeyBtnOnBackground();

    return true;
}

//由于联系人头像显示的变化导致的布局变化
void CDlgTalkUIFrame::ResetWidgetByContactPhotoVisibleChange(bool bUpdate /*= true*/)
{
    if (NULL == m_pTalkUI)
    {
        return;
    }

    //不显示联系人头像 那么topbar 要show 出来
    if (!(m_pTalkUI->HasDisplayContactImage()))
    {
        m_qTopBar.show();
    }
    else    //显示联系人头像  topbar 要隐藏
    {
        m_qTopBar.hide();
    }

    if (bUpdate)
    {
        talklogic_UpdateWnd();
    }

    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->ResetWidgetByContactPhotoVisibleChange();
    }
}

void CDlgTalkUIFrame::SlotResetWidgetByContactPhotoVisibleChange()
{
    ResetWidgetByContactPhotoVisibleChange(true);
}

void CDlgTalkUIFrame::DeleteBSFTRecordIcon()
{
    OnAddTitleIcon(PIC_RECORD_FULL, false);
    OnAddTitleIcon(PIC_RECORD_NORMAL, false);
    OnAddTitleIcon(PIC_RECORD_PAUSE, false);
}


#ifdef IF_FEATURE_ENTRANCE_GUARD
void CDlgTalkUIFrame::DrawEntranceGuard(const QString & strContent, bool bAsTxt/* = true*/)
{
    if (bAsTxt)
    {
        // 显示文字时，设置半透明效果
        QPalette pal = m_pLabEntranceGuard->palette();
        pal.setColor(QPalette::Background, QColor(0x00, 0xff, 0x00, 0x00));
        m_pLabEntranceGuard->setPalette(pal);

        m_pLabEntranceGuard->setText(strContent);
    }
    else if (pathFileExist(strContent.toUtf8().data()))
    {
        // 图片太大则进行裁剪
        QImageReader imageReader(strContent);
        QSize sz = imageReader.size();

        int iwidth = sz.width();
        int iHeight = sz.height();

        QSize szLable = m_pLabEntranceGuard->geometry().size();

        TALKUI_INFO("DrawEntranceGuard w=[%d] h=[%d] lable=[%d][%d]", iwidth, iHeight, szLable.width(),
                    szLable.height());

        // 防止图片分辨率太大，对分辨率做限制
        int iPixel = iwidth * iHeight;
        if (iPixel > 1280 * 720 * 2)
        {
            m_pLabEntranceGuard->setText(LANG_TRANSLATE(TRID_FILE_IS_TOO_LARGE));
        }
        else if (iPixel > szLable.width() * szLable.height())
        {
            QPixmap pm(strContent);
            pm = pm.scaled(szLable);
            m_pLabEntranceGuard->clear();
            m_pLabEntranceGuard->setPixmap(pm);
        }
        else
        {
            m_pLabEntranceGuard->clear();
            m_pLabEntranceGuard->setPixmap(QPixmap(strContent));
        }
    }
    else
    {
        return;
    }

    m_pLabEntranceGuard->setVisible(true);
    RefreshDoorLine();
}

bool CDlgTalkUIFrame::OnDoorLineMessage(msgObject & refMessage)
{
    TALKUI_INFO("OnDoorLineMessage msg=[%#x][%d] w=[%d] l=[%d] session=[%d]",
                refMessage.message, refMessage.message - ENTRANCE_GUARD_MESSAGE_BEGIN,
                refMessage.wParam, refMessage.lParam, m_pTalkUI ? m_pTalkUI->GetSessionId() : -1);

    bool bHanlded = true;
    switch (refMessage.message)
    {
    case ENTRANCE_GUARD_MESSAGE_STOP:
    case ENTRANCE_GUARD_MESSAGE_PAUSE:
    {
        m_pLabEntranceGuard->hide();
    }
    break;
    case ENTRANCE_GUARD_MESSAGE_START:
    {
        // 取消链接提示
        //DrawEntranceGuard(LANG_TRANSLATE(TRID_CONNECTING));
    }
    break;
    case ENTRANCE_GUARD_MESSAGE_REFRESH:
    {
        if (refMessage.GetExtraData() != NULL)
        {
            const char * strFile = (const char *)refMessage.GetExtraData();
            DrawEntranceGuard(toQString(strFile), false);
        }
    }
    break;
    case ENTRANCE_GUARD_MESSAGE_TIME_OUT:
    {
        DrawEntranceGuard(LANG_TRANSLATE(TRID_TIME_OUT));
    }
    break;
    case ENTRANCE_GUARD_MESSAGE_ERROR:
    {
        DrawEntranceGuard(LANG_TRANSLATE(TRID_LOAD_FAILED));
    }

    default:
        bHanlded = false;
    }

    return bHanlded;
}

void CDlgTalkUIFrame::RefreshDoorLine(bool bShow /*= true*/)
{
    if (bShow)
    {
        if (m_pLabEntranceGuard->isVisible())
        {
            m_pLabEntranceGuard->raise();
            m_pLabEntranceGuard->show();
        }

        // DTMF优先级更高
        if (m_pDtmfLine != NULL && m_pDtmfLine->isVisible())
        {
            m_pDtmfLine->raise();
            m_pDtmfLine->SetDtmfLineVisible(true, true);
        }
    }
    else
    {
        m_pLabEntranceGuard->lower();
        m_pLabEntranceGuard->clear();
        m_pLabEntranceGuard->hide();
    }
}
#endif
