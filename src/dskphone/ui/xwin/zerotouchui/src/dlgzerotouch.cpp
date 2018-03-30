#include "dlgzerotouch.h"
#include "settingui/src/uicallbackfun.h"
#include "imeedit/ximeedit.h"
#include "keymap.h"

#include "baseui/include/t2xpicpath.h"
#include "baseui/include/t2xwidget.h"
#include "baseui/include/baseuicommon.h"
#include "xwindow/xSoftkeyBar.h"
#include "autopcontrol/include/modautopcontrol.h"

#include "settinguilogic/include/common.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/messageboxbase.h"

#include "settingui/include/modsettingui.h"
#include "settingui/include/pageiddefine.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "modzerotouchui.h"
#include "settinguilogic/include/zero_common.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "settingui/include/modsettingui.h"
#include "commonunits/modcommonunits.h"
#include "uimanager/moduimanager.h"
#include "settingui/include/setuidatadefine.h"


namespace
{
#define  ZEROTOUCH_TEXT_PROMPT          "txtPrompt"
#define  ZEROTOUCH_REMAIN_TIME          10
#define  ZEROTOUCH_TIME_ONE_SECOND      1000
#define  ZEROTOUCH_WAITTING_EXIT_TIME   3000

#define MSG_BOX_ID_UPDATING_NETWORK     1007
#define MSG_BOX_ID_UPDATING_CONFIG      1008
#define MSG_BOX_ID_CHECK_NETWORK        1009
}

CBaseDialog * CDlgZeroTouch::CreateInstance()
{
    CDlgZeroTouch * pDlg = new CDlgZeroTouch;

    if (NULL != pDlg)
    {
        pDlg->loadContent("zerotouchui/dlgzerotouch.xml");
    }

    return pDlg;
}

CDlgZeroTouch::CDlgZeroTouch()
    : m_pTxtPrompt(NULL)
    , m_nRemainTime(ZEROTOUCH_REMAIN_TIME)
    , m_ePageType(ZERO_PAGE_NONE)
    , m_strPageAction("")
    , m_eDelayType(DOT_NONE)
{

}

CDlgZeroTouch::~CDlgZeroTouch()
{
    m_timerRemain.KillTimer();
    m_timerDelayOperate.KillTimer();
    UIManager_LockReturnToIdle(false);
}

void CDlgZeroTouch::Init()
{
    RefreshTitle(_LANG(TRID_ZERO_TOUCH));
}

void CDlgZeroTouch::Uninit()
{

}

void CDlgZeroTouch::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    if (MessageBox_TimeOut == pMessageBox->GetResult())
    {
        FallBackToThePreviousInterface();
        g_SetUICallBackFun.RefreshUI(true);
        return;
    }

    return CDlgSettingUI::MessageBoxCallBack(pMessageBox);
}

void CDlgZeroTouch::loadChildrenElements(xml_node & node)
{
    CDlgSettingUI::loadChildrenElements(node);
    m_pTxtPrompt = static_cast<xTextView *>(getViewById(ZEROTOUCH_TEXT_PROMPT));
}

void CDlgZeroTouch::RefreshUI(bool bReLoadData /* = false */)
{
    if (!bReLoadData)
    {
        return;
    }

    LoadPageData(bReLoadData);
}

bool CDlgZeroTouch::LoadPageData(bool bReLoadData /* = false */)
{
    if (!bReLoadData)
    {
        return false;
    }

    return true;
}

bool CDlgZeroTouch::onKeyPressedEvent(int key)
{
    if (PHONE_KEY_OK == key && ProcessOK())
    {
        return true;
    }

    return CDlgSettingUI::onKeyPressedEvent(key);
}

bool CDlgZeroTouch::ProcessOK()
{
    bool bRet = false;
    if (ZERO_PAGE_MAIN == m_ePageType)
    {
        StopRemain();
        SetPrompt(_LANG(TRID_IF_UPDATE_NOW));
        SettingUI_EnterPage(kszPageIDZeroNetwork);
        bRet = true;
    }
    else if (ZERO_PAGE_NOTE_UPTADE_FINISH == m_ePageType ||
             ZERO_PAGE_REDIRECTOR_FINISH == m_ePageType || ZERO_PAGE_NOTE_NETWORK_FAIL == m_ePageType ||
             ZERO_PAGE_REDIRECTOR_FAILED == m_ePageType || ZERO_PAGE_NOTE_UPDATE_FAIL == m_ePageType)
    {
        m_timerDelayOperate.KillTimer();
        ZeroTouchUI_Destroy();
        bRet = true;
    }
    return bRet;
}


bool CDlgZeroTouch::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (ST_NEXT_PAGE == iType)
    {
        StopRemain();
        SetPrompt(_LANG(TRID_IF_UPDATE_NOW));
        SettingUILogic_EnterZeroNextStage(ZERO_MAIN_PAGE, ZERO_NEXT);
    }
    else if (ST_STATUS == iType)
    {
        StopRemain();
        SetPrompt(_LANG(TRID_IF_UPDATE_NOW));
        SettingUILogic_EnterZeroNextStage(ZERO_MAIN_PAGE, ZERO_STATUS);
    }
    else if (ST_BACK == iType || ST_CANCEL == iType)
    {
        StopRemain();
        ZeroTouchUI_Destroy();
    }
    else if (ST_OK == iType)
    {
        ProcessOK();
    }

    return true;
}

void CDlgZeroTouch::SetPrompt(const yl::string & strPromp)
{
    if (NULL == m_pTxtPrompt)
    {
        return;
    }
    m_pTxtPrompt->SetText(strPromp);
}

void CDlgZeroTouch::StartRemain()
{
    //  读取配置表设置总时间
    m_nRemainTime = configParser_GetConfigInt(kszZeroWaitTime);
    if (m_nRemainTime <= 0)
    {
        m_nRemainTime = ZEROTOUCH_REMAIN_TIME;
    }
    SetRemainPrompt();
    StopRemain();
    m_timerRemain.SetTimer(ZEROTOUCH_TIME_ONE_SECOND, TIMER_METHOD(this, CDlgZeroTouch::OnRemainTimer));

}

void CDlgZeroTouch::StopRemain()
{
    if (m_timerRemain.IsTimerRuning())
    {
        // 停止计时器
        m_timerRemain.KillTimer();
    }
}

void CDlgZeroTouch::OnRemainTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    //每次超时,时间减少1s,形成倒时计效果
    if (m_nRemainTime > 1)
    {
        m_nRemainTime--;
        SetRemainPrompt();
    }
    else
    {
        StopRemain();
        //倒时计结束,退出界面
        ZeroTouchUI_Destroy();
    }
}

void CDlgZeroTouch::SetRemainPrompt()
{
    yl::string strRemain = commonAPI_FormatString("%d", m_nRemainTime);
    yl::string strPrompt = _LANG(TRID_IF_UPDATE_NOW) + " " + strRemain + " s";
    SetPrompt(strPrompt);
}

void CDlgZeroTouch::SetPageType(ZeroPageType ePageType)
{
    // 设置类型
    m_ePageType = ePageType;
    // 停止倒计时
    StopRemain();
    // 刷新界面
    RefreshPage();
}

void CDlgZeroTouch::RefreshPage()
{
    MessageBox_RemoveMessageBox(this, MessageBox_Cancel, MSG_BOX_ID_CHECK_NETWORK);
    if (ZERO_PAGE_NOTE_UPTADE_FINISH == m_ePageType || ZERO_PAGE_REDIRECTOR_FINISH == m_ePageType ||
            ZERO_PAGE_NOTE_UPDATE_FAIL == m_ePageType || ZERO_PAGE_NOTE_NETWORK_FAIL == m_ePageType ||
            ZERO_PAGE_REDIRECTOR_FAILED == m_ePageType)
    {
        // 延迟为了跟CP860保持一致，能显示清楚更新中的提示
        etlSleep(300);
        MessageBox_RemoveMessageBox(this, MessageBox_Cancel, MSG_BOX_ID_UPDATING_CONFIG);
    }

    switch (m_ePageType)
    {
    case ZERO_PAGE_MAIN:
        {
            SetSoftkeyType(ZERO_SOFTKEY_TYPE_CONTROL);
            StartRemain();
        }
        break;
    case ZERO_PAGE_NETWORK_SETTING:
        {
            UIManager_LockReturnToIdle(false);
            m_ePageType = ZERO_PAGE_MAIN;
            SettingUI_EnterPage(kszPageIDZeroNetwork);
        }
        break;
    case ZERO_PAGE_STATUS:
        {
            UIManager_LockReturnToIdle(false);
            SettingUI_EnterPage(kszPageIDZeroStatus);
        }
        break;
    case ZERO_PAGE_AUTOP:
        {
            UIManager_LockReturnToIdle(false);
            SettingUI_EnterPage(kszPageIDZeroAutoP);
        }
        break;
    case ZERO_PAGE_NOTE_UPDATING:
        {
            MSGBOX_INFO infoMsgBox;
            infoMsgBox.eType = MESSAGEBOX_NOTE;
            infoMsgBox.pAction = this;
            infoMsgBox.iID = MSG_BOX_ID_UPDATING_CONFIG;
            infoMsgBox.strNote = _LANG(TRID_CONFIG_UPDATING);
            infoMsgBox.nMsTime = -1;
            MessageBox_ShowCommonMsgBox(infoMsgBox);
        }
        break;
    case ZERO_PAGE_NOTE_UPTADE_FINISH:
    case ZERO_PAGE_REDIRECTOR_FINISH:
        {
            if (ZERO_PAGE_REDIRECTOR_FINISH == m_ePageType)
            {
                RefreshTitle(_LANG(TRID_REDIRCTOR));
            }
            SetPrompt(_LANG(TRID_UPDATE_FINISH));
            SetSoftkeyType(ZERO_SOFTKEY_TYPE_PROMPT);
            DelayOperator(DOT_UPDATE_FINISH);
        }
        break;
    case ZERO_PAGE_NOTE_REBOOTING:
        {
            MSGBOX_INFO stMsgInfo;
            stMsgInfo.eType = MESSAGEBOX_NOTE;
            stMsgInfo.strTitle = _LANG(TRID_NOTE);
            stMsgInfo.strNote = _LANG(TRID_NETWORK_CHANGED_AND_REBOOTING);
            stMsgInfo.nMsTime = -1;
            stMsgInfo.pAction = this;
            stMsgInfo.iID = MSG_BOX_ID_UPDATING_NETWORK;
            MessageBox_ShowCommonMsgBox(stMsgInfo);
            commonUnits_Reboot();
        }
        break;
    case  ZERO_PAGE_NOTE_UPDATE_FAIL:
        {
            SetPrompt(_LANG(TRID_UPDATE_SKIPPED));
            SetSoftkeyType(ZERO_SOFTKEY_TYPE_PROMPT);
            DelayOperator(DOT_UPDATE_FAIL);
        }
        break;
    case ZERO_PAGE_NOTE_NETWORK_CHECK:
        {
            MSGBOX_INFO infoMsgBox;
            infoMsgBox.eType = MESSAGEBOX_NOTE;
            infoMsgBox.pAction = this;
            infoMsgBox.iID = MSG_BOX_ID_CHECK_NETWORK;
            infoMsgBox.strNote = _LANG(TRID_NETWORK_CHECKING);
            infoMsgBox.nMsTime = -1;
            MessageBox_ShowCommonMsgBox(infoMsgBox);
            g_SettingMsgBoxMgr.AddAsyncOperate(this);
        }
        break;
    case ZERO_PAGE_NOTE_NETWORK_FAIL:
        {
            SetPrompt(_LANG(TRID_NETWORK_UNAVAILABLE));
            SetSoftkeyType(ZERO_SOFTKEY_TYPE_PROMPT);
            DelayOperator(DOT_NETWORK_FAIL);
        }
        break;
    case ZERO_PAGE_REDIRECTOR_NOTE:
        {
            RefreshTitle(_LANG(TRID_REDIRCTOR));
            SetPrompt(_LANG(TRID_SENDING_PROVISION_REQUEST));
            SetSoftkeyType(ZERO_SOFTKEY_TYPE_CANCEL);
        }
        break;
    case ZERO_PAGE_REDIRECTOR_SUBMIT:
        {
            SettingUI_EnterPage(kszPageIDZeroRedirect);
        }
        break;
    case ZERO_PAGE_REDIRECTOR_FAILED:
        {
            RefreshTitle(_LANG(TRID_REDIRCTOR));
            SetPrompt(_LANG(TRID_REDIRCT_UPDATING_FAILED));
            SetSoftkeyType(ZERO_SOFTKEY_TYPE_PROMPT);
            //3秒后退出提示
            DelayOperator(DOT_UPDATE_FAIL);
        }
        break;
    default:
        break;
    }

    if (this == UIManager_GetTopWindow())
    {
        if (ZERO_PAGE_NOTE_NETWORK_CHECK == m_ePageType || ZERO_PAGE_NOTE_UPDATING == m_ePageType || ZERO_PAGE_NOTE_REBOOTING == m_ePageType)
        {
            currentSoftKeyBar().hide();
            hide();
        }
        else
        {
            currentSoftKeyBar().show();
            show();
        }
    }
}

void CDlgZeroTouch::SetPageType(const yl::string & strPageAction)
{
    m_strPageAction = strPageAction;
    SetPageType(ActionToType(strPageAction));
}

// 将相应的页面Action转换为Type
ZeroPageType CDlgZeroTouch::ActionToType(const yl::string & strPageAction)
{
    ZeroPageType eType = ZERO_PAGE_NONE;
#ifdef ZERO_TOUCH_DEBUG
    ZEROUI_DEBUG("CDlgZeroSPTouch::ActionToType Action(%s)", strPageAction.c_str());
#endif
    if (ZERO_MAIN_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_MAIN;
    }
    else if (ZERO_NETWORK_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NETWORK_SETTING;
    }
    else if (ZERO_STATUS_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_STATUS;
    }
    else if (ZERO_UPDATING_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_UPDATING;
    }
    else if (ZERO_UPDATEFINISH_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_UPTADE_FINISH;
    }
    else if (ZERO_UPDATEFAIL_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_UPDATE_FAIL;
    }
    else if (ZERO_NETWORK_CHECK_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_NETWORK_CHECK;
    }
    else if (ZERO_NETWORK_FAIL_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_NETWORK_FAIL;
    }
    else if (ZERO_REBOOTING_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_REBOOTING;
    }
    else if (ZERO_REDIRECTOR_NOTE == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_NOTE;
    }
    else if (ZERO_REDIRECTOR_CONFIG == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_SUBMIT;
    }
    else if (ZERO_REDIRECTOR_FAILED == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_FAILED;
    }
    else if (ZERO_REDIRECTOR_FINISH == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_FINISH;
    }
    else if (ZERO_AUTOP_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_AUTOP;
    }

#ifdef ZERO_TOUCH_DEBUG
    ZEROUI_DEBUG("CDlgZeroSPTouch::ActionToType eType(%d)", eType);
#endif
    return eType;
}

void CDlgZeroTouch::OnNetworkFailPageTimeout()
{
    if (AUT_RPS == Autop_GetUpdateType())
    {
        ZeroTouchUI_EnterPage(ZERO_NETWORK_PAGE);
    }
    else if (1 == configParser_GetConfigInt(kszZeroSimplification))
    {
        // 网络检查失败后，跳到Autop界面
        ZeroTouchUI_EnterPage(ZERO_AUTOP_PAGE);
    }
    else if (1 == configParser_GetConfigInt(kszZeroNetworkAndURLEnable))
    {
        // 网络检查失败后，跳到网络配置界面
        ZeroTouchUI_EnterPage(ZERO_NETWORK_PAGE);
    }
    else
    {
        ZeroTouchUI_EnterPage(ZERO_MAIN_PAGE);
    }
}

void CDlgZeroTouch::OnDelayOperator(xThreadTimer* timer, LPVOID pExtraData)
{
    switch (m_eDelayType)
    {
    case DOT_NETWORK_FAIL:
        {
            ZeroTouchUI_Destroy();
        }
        break;
    case DOT_UPDATE_FAIL:
        {
            ZeroTouchUI_Destroy();
        }
        break;
    case DOT_UPDATE_FINISH:
        {
            if (!commonUnits_IsSystemMaintenanceStatus())
            {
                // 非系统维护状态才处理。
                ZeroTouchUI_Destroy();
            }
        }
        break;
    }
}

void CDlgZeroTouch::DelayOperator(DELAY_OPERATOR_TYPE eType)
{
    m_eDelayType = eType;
    m_timerDelayOperate.KillTimer();
    m_timerDelayOperate.SingleShot(ZEROTOUCH_WAITTING_EXIT_TIME, TIMER_METHOD(this,
                                   CDlgZeroTouch::OnDelayOperator));
}

void CDlgZeroTouch::OnReturnToIdle()
{
    SettingUILogic_ExitZero();
}

void CDlgZeroTouch::SetSoftkeyType(ZERO_SOFTKEY_TYPE sType)
{
    CArraySoftKeyBarData vecSoftKey;
    CSoftKeyBarData objSoftkeyData;
    switch (sType)
    {
    case ZERO_SOFTKEY_TYPE_PROMPT:
        {
            for (int i = 0; i < 3; ++i)
            {
                objSoftkeyData.m_iType = ST_EMPTY;
                objSoftkeyData.m_strSoftkeyTitle = "";
                vecSoftKey.push_back(objSoftkeyData);
            }

            objSoftkeyData.m_iType = ST_OK;
            objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_OK);
            vecSoftKey.push_back(objSoftkeyData);
        }
        break;
    case ZERO_SOFTKEY_TYPE_CONTROL:
        {
            objSoftkeyData.m_iType = ST_BACK;
            objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_CANCEL);
            vecSoftKey.push_back(objSoftkeyData);

            objSoftkeyData.m_iType = ST_STATUS;
            objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_STATUS);
            vecSoftKey.push_back(objSoftkeyData);

            objSoftkeyData.m_iType = ST_EMPTY;
            objSoftkeyData.m_strSoftkeyTitle = "";
            vecSoftKey.push_back(objSoftkeyData);

            objSoftkeyData.m_iType = ST_NEXT_PAGE;
#if IF_BUG_37505
            objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_SET_UP);
#else
            objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_OK);
#endif
            vecSoftKey.push_back(objSoftkeyData);
        }
        break;
    case ZERO_SOFTKEY_TYPE_CANCEL:
        {
            objSoftkeyData.m_iType = ST_BACK;
            objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_CANCEL);
            vecSoftKey.push_back(objSoftkeyData);
            for (int i = 0; i < 3; ++i)
            {
                objSoftkeyData.m_iType = ST_EMPTY;
                objSoftkeyData.m_strSoftkeyTitle = "";
                vecSoftKey.push_back(objSoftkeyData);
            }
        }
        break;
    default:
        break;
    }
    SetSoftKey(vecSoftKey);
}

bool CDlgZeroTouch::IsRejectReturnToIdle()
{
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->IsRejectReturnToIdle();
    }

    return false;
}

void CDlgZeroTouch::InitExtraData(void * pData)
{
    UIManager_LockReturnToIdle(true);
}

void CDlgZeroTouch::AsyncOperateCallBack()
{
    SettingUILogic_AsyncAutop();
}
