#include "wifiui/include/wifi_inc.h"

namespace
{
// pop dialog ID
#define POP_DIALOG_OPEN_WIFI_SWITCH         90
#define POP_DIALOG_COUNT_DOWN               91
// count down time
#define COUNT_DOWN_TIME                     120000
}

// 处理消息.
static LRESULT OnMessageHelp(msgObject & refMessage)
{
    _UIWifiManagerHelp.InterMessageHelp(refMessage);

    return TRUE;
}

LRESULT CWifiUIManagerHelp::InterMessageHelp(msgObject & refMessage)
{
    SETTINGUI_INFO("CWifiUIManagerHelp::OnMessageHelp[%d]", refMessage.message);
#ifdef IF_SUPPORT_WIFI
    if (WIFI_MSG_UPDATE_UI == refMessage.message)
    {
        if (OP_WPS_OPEN == refMessage.wParam)
        {
            if (!talklogic_SessionExist())
            {
                // 过滤，防止弹出两个倒计时框
                if (m_bPopCountDownDlg)
                {
                    SETTINGUI_INFO("[CWifiUIManagerHelp::OnMessageHelp][WPS has pop, not pop two time, sorry!!!]");
                    return TRUE;
                }

                if (!WIFI_IsEnable())
                {
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_WIFI_SWITCH_IS_CLOSED_OPEN_IT_OR_NOT),
                                                MESSAGEBOX_OkCancel, -1, "", "", POP_DIALOG_OPEN_WIFI_SWITCH);
                }
                else
                {
                    _MainWND.SetTimerLock(true);
                    WIFI_StartWps();
                    m_bPopCountDownDlg = true;
                    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_WPS_HAS_OPENED_PLEASE_CONNECT_IN)
                                                , MESSAGEBOX_Cancel, COUNT_DOWN_TIME, "", "", POP_DIALOG_COUNT_DOWN, QVariant(), true);
                }
            }
        }
        else if (OP_WPSCONNECT_RESULT == refMessage.wParam)
        {
            if (!talklogic_SessionExist())
            {
                SETTINGUI_INFO("[CWifiUIManagerHelp::OnMessageHelp][WPS Connect Reusult[%d]]", refMessage.wParam);
                MessageBox_RemoveTopMessageBox();
                if (1 == refMessage.lParam)
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_SUCCESS), MESSAGEBOX_Cancel,
                                                SHORT_TIME);
                }
                else
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_FAILED), MESSAGEBOX_Cancel,
                                                SHORT_TIME);
                }
                _MainWND.SetTimerLock(false);
            }
        }
    }
#endif
    return TRUE;
}

CWifiUIManagerHelp::CWifiUIManagerHelp()
    : m_bPopCountDownDlg(false)
{
    //
}

CWifiUIManagerHelp::~CWifiUIManagerHelp()
{
    UnInitMsg();
    m_bPopCountDownDlg = false;
}

//获取全局唯一实例
CWifiUIManagerHelp & CWifiUIManagerHelp::GetInstance()
{
    static CWifiUIManagerHelp s_Instance;
    return s_Instance;
}

void CWifiUIManagerHelp::InitMsg()
{
#ifdef IF_SUPPORT_WIFI
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnMessageHelp);
#endif
}

void CWifiUIManagerHelp::UnInitMsg()
{
#ifdef IF_SUPPORT_WIFI
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnMessageHelp);
#endif
}

// 处理弹出框回调
void CWifiUIManagerHelp::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerHelp::MessageBoxCallBack()]");
    if (NULL == pMessageBox)
    {
        return;
    }
    SETTINGUI_INFO("Result[%d], ID[%d]", pMessageBox->GetResult(), pMessageBox->GetID());
    SETTINGUI_INFO("ID");
    SETTINGUI_INFO("POP_DIALOG_OPEN_WIFI_SWITCH	= %d", POP_DIALOG_OPEN_WIFI_SWITCH);
    SETTINGUI_INFO("POP_DIALOG_COUNT_DOWN		= %d", POP_DIALOG_COUNT_DOWN);
    SETTINGUI_INFO("\nresult");
    SETTINGUI_INFO("MessageBox_Cancel	= %d", MessageBox_Cancel);
    SETTINGUI_INFO("MessageBox_OK		= %d", MessageBox_OK);
    SETTINGUI_INFO("MessageBox_Destory	= %d", MessageBox_Destory);
    SETTINGUI_INFO("MessageBox_ERROR	= %d", MessageBox_ERROR);

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case POP_DIALOG_OPEN_WIFI_SWITCH:
    {
        if (MessageBox_OK == nResult && WIFI_SetEnable(true))
        {
            _MainWND.SetTimerLock(true);
            WIFI_StartWps();
            m_bPopCountDownDlg = true;
            MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_WPS_HAS_OPENED_PLEASE_CONNECT_IN)
                                        , MESSAGEBOX_Cancel, COUNT_DOWN_TIME, "", "", POP_DIALOG_COUNT_DOWN, QVariant(), true);
        }
    }
    break;
    case POP_DIALOG_COUNT_DOWN:
    {
        SETTINGUI_INFO("[CWifiUIManagerHelp::MessageBoxCallBack][WPS iOp[%d]]", nResult);
        if (MessageBox_OK != nResult)
        {
            WIFI_CancelWps();
        }
        _MainWND.SetTimerLock(false);
        m_bPopCountDownDlg = false;
    }
    break;
    default:
        break;
    }
#endif
}
