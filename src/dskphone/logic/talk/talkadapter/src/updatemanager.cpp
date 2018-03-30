#include "talkadapter_inc.h"
#include "talk/talklogic/src/aocmanager.h"
#include "talk/talklogic/src/bttalkmanager.h"
#include "talk/talklogic/src/autoredialmanager.h"

bool CTalkUIDataManager::GetIncomingCallName(yl::string& strNum)
{
    CSessionProxy proxy = talklogic_GetIncomingCallSessionId();
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (pCallInfo == NULL)
    {
        return false;
    }

    switch (configParser_GetConfigInt(kszDisplayMethod))
    {
    case SHOW_NAME_AND_NUMBER:
    case SHOW_ONLY_NAME:
        {
            if (pCallInfo->m_tRemoteInfo.sDisplayName.empty())
            {
                strNum = pCallInfo->m_tRemoteInfo.sSIPName;
            }
            else
            {
                strNum = pCallInfo->m_tRemoteInfo.sDisplayName;
            }
        }
        break;
    case SHOW_NUMBER_AND_NAME:
    case SHOW_ONLY_NUMBER:
        {
            strNum = pCallInfo->m_tRemoteInfo.sSIPName;
        }
        break;
    case SHOW_FULL_CONTACT_INFO:
        {
            strNum = pCallInfo->m_tRemoteInfo.sFullContactInfo;
        }
        break;
#if IF_BUG_29709
    case SHOW_NULL:
        {
            if ("anonymous" != pCallInfo->m_tRemoteInfo.sSIPName.MakeLower()
                    && "private number" != pCallInfo->m_tRemoteInfo.sSIPName.MakeLower())
            {
                strNum = " ";
            }
        }
        break;
#endif
    default:
        {
            if (pCallInfo->m_tRemoteInfo.sDisplayName.empty())
            {
                strNum = pCallInfo->m_tRemoteInfo.sSIPName;
            }
            else
            {
                strNum = pCallInfo->m_tRemoteInfo.sDisplayName;
            }
        }
        break;
    }

#if IF_BUG_27560
    if (pCallInfo->m_bHideInfo && strNum == pCallInfo->m_tRemoteInfo.sSIPName)
    {
        strNum = "******";
    }
#endif

    return true;
}

bool CTalkUIDataManager::GetDataBySessionID(const CSessionStateProxy& proxy, DataLogic2UI& logicData, bool bBaseData /*= false*/)
{
    if (!proxy || proxy.IsHide())
    {
        return false;
    }

    AUTO_SCOPED_TIMER_NAME_ID(proxy.Name(), logicData.iSessionID);
    CListenInfo* priv = modtklsn_get_privdata(proxy);

    logicData.iSessionID = proxy;
    logicData.eSessionState = proxy.GetSessionState();
    DataLogic2UIBase* pData = NULL;

    switch (logicData.eSessionState)
    {
    case SESSION_BETRANSFERRED:
        {
            pData = GetBetransSessionData(proxy, priv, bBaseData);
        }
        break;
    case SESSION_LOCALCONF:
        {
            pData = GetLocalConfSessionData(proxy, priv, bBaseData);
        }
        break;
    default:
        {
            switch (proxy.GetInheritsRoutine())
            {
            case ROUTINE_TALKING:
                {
                    pData = GetTalkRoutineData(proxy, priv, bBaseData);
                }
                break;
            case ROUTINE_DIALING:
                {
                    pData = GetDialRoutineData(proxy, priv, bBaseData);
                }
                break;
            case ROUTINE_RINGING:
                {
                    pData = GetRingRoutineData(proxy, priv, bBaseData);
                }
                break;
            case ROUTINE_CONNECTING:
                {
                    pData = GetConnectRoutineData(proxy, priv, bBaseData);
                }
                break;
            case ROUTINE_FINISHED:
                {
                    pData = GetFinishRoutineData(proxy, priv, bBaseData);
                }
                break;
            case ROUTINE_PARK_NOTIFY:
                {
                    pData = GetParkNotifyRoutineData(proxy, priv, bBaseData);
                }
                break;
            default:
                {
                    pData = proxy.GetCallInfo();
                }
                break;
            }
        }
        break;
    }

    logicData.pData = pData;
    SignalParams sigp(proxy);
    sigp.wParam.pValue = pData;
    return modtklsn_emit_sync(TALK_SIG_GET_DATA_TO_UI, sigp);
}

DataLogic2UIBase* CTalkUIDataManager::GetBetransSessionData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    DataTalking2UI* data2UI = GetTalkData(proxy);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (data2UI == NULL || pCallInfo == NULL)
    {
        return NULL;
    }

    data2UI->bAllowTran = false;
    data2UI->bAllowConf = false;

    data2UI->pCallInfo = pCallInfo;
    // 通话界面是否SRTP加密
    data2UI->bEncrypt = pCallInfo->IsSRTPEncrypt();

    data2UI->bAllowHold = !pCallInfo->IsCallMask(CALL_FEATURE_ICE_CALL);

    if (proxy.IsInherits(ROUTINE_CONNECTING))
    {
        data2UI->bAllowNewCall = proxy.IsAllowNewCall();
    }

    return data2UI;
}

DataLogic2UIBase* CTalkUIDataManager::GetLocalConfSessionData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    YLVector<CSessionProxy> vec;
    if (!proxy.GetSubSession(vec))
    {
        return NULL;
    }

    DataLocalConf* data2UI = GetConfData(proxy);
    if (data2UI == NULL)
    {
        return NULL;
    }

    data2UI->bEncrypt = true;
    data2UI->bInConfManager = proxy.IsConfManager();
    data2UI->bHoldConf = proxy.IsHold();

    bool bAllowConf = true;
    int nAccountId = INVALID_ACCOUNTID;

    data2UI->listCallInfo.clear();
    for (YLVector<CSessionProxy>::iterator it = vec.begin(); it != vec.end(); ++it)
    {
        CSessionStateProxy subProxy(*it);
        if (!subProxy.IsSingle())
        {
            continue;
        }

        CCallInfo* pCallInfo = subProxy.GetCallInfo();
        if (pCallInfo == NULL)
        {
            continue;
        }

        data2UI->listCallInfo.push_back(pCallInfo);

        if (bBaseData)
        {
            continue;
        }

#ifdef IF_SUPPORT_FIVE_PARTY
        // 5方会议才会有会议中，按conf的功能
        if (subProxy.IsInherits(ROUTINE_TALKING))
        {
            bAllowConf &= subProxy.IsAllowCreateConf();
        }
#endif //IF_SUPPORT_FIVE_PARTY

        // 所有通话加密才认为是加密状态
        data2UI->bEncrypt &= pCallInfo->IsSRTPEncrypt();

        int iSubAccountID = pCallInfo->GetAccountID();
        if (nAccountId == INVALID_ACCOUNTID)
        {
            nAccountId = pCallInfo->GetAccountID();
        }

        data2UI->bAllowSecurity |= talklogic_IsShowSecurity(iSubAccountID);
    }

    // 取该会议账号的id即可
    data2UI->bAllowNewCall = proxy.IsAllowNewCall(nAccountId);
    data2UI->bAllowSplit = data2UI->bAllowNewCall || acc_IsBLAEnable(nAccountId);

#ifdef IF_SUPPORT_FIVE_PARTY
    // 5方会议才会有会议中，按conf的功能
    bool bAllowCreate = proxy.IsAllowCreateLocalConf();
    TALK_INFO("AllowConf[%d] session Allow Conf[%d]", bAllowConf, bAllowCreate);

    data2UI->bAllowConf = bAllowConf && bAllowCreate;
#endif // IF_SUPPORT_FIVE_PARTY

    return data2UI;
}

DataLogic2UIBase* CTalkUIDataManager::GetDialRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    DataDial* data2UI = GetDialData(proxy);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (data2UI == NULL || pCallInfo == NULL)
    {
        return NULL;
    }

    data2UI->eDialUIType = proxy.GetDialUIType();
    data2UI->strNumber = pCallInfo->GetDialText();
    data2UI->pCallInfo = pCallInfo;
    // 拨号界面是否TLS加密
    data2UI->bEncrypt = pCallInfo->IsTLSEncrypt();
    data2UI->iKeyCode = pCallInfo->m_iDigitKey;

#if IF_FEATURE_DIAL_ON_HOLD
    data2UI->bAllowTran = false;
    if (proxy.GetTranSponsor())
    {
        data2UI->bAllowTran = proxy.IsAllowTran();
    }
#endif

    if (proxy.IsInPreConf())
    {
        proxy.AddConfToTarget(data2UI->listConfCallInfo);
    }
    else if (proxy.IsInPreTrans())
    {
        // 添加穿梭的目标信息
        proxy.AddTransferToTarget(data2UI->listTranCallInfo);
    }

    return data2UI;
}

DataLogic2UIBase* CTalkUIDataManager::GetTalkRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    DataTalking2UI* data2UI = GetTalkData(proxy);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (data2UI == NULL || pCallInfo == NULL || priv == NULL)
    {
        return NULL;
    }

#ifdef IF_SUPPORT_LINE_DEVICE
    if (proxy.IsInherits(ROUTINE_LINE_DEVICE))
    {
        data2UI->eTalkUIType = TALKING_UI_LINEDEVICE;
        data2UI->bAllowTran = FALSE;
        data2UI->bAllowConf = proxy.IsAllowCreateConf();
        data2UI->bAllowSwap = proxy.IsAllowSwap();
        data2UI->bTranFailed = FALSE;
        // 呼出界面不加密
        data2UI->bEncrypt = false;

        return data2UI;
    }
#endif
#ifdef IF_BT_SUPPORT_PHONE
    if (pCallInfo->IsBtTalk())
    {
        data2UI->eTalkUIType = TALKING_UI_NORMAL;
        data2UI->bAllowTran = proxy.IsAllowTran();
        data2UI->bAllowConf = false;//蓝牙手机界面 不允许建立会议
        data2UI->bAllowSwap = proxy.IsAllowSwap();
        data2UI->pCallInfo = pCallInfo;
        data2UI->bAllowTranToAnother = false;
        data2UI->bOneSession = false;
        data2UI->bAllowNewCall = pCallInfo->IsLocalHold() && _BTMobileManager.IsAllowNewBTCall();
        data2UI->bChannelLinked = _BTMobileManager.IsChannelLinked();
        return data2UI;
    }
#endif

    data2UI->pCallInfo = pCallInfo;

    data2UI->bAllowHold = proxy.IsAllowHold();
    data2UI->bAllowTran = proxy.IsAllowTran();
    data2UI->bAllowConf = proxy.IsAllowConf();
    data2UI->bAllowSwap = proxy.IsAllowSwap();
    data2UI->bAllowNewCall = proxy.IsAllowNewCall();
    data2UI->bEncrypt = pCallInfo->IsSRTPEncrypt();
    data2UI->bAllowSecurity = talklogic_IsShowSecurity(pCallInfo->GetAccountID());
    data2UI->bAllowJoin = proxy.IsAllowJoinLocalConf();
    data2UI->bInConfManager = pCallInfo->IsConfManager();

    if (priv != NULL)
    {
        data2UI->bTranFailed = priv->IsTransFailed();
        data2UI->strConfName = priv->m_strConfName;
        data2UI->listNetUserInfo = priv->m_listNetUserInfo;
        data2UI->bCallParkFailed = priv->m_bCallParkFailed;
        data2UI->bCallPushFailed = priv->m_bCallPushFailed;
        data2UI->strParkFiled = priv->m_strParkFiled;
    }

    if (proxy.IsInPreTrans())
    {
        data2UI->bAllowTranToAnother = false;
    }
    else
    {
        // 这个条件是直接转移的条件
        // 是否是两路，且支持直接TRANSFER(目前只有TCT_TIPTEL_AUTO_CALLOUT支持)
        if (proxy.IsTransferDirectly())
        {
            data2UI->bAllowTranToAnother = false;
        }
        else
        {
            data2UI->bAllowTranToAnother = (proxy.GetNumberOfAllowTranTo() >= 1
                                            && configParser_GetConfigInt(kszAllowTransExistCall) != 0);
        }
    }

    if (devicelib_GetPhoneType() == PT_T19)
    {
        data2UI->bOneSession = (1 == talklogic_GetSessionNum());
    }

    if (proxy.IsInherits(ROUTINE_NETWORKCONF))
    {
        data2UI->eTalkUIType = TALKING_UI_NETWORKCONF;

        if (data2UI->bAllowConf && DSK_PHONE_TYPE_BLACK == commonUnits_GetDskPhoneType())
        {
            //存在可以加入conf的话路，并且bConfbyNewDial为true或者话路超出会议最大支持数
            if (proxy.IsExistSessionToConf()
                    && ((configParser_GetConfigInt(kszConfWithPreviousCall) == 0)
                        || talklogic_IsAllowAddLocalConfNumOverMax()))
            {
                data2UI->bAllowConfWithAnother = true;
            }
            else
            {
                data2UI->bAllowConfWithAnother = false;
            }
        }

        return data2UI;
    }

    data2UI->bShowAoc = _AOCManager.IsAocShow(pCallInfo->GetCallID());

    if (proxy.IsInPreTrans())
    {
        data2UI->eTalkUIType = TALKING_UI_TRANSFERRING;
    }
    else
    {
        data2UI->eTalkUIType = pCallInfo->IsTransferred() ? TALKING_UI_TRANSFERRED : TALKING_UI_NORMAL;
    }

    if ((priv && priv->IsPrePark()) || priv->IsPreGPark())
    {
        DataDial& DialData = pCallInfo->GetDialData();

        DialData.eDialUIType = proxy.GetDialUIType();
        DialData.pCallInfo = pCallInfo;
        data2UI->pDialData = &DialData;

        if (priv->IsPrePark())
        {
            DialData.strNumber = pCallInfo->GetDialText();
        }
    }
    else
    {
        data2UI->pDialData = NULL;
    }

    TALK_INFO("NewCall[%d] conf[%d] hold[%d]", data2UI->bAllowNewCall,
              data2UI->bAllowConf, data2UI->bAllowHold);

    if (data2UI->bAllowConf && DSK_PHONE_TYPE_BLACK == commonUnits_GetDskPhoneType())
    {
        data2UI->bAllowConfWithAnother = proxy.IsAllowConfWithAnother();
    }

    return data2UI;
}

DataLogic2UIBase* CTalkUIDataManager::GetRingRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    DataRing* data2UI = GetRingData(proxy);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (data2UI == NULL || pCallInfo == NULL || priv == NULL)
    {
        return NULL;
    }

#ifdef IF_FEATURE_EMERGENCY
    data2UI->bEmergencyCalled = proxy.IsEmergencyCalled();
#endif
    data2UI->pCallInfo = pCallInfo;

    if (pCallInfo->IsPreForword())
    {
        data2UI->bShowSilence = pCallInfo->IsRingMute();
        data2UI->bShowCallCenterInfo = false;
        data2UI->pCCInfo = NULL;

        DataDial& DialData = pCallInfo->GetDialData();
        DialData.eDialUIType = proxy.GetDialUIType();
        DialData.strNumber = pCallInfo->GetDialText();
        DialData.pCallInfo = pCallInfo;
        data2UI->pDialData = &DialData;
        data2UI->bAllowJoin = proxy.IsAllowJoinLocalConf();
    }
    else if (priv->IsCCShow())
    {
        data2UI->bShowCallCenterInfo = true;
        data2UI->pCCInfo = (CallCenterInfo*)&pCallInfo->GetCCInfo();
        data2UI->pDialData = NULL;
    }
    else
    {
        data2UI->bShowCallCenterInfo = false;
        data2UI->pCCInfo = NULL;
        data2UI->pDialData = NULL;
#if IF_FEATURE_LOOPBACK
        data2UI->bShowSilence = pCallInfo->IsRingMute() || pCallInfo->m_bIsLoopBackCall;
#else
        data2UI->bShowSilence = pCallInfo->IsRingMute();
#endif
        data2UI->bCallDeclineEnable = proxy.IsCallDeclineEnable();
        data2UI->bAllowFwd = proxy.IsAllowFwd();
        data2UI->bAllowJoin = proxy.IsAllowJoinLocalConf();
    }

    return data2UI;
}

DataLogic2UIBase* CTalkUIDataManager::GetConnectRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    DataConnect* data2UI = GetConnectData(proxy);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (data2UI == NULL || pCallInfo == NULL)
    {
        return NULL;
    }

    data2UI->bConnectByTran = proxy.GetTranSponsor();
    data2UI->pCallInfo = pCallInfo;
    data2UI->bEncrypt = pCallInfo->IsTLSEncrypt();

    return data2UI;
}

DataLogic2UIBase* CTalkUIDataManager::GetFinishRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    DataCallFinished* data2UI = GetFinishData(proxy);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (data2UI == NULL || pCallInfo == NULL)
    {
        return NULL;
    }

    data2UI->pCallInfo = pCallInfo;

    if (proxy.GetRoutineState() == ROUTINE_AUTOREDIAL)
    {
        data2UI->bAllowAutoRedial = true;
        data2UI->bAllowNewCall = false;
        data2UI->redialData.eStatus = _AutoRedialManager.GetStatus();
        data2UI->redialData.iRedialInterval = _AutoRedialManager.GetRedialInterval();
        data2UI->redialData.iRemainRedialTimes = _AutoRedialManager.GetRemainRedialTimes();
    }
#ifdef IF_BT_SUPPORT_PHONE
    else if (pCallInfo->IsBtTalk())
    {
        data2UI->strInfo = pCallInfo->m_errorInfo.strError;

        // 是否非法账号错误
        if (data2UI->strInfo == TRID_INVALID_ACCOUNT)
        {
            data2UI->bInvalidAccount = true;
        }
        else if (data2UI->strInfo == "MaxSessionError")
        {
            data2UI->bMaxSessionError = true;
            data2UI->strInfo = "Sorry,you can only make two calls!";
        }

        if (talklogic_IsSessionExist(ROUTINE_CONNECTING)
                || talklogic_IsSessionExist(ROUTINE_RINGING)
                || data2UI->strInfo == (TRID_SHARE_LINE_UNAVAILABLE))
        {
            data2UI->bAllowNewCall = false;
        }
    }
#endif
    else
    {
        const TalkErrorInfo& errorInfo = pCallInfo->m_errorInfo;
        data2UI->strInfo = errorInfo.strError;

        if (errorInfo.iReason != TARK_ERROR_CALL_RELEASED)
        {
            data2UI->bError = true;
        }
        // 是否非法账号错误
        if (errorInfo.iReason == TALK_ERROR_CALLOUT_IA)
        {
            data2UI->bInvalidAccount = true;
        }
        else if (errorInfo.iReason == TALK_ERROR_CALLOUT_MSE)
        {
            data2UI->bMaxSessionError = true;
            data2UI->strInfo = TRID_YOU_CAN_MAKE_TWO_CALLS_AT_MOST;
        }

        if (errorInfo.iReason == TALK_ERROR_CALLOUT_SLU
                || talklogic_IsConnectingSessionExist()
                || talklogic_IsRingSessionExist()
                || (!netIsNetworkReady()
#ifdef IF_FEATURE_PSTN
                    && acc_GetUsableAccount(true, ACCOUNT_TYPE_PSTN) != INVALID_ACCOUNTID
#endif // IF_FEATURE_PSTN
                   )
                || !proxy.IsAllowNewCall())
        {
            data2UI->bAllowNewCall = false;
        }

        data2UI->strInfo = Comm_GetCodeShowInfo(errorInfo.iReason, errorInfo.strError.c_str());
    }

    return data2UI;
}

DataLogic2UIBase* CTalkUIDataManager::GetParkNotifyRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData)
{
    DataParkNotify* data2UI = GetParkNotifyData(proxy);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (data2UI == NULL || pCallInfo == NULL)
    {
        return NULL;
    }

    data2UI->strDiaplayName = pCallInfo->m_strDiaplayName;
    data2UI->strNumber = pCallInfo->m_strNumber;
    return data2UI;
}

void CTalkUIDataManager::OnSessionDestory(int iSessionID)
{
    MAP_SESSION_DATA::iterator it = m_mapData2UI.find(iSessionID);
    if (it != m_mapData2UI.end())
    {
        delete it->second;
        m_mapData2UI.erase(it);
    }
}

DataTalking2UI* CTalkUIDataManager::GetTalkData(const CSessionStateProxy& proxy)
{
    if (!proxy.IsInherits(ROUTINE_TALKING))
    {
        return NULL;
    }

    return &GetDataImpl<DataTalking2UI>(proxy);
}

DataLocalConf* CTalkUIDataManager::GetConfData(const CSessionStateProxy& proxy)
{
    if (!proxy.IsLocalConf())
    {
        return NULL;
    }

    return &GetDataImpl<DataLocalConf>(proxy);
}

DataDial* CTalkUIDataManager::GetDialData(const CSessionStateProxy& proxy)
{
    if (!proxy.IsInherits(ROUTINE_DIALING))
    {
        return NULL;
    }

    return &GetDataImpl<DataDial>(proxy);
}

DataRing* CTalkUIDataManager::GetRingData(const CSessionStateProxy& proxy)
{
    if (!proxy.IsInherits(ROUTINE_RINGING))
    {
        return NULL;
    }

    return &GetDataImpl<DataRing>(proxy);
}

DataConnect* CTalkUIDataManager::GetConnectData(const CSessionStateProxy& proxy)
{
    if (!proxy.IsInherits(ROUTINE_CONNECTING))
    {
        return NULL;
    }

    return &GetDataImpl<DataConnect>(proxy);
}

DataCallFinished* CTalkUIDataManager::GetFinishData(const CSessionStateProxy& proxy)
{
    if (!proxy.IsInherits(ROUTINE_FINISHED))
    {
        return NULL;
    }

    return &GetDataImpl<DataCallFinished>(proxy);
}

DataParkNotify* CTalkUIDataManager::GetParkNotifyData(const CSessionStateProxy& proxy)
{
    if (!proxy.IsInherits(ROUTINE_PARK_NOTIFY))
    {
        return NULL;
    }

    return &GetDataImpl<DataParkNotify>(proxy);
}

