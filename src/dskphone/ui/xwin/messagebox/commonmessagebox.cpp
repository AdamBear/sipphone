#include "commonmessagebox.h"
#include "keymap.h"
#include "xwindow/xWindowManager.h"
#include "xwindow/xSoftkeyBar.h"
#include "uimanager/moduimanager.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"


namespace
{
#define MSG_BOX_LAYOUT_FILE_COMMON "messagebox/commonmsgbox.xml"
}

CCommonMessageBox::CCommonMessageBox()
    : m_strMessage("")
    , m_nCountTime(0)
    , m_bCountTime(false)
{
}

CCommonMessageBox::~CCommonMessageBox()
{
    if (m_bCountTime)
    {
        UIManager_LockReturnToIdle(false);
    }
}

CMessageBoxBase * CCommonMessageBox::CreateCommonMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    CCommonMessageBox * pCommonMsgBox = new CCommonMessageBox;

    if (NULL != pCommonMsgBox)
    {
        pCommonMsgBox->loadContent(MSG_BOX_LAYOUT_FILE_COMMON);

        pCommonMsgBox->CreateWindowObject();

        pCommonMsgBox->SetID(stMsgInfo.iID);
        pCommonMsgBox->SetType(stMsgInfo.eType);
        pCommonMsgBox->SetExtraData(stMsgInfo.extraData);
        pCommonMsgBox->SetTitle(stMsgInfo.strTitle);
        pCommonMsgBox->SetCountTime(stMsgInfo.bCountDown);
        pCommonMsgBox->SetMessage(stMsgInfo.strNote);
        pCommonMsgBox->SetCountMessage(stMsgInfo.strNote);
        pCommonMsgBox->SetExitTime(stMsgInfo.nMsTime);

        if (stMsgInfo.vecSoftkey.size() > 0)
        {
            pCommonMsgBox->SetSoftkey(stMsgInfo.vecSoftkey);
        }
        else
        {
            pCommonMsgBox->SetDefaultSoftKey(stMsgInfo.eType);
        }
    }

    return pCommonMsgBox;
}

void CCommonMessageBox::SetDefaultSoftKey(int nType)
{
    VecMsgBoxSoftkey vecSoftKey;
    MsgBoxSoftkey stSoftKey;

    stSoftKey.m_title = _LANG(TRID_CANCEL);
    stSoftKey.m_result = MessageBox_Cancel;
    vecSoftKey.push_back(stSoftKey);

    stSoftKey.m_title = "";
    stSoftKey.m_result = MessageBox_ERROR;
    vecSoftKey.push_back(stSoftKey);
    vecSoftKey.push_back(stSoftKey);

    if (MESSAGEBOX_OkCancel == nType)
    {
        stSoftKey.m_title = _LANG(TRID_OK);
        stSoftKey.m_result = MessageBox_OK;
        vecSoftKey.push_back(stSoftKey);
    }
    else
    {
        vecSoftKey.push_back(stSoftKey);
    }

    SetSoftkey(vecSoftKey);
}

bool CCommonMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (MESSAGEBOX_OkCancel == GetType() && PHONE_KEY_OK == nKeyCode && bPress)
    {
        ExitMessageBox(MessageBox_OK);

        return true;
    }

    if (PHONE_KEY_FN1 == nKeyCode || PHONE_KEY_FN2 == nKeyCode
            || PHONE_KEY_FN3 == nKeyCode || PHONE_KEY_FN4 == nKeyCode)
    {
        return false;
    }

    return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
}

void CCommonMessageBox::SetCountMessage(const yl::string & strMessage)
{
    m_strMessage = strMessage;
}

void CCommonMessageBox::RestartExitTime()
{
    int nExitTime = GetExitTime();
    if (!m_bCountTime || nExitTime <= 0)
    {
        return CMessageBoxBase::RestartExitTime();
    }

    //倒计时过程中不能返回idle
    UIManager_LockReturnToIdle(true);

    m_nCountTime = nExitTime / 1000;

    SetShowMessage(m_nCountTime);

    if (m_timer.IsTimerRuning())
    {
        m_timer.KillTimer();
    }

    m_timer.SingleShot(1000, TIMER_METHOD(this, CCommonMessageBox::OnCountTime));
}

void CCommonMessageBox::OnCountTime(xThreadTimer* timer, LPVOID pExtraData)
{
    if (m_timer.IsTimerRuning())
    {
        m_timer.KillTimer();
    }

    --m_nCountTime;

    SetShowMessage(m_nCountTime);

    if (m_nCountTime > 0)
    {
        m_timer.SingleShot(1000, TIMER_METHOD(this, CCommonMessageBox::OnCountTime));
    }
    else
    {
        return OnExitTimeOut(&m_timer, NULL);
    }
}

void CCommonMessageBox::SetCountTime(bool bCountTime)
{
    m_bCountTime = bCountTime;
}

void CCommonMessageBox::SetShowMessage(int nCountTime)
{
    yl::string strText = commonAPI_FormatString("%s %ds!", m_strMessage.c_str(), nCountTime);
    if (NULL != m_pMessage)
    {
        m_pMessage->SetText(strText);
        m_pMessage->update();
    }
}
