#include "warningcenter.h"
#include "extended/warningcenter/include/warningdef.h"
#include "path_define.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "idlescreen/include/modidlescreen.h"
#include "account/include/modaccount.h"
#include "autopcontrol/include/autopcontrollmsgdefine.h"
#include "autopcontrol/include/autop_common.h"
#include "feature/include/modfeature.h"
#include "service_config.h"

#define CASE_WARNING_STRING(x)  case x: return #x

yl::string GetWarningStr(int iWarningType)
{
    switch (iWarningType)
    {
        CASE_WARNING_STRING(WT_NETWORK_FAILED);
        CASE_WARNING_STRING(WT_DEFAULT_USERPSWD_USING);
        CASE_WARNING_STRING(WT_AUTOP_AUTH_FAILED);
        CASE_WARNING_STRING(WT_ACCOUNT_REG_FAILED);
    default:
        break;
    }

    return "";
}

IMPLEMENT_GETINSTANCE(CWarningManager)
CWarningManager::CWarningManager()
{
}

CWarningManager::~CWarningManager()
{
}

void CWarningManager::Init()
{
    SetWraningModeEnable(configParser_GetConfigInt(kszWarningDisplayMode) == 1);
    COMMONUNITS_INFO("[Warning center][Init][Switch:%d]", IsWarningModeEnable());

    m_vecWarning.clear();
    CheckWarningInfo();

    etl_RegisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                          CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_RegisterMsgHandle(AUTOP_AUTOP_STATUS, AUTOP_AUTOP_STATUS,
                          CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_RegisterMsgHandle(AUTOP_AUTOP_FINISH, AUTOP_AUTOP_FINISH,
                          CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          CMSGSubscriber(this, &CWarningManager::OnMessage));
}

void CWarningManager::UnInit()
{
    COMMONUNITS_INFO("[Warning center][UnInit]");

    ClearWarningInfo();

    etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                            CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                            CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_UnregisterMsgHandle(AUTOP_AUTOP_STATUS, AUTOP_AUTOP_STATUS,
                            CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_UnregisterMsgHandle(AUTOP_AUTOP_FINISH, AUTOP_AUTOP_FINISH,
                            CMSGSubscriber(this, &CWarningManager::OnMessage));
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            CMSGSubscriber(this, &CWarningManager::OnMessage));
}

void CWarningManager::CheckWarningInfo()
{
    COMMONUNITS_INFO("[Warning center][Check warning info]");
    CheckNetwork();
    CheckAccount();
    CheckPhonePSWD();
}

bool CWarningManager::CheckNetwork()
{
    if (!IsWarningModeEnable())
    {
        return false;
    }

    if (!netIsNetworkReady())
    {
        AddWarning(WT_NETWORK_FAILED);
    }
    else
    {
        RemoveWarning(WT_NETWORK_FAILED);
    }

    COMMONUNITS_DEBUG("[Warning center][Check network][status:%d]", netIsNetworkReady());

    return true;
}

bool CWarningManager::CheckAccount(int iAccountID /*= INVALID_ACCOUNTID*/)
{
    if (!IsWarningModeEnable())
    {
        return false;
    }

    if (INVALID_ACCOUNTID != iAccountID)
    {
        LINE_STATE eState = acc_GetState(iAccountID);
        if (LS_REGISTERED != eState && LS_REGISTER_FAIL != eState)
        {
            return false;
        }
    }

    ListAccounts listAccount = acc_GetRegisteFailAccountList();
    if (listAccount.size() > 0)
    {
        AddWarning(WT_ACCOUNT_REG_FAILED);
    }
    else
    {
        RemoveWarning(WT_ACCOUNT_REG_FAILED);
    }

    COMMONUNITS_DEBUG("[Warning center][Check account][Reg fail size:%d]", listAccount.size());

    return true;
}

bool CWarningManager::CheckAutop(msgObject& msg)
{
    if (!IsWarningModeEnable())
    {
        return false;
    }

    if (msg.message == AUTOP_AUTOP_STATUS
            && msg.wParam == AUT_STATUS_AUTH)
    {
        AddWarning(WT_AUTOP_AUTH_FAILED);
    }
    else if (msg.message == AUTOP_AUTOP_FINISH)
    {
        RemoveWarning(WT_AUTOP_AUTH_FAILED);
    }

    COMMONUNITS_DEBUG("[Warning center][Check autop auth][Auth succeed:%d]",
                      msg.message == AUTOP_AUTOP_FINISH);

    return true;
}

bool CWarningManager::CheckPhonePSWD()
{
    if (!IsWarningModeEnable())
    {
        return false;
    }

    yl::string strDefault = "";
    switch (feature_UserModeGetCurrType())
    {
    case UT_ADMIN:
        strDefault = DEFAULT_USER_ADMIN;
        break;
    case UT_USER:
        strDefault = DEFAULT_USER_USER;
        break;
    case UT_VAR:
        strDefault = DEFAULT_USER_VAR;
        break;
    default:
        return false;
    }

    bool bUsingDefault = commonAPI_CheckPassword(strDefault.c_str(), strDefault.c_str());
    if (bUsingDefault)
    {
        AddWarning(WT_DEFAULT_USERPSWD_USING);
    }
    else
    {
        RemoveWarning(WT_DEFAULT_USERPSWD_USING);
    }

    COMMONUNITS_DEBUG("[Warning center][Check phone pswd][User:%s][Using default:%d]",
                      strDefault.c_str(), bUsingDefault);

    return true;
}

int CWarningManager::GetWarningList(YLVector<int>& vecWarning)
{
    vecWarning = m_vecWarning;
    return vecWarning.size();
}

bool CWarningManager::OnMessage(msgObject& msg)
{
    bool bHandled = true;
    switch (msg.message)
    {
    case SYS_MSG_NOTIFY_PHY_STATUS:
    case SYS_MSG_NOTIFY_NETWORK_STATUS:
        {
            CheckNetwork();
        }
        break;
    case ACCOUNT_STATUS_CHANGED:
        {
            CheckAccount(msg.wParam);
        }
        break;
    case AUTOP_AUTOP_STATUS:
    case AUTOP_AUTOP_FINISH:
        {
            CheckAutop(msg);
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_USER_INFO_CHANGED == msg.wParam)
            {
                CheckPhonePSWD();
            }
            else if (ST_WARNING_DISPLARY_MODE == msg.wParam)
            {
                ProcessWraningModeChange();
            }
        }
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

void CWarningManager::RemoveWarning(int iWarning)
{
    for (int i = 0; i < m_vecWarning.size(); ++i)
    {
        // 警告类型存在, 则移除
        if (WT_NONE == iWarning
                || iWarning == m_vecWarning[i])
        {
            m_vecWarning.removeAt(i);
            COMMONUNITS_INFO("[Warning center][Remove][Warning type:%s]",
                             GetWarningStr(iWarning).c_str());
        }
    }

    if (GetWarningStatus() &&  m_vecWarning.size() <= 0)
    {
        SetWarningStatus(false);
    }
}

void CWarningManager::ProcessWraningModeChange()
{
    bool bEnable = configParser_GetConfigInt(kszWarningDisplayMode) == 1;
    if (IsWarningModeEnable() == bEnable)
    {
        return;
    }

    SetWraningModeEnable(bEnable);
    COMMONUNITS_INFO("[Warning center][Mode changed][status:%d]", IsWarningModeEnable());

    if (IsWarningModeEnable())
    {
        CheckWarningInfo();
    }
    else
    {
        ClearWarningInfo();
    }
}

void CWarningManager::ClearWarningInfo()
{
    COMMONUNITS_INFO("[Warning center][Clear warning info]");
    m_vecWarning.clear();
    SetWarningStatus(false);
}

void CWarningManager::SetWarningStatus(bool bStatus)
{
    COMMONUNITS_INFO("[Warning center][Set warning status:%d]", bStatus);
    m_bStatus = bStatus;

    if (bStatus)
    {
        idleScreen_SetStatus(PS_STATE_WARNING);
    }
    else
    {
        idleScreen_DropStatus(PS_STATE_WARNING);
    }
}

bool CWarningManager::GetWarningStatus()
{
    if (!IsWarningModeEnable())
    {
        return false;
    }

    return m_bStatus;
}

void CWarningManager::AddWarning(int iWarning)
{
    int iNum = m_vecWarning.size();

    for (int i = 0; i < m_vecWarning.size(); ++i)
    {
        // 警告类型存在, 则返回
        if (iWarning == m_vecWarning[i])
        {
            return;
        }
    }

    m_vecWarning.push_back(iWarning);
    COMMONUNITS_INFO("[Warning center][Add][Warning type:%s]", GetWarningStr(iWarning).c_str());

    // 新增警告信息, 需设置警告状态
    if (m_vecWarning.size() > iNum && !GetWarningStatus())
    {
        SetWarningStatus(true);
    }
}
