#include "callcenter_inc.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

enum ModeType
{
    MODE_TYPE_GENARAL               = 0,
    MODE_TYPE_HOTELING_HOST         = 1,
    MODE_TYPE_FLEXIBLE_SEATING_HOST = 2,
    MODE_TYPE_GUEST                 = 3,
};

bool HotelingModel::ReqChangeHotelingStatus(hoteling_Request & refData)
{
    //为-1表示没有注册上的账号开启Hoteling功能
    if (m_nAccountId == -1)
    {
        return false;
    }

    //绑定账号为空，直接返回
    if (!acc_IsAccountAvailable(m_nAccountId))
    {
        return false;
    }


    if (refData.m_eStatus == HOTELING_STATUS_LOGIN)
    {
        if (!refData.m_strUser.empty())
        {
            m_strUserName = refData.m_strUser;
            //保存账号密码到配置
            configParser_SetCfgItemStringValue(kszAccountHotelingUserID, m_nAccountId, m_strUserName.c_str());

            m_strPassword = refData.m_strPassword;

            //自动登陆
            if (refData.m_bIsAuto)
            {
                if (!refData.m_strPassword.empty())
                {
                    configParser_SetCfgItemStringValue(kszAccountHotelingPassword, m_nAccountId, m_strPassword.c_str());
                }
                else if (refData.m_strPassword.empty())
                {
                    m_strPassword = configParser_GetCfgItemStringValue(kszAccountHotelingPassword, m_nAccountId);
                }
            }

            //设置自动登陆
            configParser_SetCfgItemIntValue(kszAccountHotelingAutoLoginSwitch, m_nAccountId,
                                            refData.m_bIsAuto ? 1 : 0);
        }
    }

    //向sip发送本地状态改变请求
    return modVoip_ReqHoteling(m_nAccountId, refData.m_eStatus, m_strUserName.c_str(),
                               m_strPassword.c_str());
//  pAccount->PostSIPClientThreadMsgEx(SIP_MSG_HOTELING_ACTION,
//      refData.m_eStatus,
//      0,
//      sizeof(stSUI) + 1,
//      &stSUI);
    //For Test -syl
//  if (m_eStatus == HOTELING_STATUS_LOGIN)
//  {
//      m_eStatus = HOTELING_STATUS_LOGOUT;
//  }
//  else
//  {
//      m_eStatus = HOTELING_STATUS_LOGIN;
//  }
}

void HotelingModel::SetStatus(HOTELING_STATUS eState)
{
    m_eStatus = eState;

    if (m_eStatus == HOTELING_STATUS_LOGIN)
    {
        dsskey_SetDsskeyStatusByType(DT_HOTELING, DS_HOTELING_ON);
    }
    else
    {
        dsskey_SetDsskeyStatusByType(DT_HOTELING, DS_HOTELING_OFF);
    }
}

HOTELING_STATUS HotelingModel::GetHotelingStatus()
{
    return m_eStatus;
}

//返回自动登陆是否开启
bool HotelingModel::IsAutoLogin()
{
    return configParser_GetCfgItemIntValue(kszAccountHotelingAutoLoginSwitch, m_nAccountId) == 1;

}

//返回FlexibleSeating是否开启
bool HotelingModel::IsFlexibleSeatingEnable(int iAccountId/* = -1*/)
{
    int iActiveID = (-1 == iAccountId) ? m_nAccountId : iAccountId;

    // 为-1表示没有注册上的账号开启Hoteling功能
    if (-1 == iActiveID)
    {
        return false;
    }

    CSipAccount* pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(iActiveID));

    //绑定账号为空，直接返回
    if (pAccount == NULL)
    {
        return false;
    }

    // 优先判断Mode值：Flexible Seating的Host和Guest, 都算开启
    ModeType eMode = (ModeType)configParser_GetCfgItemIntValue(kszAccountHotelingMode, iActiveID);
    if (MODE_TYPE_FLEXIBLE_SEATING_HOST == eMode || MODE_TYPE_GUEST == eMode)
    {
        return (pAccount->GetHotelingSwitch() || pAccount->GetFlexibleSeatingSwitch());
    }

    // 保留此处, 兼容旧客户
    return (!pAccount->GetHotelingSwitch() && pAccount->GetFlexibleSeatingSwitch());
}

// 是否是Guest话机
bool HotelingModel::IsGuestPhone()
{
    ModeType eMode = (ModeType)configParser_GetCfgItemIntValue(kszAccountHotelingMode, m_nAccountId);
    return (MODE_TYPE_GUEST == eMode);
}

// Guest话机是否支持键盘锁
bool HotelingModel::EnableGuestLock()
{
    // 不是Guest话机不允许
    if (!IsGuestPhone())
    {
        return false;
    }

    // 服务器没有下推密码不开启
    if (configParser_GetCfgItemStringValue(kszAccountHotelingUnlockPin, m_nAccountId).empty())
    {
        return false;
    }

    return true;
}

// Guest话机的键盘锁密码是否匹配
bool HotelingModel::IsMatchGuestLockPin(const yl::string & strUnlockPin)
{
    return (strUnlockPin == configParser_GetCfgItemStringValue(kszAccountHotelingUnlockPin,
            m_nAccountId));
}

void HotelingModel::ClearFlexibleSeating()
{
    if (GetAccountId() == -1)
    {
        return ;
    }

    // 该配置只能是写Autop等级
    // 该配置主要是DMS部署Autop写进来的
    // UI清除是为了保证有UnLockPin Guest登录登出后, 密码不残留
    configParser_SetCfgItemIntValue(kszAccountHotelingMode, m_nAccountId, 0, CONFIG_LEVEL_AUTOP);
    configParser_SetCfgItemStringValue(kszAccountHotelingUnlockPin, m_nAccountId, "",
                                       CONFIG_LEVEL_AUTOP);
}

//返回自动登陆的userId
void HotelingModel::GetUserId(yl::string & strUserId)
{
    strUserId = configParser_GetCfgItemStringValue(kszAccountHotelingUserID, m_nAccountId);
}

HotelingModel::HotelingModel()
{
    m_eStatus = HOTELING_STATUS_LOGOUT;
    //初始化时就立即绑定可用账号，如未-1则说明绑定失败
    m_nAccountId = acc_GetHetlingAccountID();
    m_strUserName = "";
    m_strPassword = "";
    m_IsGuestLock = false;

#if IF_BUG_25622
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        // 默认Hoteling订阅的初始状态
        m_vecHotelingSubscibeResult.push_back(false);
    }
#endif
}

//返回绑定的AccountID
int HotelingModel::GetAccountId()
{
    return m_nAccountId;
}

void HotelingModel::SetAccountId(int nId)
{
    m_nAccountId = nId;

    if (nId == -1)
    {
        SetStatus(HOTELING_STATUS_LOGOUT);
    }
}


HotelingModel::~HotelingModel()
{
}

const yl::string & HotelingModel::GetDisPlayName()
{
    return m_strDisPalyName;
}

void HotelingModel::SetDisPlayName(const yl::string & strName)
{
    m_strDisPalyName = strName;

    acc_SetCustomShowText(m_nAccountId, m_strDisPalyName);
}

#if IF_BUG_25622
bool HotelingModel::UpdateHotelingSubscibeResult(int nAccountId, bool bSubscibeResult)
{
    if (nAccountId < 0 || nAccountId >= m_vecHotelingSubscibeResult.size())
    {
        return false;
    }

    m_vecHotelingSubscibeResult[nAccountId] = bSubscibeResult;

    return true;
}

int HotelingModel::GetFirstHotelingOn()
{
    for (int i = 0; i < m_vecHotelingSubscibeResult.size(); ++i)
    {
        if (m_vecHotelingSubscibeResult[i])
        {
            return i;
        }
    }
    return -1;
}
#endif

#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
