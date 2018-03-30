#include "autovolumebar.h"

#include "keymap.h"
#include "xwindow/xWindowManager.h"
#include "voice/include/modvoice.h"
#include "moduimanager.h"
#include "baseui/include/t2xpicpath.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace
{
// 音量的最小值
#define AUTO_VOLUME_BAR_MIN_RANGE 0
// 音量的最大值
#define AUTO_VOLUME_BAR_MAX_RANGE 15

// 音量的显示时间
#define AUTO_VOLUME_BAR_HIDE_TIME 3000

// 音量的显示位置
#define VOLUME_BLOCK_X 15
#define VOLUME_BLOCK_Y 5
#define VOLUME_BLOCK_WIDTH 3
#define VOLUME_BLOCK_HEIGHT 8
#define VOLUME_BLOCK_INTERVAL 3

#define VOLUME_BLOCK_COLOR xColor(0, 0, 0)

const char * AUTO_VOLUMEBAR_CHANNEL_ICON = "channelicon";
const char * AUTO_VOLUMEBAR_PROGRESS = "progressbar";

const char * AUTO_VOLUMEBAR_BLOCK_LEFT = "android:volumebarblockleft";
const char * AUTO_VOLUMEBAR_BLOCK_TOP = "android:volumebarblocktop";

const char * AUTO_VOLUMEBAR_BLOCK_WIDTH = "android:volumebarblockwidth";
const char * AUTO_VOLUMEBAR_BLOCK_HEIGHT = "android:volumebarblockheight";
const char * AUTO_VOLUMEBAR_BLOCK_INTERVAL = "android:volumebarblockinterval";

}

#if IS_COLOR
LRESULT CAutoVolumeBar::OnVolumeBarMessage(msgObject & refMessage)
{
    switch (refMessage.message)
    {
    case VOICE_CHANGE_CHANNEL:
    {
        CAutoVolumeBar * pVolumebar = _UIManager.GetVolumeBar();
        if (NULL == pVolumebar)
        {
            return FALSE;
        }

        pVolumebar->SetVolumeBarIconByChannelType((CHANNEL_MODE)refMessage.wParam);
    }
    break;
    default:
        break;
    }
    return FALSE;
}
#endif

CAutoVolumeBar::CAutoVolumeBar()
    : m_nVolumeType(VT_AUTO)
    , m_nMinVolume(AUTO_VOLUME_BAR_MIN_RANGE)
    , m_nMaxVolume(AUTO_VOLUME_BAR_MAX_RANGE)
    , m_nVolume(0)
    , m_bAutoHide(true) // 默认自动隐藏
    , m_pChannelIcon(NULL)
    , m_pProgressBar(NULL)
    , m_nVolumeBlockLeft(VOLUME_BLOCK_X)
    , m_nVolumeBlockTop(VOLUME_BLOCK_Y)
    , m_nVolumeBlockWidth(VOLUME_BLOCK_WIDTH)
    , m_nVolumeBlockHeight(VOLUME_BLOCK_HEIGHT)
    , m_nVolumeBlockInterval(VOLUME_BLOCK_INTERVAL)
{
    InitWidget();
    SLOT_CONNECT(this, signalAppear, this, CAutoVolumeBar::slotOnViewAppear);
}

CAutoVolumeBar::~CAutoVolumeBar()
{
    m_timerHideVolumeBar.KillTimer();
    SLOT_DISCONNECT(this, signalAppear, this, CAutoVolumeBar::slotOnViewAppear);
}

void CAutoVolumeBar::loadChildrenElements(xml_node & node)
{
    xLinearLayout::loadChildrenElements(node);

    m_pChannelIcon = static_cast<xImageView *>(getViewById(AUTO_VOLUMEBAR_CHANNEL_ICON));
    m_pProgressBar = static_cast<xSliderProgressBar *>(getViewById(AUTO_VOLUMEBAR_PROGRESS));
}

void CAutoVolumeBar::loadAttributes(xml_node& node)
{
    xLinearLayout::loadAttributes(node);

    PopupAndroidInt(node, AUTO_VOLUMEBAR_BLOCK_LEFT, m_nVolumeBlockLeft);
    PopupAndroidInt(node, AUTO_VOLUMEBAR_BLOCK_TOP, m_nVolumeBlockTop);

    PopupAndroidInt(node, AUTO_VOLUMEBAR_BLOCK_WIDTH, m_nVolumeBlockWidth);
    PopupAndroidInt(node, AUTO_VOLUMEBAR_BLOCK_HEIGHT, m_nVolumeBlockHeight);
    PopupAndroidInt(node, AUTO_VOLUMEBAR_BLOCK_INTERVAL, m_nVolumeBlockInterval);
}

void CAutoVolumeBar::ConnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == pHandler)
    {
        return;
    }

    signalVisibleChange.connect(pHandler, pMethod);
}

void CAutoVolumeBar::DisconnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == pHandler)
    {
        return;
    }

    signalVisibleChange.disconnect(pHandler, pMethod);
}

void CAutoVolumeBar::ConnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == pHandler)
    {
        return;
    }

    signalVolumeChange.connect(pHandler, pMethod);
}

void CAutoVolumeBar::DisconnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == pHandler)
    {
        return;
    }

    signalVolumeChange.disconnect(pHandler, pMethod);
}

void CAutoVolumeBar::InitWidget()
{
    loadContent("volumebar.xml");

    //音量条总是在弹框的上层
    m_pWindow = m_pHostStation->createWindow(this, TML_USER + 1);

    SetVolumeType(VT_AUTO);

    SetVolumeData();
}

void CAutoVolumeBar::SetHideVolumeBarTimer()
{
    if (m_timerHideVolumeBar.IsTimerRuning())
    {
        m_timerHideVolumeBar.KillTimer();
    }

    m_timerHideVolumeBar.SingleShot(AUTO_VOLUME_BAR_HIDE_TIME, TIMER_METHOD(this,
                                    CAutoVolumeBar::OnHideVolumeBarTimer));
}

void CAutoVolumeBar::OnHideVolumeBarTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    m_timerHideVolumeBar.KillTimer();

    SetVolumeBarVisible(false);
}

int CAutoVolumeBar::GetVolumeType()
{
    return m_nVolumeType;
}

void CAutoVolumeBar::SetVolumeType(int nVolType)
{
    m_nVolumeType = nVolType;

#if IS_COLOR
    SetVolumeBarIcon((VOLUME_TYPE)m_nVolumeType);
#endif
}

void CAutoVolumeBar::SetVolumeData()
{
    //获取当前音量
    m_nVolume = voice_GetVolume((VOLUME_TYPE)m_nVolumeType);

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->SetPosition(m_nVolume);
    }

    update();
}

void CAutoVolumeBar::SetVolumeBarVisible(bool bVisible)
{
    CBaseDialog * pDialog = UIManager_GetTopWindow();
    yl::string strBG;
    int nVolumeType = VT_AUTO;
    if (bVisible && NULL != pDialog
            && pDialog->IsShowVolumeBar(nVolumeType, m_nMinVolume, m_nMaxVolume, strBG))
    {
        adjustVolumeRange();

        SetVolumeType(nVolumeType);
        SetVolumeData();

#if IS_COLOR
        etl_RegisterMsgHandle(VOICE_CHANGE_CHANNEL, VOICE_CHANGE_CHANNEL, CAutoVolumeBar::OnVolumeBarMessage);
#endif

        show();
    }
    else
    {
#if IS_COLOR
        etl_UnregisterMsgHandle(VOICE_CHANGE_CHANNEL, VOICE_CHANGE_CHANNEL, CAutoVolumeBar::OnVolumeBarMessage);
#endif

        hide();
    }
}

void CAutoVolumeBar::slotKeyEvent(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                  LPVOID pData)
{
    if (1 != lParam)
    {
        return ;
    }

    int nKeyCode = wParam;

    if (KEY_REPEAT_MASK == (nKeyCode & KEY_REPEAT_MASK))
    {
        nKeyCode = GET_KEY_VALUE(nKeyCode);

        ProcessKeyEvent(nKeyCode, true);
    }
}

bool CAutoVolumeBar::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    CBaseDialog * pDialog = UIManager_GetTopWindow();
    yl::string strBG;

    int nVolumeType = VT_AUTO;

    if (NULL != pDialog && !pDialog->IsShowVolumeBar(nVolumeType, m_nMinVolume, m_nMaxVolume, strBG))
    {
        return false;
    }

    adjustVolumeRange();

    SetVolumeType(nVolumeType);

    if (PHONE_KEY_VOLUME_DECREASE != nKeyCode && PHONE_KEY_VOLUME_INCREASE != nKeyCode)
    {
        if (checkVisible())
        {
            // 当音量条存在的时候,按其他按键需要关闭音量条
            SetVolumeBarVisible(false);
        }

        return false;
    }

    if (!voice_IsVolumeCanChange())
    {
        return false;
    }

    //音量条已经显示的情况
    if (checkVisible())
    {
        //音量控制
        ChangeVolume(PHONE_KEY_VOLUME_INCREASE == nKeyCode);

        if (m_bAutoHide)
        {
            // 当有左右按键的时候,重置隐藏音量条的计时器
            SetHideVolumeBarTimer();
        }
    }
    else
    {
        // 音量条还未显示的情况下，需要显示出音量条
        SetVolumeBarVisible(true);
    }

    //总是返回false，让其他模块也能处理到
    return false;
}

bool CAutoVolumeBar::onPaintContent(xPainter & p)
{
    if (NULL == m_pProgressBar)
    {
        chRect rcBlock(m_nVolumeBlockLeft, m_nVolumeBlockTop, m_nVolumeBlockLeft + m_nVolumeBlockWidth,
                       m_nVolumeBlockTop + m_nVolumeBlockHeight);

        for (int i = 0; i < AUTO_VOLUME_BAR_MAX_RANGE; ++i)
        {
            if (i < m_nVolume)
            {
                p.fillRect(rcBlock, VOLUME_BLOCK_COLOR);
            }
            else
            {
                p.drawRect(rcBlock, VOLUME_BLOCK_COLOR);
            }

            rcBlock.OffsetRect(m_nVolumeBlockWidth + m_nVolumeBlockInterval, 0);
        }

        return true;
    }
    else
    {
        return true;
    }
}

void CAutoVolumeBar::ChangeVolume(bool bAdd)
{
    int nCurrentVolume = m_nVolume;

    if (bAdd && nCurrentVolume < m_nMaxVolume)
    {
        ++nCurrentVolume;

        UpdateVolume(nCurrentVolume);
    }
    else if (!bAdd && nCurrentVolume > m_nMinVolume)
    {
        --nCurrentVolume;

        UpdateVolume(nCurrentVolume);
    }
}

void CAutoVolumeBar::UpdateVolume(int nVolume)
{
    if (nVolume < m_nMinVolume || nVolume > m_nMaxVolume)
    {
        return;
    }

    m_nVolume = nVolume;

    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->SetPosition(m_nVolume);
    }

    // 把音量值保存到文件当中
    voice_SetVolume(nVolume, (VOLUME_TYPE)m_nVolumeType, true);

    signalVolumeChange.emitSignal((WPARAM)nVolume, (LPARAM)m_nVolumeType, 0, 0);

    update();
}

void CAutoVolumeBar::slotOnViewAppear(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                      int nDataBytes, LPVOID pData)
{
    if (1 == wParam)
    {
        //发出显示信号
        signalVisibleChange.emitSignal((WPARAM)true, 0, 0, 0);

        SLOT_CONNECT(&ownerStation(), signalKeyEvent, this, CAutoVolumeBar::slotKeyEvent);

        if (m_bAutoHide)
        {
            //启动隐藏定时器
            SetHideVolumeBarTimer();
        }
    }
    else
    {
        signalVisibleChange.emitSignal((WPARAM)false, 0, 0, 0);

        SLOT_DISCONNECT(&ownerStation(), signalKeyEvent, this, CAutoVolumeBar::slotKeyEvent);

        m_timerHideVolumeBar.KillTimer();
    }
}

#if IS_COLOR
void CAutoVolumeBar::SetVolumeBarIconByChannelType(CHANNEL_MODE eChannelType /*= CHANNEL_AUTO*/)
{
    if (CHANNEL_AUTO == eChannelType)
    {
        //获取通道类型
        eChannelType = voice_GetCurrentChannel();
    }

    switch (eChannelType)
    {
    case CHANNEL_HANDFREE:
    case CHANNEL_GROUP:
        //免提
        SetVolumeBarIcon(PIC_CHANNEL_ICON_HANDFREE);
        break;
    case CHANNEL_HANDSET:
        //手柄
        SetVolumeBarIcon(PIC_CHANNEL_ICON_HANDSET);
        break;
    case CHANNEL_HEADSET:
    case CHANNEL_2HEADSET:
        //耳麦
        SetVolumeBarIcon(PIC_CHANNEL_ICON_HEADSET_BIG);
        break;
    case CHANNEL_RING:
    {
        if (voice_IsRingHeadset())
        {
            // 耳麦响铃
            SetVolumeBarIcon(PIC_CHANNEL_ICON_HEADSET_BIG);
        }
        else
        {
            // 免提响铃
            SetVolumeBarIcon(PIC_CHANNEL_ICON_HANDFREE);
        }
    }
    break;
    default:
        MAINWND_INFO("Unknown icon for channel[%d]", eChannelType);
        break;
    }
}

void CAutoVolumeBar::SetVolumeBarIcon(VOLUME_TYPE eVolType /*= VT_AUTO*/)
{
    if (eVolType == VT_AUTO)
    {
        SetVolumeBarIconByChannelType(CHANNEL_AUTO);
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
            SetVolumeBarIcon(PIC_CHANNEL_ICON_HANDFREE);
        }
        break;
        case VT_TALK_HANDSET:
        case VT_TONE_HANDSET:

        {
            // 手柄通话音量handset
            SetVolumeBarIcon(PIC_CHANNEL_ICON_HANDSET);
        }
        break;
        case VT_TALK_HEADSET:
        case VT_TONE_HEADSET:
        {
            // 耳麦通话音量headset
            SetVolumeBarIcon(PIC_CHANNEL_ICON_HEADSET_BIG);
        }
        break;
        default:
        {
            //未知
            SetVolumeBarIcon(PIC_CHANNEL_ICON_HANDFREE);
        }
        break;
        }
    }
}

void CAutoVolumeBar::SetVolumeBarIcon(const yl::string & strIcon)
{
    if (NULL != m_pChannelIcon)
    {
        m_pChannelIcon->setPixmap(strIcon);
    }
}
#endif

void CAutoVolumeBar::adjustVolumeRange()
{
    if (voice_GetCurrentChannel() == CHANNEL_RING)
    {
        // 铃声音量可以调节为0
        m_nMinVolume = (m_nMinVolume > 0 ? m_nMinVolume : 0);
    }
    else
    {
        // http://192.168.1.99/Bug.php?BugID=19177
        int nSystemDefaultMin = ((configParser_GetConfigInt(kszEnableSilenceMode) != 0) ? 0 : 1);
        m_nMinVolume = (m_nMinVolume > nSystemDefaultMin ? m_nMinVolume : nSystemDefaultMin);
    }
}

