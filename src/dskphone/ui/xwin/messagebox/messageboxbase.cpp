#include "messageboxbase.h"
#include "keymap.h"
#include "messageboxmanager.h"
#include "uimanager/moduimanager.h"
#include "xwindow/xSoftkeyBar.h"
#include "xwindow/xWindowManager.h"


const char * MSGBOX_ID_TEXT_TITLE = "txtTitle";
const char * MSGBOX_ID_TEXT_MSG = "txtMsg";

CMessageBoxBase::CMessageBoxBase()
    : m_nType(MESSAGEBOX_Error)
    , m_eResult(MessageBox_ERROR)
    , m_iID(-1)
    , m_ExtraData(NULL)
    , m_iTimeToDie(0)
    , m_bRestartDieTimeByKey(false)
    , m_bRemovedByManager(false)
    , m_bRemoveByKey(true)
    , m_bIsExistOnly(false)
    , m_bIsForbitAutoReturnToIdle(false)
    , m_bShowAlways(false)
    , m_pTitle(NULL)
    , m_pMessage(NULL)
{

}

CMessageBoxBase::~CMessageBoxBase()
{
    if (!IsRemovedByManager())
    {
        _MessageBoxManager.RemoveMessageBox(this, MessageBox_Destory, false);
    }

    if (m_timer.IsTimerRuning())
    {
        m_timer.KillTimer();
    }
}

bool CMessageBoxBase::CreateWindowObject(bool bAboveSoftkey/* = false*/)
{
    if (NULL == m_pHostStation)
    {
        return false;
    }

    m_pWindow = m_pHostStation->createWindow(this, TML_USER +
                (bAboveSoftkey
                 ? MESSAGEBOX_WINDOW_LAYOUT_LEVEL_ABOVE_SOFTKEY
                 : MESSAGEBOX_WINDOW_LAYOUT_LEVEL_UNDER_SOFTKEY));

    return true;
}

void CMessageBoxBase::DeleteThisBox()
{
    g_WorkStation.close_helper(this, true);
}

void CMessageBoxBase::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pTitle = static_cast<xTextView *>(getViewById(MSGBOX_ID_TEXT_TITLE));
    m_pMessage = static_cast<xTextView *>(getViewById(MSGBOX_ID_TEXT_MSG));
}

bool CMessageBoxBase::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    //特需弹出框只允许自动关闭（重启或恢复出厂状态）
    if (!m_bRemoveByKey)
    {
        return true;
    }

    //有键盘事件时重置计时器
    if (m_bRestartDieTimeByKey)
    {
        RestartExitTime();
    }

    // 不响应抬起事件
    if (!bPress)
    {
        return false;
    }

    bool bResult = false;
    switch (nKeyCode)
    {
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
                bResult = !(PHONE_KEY_HANDFREE == nKeyCode
                            || PHONE_KEY_HANDSET == nKeyCode
                            || PHONE_KEY_HANDSET_ON_HOOK == nKeyCode
                            || PHONE_KEY_HANDSET_OFF_HOOK == nKeyCode
                            || PHONE_KEY_HEADSET_CTRL == nKeyCode
                            || PHONE_KEY_VOLUME_DECREASE == nKeyCode
                            || PHONE_KEY_VOLUME_INCREASE == nKeyCode);
            }
        }
        break;
    }

    return bResult;
}

void CMessageBoxBase::SetExitTime(int iTimeToExit)
{
    m_iTimeToDie = iTimeToExit;

    if (m_timer.IsTimerRuning())
    {
        m_timer.KillTimer();
    }

    RestartExitTime();
}

void CMessageBoxBase::RestartExitTime()
{
    if (m_iTimeToDie <= 0)
    {
        return;
    }

    m_timer.SingleShot(m_iTimeToDie, TIMER_METHOD(this, CMessageBoxBase::OnExitTimeOut));
}

void CMessageBoxBase::SetRestartDieTimeByKey(bool bEnable)
{
    m_bRestartDieTimeByKey = bEnable;
}

void CMessageBoxBase::SetRemoveByKey(bool bEnable)
{
    m_bRemoveByKey = bEnable;
}

void CMessageBoxBase::OnExitTimeOut(xThreadTimer* timer, LPVOID pExtraData)
{
    ExitMessageBox(MessageBox_TimeOut);
}

void CMessageBoxBase::ExitMessageBox(MESSAGEBOX_RESULT eResult)
{
    _MessageBoxManager.RemoveMessageBox(this, eResult);
}

bool CMessageBoxBase::GetTitleExist()
{
    return (NULL != m_pTitle);
}

void CMessageBoxBase::SetTitle(const yl::string & strTitle)
{
    if (NULL == m_pTitle)
    {
        return;
    }

    m_pTitle->SetText(strTitle);
}

void CMessageBoxBase::SetMessage(const yl::string & strMessage)
{
    if (NULL == m_pMessage)
    {
        return;
    }

    m_pMessage->SetText(strMessage);
}


void CMessageBoxBase::SetSoftkey(const VecMsgBoxSoftkey & vecSoftkey)
{
    clearViewSoftKey(this);

    for (int i = 0; i < vecSoftkey.size(); ++i)
    {
        const MsgBoxSoftkey & stSoftkey = vecSoftkey[i];

        if (MessageBox_ERROR != stSoftkey.m_result)
        {
            setupViewSoftKey(this, i, stSoftkey.m_title, stSoftkey.m_result,
                             SLOT_METHOD(this, CMessageBoxBase::slotOnSoftkeyPress),
                             SLOT_METHOD(this, CMessageBoxBase::slotOnSoftKeyLongPress));
        }
    }
}

void CMessageBoxBase::slotOnSoftkeyPress(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    OnSoftkeyAction(wParam, false);
}

void CMessageBoxBase::slotOnSoftKeyLongPress(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    OnSoftkeyAction(wParam, true);
}

bool CMessageBoxBase::OnSoftkeyAction(int nType, bool bLongPress)
{
    if (!bLongPress && MessageBox_ERROR != nType)
    {
        ExitMessageBox((MESSAGEBOX_RESULT)nType);
    }

    return true;
}
