#include "callcenter_inc.h"

#ifdef IF_FEATURE_ACD

CACDModelCosmocom::CACDModelCosmocom(void)
{
    Set_ACDType(ACD_COSMOCOM);
}

CACDModelCosmocom::~CACDModelCosmocom(void)
{
}

// 登录ACD
bool CACDModelCosmocom::ReqLogin(acd_accout * pAcd)
{
    if (NULL == pAcd
            || -1 == GetAccountId())
    {
        return false;
    }

    configParser_SetCfgItemStringValue(kszLoginName, GetAccountId(), pAcd->user, CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszLoginPassword, GetAccountId(), pAcd->password,
                                       CONFIG_LEVEL_PHONE);

    return modVoip_ACDAction(GetAccountId(), 0, AA_LOGIN, pAcd->user, pAcd->password);
}

//请求状态改变，包括登陆，登陆时需考虑考虑初始化状态
bool CACDModelCosmocom::ReqChangeACDStatus(ACD_ACTION eACDAction)
{
    return modVoip_ACDAction(GetAccountId(), 0, eACDAction);
}

void CACDModelCosmocom::AutoLogin()
{
    ACD_STATUS eStatus = (ACD_STATUS)configParser_GetCfgItemIntValue(kszACDStatus, GetAccountId());
    if (AS_IDLE == eStatus)
    {
        return;
    }

    acd_accout acdInfo;
    yl::string strUserName = configParser_GetCfgItemStringValue(kszLoginName, GetAccountId());
    strcpy(acdInfo.user, strUserName.c_str());

    yl::string strPassword = configParser_GetCfgItemStringValue(kszLoginPassword, GetAccountId());
    strcpy(acdInfo.password, strPassword.c_str());

    ReqLogin(&acdInfo);
}

void CACDModelCosmocom::SetAccountId(int iAccountId)
{
    CACDModelBase::SetAccountId(iAccountId);

    bool bAutoLogin = configParser_GetCfgItemIntValue(kszACDAutoLogin, GetAccountId()) != 0;

    if (iAccountId != -1 && bAutoLogin)
    {
        AutoLogin();
    }
}

#endif
