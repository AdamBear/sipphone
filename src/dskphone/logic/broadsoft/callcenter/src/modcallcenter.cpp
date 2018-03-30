#include "callcenter_inc.h"

#if IF_FEATURE_START2START_ACD
#include "broadsoft/callcenter/src/acdmodelstart2start.h"
#include "etlmsghandler/modetlmsghandler.h"
#endif

#if defined(IF_FEATURE_ACD) || defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD) || IF_FEATURE_METASWITCH_ACD

void CallCenter_Init()
{
    if (g_CallCenterController != NULL)
    {
        g_CallCenterController->RegistMsg();
    }
}

//ACD和Hoteling登陆接口
bool CallCenter_Login(ACDLogin & refData)
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->Login(refData);
}

bool ACD_ReqChangeACDStatus(ACD_ACTION eACDAction)
{
    return g_ACDModel == NULL ?
           false : g_ACDModel->ReqChangeACDStatus(eACDAction);
}

bool CallCenter_IsACDOn()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->IsACDOn();
}

//返回Account iD信息
int ACD_GetAccountId()
{
    return g_ACDModel == NULL ? -1 : g_ACDModel->GetAccountId();
}

// 返回ACD类型
ACD_TYPE Get_ACDType()
{
    if (NULL == g_ACDModel)
    {
        return ACD_BASE;
    }

    return g_ACDModel->Get_ACDType();
}

//获取ACD当前状态
ACD_STATUS ACD_GetAcdStatus()
{
    return g_ACDModel == NULL ?
           AS_IDLE : g_ACDModel->GetAcdStatus();
}

bool ACD_ReqLogin()
{
    return g_ACDModel == NULL ? false : g_ACDModel->ReqLogin();
}

//ACD和Hoteling登出接口
bool CallCenter_Logout()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->Logout();
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

HOTELING_STATUS CallCenter_GetHotelingStatus()
{
    return g_CallCenterController == NULL ?
           HOTELING_STATUS_NONE : g_CallCenterController->GetHotelingStatus();

}

//像服务器发送追踪信令
bool CallCenter_OriginatedTrace(int nCallId)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return false;
    }

    return ((ACDModel *)g_ACDModel)->OriginatedTrace(nCallId);
}

bool CallCenter_SendDispCode(const yl::string & strCode, int nCallId)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return false;
    }

    return ((ACDModel *)g_ACDModel)->SendDispCode(strCode, nCallId);
}

bool CallCenter_EmergencyEscalation(const yl::string & strNumber, int nCallId)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return false;
    }

    return ((ACDModel *)g_ACDModel)->EmergencyEscalation(strNumber, nCallId);
}

bool CallCenter_IsHotelingOn()
{
    return g_CallCenterController == NULL ? false : g_CallCenterController->IsHotelingOn()
           && (configParser_GetConfigInt(kszBroadsoftActive) != 0);
}

bool CallCenter_IsFlexibleSeatingOn(int iAccountId/* = -1*/)
{
    return g_CallCenterController == NULL ? false : g_CallCenterController->IsFlexibleSeatingEnable(iAccountId);
}

// Guest话机是否支持键盘锁
bool CallCenter_EnableGuestLock()
{
    HotelingModel * pHoteling = g_CallCenterController->GetHotelingModel();
    if (NULL != pHoteling)
    {
        return pHoteling->EnableGuestLock();
    }

    return false;
}

// Guest话机的键盘锁是否生效
bool CallCenter_IsGuestLock()
{
    HotelingModel * pHoteling = g_CallCenterController->GetHotelingModel();
    if (NULL != pHoteling)
    {
        return pHoteling->IsGuestLock();
    }

    return false;
}

// Guest话机的键盘锁密码是否匹配
bool CallCenter_IsMatchGuestLockPin(const yl::string & strUnlockPin)
{
    HotelingModel * pHoteling = g_CallCenterController->GetHotelingModel();
    if (NULL != pHoteling)
    {
        return pHoteling->IsMatchGuestLockPin(strUnlockPin);
    }

    return false;
}

// Guest话机开启/关闭键盘锁
void CallCenter_GuestLock(bool bLock)
{
    HotelingModel * pHoteling = g_CallCenterController->GetHotelingModel();
    if (NULL != pHoteling)
    {
        pHoteling->GuestLock(bLock);
    }
}

// 清除FlexibleSeating的配置
void CallCenter_ClearFlexibleSeating()
{
    HotelingModel * pHoteling = g_CallCenterController->GetHotelingModel();
    if (NULL != pHoteling)
    {
        pHoteling->ClearFlexibleSeating();
    }
}

bool CallCenter_IsTraceOn()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->IsTraceOn();
}

bool CallCenter_IsDispCodeOn()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->IsDispCodeOn();
}

bool CallCenter_IsEmergencyEscalationOn()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->IsEmergencyEscalationOn();
}

bool Hoteling_IsAutoLogin()
{
    return g_HotelingModel == NULL ?
           false : g_HotelingModel->IsAutoLogin();
}

void Hoteling_GetUserId(yl::string & strUserId)
{
    if (g_HotelingModel != NULL)
    {
        g_HotelingModel->GetUserId(strUserId);
    }
}

bool ACD_ProcessAfterLogin()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return false;
    }

    return g_ACDModel->ProcessAfterLogin();
}

//设置初始化状态
bool ACD_SetInitStatus(ACD_STATUS eStatus)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return false;
    }

    return ((ACDModel *)g_ACDModel)->SetInitStatus(eStatus);
}

//获取初始登陆状态
ACD_STATUS ACD_GetInitStatus()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return AS_IDLE;
    }

    return ((ACDModel *)g_ACDModel)->GetInitStatus();
}

//获取UnAvailable Reason List
bool ACD_GetReasonMap(DispCodeList & listData)
{
    return g_ACDModel->GetReasonCode(listData);
}

//设置Unavailable Reason
void ACD_SetCurReason(const yl::string & strReason)
{
    if (g_ACDModel == NULL)
    {
        return;
    }

    if (g_ACDModel->Get_ACDType() == ACD_BROADSOFT
#if IF_FEATURE_METASWITCH_ACD
            || g_ACDModel->Get_ACDType() == ACD_METASWITCH
#endif
       )
    {
        ((ACDModel *)g_ACDModel)->SetCurReason(strReason);
    }
#if IF_FEATURE_START2START_ACD
    else if (g_ACDModel && g_ACDModel->Get_ACDType() == ACD_START2START)
    {
        ((CACDModelStart2Start *)g_ACDModel)->SetCurReason(strReason);
    }
#endif
}

void ACD_GetDispCodeList(DispCodeList & refList)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return;
    }

    ((ACDModel *)g_ACDModel)->GetDispCodeList(refList);
}

void ACD_GetSupervisorList(SupervisorList & refList)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return;
    }

    ((ACDModel *)g_ACDModel)->GetSupervisorList(refList);
}

//设置当前Call Center信息
bool ACD_SetCurCallInfo(yl::string & strXmlInfo, int nCallId)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return false;
    }

    return ((ACDModel *)g_ACDModel)->SetCurCallInfo(strXmlInfo, nCallId);
}

//返回Call Center信息
const CallCenterInfo & ACD_GetCallInfo()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        static CallCenterInfo tInfo;
        return tInfo;
    }

    return ((ACDModel *)g_ACDModel)->GetCallInfo();
}

//返回队列状态
const CallCenterQueueStatus & ACD_GetQueueStatus()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        static CallCenterQueueStatus tStatus;
        return tStatus;
    }

    return ((ACDModel *)g_ACDModel)->GetQueueStatus();
}

//获取CallInfo显示时间
int CallCenter_GetCallInfoShowTime()
{
    if (g_CallCenterController == NULL)
    {
        return -1;
    }
    return g_CallCenterController->GetCallInfoShowTime();
}

//返回Account iD信息
int Hoteling_GetAccountId()
{
    return g_HotelingModel == NULL ? -1 : g_HotelingModel->GetAccountId();
}

//获取hoteling显示名字
void Hoteling_GetDisplayName(yl::string & strName)
{
    if (g_HotelingModel == NULL)
    {
        strName = "";
        return;
    }
    strName = g_HotelingModel->GetDisPlayName();

}

//获取不可用原因code
void ACD_GetUnavailCode(yl::string & strCodeName)
{

    if (g_ACDModel == NULL)
    {
        return;
    }

    if (g_ACDModel->Get_ACDType() == ACD_BROADSOFT
#if IF_FEATURE_METASWITCH_ACD
            || g_ACDModel->Get_ACDType() == ACD_METASWITCH
#endif
       )
    {
        ((ACDModel *)g_ACDModel)->GetCurReason(strCodeName);
    }
#if IF_FEATURE_START2START_ACD
    else if (g_ACDModel && g_ACDModel->Get_ACDType() == ACD_START2START)
    {
        ((CACDModelStart2Start *)g_ACDModel)->GetCurReason(strCodeName);
    }
#endif
}

//ACD Call info功能是否开启
bool CallCenter_IsCallInfoOn()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->IsCallInfoOn();
}

int ACD_GetCallInfoCallId()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_BROADSOFT)
    {
        return -1;
    }

    return ((ACDModel *)g_ACDModel)->GetCallInfoCallerId();
}

//队列状态灯是否开启
bool CallCenter_IsEnableQueueStatusLightOn()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->IsEnableQueueStatusLightOn();
}

//队列状态是否开启
bool CallCenter_IsEnableQueueStatus()
{
    return g_CallCenterController == NULL ?
           false : g_CallCenterController->IsQueueStatusOn();
}

bool CallCenter_IsUnavailReasonCodeOn()
{
    return g_ACDModel->IsUnavailReasonCodeOn();
}

bool ACD_IsGensACD()
{
#if IF_BUG_30788
    ACD_TYPE eType = (ACD_TYPE)configParser_GetConfigInt(kszAcdType);
    if (eType == ACD_GENESYS)
#else
    SIP_SERVER_TYPE eType = (SIP_SERVER_TYPE)configParser_GetCfgItemIntValue(kszAccountSIPServerType,
                            ACD_GetAccountId());
    if (eType == SST_GENESYS || eType == SST_GENESYS_ADV)
#endif
    {
        return true;
    }

    return false;
}

yl::string ACD_GetACDLoginName()
{
    return g_ACDModel == NULL ? "" : g_ACDModel->GetLoginName();
}

Dsskey_Status ACD_GetDsskeyStatus(ACD_STATUS eAcdStatus)
{
    switch (eAcdStatus)
    {
    case AS_IDLE:
        return DS_ACD_IDLE;
#if IF_BUG_24860
    case AS_AVAILABLE:
    case AS_UNAVAILABLE:
    case AS_WRAPUP:
        {
            return DS_ACD_AVAILABLE;
        }
#else
    case AS_AVAILABLE:
        return DS_ACD_AVAILABLE;
    case AS_UNAVAILABLE:
        return DS_ACD_UNAVAILABLE;
    case AS_WRAPUP:
        return DS_ACD_WRAPUP;
#endif
    default:
        return DS_ACD_IDLE;
    }
}

#else
int CallCenter_GetCallInfoShowTime()
{
    return -1;
}

bool ACD_ProcessAfterLogin()
{
    return false;
}

#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER


#if IF_FEATURE_START2START_ACD
void ACD_SetAcdStatus(ACD_STATUS eNewStatus)
{
    if (g_ACDModel)
    {
        g_ACDModel->SetStatus(eNewStatus);
    }
}

void ACD_SetSessionID(const yl::string & strSessionid)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_START2START)
    {
        // 其它类型不开放此接口
        return;
    }

    g_ACDModel->SetSessionID(strSessionid);
}

yl::string CallCenter_GetNextCallOutParam()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_START2START)
    {
        // 其它类型不开放此接口
        return yl::string();
    }

    return ((CACDModelStart2Start *)g_ACDModel)->GetNextCallOutParam();
}

int CallCenter_GetNextInterval()
{
    yl::string strNextParam = CallCenter_GetNextCallOutParam();

    if (strNextParam.empty())
    {
        return -1;
    }

    // 需求要求兼顾浮点数
    double interval = atof(strNextParam.c_str());

    if (interval > 20 || interval < 1)
    {
        // 合法区间 [1, 20]
        interval = 1;
    }

    return ((int)interval * 1000);
}

void CallCenter_SetAutoRefreshTime(unsigned long iTimer)
{
    if (g_ACDModel && g_ACDModel->Get_ACDType() == ACD_START2START)
    {
        ((CACDModelStart2Start *)g_ACDModel)->ReSetTimer(iTimer);
    }
}
#endif  //IF_FEATURE_START2START_ACD

#else
//ACD和Hoteling登陆接口
bool CallCenter_Login(ACDLogin & refData)
{
    return false;
}

//ACD和Hoteling登出接口
bool CallCenter_Logout()
{
    return false;
}

bool ACD_ReqChangeACDStatus(ACD_ACTION eACDAction)
{
    return false;
}

bool ACD_ReqLogin()
{
    return false;
}

HOTELING_STATUS CallCenter_GetHotelingStatus()
{
    return HOTELING_STATUS_NONE;
}

//像服务器发送追踪信令
bool CallCenter_OriginatedTrace(int nCallId)
{
    return false;
}

bool CallCenter_SendDispCode(const yl::string & strCode, int nCallId)
{
    return false;
}

bool CallCenter_EmergencyEscalation(const yl::string & strNumber, int nCallId)
{
    return false;
}

bool CallCenter_IsACDOn()
{
    return false;
}

bool CallCenter_IsHotelingOn()
{
    return false;
}

bool CallCenter_IsFlexibleSeatingOn(int iAccountId/* = -1*/)
{
    return false;
}

bool CallCenter_EnableGuestLock()
{
    return false;
}

bool CallCenter_IsGuestLock()
{
    return false;
}

bool CallCenter_IsMatchGuestLockPin(const yl::string & strUnlockPin)
{
    return false;
}

void CallCenter_GuestLock(bool bLock)
{
}

void CallCenter_ClearFlexibleSeating()
{
}

bool CallCenter_IsTraceOn()
{
    return false;
}

bool CallCenter_IsDispCodeOn()
{
    return false;
}

bool CallCenter_IsEmergencyEscalationOn()
{
    return false;
}

bool Hoteling_IsAutoLogin()
{
    return false;
}

void Hoteling_GetUserId(yl::string & strUserId)
{
    return;
}

//设置初始化状态
bool ACD_SetInitStatus(ACD_STATUS eStatus)
{

    return false;
}

//获取初始登陆状态
ACD_STATUS ACD_GetInitStatus()
{
    return AS_IDLE;
}

//获取ACD当前状态
ACD_STATUS ACD_GetAcdStatus()
{
    return AS_IDLE;
}

//获取UnAvailable Reason List
bool ACD_GetReasonMap(DispCodeList & listData);
{
}

//设置Unavailable Reason
void ACD_SetCurReason(const yl::string & strReason)
{
    return;
}

void ACD_GetDispCodeList(DispCodeList & refList)
{
    return;
}

void ACD_GetSupervisorList(SupervisorList & refList)
{
    return;
}

//设置当前Call Center信息
bool ACD_SetCurCallInfo(yl::string & strXmlInfo, int nCallId)
{
    return false;
}

//返回Call Center信息
const CallCenterInfo & ACD_GetCallInfo()
{
    static CallCenterInfo t_CallCenterInfo;
    return t_CallCenterInfo;
}

//返回队列状态
const CallCenterQueueStatus & ACD_GetQueueStatus()
{
    static CallCenterQueueStatus t_CallCenterQueueStatus;
    return t_CallCenterQueueStatus;
}

//获取CallInfo显示时间
int CallCenter_GetCallInfoShowTime()
{
    return -1;
}

//返回Account iD信息
int ACD_GetAccountId()
{
    return -1;
}

//返回Account iD信息
int Hoteling_GetAccountId()
{
    return -1;
}

//获取hoteling显示名字
void Hoteling_GetDisplayName(yl::string & strName)
{
    return;
}

//获取不可用原因code
void ACD_GetUnavailCode(yl::string & strCodeName)
{
    return;
}

//ACD Call info功能是否开启
bool CallCenter_IsCallInfoOn()
{
    return false;
}

int ACD_GetCallInfoCallId()
{
    return -1;
}

//队列状态灯是否开启
bool CallCenter_IsEnableQueueStatusLightOn()
{
    return false;
}

//队列状态是否开启
bool CallCenter_IsEnableQueueStatus()
{
    return false;
}

bool CallCenter_IsUnavailReasonCodeOn()
{
    return false;
}

// 返回ACD类型
ACD_TYPE Get_ACDType()
{
    return ACD_BASE;
}


bool ACD_ProcessAfterLogin()
{
    return false;
}

yl::string ACD_GetACDLoginName()
{
    return "";
}

Dsskey_Status ACD_GetDsskeyStatus(ACD_STATUS eAcdStatus)
{
    return DS_ACD_IDLE;
}
#endif

#if IF_FEATURE_METASWITCH_ACD
bool MTSWACD_DownloadData(MtswActionType eDownloadType, const yl::string & strMlhgName /*= ""*/)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->DownloadData(eDownloadType, strMlhgName);
}

bool MTSWACD_GetMLHGList(YLVector<SMLHGData> & vecData)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->GetMLHGList(vecData);
}

bool MTSWACD_BeHasPreminumAcd()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->BeHasPreminumAcd();
}

bool MTSWACD_GetUnavailReasonList(YLList<yl::string> & listUnavailReason)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->GetUnavailReasonList(listUnavailReason);
}

yl::string MTSWACD_GetCurUnavailReason()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }
    yl::string strReason;
    ((CACDModelMTSW *)g_ACDModel)->GetCurReason(strReason);

    return strReason;
}

const SMLGHQueuesData * MTSWACD_GetCurQueuesData(yl::string strMlhgName)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->GetQueuesData(strMlhgName);
}

bool MTSWACD_SetCurrentMLHG(yl::string strMlhgName)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->SetCurrentMLHG(strMlhgName);
}

yl::string MTSWACD_GetCurrentMLHG()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->GetCurrentMLHG();
}

bool MTSWACD_GetMLHGMemberList(YLVector<SMLHGMemberData> & vecData, yl::string strMlhgName)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->GetMLHGMemberList(vecData, strMlhgName);
}

bool MTSWACD_IsAllowMultipleLogin()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->IsAllowMultipleLogin();
}

bool MTSWACD_GetDispCodeList(YLVector<yl::string> & vecData, yl::string strMlhgName)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->GetDispositionCodeList(vecData, strMlhgName);
}

bool MTSWACD_SetDispCode(yl::string strCode, yl::string strMlhgName)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->SetDispCode(strCode, strMlhgName);
}

bool MTSWACD_IsDispCodeRequired(yl::string strMlhgName /*= ""*/)
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->IsDispCodeRequired(strMlhgName);
}

bool MTSWACD_IsEnableShowState()
{
    bool bIsAllLogout = true;
    YLVector<SMLHGData> vecMLGHData;
    MTSWACD_GetMLHGList(vecMLGHData);
    for (int i = 0 ; i < vecMLGHData.size(); i++)
    {
        if (vecMLGHData[i].IsLogin())
        {
            bIsAllLogout = false;
            break;
        }
    }

    return !bIsAllLogout && MTSWACD_BeHasPreminumAcd();
}

yl::string MTSWACD_GetLastAnseredMHLG()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->GetLastAnseredMHLG();
}

bool MTSWACD_IsAllMLHGLogout()
{
    if (NULL == g_ACDModel
            || g_ACDModel->Get_ACDType() != ACD_METASWITCH)
    {
        return false;
    }

    return ((CACDModelMTSW *)g_ACDModel)->IsAllMLHGLogout();
}
#endif
