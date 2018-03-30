#include "usbaudioitem.h"

#include "baseui/t4xpicpath.h"
#include "uikernel/ilistitemdelegate.h"
#include "uikernel/languagehelper.h"
#include "uicommon/imagemanager/modimagemanager.h"

namespace
{
//文件名位置
#define USB_AUDIO_ITEM_NAME_LABEL_LEFT          87
#define USB_AUDIO_ITEM_NAME_LABEL_TOP           7
#define USB_AUDIO_ITEM_NAME_LABEL_WIDTH         340
#define USB_AUDIO_ITEM_NAME_LABEL_HEIGHT        20

//进度条位置
#define USB_AUDIO_ITEM_PROCESS_BAR_LEFT         77
#define USB_AUDIO_ITEM_PROCESS_BAR_TOP          7
#define USB_AUDIO_ITEM_PROCESS_BAR_WIDTH        355
#define USB_AUDIO_ITEM_PROCESS_BAR_HEIGHT       20

//大小信息位置
#define USB_AUDIO_ITEM_DETAIL_LABEL_LEFT        87
#define USB_AUDIO_ITEM_DETAIL_LABEL_TOP         29
#define USB_AUDIO_ITEM_DETAIL_LABEL_WIDTH       100
#define USB_AUDIO_ITEM_DETAIL_LABEL_HEIGHT      20

//暂停提示图标位置
#define USB_AUDIO_ITEM_PAUSE_ICON_LEFT          225
#define USB_AUDIO_ITEM_PAUSE_ICON_TOP           29
#define USB_AUDIO_ITEM_PAUSE_ICON_WIDTH         12
#define USB_AUDIO_ITEM_PAUSE_ICON_HEIGHT        18

//现在播放时间位置
#define USB_AUDIO_ITEM_CURRENT_TIME_LEFT        253
#define USB_AUDIO_ITEM_CURRENT_TIME_TOP         29
#define USB_AUDIO_ITEM_CURRENT_TIME_WIDTH       80
#define USB_AUDIO_ITEM_CURRENT_TIME_HEIGHT      20

//分割现在时间和总时间斜杠位置
#define USB_AUDIO_ITEM_SLASH_LEFT               333
#define USB_AUDIO_ITEM_SLASH_TOP                29
#define USB_AUDIO_ITEM_SLASH_WIDTH              20
#define USB_AUDIO_ITEM_SLASH_HEIGHT             20

//总时间位置
#define USB_AUDIO_ITEM_TOTAL_TIME_LEFT          353
#define USB_AUDIO_ITEM_TOTAL_TIME_TOP           29
#define USB_AUDIO_ITEM_TOTAL_TIME_WIDTH         80
#define USB_AUDIO_ITEM_TOTAL_TIME_HEIGHT        20

//提示文字位置
#define USB_AUDIO_ITEM_HINTLAB_LEFT             353
#define USB_AUDIO_ITEM_HINTLAB_TOP              29
#define USB_AUDIO_ITEM_HINTLAB_WIDTH            100
#define USB_AUDIO_ITEM_HINTLAB_HEIGHT           20

#define USB_AUDIO_ITEM_HINT_DISAPPEAR_TIME      1000
}

CUSBAudioItem::CUSBAudioItem(QWidget * pParent, QWidgetPair & pair)
    : CUSBItemBase(pParent, pair)
    , CUSBTimeCounter()
    , m_pProgressBar(NULL)
    , m_PlayState(USB_PLAY_STATE_STOP)
{
    USBAudioItemInit();
    m_fileInfo.Reset();
}

CUSBAudioItem::~CUSBAudioItem(void)
{
    if (!m_fileInfo.strName.empty())
    {
        UILogicDoAction(USB_CLICK_ACTION_STOP);
    }

    if (NULL != m_pProgressBar)
    {
        disconnect(m_pProgressBar, SIGNAL(ProcessBarSwift(bool, bool)), this, SLOT(OnProcessBarSwift(bool,
                   bool)));
        disconnect(m_pProgressBar, SIGNAL(mouseAction(int)), this, SLOT(mouseAction(int)));
        m_pProgressBar->setParent(NULL);
        m_pProgressBar->deleteLater();
        m_pProgressBar = NULL;
    }
}

void CUSBAudioItem::USBAudioItemInit()
{
    if (NULL != m_pLabDisplay)
    {
        m_pLabDisplay->setParent(this);

        m_pLabDisplay->show();

        m_pLabDisplay->setAttribute(Qt::WA_TranslucentBackground, true);
    }

    m_labelBottom.show();

    m_strCurrentTime.setParent(this);
    m_strCurrentTime.hide();
    m_strCurrentTime.setText("");
    m_strCurrentTime.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_strCurrentTime.setAttribute(Qt::WA_TranslucentBackground, true);

    m_strTotalTime.setParent(this);
    m_strTotalTime.show();
    m_strTotalTime.setText("");
    m_strTotalTime.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_strTotalTime.setAttribute(Qt::WA_TranslucentBackground, true);

    m_strSlash.setParent(this);
    m_strSlash.hide();
    m_strSlash.setText("/");
    m_strSlash.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_strSlash.setAttribute(Qt::WA_TranslucentBackground, true);

    m_PicShowPause.setParent(this);
    m_PicShowPause.setPixmap(THEME_GET_BMP(PIC_USB_BROWSE_AUDIO_HINT_PAUSE));
    m_PicShowPause.hide();

    m_strHint.setParent(this);
    m_strHint.setText("");
    m_strHint.hide();
    m_strHint.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_strHint.setAttribute(Qt::WA_TranslucentBackground, true);

    QPalette textPalette;
    textPalette.setColor(QPalette::WindowText, USB_COLOR_GRAY);
    m_strTotalTime.setPalette(textPalette);
    m_strCurrentTime.setPalette(textPalette);
    m_strSlash.setPalette(textPalette);
    textPalette.setColor(QPalette::WindowText, USB_COLOR_BLACK);
    m_strHint.setPalette(textPalette);

    InitProgressBar();

    m_timer.setParent(this);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimerCountOut()));

    Relayout();
}

void CUSBAudioItem::InitProgressBar()
{
    m_pProgressBar = new CUSBProcessBar(this);
    if (NULL == m_pProgressBar)
    {
        return;
    }

    m_pProgressBar->hide();

    m_pProgressBar->setAttribute(Qt::WA_TranslucentBackground, true);

    m_pProgressBar->SetImage(CProcessBarUtility::PCE_PROGRESSBAR, PIC_USB_AUDIO_PROCESSBAR_EMPTY,
                             PIC_USB_AUDIO_PROCESSBAR_FULL, PIC_USB_AUDIO_PROGRESSBAR_CURPOS);

    //设置焦点代理
    m_pProgressBar->setFocusProxy(this);

    m_pProgressBar->SetValue(0);

    //默认情况设置有效值
    m_pProgressBar->SetRange(0, USB_PROCESSBAR_TOTAL);

    //设置步长
    m_pProgressBar->SetStep(1);

    m_pProgressBar->SetSliderWidth(USB_AUDIO_ITEM_PROCESS_BAR_WIDTH);
    m_pProgressBar->SetSliderHeight(USB_AUDIO_ITEM_PROCESS_BAR_HEIGHT);

    //进度条被按左右键时
    connect(m_pProgressBar, SIGNAL(ProcessBarSwift(bool, bool)), this, SLOT(OnProcessBarSwift(bool,
            bool)));

    //进度条被操作时发信号给父控件
    connect(m_pProgressBar, SIGNAL(mouseAction(int)), this, SLOT(mouseAction(int)));
}

//加载播放数据
void CUSBAudioItem::LoadPlayData(long nTotalSeconds, long nCurrentSeconds)
{
    SETTINGUI_INFO("CUSBAudioItem::LoadPlayData nTotalSeconds[%d] nCurrentSeconds[%d]", nTotalSeconds,
                   nCurrentSeconds);
    CUSBItemBase::LoadPlayData(nTotalSeconds, nCurrentSeconds);

    if (nTotalSeconds > 0)
    {
        m_strTotalTime.SetTimeText(nTotalSeconds);
    }

    m_strCurrentTime.SetTimeText(nCurrentSeconds);

    StopCountTime();

    m_nTotalSeconds = nTotalSeconds;
    m_nCurrentSeconds = nCurrentSeconds;

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->SetTimeValue(m_nTotalSeconds, m_nCurrentSeconds);
    }
}

void CUSBAudioItem::SetPlayStatus(USB_PLAY_STATE PlayState)
{
    if (m_PlayState == PlayState)
    {
        return;
    }

    m_PlayState = PlayState;

    switch (m_PlayState)
    {
    case USB_PLAY_STATE_PLAY:
        {
            if (NULL != m_pLabDisplay)
            {
                m_pLabDisplay->hide();
            }
            if (NULL != m_pProgressBar)
            {
                m_pProgressBar->show();
            }
            m_PicShowPause.hide();
            m_strSlash.show();
            m_strCurrentTime.show();
            m_strTotalTime.show();
        }
        break;
    case USB_PLAY_STATE_PAUSE:
        {
            if (NULL != m_pLabDisplay)
            {
                m_pLabDisplay->hide();
            }
            if (NULL != m_pProgressBar)
            {
                m_pProgressBar->show();
            }
            m_PicShowPause.show();
            m_strSlash.show();
            m_strCurrentTime.show();
            m_strTotalTime.show();
        }
        break;
    case USB_PLAY_STATE_STOP:
        {
            if (NULL != m_pLabDisplay)
            {
                m_pLabDisplay->show();
            }
            if (NULL != m_pProgressBar)
            {
                m_pProgressBar->hide();
            }
            m_PicShowPause.hide();
            m_strSlash.hide();
            m_strCurrentTime.hide();
            m_strTotalTime.hide();
        }
        break;
    case USB_PLAY_STATE_WAIT:
        {
            if (NULL != m_pLabDisplay)
            {
                m_pLabDisplay->show();
            }
            if (NULL != m_pProgressBar)
            {
                m_pProgressBar->hide();
            }
            m_PicShowPause.hide();
            m_strSlash.hide();
            m_strCurrentTime.hide();
            m_strTotalTime.hide();
        }
        break;
    default:
        {
            if (NULL != m_pLabDisplay)
            {
                m_pLabDisplay->hide();
            }
            if (NULL != m_pProgressBar)
            {
                m_pProgressBar->hide();
            }
            m_PicShowPause.hide();
            m_strSlash.hide();
            m_strCurrentTime.hide();
            m_strTotalTime.hide();
        }
        break;
    }
    update();
}
void CUSBAudioItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    ApplyNomalStyle();

    if (m_PlayState == USB_PLAY_STATE_STOP
            || m_PlayState == USB_PLAY_STATE_WAIT)
    {
        PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
    }

    if (USB_PLAY_STATE_PLAY == m_PlayState)
    {
        //左侧：如果是播放就显示暂停图标
        m_btnLeft.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_PAUSE, PIC_USB_BROWSE_ITEM_PAUSE_PRESS, "",
                              USB_ITEM_LEFT_BTN_PIC_RECT);
        //右侧：如果是播放就显示停止图标
        m_btnRight.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_STOP, PIC_USB_BROWSE_ITEM_STOP_PRESS, "",
                               USB_ITEM_RIGHT_BTN_PIC_RECT);
    }
    else if (USB_PLAY_STATE_PAUSE == m_PlayState)
    {
        //左侧：如果是暂停就显示播放图标
        m_btnLeft.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_PLAY, PIC_USB_BROWSE_ITEM_PLAY_PRESS, "",
                              USB_ITEM_LEFT_BTN_PIC_RECT);
        //右侧：如果是暂停也显示停止图标
        m_btnRight.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_STOP, PIC_USB_BROWSE_ITEM_STOP_PRESS, "",
                               USB_ITEM_RIGHT_BTN_PIC_RECT);
    }
    else if (USB_PLAY_STATE_STOP == m_PlayState)
    {
        //左侧：如果是停止也显示播放图标
        m_btnLeft.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_PLAY, PIC_USB_BROWSE_ITEM_PLAY_PRESS, "",
                              USB_ITEM_LEFT_BTN_PIC_RECT);
        //右侧：如果是停止则显示删除图标
        m_btnRight.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_DELETE, PIC_USB_BROWSE_ITEM_DELETE_PRESS,
                               "", USB_ITEM_RIGHT_DELETE_BTN_PIC_RECT);
    }
    else if (USB_PLAY_STATE_WAIT == m_PlayState)
    {
        //左侧：如果是等待则显示播放图标
        m_btnLeft.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_PLAY, PIC_USB_BROWSE_ITEM_PLAY_PRESS, "",
                              USB_ITEM_LEFT_BTN_PIC_RECT);
        //右侧：不显示图标
        m_btnRight.PaintButton(stylePainter, "", "");
    }
    else
    {
        //Error
    }
}

void CUSBAudioItem::Relayout()
{
    if (NULL == m_pLabDisplay)
    {
        return;
    }

    m_pLabDisplay->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_pLabDisplay->setGeometry(USB_AUDIO_ITEM_NAME_LABEL_LEFT
                               , USB_AUDIO_ITEM_NAME_LABEL_TOP
                               , USB_AUDIO_ITEM_NAME_LABEL_WIDTH
                               , USB_AUDIO_ITEM_NAME_LABEL_HEIGHT);

    m_labelBottom.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_labelBottom.setGeometry(USB_AUDIO_ITEM_DETAIL_LABEL_LEFT
                              , USB_AUDIO_ITEM_DETAIL_LABEL_TOP
                              , USB_AUDIO_ITEM_DETAIL_LABEL_WIDTH
                              , USB_AUDIO_ITEM_DETAIL_LABEL_HEIGHT);

    m_btnLeft.SetRect(USB_ITEM_LEFT_BTN_LEFT, USB_ITEM_LEFT_BTN_TOP
                      , USB_ITEM_LEFT_BTN_WIDTH, USB_ITEM_LEFT_BTN_HEIGHT);

    m_btnRight.SetRect(USB_ITEM_RIGHT_BTN_LEFT, USB_ITEM_RIGHT_BTN_TOP
                       , USB_ITEM_RIGHT_BTN_WIDTH, USB_ITEM_RIGHT_BTN_HEIGHT);

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->setGeometry(USB_AUDIO_ITEM_PROCESS_BAR_LEFT
                                    , USB_AUDIO_ITEM_PROCESS_BAR_TOP
                                    , USB_AUDIO_ITEM_PROCESS_BAR_WIDTH
                                    , USB_AUDIO_ITEM_PROCESS_BAR_HEIGHT);
    }

    m_PicShowPause.setGeometry(USB_AUDIO_ITEM_PAUSE_ICON_LEFT
                               , USB_AUDIO_ITEM_PAUSE_ICON_TOP
                               , USB_AUDIO_ITEM_PAUSE_ICON_WIDTH
                               , USB_AUDIO_ITEM_PAUSE_ICON_HEIGHT);

    m_strCurrentTime.setGeometry(USB_AUDIO_ITEM_CURRENT_TIME_LEFT
                                 , USB_AUDIO_ITEM_CURRENT_TIME_TOP
                                 , USB_AUDIO_ITEM_CURRENT_TIME_WIDTH
                                 , USB_AUDIO_ITEM_CURRENT_TIME_HEIGHT);

    m_strTotalTime.setGeometry(USB_AUDIO_ITEM_TOTAL_TIME_LEFT
                               , USB_AUDIO_ITEM_TOTAL_TIME_TOP
                               , USB_AUDIO_ITEM_TOTAL_TIME_WIDTH
                               , USB_AUDIO_ITEM_TOTAL_TIME_HEIGHT);

    m_strSlash.setGeometry(USB_AUDIO_ITEM_SLASH_LEFT
                           , USB_AUDIO_ITEM_SLASH_TOP
                           , USB_AUDIO_ITEM_SLASH_WIDTH
                           , USB_AUDIO_ITEM_SLASH_HEIGHT);

    m_strHint.setGeometry(USB_AUDIO_ITEM_HINTLAB_LEFT
                          , USB_AUDIO_ITEM_HINTLAB_TOP
                          , USB_AUDIO_ITEM_HINTLAB_WIDTH
                          , USB_AUDIO_ITEM_HINTLAB_HEIGHT);

    update();
}

void CUSBAudioItem::mouseAction(int nMouseEvent)
{
    if (NULL == m_pProgressBar)
    {
        return;
    }

    //当鼠标移动释放的时候，即是调整进度生效的时候
    if (QEvent::MouseButtonRelease == nMouseEvent)
    {
        UILogicDoAction(m_pProgressBar->GetEmitValue());
        m_nCurrentSeconds = m_pProgressBar->GetCurrentSeconds(m_nTotalSeconds);
        m_strCurrentTime.SetTimeText(m_nCurrentSeconds);

        if (USB_PLAY_STATE_PLAY == GetPlayStatus())
        {
            //重新开启计时
            SetCountTime();
            //重新开始播放
            UILogicDoAction(USB_CLICK_ACTION_RESUME);
        }
        else
        {
            StopCountTime();
            UILogicDoAction(USB_CLICK_ACTION_PAUSE);
        }

    }
    //鼠标开始移动，则只更新进度条显示的位置，不进行实际操作
    else if (QEvent::MouseButtonPress == nMouseEvent)
    {
        //由于要移动进度条，进度条要跟随鼠标移动，所以先把计时停止，以免进度条跳动
        StopCountTime();
        //把播放也暂停，防止拖动过程中播放完了
        UILogicDoAction(USB_CLICK_ACTION_PAUSE);
        m_strCurrentTime.SetTimeText(m_pProgressBar->GetCurrentSeconds(m_nTotalSeconds));
    }
}

void CUSBAudioItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return;
    }

    //左边按钮判断
    if (m_btnLeft.ProcessMousePress(pEvent))
    {
        return;
    }

    //右边按钮判断
    if (m_btnRight.ProcessMousePress(pEvent))
    {
        return;
    }

    //item判断
    ProcessMousePress(pEvent);
    pEvent->accept();
}

void CUSBAudioItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    //左边按钮判断
    if (m_btnLeft.ProcessMouseRelease(pEvent))
    {
        AudioItemDoAction(USB_CLICK_BUTTON_SIDE_LEFT);
        return;
    }

    //右边按钮判断
    if (m_btnRight.ProcessMouseRelease(pEvent))
    {
        AudioItemDoAction(USB_CLICK_BUTTON_SIDE_RIGHT);
        return;
    }

    //item判断
    ProcessMouseRelease(pEvent);
}

void CUSBAudioItem::ResetItemInfo(const QWidgetPair & itemData)
{
    CUSBItemBase::ResetItemInfo(itemData);
    SetClickAction(itemData.third);
}

void CUSBAudioItem::AudioItemDoAction(USB_CLICK_BUTTON_SIDE PressedButton)
{
    switch (m_PlayState)
    {
    case USB_PLAY_STATE_PLAY:
        {
            if (USB_CLICK_BUTTON_SIDE_LEFT == PressedButton)
            {
                AudioItemActionPause();
            }
            else
            {
                AudioItemActionStop();
            }
        }
        break;
    case USB_PLAY_STATE_PAUSE:
        {
            if (USB_CLICK_BUTTON_SIDE_LEFT == PressedButton)
            {
                AudioItemActionResume();
            }
            else
            {
                AudioItemActionStop();
            }
        }
        break;
    case USB_PLAY_STATE_STOP:
        {
            if (USB_CLICK_BUTTON_SIDE_LEFT == PressedButton)
            {
                AudioItemActionPlay();
            }
            else
            {
                UILogicDoAction(USB_CLICK_ACTION_DELETE);
            }
        }
        break;
    case USB_PLAY_STATE_WAIT:
        {
            if (USB_CLICK_BUTTON_SIDE_LEFT == PressedButton)
            {
                AudioItemActionPlay();
            }
            else
            {
                //无操作
            }
        }
        break;
    default:
        {
            //无操作
        }
        break;
    }
}

//当收到其他item被点击play的信号
void CUSBAudioItem::OnOtherItemPlay(CUSBAudioItem * pItem)
{
    //如果不是该item发出的，则应进入wait状态
    if (pItem != this)
    {
        AudioItemActionWait();
    }
}

//当收到其他item停止的信号（可能是被手动停止也可能是播放完成）
void CUSBAudioItem::OnOtherItemStop(CUSBAudioItem * pItem)
{
    //如果不是该item发出的，则应进入stop状态
    if (pItem != this)
    {
        AudioItemActionWaitToStop();
    }
}

bool CUSBAudioItem::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->eventFilter(pObject, pEvent);
    }

    return CUSBItemBase::eventFilter(pObject, pEvent);
}

//当计时器时间到时（1秒计时器）
void CUSBAudioItem::OnTimerCountOut()
{
    //如果还没有到最大值，则可以继续增加，并更新进度条和时间显示
    if (m_nTotalSeconds > m_nCurrentSeconds)
    {
        ++m_nCurrentSeconds;
        m_strCurrentTime.SetTimeText(m_nCurrentSeconds);
        if (NULL != m_pProgressBar)
        {
            m_pProgressBar->SetTimeValue(m_nTotalSeconds, m_nCurrentSeconds);
        }
    }
    else
        //如果已经到了最大值，则应停止计时器，设置状态为stop，进行complete的操作
    {
        AudioItemActionComplete();
    }
}

void CUSBAudioItem::AudioItemActionPlay()
{
    if (modRecord_IsRecording(RECORD_AUDIO)
            || modRecord_IsRecordPause(RECORD_AUDIO))
    {
        emit PlayFail(this);
        return;
    }

    // 如果当前Item已经在播放,则先停止播放
    if (!m_fileInfo.strName.empty())
    {
        StopCountTime();
        UILogicDoAction(USB_CLICK_ACTION_STOP);
    }

    if (!modRecord_GetResourceByIndex(m_fileInfo, GetDataIndex(), RECORD_AUDIO))
    {
        return;
    }
    // 先发送播放按下信号,让其他Item关闭正在播放的录音
    emit PlayPress(this);

    HideHintLabel();
    UILogicDoAction(USB_CLICK_ACTION_PLAY);

    SetCountTime();
    SetPlayStatus(USB_PLAY_STATE_PLAY);

    emit PlayStatusChange(this);
}

void CUSBAudioItem::AudioItemActionPause()
{
    UILogicDoAction(USB_CLICK_ACTION_PAUSE);

    StopCountTime();
    SetPlayStatus(USB_PLAY_STATE_PAUSE);
    emit PlayStatusChange(this);
}

void CUSBAudioItem::AudioItemActionResume()
{
    if (m_fileInfo.strName.empty())
    {
        if (!modRecord_GetResourceByIndex(m_fileInfo, GetDataIndex(), RECORD_AUDIO))
        {
            return;
        }
    }

    UILogicDoAction(USB_CLICK_ACTION_RESUME);

    SetCountTime();
    SetPlayStatus(USB_PLAY_STATE_PLAY);
    emit PlayStatusChange(this);
}

void CUSBAudioItem::AudioItemActionStop(bool bActivedStopped/* = true*/)
{
    UILogicDoAction(USB_CLICK_ACTION_STOP);

    StopCountTime();

    //只有从播放状态转为停止状态时才去显示提示
    if (USB_PLAY_STATE_PLAY == GetPlayStatus() ||
            USB_PLAY_STATE_PAUSE == GetPlayStatus())
    {
        m_strHint.show();
        m_strHint.setText(LANG_TRANSLATE(TRID_USB_HINT_STOPPED));
        QTimer::singleShot(USB_AUDIO_ITEM_HINT_DISAPPEAR_TIME, this, SLOT(HideHintLabel()));
    }

    SetPlayStatus(USB_PLAY_STATE_STOP);

    m_fileInfo.Reset();

    //如果是被动关闭的话就不用在发送StopPress信号出去了
    if (bActivedStopped)
    {
        emit StopPress(this);
        emit PlayStatusChange(this);
    }
}

void CUSBAudioItem::AudioItemActionWait()
{
    if (!m_fileInfo.strName.empty())
    {
        //当其他item在播放时该item被关闭则属于被动关闭
        AudioItemActionStop(false);
    }

    SetPlayStatus(USB_PLAY_STATE_WAIT);
}

void CUSBAudioItem::AudioItemActionWaitToStop()
{
    SetPlayStatus(USB_PLAY_STATE_STOP);
}

void CUSBAudioItem::AudioItemActionComplete()
{
    UILogicDoAction(USB_CLICK_ACTION_COMPLETE);

    StopCountTime();

    if (USB_PLAY_STATE_PLAY == GetPlayStatus()
            || USB_PLAY_STATE_PAUSE == GetPlayStatus())
    {
        m_strHint.show();
        m_strHint.setText(LANG_TRANSLATE(TRID_USB_HINT_FINISHED));
        QTimer::singleShot(USB_AUDIO_ITEM_HINT_DISAPPEAR_TIME, this, SLOT(HideHintLabel()));
    }

    SetPlayStatus(USB_PLAY_STATE_STOP);

    m_fileInfo.Reset();

    emit StopPress(this);
    emit PlayStatusChange(this);
}

void CUSBAudioItem::AudioItemSelect()
{
    if (GetPlayStatus() == USB_PLAY_STATE_STOP
            || GetPlayStatus() == USB_PLAY_STATE_WAIT)
    {
        AudioItemActionPlay();
    }
}

void CUSBAudioItem::HideHintLabel()
{
    m_strHint.hide();
}

void CUSBAudioItem::OnProcessBarSwift(bool bIsLeftKeyPress, bool bPlay)
{
    if (m_nCurrentSeconds > m_nTotalSeconds || NULL == m_pProgressBar)
    {
        return;
    }

    //如果是左键被点说明是快速后退
    if (bIsLeftKeyPress)
    {
        //如果当前时间大于USB_SWIFT_SECONDS秒则可以减USB_SWIFT_SECONDS秒
        if (USB_SWIFT_SECONDS <= m_nCurrentSeconds)
        {
            m_nCurrentSeconds -= USB_SWIFT_SECONDS;
            m_pProgressBar->SetTimeValue(m_nTotalSeconds, m_nCurrentSeconds);
            m_strCurrentTime.SetTimeText(m_nCurrentSeconds);
        }
        //如果当前时间小于USB_SWIFT_SECONDS则置为零
        else
        {
            m_nCurrentSeconds = 0;
            m_pProgressBar->SetTimeValue(m_nTotalSeconds, m_nCurrentSeconds);
            m_strCurrentTime.SetTimeText(m_nCurrentSeconds);
        }
    }
    else
    {
        //如果当前时间与总时间的差大于USB_SWIFT_SECONDS秒则可以加USB_SWIFT_SECONDS秒
        if (m_nTotalSeconds > m_nCurrentSeconds + USB_SWIFT_SECONDS)
        {
            m_nCurrentSeconds += USB_SWIFT_SECONDS;
            m_pProgressBar->SetTimeValue(m_nTotalSeconds, m_nCurrentSeconds);
            m_strCurrentTime.SetTimeText(m_nCurrentSeconds);
        }
        //如果当前时间与总时间的差小于USB_SWIFT_SECONDS秒则置为总时长
        else
        {
            m_nCurrentSeconds = m_nTotalSeconds;
            m_pProgressBar->SetTimeValue(m_nTotalSeconds, m_nCurrentSeconds);
            m_strCurrentTime.SetTimeText(m_nCurrentSeconds);
        }
    }

    //正在加速快进退的过程中无需计时,待加速结束后计时并播放
    if (bPlay)
    {
        UILogicDoAction(USB_CLICK_ACTION_SEEK);
        if (modRecord_IsPlaying(RECORD_AUDIO))
        {
            SetCountTime();
        }
    }
    else
    {
        StopCountTime();
    }
}

bool CUSBAudioItem::ProcessKeyEvent(QObject * pObject, QEvent * pEvent)
{
    QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
    if (pKeyEvent->key() == PHONE_KEY_OK
            && pEvent->type() == QEvent::KeyPress
            && !m_fileInfo.strName.empty())
    {
        if (GetPlayStatus() == USB_PLAY_STATE_WAIT)
        {
            if (modRecord_IsPlayPause(RECORD_AUDIO))
            {
                UILogicDoAction(USB_CLICK_ACTION_RESUME);
                SetCountTime();
            }
            else if (modRecord_IsPlaying(RECORD_AUDIO))
            {
                UILogicDoAction(USB_CLICK_ACTION_PAUSE);
                StopCountTime();
            }
        }
        else if (GetPlayStatus() == USB_PLAY_STATE_PAUSE)
        {
            AudioItemActionResume();
        }
        else if (GetPlayStatus() == USB_PLAY_STATE_PLAY)
        {
            AudioItemActionPause();
        }

        return true;
    }

    if (!m_fileInfo.strName.empty()
            && NULL != m_pProgressBar
            && m_pProgressBar->ProcessKeyEvent(pObject, pEvent))
    {
        return true;
    }
    return false;
}

bool CUSBAudioItem::UILogicDoAction(int nAction)
{
    yl::string strFile = m_fileInfo.strPath + m_fileInfo.strName;
    SETTINGUI_INFO("CUSBAudioItem::UILogicDoAction nAction[%d] strFile[%s]", nAction, strFile.c_str());
    //如果nAction的范围在USB_PROCESSBAR_OFFSET到USB_PROCESSBAR_OFFSET + USB_PROCESSBAR_TOTAL则说明是进度条被拖动
    if (USB_PROCESSBAR_OFFSET <= nAction && nAction <= USB_PROCESSBAR_OFFSET + USB_PROCESSBAR_TOTAL)
    {
        int iCurrentTime = CalculateSeekTimePointByPercent(nAction - USB_PROCESSBAR_OFFSET);
        modRecord_PlaySeek(RECORD_AUDIO, strFile.c_str(), iCurrentTime);
    }
    else if (USB_CLICK_ACTION_PLAY == nAction)
    {
        LoadPlayData(m_fileInfo.lTime, 0);
        modRecord_PlayStart(RECORD_AUDIO, strFile.c_str());
    }
    else if (USB_CLICK_ACTION_RESUME == nAction)
    {
        modRecord_PlayResume(RECORD_AUDIO, strFile.c_str());
    }
    else if (USB_CLICK_ACTION_PAUSE == nAction)
    {
        modRecord_PlayPause(RECORD_AUDIO, strFile.c_str(), CalculatePausePointByCurrentTime());
    }
    else if (USB_CLICK_ACTION_COMPLETE == nAction
             || USB_CLICK_ACTION_STOP == nAction)
    {
        modRecord_PlayStop(RECORD_AUDIO, strFile.c_str());
    }
    else if (USB_CLICK_ACTION_SEEK == nAction)
    {
        modRecord_PlaySeek(RECORD_AUDIO, strFile.c_str(), CalculatePausePointByCurrentTime());
    }
    else
    {
        CUSBItemBase::UILogicDoAction(nAction);
    }

    return true;
}
