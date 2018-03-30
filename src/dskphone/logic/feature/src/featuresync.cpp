#include "featuresync.h"

#include "account/include/modaccount.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "voip/modvoip.h"

#include "featurecommon.h"
#include "syncprocess.h"

#include "featurecode.h"

IMPLEMENT_GETINSTANCE(CFeatureSyncManager)

CFeatureSyncManager::CFeatureSyncManager()
{
    //m_bFeatureKeySync = false;
    //m_bDndSync = false;
    //m_bForwardSync = false;
#if IF_BUG_27027
    m_bFortinetDndSync = FALSE;
#endif
}

CFeatureSyncManager::~CFeatureSyncManager()
{
}

void CFeatureSyncManager::InitFeatureSync()
{
#if IF_BUG_27027
    m_bFortinetDndSync = (configParser_GetConfigInt(kszFortinetDnd) != 0);
    SingleMsgReg(SIP_SUB_FORTINET_DND_SUBSCRIBED_RESULT, CFeatureSyncManager::OnSIPMessage);
    SingleMsgReg(SIP_SUB_FORTINET_DND_NOTIFY_RESULT, CFeatureSyncManager::OnSIPMessage);
#endif
    // Phone模式下必须同步到所有账号,否则服务器状态和话机不同步
    SingleMsgReg(SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT, CFeatureSyncManager::OnSIPMessage);
    SingleMsgReg(SIP_SUB_UPDATE_FEATURE_KEY_STATUS, CFeatureSyncManager::OnSIPMessage);
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CFeatureSyncManager::OnConfigChange);
    // 重新加载配置
    ReloadConfig();
}

LRESULT CFeatureSyncManager::OnSIPMessage(msgObject & objMsg)
{
    BOOL bHandled = FALSE;

    switch (objMsg.message)
    {
    case SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT:
        {

            bHandled = TRUE;
            int iAccount = objMsg.wParam;
            bool bFeatureSync = _FeatureSyncMng.IsFeatureKeySync(iAccount);
            COMMONUNITS_INFO("feature key subscribe result: wapram[%d] lparam[%d] current sync state[%d]", objMsg.wParam, objMsg.lParam, bFeatureSync);

            // 同步开关打开的前提下才处理订阅的结果
            if (bFeatureSync
                    && acc_IsAccountAvailable(iAccount)) // 配置更新消息还没处理完之前,服务器下发订阅结果,会导致不处理
            {
                sip_reason_info_t * pResult = (sip_reason_info_t *)(objMsg.GetExtraData());
                if (NULL != pResult)
                {
                    COMMONUNITS_INFO("update account [%d] sync subscribe state to[%d]", pResult->code);
                    // 只有在账号注册上才处理
                    acc_SetSyncSubscribe(iAccount, pResult->code == 0);
                }
            }
        }
        break;
    case SIP_SUB_UPDATE_FEATURE_KEY_STATUS:
        {
            bHandled = TRUE;

            int iAccount = objMsg.wParam;
            COMMONUNITS_INFO("update account [%d ]feature key status, current account avaliable state[%d]", objMsg.wParam, objMsg.lParam, acc_IsAccountAvailable(iAccount));

            if (_FeatureSyncMng.IsFeatureKeySync(iAccount) && acc_IsAccountAvailable(iAccount))
            {
                LPCSTR strExtraData = (LPCSTR)objMsg.GetExtraData();
                if (!textEMPTY(strExtraData))
                {
                    _FeatureSyncMng.ParseXMLExtraData(strExtraData, objMsg.wParam);
                }
            }
        }
        break;
#if IF_BUG_27027
    case SIP_SUB_FORTINET_DND_SUBSCRIBED_RESULT:
        {
            bHandled = TRUE;

            // 同步开关打开的前提下才处理订阅的结果
            if (m_bFortinetDndSync) // 配置更新消息还没处理完之前,服务器下发订阅结果,会导致不处理
            {
                int iAccount = objMsg.wParam;
                if (acc_IsAccountAvailable(iAccount))
                {
                    // 只有在账号注册上才处理
                    acc_SetSyncSubscribe(iAccount, objMsg.lParam == 1);
                }
            }
        }
        break;
    case SIP_SUB_FORTINET_DND_NOTIFY_RESULT:
        {
            COMMONUNITS_INFO("CFeatureSyncManager::OnSIPMessage(SIP_SUB_FORTINET_DND_NOTIFY_RESULT) wParam[%d] lParam[%d]",
                             objMsg.wParam, objMsg.lParam);
            bHandled = TRUE;

            int iAccount = objMsg.wParam;
            if (acc_IsAccountIDValid(iAccount))
            {
                COMMONUNITS_INFO("CFeatureCode: iAccount:%d SyncSubscribe:%d, AccountEnable:%d, AccountState:%d",
                                 objMsg.wParam, acc_GetSyncSubscribe(iAccount), acc_IsAccountEnable(iAccount),
                                 acc_GetState(iAccount));
            }
            else
            {
                COMMONUNITS_WARN("CFeatureCode: iAccount[%d] not found!", iAccount);
            }

            if (acc_GetSyncSubscribe(iAccount)
                    && acc_IsAccountAvailable(iAccount))
            {
                // 同步到本地配置
                dnd_SyncDNDData(iAccount, objMsg.lParam == 1);
            }
        }
        break;
#endif
    default:
        break;
    }

    return bHandled;
}

LRESULT CFeatureSyncManager::OnConfigChange(msgObject & objMsg)
{
    if (ST_FEATURE_SYNC != objMsg.wParam && ST_DND_SYNC != objMsg.wParam)
    {
        return FALSE;
    }

    _FeatureSyncMng.ReloadConfig();

    ListAccounts listAccount = acc_GetAvailableAccountList(ACCOUNT_TYPE_SIP);
    for (ListAccounts::iterator it = listAccount.begin();
            it != listAccount.end();
            ++it)
    {
        bool bFeatureKeySync = _FeatureSyncMng.IsFeatureKeySync(*it);
#if IF_BUG_27027
        m_bFortinetDndSync = (configParser_GetConfigInt(kszFortinetDnd) != 0);
        COMMONUNITS_INFO("account [%d]  feature sync key [%d] m_bFortinetDndSync[%d]", *it,
                         bFeatureKeySync, m_bFortinetDndSync);

        if (!bFeatureKeySync && !m_bFortinetDndSync)
#else
        if (!bFeatureKeySync)
#endif
        {
            acc_SetSyncSubscribe(*it, false);
        }
    }

    return TRUE;
}

BOOL CFeatureSyncManager::ProcessDNDMsg(int iAccountId)
{
    if (iAccountId == FWDDND_ACCOUNT_GLOBAL)
    {
        bool bResult = true;
        // 全局DND状态改变需要通知每个账号
        ListAccounts accounts = acc_GetAvailableAccountList(ACCOUNT_TYPE_SIP);
        for (ListAccounts::iterator it = accounts.begin(); it != accounts.end(); ++it)
        {
            bResult = PostSyncStatusToServer(SFT_DND, *it) && bResult;
        }
        return bResult;
    }
    else
    {
        // 账号DND状态改变只需通知对应账号
        return PostSyncStatusToServer(SFT_DND, iAccountId);
    }
}

BOOL CFeatureSyncManager::ProcessAccountMsg(msgObject & objMsg)
{
    //状态改变才发Code
    if (objMsg.lParam != acc_GetState(objMsg.wParam))
    {
        SendAnonymousCode(objMsg.wParam);
        SendAnonymousRejectCode(objMsg.wParam);
        return TRUE;
    }

    return FALSE;
}

BOOL CFeatureSyncManager::ProcessFWDMsg(int iAccountId, FWD_TYPE eType)
{
    if (iAccountId == FWDDND_ACCOUNT_GLOBAL)
    {
        bool bResult = true;
        // 全局FWD状态改变需要通知每个账号
        ListAccounts accounts = acc_GetAvailableAccountList(ACCOUNT_TYPE_SIP);
        for (ListAccounts::iterator it = accounts.begin(); it != accounts.end(); ++it)
        {
            bResult = PostSyncStatusToServer(SFT_FWD, *it, &eType) && bResult;
        }
        return bResult;
    }
    else
    {
        // 账号FWD状态改变只需通知对应账号
        return PostSyncStatusToServer(SFT_FWD, iAccountId, &eType);
    }
}

void CFeatureSyncManager::SendAnonymousCode(int iAccount)
{
    if (!acc_IsAccountAvailable(iAccount))
    {
        return;
    }
    yl::string strValue = configParser_GetCfgItemStringValue(kszAnonymousCallServerBaseOnly, iAccount);
    CCodeInfo sCallInfo;
    sCallInfo.SetOnCode(configParser_GetCfgItemStringValue(kszAnonymousCallOnCode, iAccount).c_str());
    sCallInfo.SetOffCode(configParser_GetCfgItemStringValue(kszAnonymousCallOffCode, iAccount).c_str());

    COMMONUNITS_INFO("send anonymous code sever base only[%s] oncode[%s] offcode[%s]", strValue.c_str(),
                     sCallInfo.GetOnCode(), sCallInfo.GetOffCode());

    //没有配置时，保持旧体验
    if (strValue.empty())
    {
        if (configParser_GetCfgItemIntValue(kszSendAnonymousCode, iAccount) != 0)
        {
            // 开启匿名呼出,发送匿名呼叫code
            _FeatureCode.SendOnCode(iAccount, sCallInfo);
        }
        else
        {
            // 关闭匿名呼出,发送关闭匿名呼叫code
            _FeatureCode.SendOffCode(iAccount, sCallInfo);
        }
    }
    else
    {
        if (configParser_GetCfgItemIntValue(kszAnonymousCallSwitch, iAccount) != 0)
        {
            _FeatureCode.SendOnCode(iAccount, sCallInfo);
        }
        else
        {
            _FeatureCode.SendOffCode(iAccount, sCallInfo);
        }
    }
}

// 发送Anonymous reject code
void CFeatureSyncManager::SendAnonymousRejectCode(int iAccount)
{
    if (!acc_IsAccountAvailable(iAccount))
    {
        return;
    }

    CCodeInfo sCallInfo;
    yl::string strValue = configParser_GetCfgItemStringValue(kszAnonymousCallServerBaseOnly, iAccount);

    sCallInfo.SetOnCode(configParser_GetCfgItemStringValue(kszRejectAnonymousCallOnCode,
                        iAccount).c_str());
    sCallInfo.SetOffCode(configParser_GetCfgItemStringValue(kszRejectAnonymousCallOffCode,
                         iAccount).c_str());

    COMMONUNITS_INFO("send anonymous reject code sever base only[%s] oncode[%s] offcode[%s]",
                     strValue.c_str(), sCallInfo.GetOnCode(), sCallInfo.GetOffCode());
    if (strValue.empty())
    {
        if (configParser_GetCfgItemIntValue(kszAccountSendAnonymousRejection, iAccount) != 0)
        {
            // 开启匿名呼出,发送匿名呼叫code
            _FeatureCode.SendOnCode(iAccount, sCallInfo);
        }
        else
        {
            // 关闭匿名呼出,发送关闭匿名呼叫code
            _FeatureCode.SendOffCode(iAccount, sCallInfo);
        }
    }
    else
    {
        if (configParser_GetCfgItemIntValue(kszRejectAnonymousCallSwitch, iAccount) != 0)
        {
            _FeatureCode.SendOnCode(iAccount, sCallInfo);
        }
        else
        {
            _FeatureCode.SendOffCode(iAccount, sCallInfo);
        }
    }
}

bool CFeatureSyncManager::IsFeatureKeySync(int iAccountID/* = -1*/)
{
    // 内存中存储的值是根据规则计算出来的值，并不等于配置中的
    typeFeatureKeySyncMap::iterator it = m_mapFeatureKeyInfo.find(iAccountID);
    return it == m_mapFeatureKeyInfo.end() ? false : it->second.bFeatureKeySync;
}

bool CFeatureSyncManager::IsDNDSync(int iAccountID/* = -1*/)
{
    typeFeatureKeySyncMap::iterator it = m_mapFeatureKeyInfo.find(iAccountID);
    return it == m_mapFeatureKeyInfo.end() ? false : it->second.bDndSync;
}

bool CFeatureSyncManager::IsForwardSync(int iAccountID/* = -1*/)
{
    typeFeatureKeySyncMap::iterator it = m_mapFeatureKeyInfo.find(iAccountID);
    return it == m_mapFeatureKeyInfo.end() ? false : it->second.bForwardSync;
}

void CFeatureSyncManager::SetFeatureKeySync(bool bEnable, int iAccountID/* = -1*/)
{
    if (bEnable != IsFeatureKeySync(iAccountID))
    {
        if (iAccountID == -1)
        {
            configParser_SetConfigInt(kszBroadSoftFeatureSync, bEnable);
        }
        else
        {
            configParser_SetCfgItemIntValue(kszAccountFeatureKeySync, iAccountID, bEnable);
        }

        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_FEATURE_SYNC, 0);
        ReloadConfig();
    }
}

void CFeatureSyncManager::SetDNDSync(bool bEnable, int iAccountID/* = -1*/)
{
    if (bEnable != IsDNDSync(iAccountID))
    {
        if (iAccountID == -1)
        {
            configParser_SetConfigInt(kszDNDFeatureKeySync, bEnable);
        }
        else
        {
            configParser_SetCfgItemIntValue(kszAccountDNDFeatureKeySync, iAccountID, bEnable);
        }

        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_DND_SYNC, 0);
        ReloadConfig();
    }
}

void CFeatureSyncManager::SetForwardSync(bool bEnable, int iAccountID/* = -1*/)
{
    if (bEnable != IsForwardSync(iAccountID))
    {
        if (iAccountID == -1)
        {
            configParser_SetConfigInt(kszForwardFeatureKeySync, bEnable);
        }
        else
        {
            configParser_SetCfgItemIntValue(kszAccountForwardFeatureKeySync, iAccountID, bEnable);
        }

        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_FEATURE_SYNC, 0);
        ReloadConfig();
    }
}

bool CFeatureSyncManager::IsDNDLocalProcess(int iAccount)
{
    // 优先使用账号配置
    yl::string strLocalProcess = configParser_GetCfgItemStringValue(kszAccountDNDSyncLocalProcess,
                                 iAccount);
    if (strLocalProcess == "0")
    {
        return false;
    }
    else if (strLocalProcess == "1")
    {
        return true;
    }
    else
    {
        // 其次使用全局配置
        return configParser_GetConfigInt(kszDNDSyncLocalProcess);
    }
}

bool CFeatureSyncManager::IsForwardLocalProcess(int iAccount)
{
    // 优先使用账号配置
    yl::string strLocalProcess = configParser_GetCfgItemStringValue(kszAccountForwardSyncLocalProcess,
                                 iAccount);
    if (strLocalProcess == "0")
    {
        return false;
    }
    else if (strLocalProcess == "1")
    {
        return true;
    }
    else
    {
        // 其次使用全局配置
        return configParser_GetConfigInt(kszForwardSyncLocalProcess);
    }
}

void CFeatureSyncManager::ReloadConfig()
{
    // 加载全局配置
    FeatureSyncInfo syncInfo;
    syncInfo.bFeatureKeySync = configParser_GetConfigInt(kszBroadSoftFeatureSync) == 1;
    syncInfo.bDndSync = configParser_GetConfigInt(kszDNDFeatureKeySync) == 1;
    syncInfo.bForwardSync = configParser_GetConfigInt(kszForwardFeatureKeySync) == 1;

    bool bUpdateStatus = false;

    //加载账号配置
    ListAccounts listAccount = acc_GetAccountList(ACCOUNT_TYPE_SIP);
    for (ListAccounts::iterator it = listAccount.begin();
            it != listAccount.end();
            ++it)
    {
        int iAccountID = *it;

        bool bOldSyncStatus = IsFeatureKeySync(iAccountID);
        bool bOldForwardStatus = IsForwardSync(iAccountID);
        bool bOldDNDStatus = IsDNDSync(iAccountID);

        FeatureSyncInfo accSyncInfo;
        GetAccountFeatureSyncInfo(syncInfo, iAccountID, accSyncInfo);
        m_mapFeatureKeyInfo[iAccountID] = accSyncInfo;

        // 开关由禁用到启用，通知刷新状态
        // 总开关是开启的，才判断功能开关
        if (!bOldSyncStatus && accSyncInfo.bFeatureKeySync)
        {
            bUpdateStatus |= (!bOldForwardStatus && accSyncInfo.bForwardSync) || (!bOldDNDStatus && accSyncInfo.bDndSync);
        }
    }

    //处理完账号配置之后，再更新全局配置
    m_mapFeatureKeyInfo[FeatureSyncIDGlobal] = syncInfo;
    if (bUpdateStatus)
    {
        PostUpdateRequestToServer();
    }
}

void CFeatureSyncManager::GetAccountFeatureSyncInfo(const FeatureSyncInfo & globalSyncInfo, int iAccountID, FeatureSyncInfo & accSyncInfo)
{
    // 以分账号配置优先，分账号配置默认为空，当其为空时，以全局开关的配置值生效
    yl::string strValue = configParser_GetCfgItemStringValue(kszAccountFeatureKeySync, iAccountID);
    accSyncInfo.bFeatureKeySync = strValue.empty() ? globalSyncInfo.bFeatureKeySync : (atoi(strValue.c_str()) == 1);

    strValue = configParser_GetCfgItemStringValue(kszAccountDNDFeatureKeySync, iAccountID);
    accSyncInfo.bDndSync = strValue.empty() ? globalSyncInfo.bDndSync : (atoi(strValue.c_str()) == 1);


    strValue = configParser_GetCfgItemStringValue(kszAccountForwardFeatureKeySync, iAccountID);
    accSyncInfo.bForwardSync = strValue.empty() ? globalSyncInfo.bForwardSync : (atoi(strValue.c_str()) == 1);
}

BOOL CFeatureSyncManager::PostSyncStatusToServer(SYNC_FEATURE_TYPE eType, int iLineId,
        void * pTypeData /*= NULL*/)
{
    if ((eType == SFT_DND && !IsDNDSync(iLineId))
            || (eType == SFT_FWD && !IsForwardSync(iLineId)))
    {
        return FALSE;
    }

#if IF_BUG_27027
    COMMONUNITS_INFO("CFeatureSyncManager::PostSyncStatusToServer [%d] eType[%d] iLineId[%d]",
                     m_bFortinetDndSync, eType, iLineId);
    if (!m_bFortinetDndSync)
    {
        return FALSE;
    }
#endif

    // 如果账号注册失败，则不发送同步消息
    if (!acc_IsAccountAvailable(iLineId))
    {
        return FALSE;
    }

#if IF_BUG_27027
    // Fortinet DND同步直接呼出DNDTGL
    if (eType == SFT_DND && m_bFortinetDndSync)
    {
        talklogic_CallOut("DNDTGL", "", iLineId);
        return TRUE;
    }
#endif

    CSyncBaseProcess * pSyncProcess = GetSyncProcesser(eType, iLineId);
    if (pSyncProcess == NULL)
    {
        return FALSE;
    }

    yl::string strData = pSyncProcess->GetXMLExtraData(iLineId, pTypeData);

    // 释放
    delete pSyncProcess;
    pSyncProcess = NULL;

    // 给SIP发消息
    return modVoip_FeatureKeySync(iLineId, iLineId, strData.c_str());
}

void CFeatureSyncManager::ParseXMLExtraData(LPCSTR pXMLContent, int iAccount)
{
    if (!IsFeatureKeySync(iAccount) || NULL == pXMLContent)
    {
        return;
    }
    // 直接用xml_document解析
    xml_document docXmlDoc;
    docXmlDoc.load_buffer(pXMLContent, strlen(pXMLContent));
    xml_node xmlBaseNode = docXmlDoc.first_child();
    while (!xmlBaseNode.empty())
    {
        // 根据子节点的名称调用相应的Processer来处理
        CSyncBaseProcess * pSyncProcess = GetSyncProcesser(xmlBaseNode.name(), iAccount);
        if (pSyncProcess != NULL)
        {
            pSyncProcess->ProcessXMLInfo(xmlBaseNode, iAccount);
            delete pSyncProcess;
            pSyncProcess = NULL;
        }
        xmlBaseNode = xmlBaseNode.next_sibling();
    }
}

// 获取处理对象
CSyncBaseProcess * CFeatureSyncManager::GetSyncProcesser(SYNC_FEATURE_TYPE eType, int iAccountID/* = -1*/)
{
    switch (eType)
    {
    case SFT_DND:
        if (IsDNDSync(iAccountID))
        {
            return new CSyncDNDProcess;
        }
        break;
    case SFT_FWD:
        if (IsForwardSync(iAccountID))
        {
            return new CSyncFWDProcess;
        }
        break;
    case SFT_UPDATE:
        return new CSyncUpdateProcess;
    default:
        break;
    }
    return NULL;
}

CSyncBaseProcess * CFeatureSyncManager::GetSyncProcesser(LPCSTR lpszNodeName, int iAccountID /*= -1*/)
{
    if (strcmp(lpszNodeName, DND_NODE_NAME) == 0 && IsDNDSync(iAccountID))
    {
        return (new CSyncDNDProcess);
    }
    else if (strcmp(lpszNodeName, FWD_NODE_NAME) == 0 && IsForwardSync(iAccountID))
    {
        return (new CSyncFWDProcess);
    }
    else if (strcmp(lpszNodeName, ACD_LOGON_NAME) == 0
             || strcmp(lpszNodeName, ACD_READY_NAME) == 0
             || strcmp(lpszNodeName, ACD_NOTREADY_NAME) == 0
             || strcmp(lpszNodeName, ACD_LOGOFF_NAME) == 0
#if NABLECOMM_BUGFREE_16773
             || strcmp(lpszNodeName, ACD_WRAPUP_NAME) == 0
#endif
            )
    {
        return (new CSyncACDProcess);
    }
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    else if (strcmp(lpszNodeName, CALL_RECORD_NAME) == 0)
    {
        return (new CSyncCallRecordProcess);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    else if (strcmp(lpszNodeName, ASSISTANT_NODE_NAME) == 0
             || strcmp(lpszNodeName, EXECUTIVE_NODE_NAME) == 0)
    {
        return new CSyncExecutiveAssistantProcess;
    }
#endif

    return NULL;
}

void CFeatureSyncManager::PostUpdateRequestToServer()
{
    ListAccounts accounts = acc_GetAvailableAccountList(ACCOUNT_TYPE_SIP);
    for (ListAccounts::iterator it = accounts.begin(); it != accounts.end(); ++it)
    {
        PostSyncStatusToServer(SFT_UPDATE, *it);
    }
}
