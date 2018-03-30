#include "usbprocessbar.h"
#include "usbuicommon.h"

#include "keymap.h"

CUSBProcessBar::CUSBProcessBar(QWidget * parent/* = 0*/)
    : CProcessBarUtility(parent)
    , CUSBTimeCounter()
    , m_bIsLeftKeyPress(true)
{
    InitData();
}

CUSBProcessBar::~CUSBProcessBar()
{

}

void CUSBProcessBar::InitData()
{
    m_LongPressTimer.setParent(this);
    connect(&m_LongPressTimer, SIGNAL(timeout()), this, SLOT(OnLongPressTimeOut()));
}

void CUSBProcessBar::SetTimeValue(long nTotalSeconds, long nCurrentSeconds)
{
    SetValue(float(nCurrentSeconds) / nTotalSeconds * USB_PROCESSBAR_TOTAL);
}

long CUSBProcessBar::GetCurrentSeconds(long nTotalSeconds)
{
    return float(GetValue()) / USB_PROCESSBAR_TOTAL * nTotalSeconds;
}

int CUSBProcessBar::GetEmitValue()
{
    //加USB_PROCESSBAR_OFFSET用来区分其他Action
    return GetValue() + USB_PROCESSBAR_OFFSET;
}

void CUSBProcessBar::SetLongPressTimerStart()
{
    StopLongPressTimer();
    m_LongPressTimer.start(USB_SWIFT_LONG_PRESS);
}

void CUSBProcessBar::StopLongPressTimer()
{
    if (m_LongPressTimer.isActive())
    {
        m_LongPressTimer.stop();
    }
}

void CUSBProcessBar::OnLongPressTimeOut()
{
    //当长按计时时间到，则停止计时器，开始快速进退
    StopLongPressTimer();
    m_LongPressTimer.start(USB_SWIFT_INTERVAL);
    emit ProcessBarSwift(m_bIsLeftKeyPress, false);
}

bool CUSBProcessBar::ProcessKeyEvent(QObject * pObject, QEvent * pEvent)
{
    // 判断事件类型
    switch (pEvent->type())
    {
    case  QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            //快进(退)
            case PHONE_KEY_LEFT:
            case PHONE_KEY_RIGHT:
                {
                    m_bIsLeftKeyPress = (pKeyEvent->key() == PHONE_KEY_LEFT);
                    SetLongPressTimerStart();
                    return true;
                }
                break;
            default:
                break;
            }
        }
        break;
    case  QEvent::KeyRelease:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_LEFT:
            case PHONE_KEY_RIGHT:
                {
                    StopLongPressTimer();
                    emit ProcessBarSwift(pKeyEvent->key() == PHONE_KEY_LEFT, true);
                    return true;
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
