#include "autovolumebar.h"
#include "baseui/t4xpicpath.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "uikernel/qwidgetutility.h"
#include "imagemanager/modimagemanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "mainwnd.h"

namespace
{
#define VOLUME_BAR_POS_ICON_LEFT 1
#define VOLUME_BAR_POS_ICON_TOP 0
#define VOLUME_BAR_POS_ICON_WIDTH 34
#define VOLUME_BAR_POS_ICON_HEIGHT 34

#define VOLUME_BAR_POS_PROGRESS_LEFT 43
#define VOLUME_BAR_POS_PROGRESS_RIGHT_MARGIN 1
}

LRESULT CAutoVolumeBar::OnMessage(msgObject & refMessage)
{
    switch (refMessage.message)
    {
    case VOICE_CHANGE_CHANNEL:
    {
        _MainWND.RefreshVolumeBarIconByChannelType(refMessage.wParam);
    }
    break;
    default:
        break;
    }
    return false;
}

CAutoVolumeBar::CAutoVolumeBar(QWidget * parent /* = 0 */)
    : CVolumeBar(parent)
    , m_pTimerAutoHide(NULL)
    , m_pLabVolumeBarIcon(NULL)
    , m_eVolumeType(VT_AUTO)
{
    InitWidget();
}

CAutoVolumeBar::~CAutoVolumeBar()
{

}

//初始化控件
void CAutoVolumeBar::InitWidget()
{
    setAlignment(Qt::AlignLeft);

    //设置强焦点
    this->setFocusPolicy(Qt::StrongFocus);

    //设置音量控制条控件位置
    this->SetVolumeBarMargin(VOLUME_BAR_POS_PROGRESS_LEFT, VOLUME_BAR_POS_PROGRESS_RIGHT_MARGIN);

    //设置音量调节的值有效范围
    this->SetRange(0, 10);

    //设置调节单位音量大小
    this->SetStep(1);

    //连接设置音量调节插槽函数
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));

    //连接鼠标的定时器
    connect(this, SIGNAL(mouseAction()), this, SLOT(OnMouseAction()));

    if (m_pTimerAutoHide == NULL)
    {
        // 初始化音量条的计时器
        m_pTimerAutoHide = new QTimer(this);
        if (NULL != m_pTimerAutoHide)
        {
            connect(m_pTimerAutoHide, SIGNAL(timeout()), this, SLOT(OnAutoHideTimeOut()));
        }
    }

    installEventFilter(this);
}

//设置音量类型
void CAutoVolumeBar::SetVolumeValue(VOLUME_TYPE eVolType/* = VT_AUTO */)
{
    m_eVolumeType = eVolType;
    SetValue(voice_GetVolume(m_eVolumeType));
}

//获取音量类型
VOLUME_TYPE CAutoVolumeBar::GetVolumeType()
{
    return m_eVolumeType;
}

QLabel * CAutoVolumeBar::GetVolumeBarIcon()
{
    if (m_pLabVolumeBarIcon == NULL)
    {
        m_pLabVolumeBarIcon = new QLabel(this);

        //设置位置
        if (m_pLabVolumeBarIcon != NULL)
        {
            m_pLabVolumeBarIcon->setGeometry(VOLUME_BAR_POS_ICON_LEFT, VOLUME_BAR_POS_ICON_TOP,
                                             VOLUME_BAR_POS_ICON_WIDTH, VOLUME_BAR_POS_ICON_HEIGHT);
            SetVolumeBarIcon(PIC_ICON_VOLUME_HANDFREE);
        }
    }
    return m_pLabVolumeBarIcon;
}

// 设置背景图片
void CAutoVolumeBar::SetVolumeBackGround(const yl::string & strPath)
{
    qWidgetUtility::SetBackGround(this, strPath.c_str());
}

//设置音量条可见性
void CAutoVolumeBar::SetVolumeBarVisible(bool bVisible)
{
    if (bVisible)
    {
        show();

        //repaint();

        //启动隐藏定时器
        StartAutoHideTimer();
        //发出显示信号
        emit visibleChanged(true);
    }
    else
    {
        // 停止计时器
        if (NULL != m_pTimerAutoHide && m_pTimerAutoHide->isActive())
        {
            m_pTimerAutoHide->stop();
        }

        this->hide();
        emit visibleChanged(false);
    }
}

void CAutoVolumeBar::SetVolumeBarIcon(const QString & strIcon)
{
    QLabel * pVolumeBarIcon = GetVolumeBarIcon();
    if (NULL != pVolumeBarIcon)
    {
        pVolumeBarIcon->setPixmap(THEME_GET_BMP(strIcon.toUtf8().data()));
        pVolumeBarIcon->update();
    }
}

void CAutoVolumeBar::SetVolumeBarIcon(VOLUME_TYPE eVolType/* = VT_AUTO */)
{
    if (eVolType == VT_AUTO)
    {
        SetVolumeBarIconByChannelType();
    }
    else
    {
        switch (eVolType)
        {
        case VT_TALK_HANDFREE:
        case VT_RING:
        //      case VT_COLORRINGBACK:  //VP才区分彩铃与通话音量,其他机型不区分,by kjf
        case VT_TONE_HANDFREE:
        {
            //免提通话音量handfree,铃声音量,彩铃音量
            SetVolumeBarIcon(PIC_ICON_VOLUME_HANDFREE);
        }
        break;
        case VT_TALK_HANDSET:
        case VT_TONE_HANDSET:
        {
            // 手柄通话音量handset
            SetVolumeBarIcon(PIC_ICON_VOLUME_HANDSET);
        }
        break;
        case VT_TALK_HEADSET:
        case VT_TONE_HEADSET:
        {
            // 耳麦通话音量headset
            SetVolumeBarIcon(PIC_ICON_VOLUME_HEADSET);
        }
        break;
        default:
        {
            //未知
            SetVolumeBarIcon(PIC_ICON_VOLUME_HANDFREE);
        }
        break;
        }
    }
}

void CAutoVolumeBar::SetVolumeBarIconByChannelType(CHANNEL_MODE eChannelType /*= CHANNEL_AUTO*/)
{
    if (eChannelType == CHANNEL_AUTO)
    {
        eChannelType = voice_GetCurrentChannel();
    }

    switch (eChannelType)
    {
    case CHANNEL_HANDFREE:
    case CHANNEL_GROUP:
        //免提
        SetVolumeBarIcon(PIC_ICON_VOLUME_HANDFREE);
        break;
    case CHANNEL_HANDSET:
        //手柄
        SetVolumeBarIcon(PIC_ICON_VOLUME_HANDSET);
        break;
    case CHANNEL_HEADSET:
    case CHANNEL_2HEADSET:
        //耳麦
        SetVolumeBarIcon(PIC_ICON_VOLUME_HEADSET);
        break;
    case CHANNEL_RING:
    {
        if (voice_IsRingHeadset())
        {
            // 耳麦响铃
            SetVolumeBarIcon(PIC_ICON_VOLUME_HEADSET);
        }
        else
        {
            // 免提响铃
            SetVolumeBarIcon(PIC_ICON_VOLUME_HANDFREE);
        }
    }
    break;
    default:
        MAINWND_INFO("Unknown icon for channel[%d]", eChannelType);
        break;
    }
}

//音量调节插槽函数
void CAutoVolumeBar::OnValueChanged(int nValue)
{
    // 把音量值保存到文件当中
    voice_SetVolume(nValue, m_eVolumeType, true);
}

void CAutoVolumeBar::StartAutoHideTimer()
{
    // 数据的有效性判断
    if (m_pTimerAutoHide != NULL)
    {
        // 启动计时器
        m_pTimerAutoHide->start(VOLUMEBAR_AUTO_HIDE_TIME);
    }
}

void CAutoVolumeBar::OnAutoHideTimeOut()
{
    SetVolumeBarVisible(false);
}

void CAutoVolumeBar::OnMouseAction()
{
    //有鼠标操作时，重新启动计时器
    StartAutoHideTimer();
}

// 处理案件事件
bool CAutoVolumeBar::ProcessKeyEvent(int nKey, bool bPress)
{
    if (!isVisible())
    {
        //不可见时直接返回，由其他进行处理
        return false;
    }

    if (bPress)
    {
        switch (nKey)
        {
        case PHONE_KEY_VOLUME_DECREASE:
        case PHONE_KEY_VOLUME_INCREASE:
        {
            //音量控制
            (nKey == PHONE_KEY_VOLUME_DECREASE) ? this->MinusValue() : this->AddValue();

            // 当有左右按键的时候,重置隐藏音量条的计时器
            StartAutoHideTimer();
            return true;
        }
        break;
        default:
        {
            // 当音量条存在的时候,按其他按键需要关闭音量条
            SetVolumeBarVisible(false);
        }
        break;
        }
    }

    return false;
}

//事件过滤器
bool CAutoVolumeBar::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::Show:
    {
        etl_RegisterMsgHandle(VOICE_CHANGE_CHANNEL, VOICE_CHANGE_CHANNEL, CAutoVolumeBar::OnMessage);
    }
    break;
    case QEvent::Hide:
    {
        etl_UnregisterMsgHandle(VOICE_CHANGE_CHANNEL, VOICE_CHANGE_CHANNEL, CAutoVolumeBar::OnMessage);
    }
    break;
    case QEvent::KeyPress:
    {
        if (!isVisible())
        {
            //不可见时直接返回，由其他进行处理
            return false;
        }

        QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
        ProcessKeyEvent(pKeyEvent->key(), true);
    }
    break;
    case QEvent::MouseButtonPress:
    {
        if (!isVisible())
        {
            //不可见时直接返回，由其他进行处理
            return false;
        }

        //若音量条显示且点击事件非产生在音量条上，则隐藏音量条
        if (!isChildOf(pObject, this))
        {
            //非点击在自身子控件，则隐藏显示
            SetVolumeBarVisible(false);
        }
    }
    break;
    default:
        break;
    }

    return false;
}
