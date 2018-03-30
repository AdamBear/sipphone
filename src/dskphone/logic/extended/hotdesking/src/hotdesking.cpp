#include "hotdesking_inc.h"
#include "voip/modvoip.h"
#include "adapterbox/include/modadapterbox.h"
#include "setting_page_define.h"

#ifdef IF_FEATURE_HOTDESKING

// HotDeskingSoftkey对配置改变的处理
LRESULT hotDesking_OnConfigChange(msgObject & objMessage)
{
#ifdef IF_ADVANCE_HOTDESKING
    if (objMessage.wParam == ST_DESKKEY)
    {
        _HotDeskingSoftKey.LoadSoftKey();
    }
    else if (objMessage.wParam == ST_DESKLINE)
    {
        _HotDesking.OnDesklineChange(objMessage.lParam);
    }
#endif  //IF_ADVANCE_HOTDESKING
    return TRUE;
}

// 上电后进入登录向导
static void EnterLogonWizard()
{
    //进入账号配置界面
    AdapterBox_SettingUIEnterPage(PAGE_TYPE_HOTDESK);
    _HotDesking.SetLogonWizard(FALSE);
}

static LRESULT OnHotDeskingTimer(msgObject & msg)
{
    if (msg.message != TM_TIMER)
    {
        return -1;
    }

    if (msg.wParam == (UINT)EnterLogonWizard)
    {
        timerKillThreadTimer(msg.wParam);
        EnterLogonWizard();
        return 1;
    }

    return 0;
}

IMPLEMENT_GETINSTANCE(CHotDesking)

CHotDesking::CHotDesking()
{
#ifdef IF_ADVANCE_HOTDESKING
    m_IsAdvMode = FALSE;
#endif  //IF_ADVANCE_HOTDESKING
    m_bIsLogonWizard = FALSE;
}

CHotDesking::~CHotDesking()
{
}

VOID CHotDesking::Init()
{
    m_bIsLogonWizard = (configParser_GetConfigInt(kszLogonWizard) == 1);
    // 账号都disable的情况下才启用注册向导
    m_bIsLogonWizard &= acc_IsAllAccountDisable();
    SingleMsgReg(TM_TIMER, OnHotDeskingTimer);
}

// 进入界面
BOOL CHotDesking::EnterLoginWizard()
{
    if (m_bIsLogonWizard)
    {
        timerSetThreadTimer((UINT)EnterLogonWizard, 100);
    }

    return m_bIsLogonWizard;
}

// 清除用户信息
VOID CHotDesking::ClearUserInfo()
{
    if (configParser_GetConfigInt(kszHotDeskingClearCallLog) == 1)
    {
        // 清空所有呼叫记录
        LocalLog_ClearLogs(kszAllContactId);
    }

#if IF_BUG_25349
    // 后台清空账号
    yl::string strClearAccount = configParser_GetConfigString(kszHotDeskingClearAccount);
    YLList<yl::string> listAccountID;
    if (commonAPI_splitStringBySeparator(strClearAccount, ',', listAccountID)
            && listAccountID.size() > 0)
    {
        YLList<yl::string>::iterator it = listAccountID.begin();
        for (; it != listAccountID.end(); it++)
        {
            yl::string strAccountID = *it;
            int iAccountID = atoi(strAccountID.c_str());

            if (listAccountID.size() == 1
                    && (iAccountID > acc_AccountNum() || iAccountID <= 0))
            {
                acc_EraseAllAccount(ACCOUNT_TYPE_SIP, ACCOUNT_COMMON);
                acc_EmptyAllAccount(ACCOUNT_TYPE_SIP, ACCOUNT_COMMON);
                break;
            }
            else
            {
                acc_EraseAccountById(iAccountID - 1);
                acc_EmptyAccountById(iAccountID - 1);
            }
        }
    }
    else
#endif
    {
        // 后台清空账号
        acc_EraseAllAccount(ACCOUNT_TYPE_SIP, ACCOUNT_COMMON);
        acc_EmptyAllAccount(ACCOUNT_TYPE_SIP, ACCOUNT_COMMON);
    }
}

// 启用普通hotdesking模式
void CHotDesking::StartLocalHotDesking()
{
// #if 0
    // xmlbrowser先不处理
//  if (xmlbrowser_IsActive())
//  {
//      return;
//  }

#ifdef IF_ADVANCE_HOTDESKING
    if (m_IsAdvMode)
    {
        // 高级模式下提示需要先登出hotdesking账号
        popupBox_CreateTips(this, TIPS_NORMAL,
                            _LANG(TRID_PLEASE_LOGOUT_ACCOUNT_FIRST), "", 3000);
    }
    else
#endif  //IF_ADVANCE_HOTDESKING
    {
        // 转入UI流程
        // hotDeskingUI_StartHotDesking();
        // 使用状态通知IdleUI
        idleScreen_SetStatus(PS_STATE_NOTE_HOTDESK);
    }
// #endif
    //UIManager_UpdateWnd();
}

#ifdef IF_ADVANCE_HOTDESKING
// 启用高级hotdesking模式
void CHotDesking::StartRemoteHotDesking(LPCSTR lpURL, int iDsskeyKeyCode)
{
    if (NULL == lpURL
            || talklogic_SessionExist())
    {
        return;
    }
#if 0 // xmlbrowser先不处理
    if (m_IsAdvMode)
    {
        if (xmlbrowser_IsActive())
        {
            msgPostMsgToThread(msgGetThreadByName(VP_BROWSER_THREAD_NAME), XMLBROWSER_MSG_SHOW_NOTE, 1, 0);
            etlSleep(50);
        }
        else if (!talklogic_SessionExist())
        {
            popupBox_CreateTips(this, TIPS_NORMAL, _LANG(TRID_DSSKEY_TIP_PLEASE_LOG_OUT_ACCOUNT_FIRST), "",
                                1500);
        }
    }
    else
    {
        acc_BackupRestoreCfg(0, false);
        _HotDeskingSoftKey.ClearAllKeyInfo();
        yl::string strURL = lpURL;
        commonAPI_AddHttpHead(strURL);
        //g_pobjPhone->m_objDsskeyManager.DrawLoadNote();

        //g_bIsBrowserActive = TRUE;
        //m_bIsLoadXML = TRUE;

        if (xmlbrowser_IsActive())
        {
            // 正处于xmlbrowser线程,提示框由xmlbrowser来现实
            msgPostMsgToThread(msgGetThreadByName(VP_BROWSER_THREAD_NAME), XMLBROWSER_MSG_SHOW_NOTE, 0, 0);
            etlSleep(50);
        }
        else
        {
            popupBox_CreateTips(NULL, TIPS_NORMAL, _LANG(TRID_LOADING_PLEASE_WAIT), "", 0,
                                CBT_DOWNLOAD_HOTDESKING);
        }

        msgPostMsgToThreadEx(msgGetThreadByName(VP_LOAD_THREAD_NAME),
                             XMLBROWSER_MSG_LOAD_XML, 1,
                             _DsskeyManager.KeyCodeToArrayIndex(iDsskeyKeyCode) + 1,
                             strURL.size() + 1,
                             strURL.c_str());
    }
#endif
}

// 设置进入高级hotdesking模式标志
void CHotDesking::SetAdvMode(BOOL bAdvMode)
{
    m_IsAdvMode = bAdvMode;
}
#endif  //IF_ADVANCE_HOTDESKING

// 响应ST_DESKLINE
VOID CHotDesking::OnDesklineChange(int iAccount)
{
    // 读取Hot Desking Line信息
    HOTDESKING_INFO("ST_SIPLINE Line id = %d", iAccount);
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccount);

    //FlashVoiceNote((int)iAccount);
    ////_StatusManager.ClearVoiceMail(iAccount);
    acc_LoadAccount(iAccount);
    acc_RefreshLineState(iAccount);

#ifdef IF_ADVANCE_HOTDESKING
    m_IsAdvMode = FALSE;
    if (!acc_GetRegisterName(iAccount).empty())
    {
        m_IsAdvMode = TRUE;
    }
#endif  //IF_ADVANCE_HOTDESKING

    if (iAccount == 0)
    {
#ifdef IF_ADVANCE_HOTDESKING
        m_IsAdvMode = FALSE;
        if (!acc_GetRegisterName(iAccount).empty())
        {
            m_IsAdvMode = TRUE;
        }
        configParser_SetConfigInt(kszIsHotDeskAccount, (m_IsAdvMode ? 2 : 0));
        if (!m_IsAdvMode)
#else
        configParser_SetConfigInt(kszIsHotDeskAccount, 0);
#endif  //IF_ADVANCE_HOTDESKING
        {
            acc_BackupRestoreCfg(0, true);
        }
    }
}

#endif // #ifdef IF_FEATURE_HOTDESKING
