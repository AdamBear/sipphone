#include "messageboxbase.h"
#include <ETLLib/ETLLib.hpp>
#include "baseui/maskframe.h"
#include "uicommon.h"
#include "keymap.h"
#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"
#include <QPalette>
#include "uimanager/moduimanager.h"
#include "uimanager/uimanager_common.h"
#include "messageboxmanager.h"

CMessageBoxBase::CMessageBoxBase(CMaskFrame * pMaskFrame)
    : QDialog(NULL, Qt::FramelessWindowHint)
    , m_pBtnOk(NULL)
    , m_pBtnCancel(NULL)
    , m_pMaskFrame(pMaskFrame)
    , m_pFilter(NULL)
    , m_bAllowOkButton(false)
    , m_BtnSet(this)
    , m_eResult(MessageBox_ERROR)
    , m_eType(MESSAGEBOX_NOTE)
    , m_iID(-1)
{
    setObjectName("CMessageBoxBase");

    if (NULL != m_pMaskFrame)
    {
        m_pMaskFrame->setObjectName("MsgMaskFrame");
        setParent(pMaskFrame);
//      connect(m_pMaskFrame, SIGNAL(click()), this, SLOT(OnCancelPressed()));

        m_pMaskFrame->SetMaskRect(QRect());
    }

    m_eTimeoutResult = MessageBox_Cancel;

    m_timer.setParent(this);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnExitTimeOut()));

    setAttribute(Qt::WA_DeleteOnClose);
    this->setFocusPolicy(Qt::NoFocus);      //设置无焦点

    m_BtnSet.SetMaxBtnCount(4);
    m_BtnSet.SetBackGround(PIC_BTN_MSG);
    m_BtnSet.hide();
}

CMessageBoxBase::~CMessageBoxBase()
{
    if (NULL != m_pMaskFrame)
    {
        if (parent() == m_pMaskFrame)
        {
            setParent(NULL);
        }
    }
}

void CMessageBoxBase::SetMaskRect(const QRect & rcMask)
{
    if (NULL != m_pMaskFrame)
    {
        m_pMaskFrame->SetMaskRect(rcMask);
    }
}

void CMessageBoxBase::SetExitTime(int iTimeToExit)
{
    // 若时间小于0则默认其不会自动关闭
    if (m_timer.isActive())
    {
        m_timer.stop();
    }

    if (iTimeToExit > 0)
    {
        m_timer.start(iTimeToExit);
    }
}

void CMessageBoxBase::OnExitTimeOut()
{
    ExitMessageBox(m_eTimeoutResult);
}

void CMessageBoxBase::SetEventFilter(IMsgBoxEventFilter * pFilter)
{
    m_pFilter = pFilter;
}

IMsgBoxEventFilter * CMessageBoxBase::GetEventFilter() const
{
    return m_pFilter;
}

bool CMessageBoxBase::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (NULL != m_pFilter)
    {
        bool bFilter = false;

        if (m_pFilter->ProcessKeyEvent(nKeyCode, bPress, bFilter))
        {
            return bFilter;
        }
    }

    // 不响应抬起事件
    if (!bPress)
    {
        return false;
    }

    bool bResult = false;
    switch (nKeyCode)
    {
    case PHONE_KEY_OK:
        {
            if (m_bAllowOkButton)
            {
                ExitMessageBox(MessageBox_OK);
            }
            bResult = true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            ExitMessageBox(MessageBox_Cancel);
            bResult = true;
        }
        break;
    default:
        {

            if (UIManager_IsInIdle())// idle界面允许所有按键通过
            {
                bResult = false;
            }
            else
            {
                // 其他界面只允许特殊按键通过
                bResult = !(nKeyCode == PHONE_KEY_HANDFREE
                            || nKeyCode == PHONE_KEY_HANDSET
                            || nKeyCode == PHONE_KEY_HANDSET_ON_HOOK
                            || nKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
                            || nKeyCode == PHONE_KEY_EHS_OPEN
                            || nKeyCode == PHONE_KEY_EHS_CLOSE
                            || nKeyCode == PHONE_KEY_HEADSET_CTRL
                            || nKeyCode == PHONE_KEY_VOLUME_DECREASE
                            || nKeyCode == PHONE_KEY_VOLUME_INCREASE
                            || nKeyCode == PHONE_KEY_FN1
                            || nKeyCode == PHONE_KEY_FN2
                            || nKeyCode == PHONE_KEY_FN3
                            || nKeyCode == PHONE_KEY_FN4);
            }
        }
        break;
    }

    return bResult;
}

bool CMessageBoxBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    int nType = pEvent->type();

    if (QEvent::MouseButtonPress != nType && QEvent::MouseButtonRelease != nType)
    {
        return false;
    }

    //http://10.2.1.199/Bug.php?BugID=92438
    if (!IfClickOutExit())
    {
        return false;
    }
    else
    {
        if (NULL != pObject)
        {
            QMouseEvent * pMouseEvent = (QMouseEvent *)pEvent;
            QPoint ptMouse = pMouseEvent->globalPos();
            //      QRect rcPopupBox = this->geometry();

            QPoint MyMsgBoxPoint = mapToGlobal(QPoint(0, 0));
            QRect rcPopupBox = QRect(MyMsgBoxPoint.x(), MyMsgBoxPoint.y(), width(), height());

            if (!rcPopupBox.contains(ptMouse))
            {
                if (QEvent::MouseButtonPress == nType)
                {
                    ExitMessageBox(MessageBox_Cancel);
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }

    return false;
}

void CMessageBoxBase::ExitMessageBox(MESSAGEBOX_RESULT eResult)
{
    _MessageBoxManager.RemoveMessageBox(this, eResult);
}

void CMessageBoxBase::OnCancelPressed()
{
    ExitMessageBox(MessageBox_Cancel);
}

void CMessageBoxBase::OnOkPressed()
{
    ExitMessageBox(MessageBox_OK);
}

void CMessageBoxBase::OnButtonPressed(const QString & title)
{
    ItMsgBoxSoftkey it = m_vecSoftkey.begin();
    for (; it != m_vecSoftkey.end(); ++it)
    {
        if (title == it->m_title)
        {
            ExitMessageBox(it->m_result);
        }
    }
}
