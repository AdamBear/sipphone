#include "scamanager.h"
#include "account/include/modaccount.h"
#include "adapterbox/include/modadapterbox.h"
#include "dsskey/include/moddsskey.h"
#include "scamessagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "voip/modvoip.h"
#include "talk/talklogic/src/talklogic_inc.h"

#ifdef IF_FEATURE_BROADSOFT_SCA

bool CSCAManager::GetInfoByIndex::operator()(CCallInfo* p)
{
    return p->GetAccountID() == iAccountID && p->m_iBLAIndex == iIndex;
}

bool CSCAManager::GetInfoByStatus::operator()(CCallInfo* p)
{
    return p->GetAccountID() == iAccountID
           && p->m_iBLAIndex == 0
           && strcmp(p->m_tRemoteInfo.sSIPName.c_str(), tStateData.remote_user_info.user_name) == 0
           && strcmp(p->m_tRemoteInfo.sServerName.c_str(), tStateData.remote_user_info.server_name) == 0;
}

bool CSCAManager::GetInfoByNetConfURI::operator()(CCallInfo* p)
{
    return GetInfoByIndex::operator()(p) && p->m_strNetConfUri.empty();
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_GETINSTANCE(CSCAManager)


CSCAManager::CSCAManager()
{
    m_iCurrentUpdateNum = 0;
}

CSCAManager::~CSCAManager()
{
    // delete map中的内容
    // TODO
    ReleaseScaMap();
}

VOID CSCAManager::ReleaseScaMap()
{
    if (m_mapSCA.size() <= 0)
    {
        return;
    }
    AccountSCAMap::iterator iter = m_mapSCA.begin();
    while (iter != m_mapSCA.end())
    {
        CSCAStatusManager* pScaSta = iter->second;
        if (pScaSta != NULL)
        {
            delete pScaSta;
            pScaSta = NULL;
        }

        ++iter;
    }
    m_mapSCA.clear();
}

// 状态改变的响应
LRESULT CSCAManager::OnMessage(msgObject& objMsg)
{
    switch (objMsg.message)
    {
    case SIP_SUB_BLF_SUB_SELF_RESULT:
        {
            return _BSSCAManager.ParseSCANotify(objMsg.wParam, objMsg.lParam, BLA_GENBAND,
                                                objMsg.GetExtraData(), objMsg.GetExtraSize());
        }
        break;
    case SIP_SUB_UPDATE_BROADSOFT_SCA_STATUS:
        {
            // 处理share line的通话状态信息
            return _BSSCAManager.ParseSCANotify(objMsg.wParam, objMsg.lParam, BLA_BROADSOFT_SCA,
                                                objMsg.GetExtraData(), objMsg.GetExtraSize());
        }
        break;
    case SIP_SUB_UPDATE_DRAFT_BLA_STATUS:
        {
            return _BSSCAManager.ParseSCANotify(objMsg.wParam, objMsg.lParam, BLA_DRAFT_BLA,
                                                objMsg.GetExtraData(), objMsg.GetExtraSize());
        }
        break;
    case BLA_MESSAGE_DSSKEY_CHANGED:
        {
            SCA_INFO("OnConfigChange [%d][%d]", objMsg.wParam, objMsg.lParam);
            if (objMsg.wParam == ST_VPPHONE
                    || objMsg.wParam == ST_EXT)
            {
                return _BSSCAManager.ConfigChange(objMsg);
            }
        }
        break;
    case ACCOUNT_STATUS_CHANGED:
        {
            // 账号ID
            int iAccountID = objMsg.wParam;
            if (iAccountID < 0 || iAccountID >= acc_AccountNum())
            {
                return -1;
            }

            //  检查状态从注册上到未注册上时，清空SCA的信息
            LINE_STATE eState = acc_GetState(iAccountID);
            if (LS_REGISTERED != eState)
            {
                CSCAStatusManager* pManager = _BSSCAManager.GetManagerByAccount(iAccountID);
                if (pManager != NULL)
                {
                    SCA_INFO("Init BS SCA Manager[%d].", iAccountID);
                    pManager->InitBSSCAManager(iAccountID);
                }
            }

            //  账号注册上，创建BLA
            if (eState == LS_REGISTERED || LS_SYSTEM_DEFAULT_ACCOUNT == eState)
            {
                CSCAStatusManager* pSCAStatusMng = _BSSCAManager.GetManagerByAccount(iAccountID);
                if (NULL != pSCAStatusMng)
                {
                    SCA_INFO("Reload BS SCA Manager[%d].", iAccountID);
                    pSCAStatusMng->ReloadConfig();
                }
            }
        }
        break;
    case SIP_SUB_BW_CONF_INDEX:
        {
            if (0 == configParser_GetConfigInt(kszBroadsoftActive)
                    || 0 == configParser_GetCfgItemIntValue(kszNetConfMemberManagerEable, objMsg.wParam))
            {
                break;
            }

            sip_bw_conf_index_t* pBwConfIndexData = (sip_bw_conf_index_t*)objMsg.GetExtraData();
            if (pBwConfIndexData != NULL)
            {
                for (int i = 0; i < objMsg.lParam; i++)
                {
                    _BSSCAManager.SetNetConfUribyIndex(objMsg.wParam, pBwConfIndexData[i].call_index, pBwConfIndexData[i].conf_uri);
                }
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

void CSCAManager::Init()
{
    ReleaseScaMap();

    for (int i = 0; i < acc_AccountNum(); i++)
    {
        if (acc_IsAccountIDValid(i))
        {
            // 不判断是否SCA账号，都创建SCAStatus对象
            // 要改成按账号加载，账号配置更改时再根据需要增删 TODO
            CSCAStatusManager* pManager = new CSCAStatusManager();
            pManager->InitBSSCAManager(i);
            m_mapSCA.put(i, pManager);
        }
    }

    // 注册账号消息
    SingleMsgReg(ACCOUNT_STATUS_CHANGED, CSCAManager::OnMessage);
    SingleMsgReg(SIP_SUB_UPDATE_BROADSOFT_SCA_STATUS, CSCAManager::OnMessage);
    SingleMsgReg(SIP_SUB_UPDATE_DRAFT_BLA_STATUS, CSCAManager::OnMessage);
    SingleMsgReg(BLA_MESSAGE_DSSKEY_CHANGED, CSCAManager::OnMessage);
    SingleMsgReg(SIP_SUB_BLF_SUB_SELF_RESULT, CSCAManager::OnMessage);
    SingleMsgReg(SIP_SUB_BW_CONF_INDEX, CSCAManager::OnMessage);
}

CSCAStatusManager* CSCAManager::GetManagerByAccount(int iAccount)
{
    AccountSCAMap::iterator iter = m_mapSCA.find(iAccount);
    if (iter != m_mapSCA.end())
    {
        return iter->second;
    }

    SCA_WARN("Can't get SCA Manager By Account[%d]", iAccount);
    return NULL;
}

void CSCAManager::SetFreeIndex(int iIndex, int iAccount)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);
    if (pManager != NULL)
    {
        pManager->SetFreeIndex(iIndex);
    }
}

int CSCAManager::GetFirstFreeIndex(int iAccount)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);
    if (pManager != NULL)
    {
        return pManager->GetFirstFreeIndex();
    }

    return 0;
}

int CSCAManager::GetAlertingIndex(int iAccount)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);
    if (pManager != NULL)
    {
        return pManager->GetAlertingIndex();
    }

    return 0;
}

BOOL CSCAManager::ParseSCANotify(int iAccount, int iIndex, BLA_TYPE eType, void* pStateArray,
                                 int iSize)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);
    if (pManager != NULL)
    {
        if (pManager->GetBlaType() != eType)
        {
            if (1 == configParser_GetConfigInt(kszBroadsoftActive)
                    && 1 == configParser_GetCfgItemIntValue(kszNetConfMemberManagerEable, iAccount))
            {
                return ParseSCAIndex(iAccount, iIndex, pStateArray);
            }

            SCA_ERR("sca error for type not matched acc=[%d] [%d]!=[%d]", iAccount, pManager->GetBlaType(),
                    eType);

            return FALSE;
        }

        return pManager->ParseSCANotify(iIndex, pStateArray, iSize);
    }

    return FALSE;
}

int CSCAManager::GetFirstIndexByStatus(BLF_STATUS_CODE eState, int iAccount, int iDsskeyIndex)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);
    if (pManager && pManager->GetBlaType() == BLA_DRAFT_BLA)
    {
        return pManager->GetBlaIndexByStatus(eState, iDsskeyIndex);
    }
    else if (pManager != NULL)
    {
        SCA_Info* pInfo = pManager->GetFirstInfoByStatus(eState, iDsskeyIndex);
        return pInfo != NULL ? pInfo->iIndex : 0;
    }

    return 0;
}

BLF_STATUS_CODE CSCAManager::GetStatusByIndex(int iIndex, int iAccount)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);
    if (pManager != NULL)
    {
        return pManager->GetStatusByIndex(iIndex);
    }

    return CN_BLA_STATUS_UNKOWN;
}

BOOL CSCAManager::GetLineIndexCallInfo(int iAccoundID, int iScaIndex, yl::string& strUserName,
                                       yl::string& strUri)
{
    CSCAStatusManager* pStatusMng = GetManagerByAccount(iAccoundID);
    if (pStatusMng == NULL)
    {
        return FALSE;
    }
    // 获取账号线路信息
    SCA_Info* pInfo = pStatusMng->GetScaInfoByIndex(iScaIndex);
    if (pInfo == NULL)
    {
        return FALSE;
    }

    // 获取显示名信息
    strUserName = pInfo->strDisplay;
    strUri = pInfo->strURI;
    return TRUE;
}

// 获取指定账号的SCA 呼入数据信息
SCA_Info* CSCAManager::GetCallInData(int iAccoundID, int iScaIndex)
{
    CSCAStatusManager* pStatusMng = GetManagerByAccount(iAccoundID);
    if (pStatusMng == NULL)
    {
        return NULL;
    }

    return pStatusMng->GetScaInfoByIndex(iScaIndex);
}

// 设置要显示SCA状态的账号ID
BOOL CSCAManager::ShowSCAStatus(int iAccoundID)
{
    CSCAStatusManager* pStatusMng = GetManagerByAccount(iAccoundID);
    if (pStatusMng != NULL
            && pStatusMng->BeShowSharedCalls())
    {
        m_stScaMenuData.iAccount = iAccoundID;
        m_stScaMenuData.pScaInfoList = pStatusMng->GetList();
        m_stScaMenuData.nReturnTime = configParser_GetCfgItemIntValue(kszAccountSCAShowTime,
                                      iAccoundID) * 1000;

        AdapterBox_ShowSCAMenu(&m_stScaMenuData);

        return TRUE;
    }
    return FALSE;
}

// 刷新状态界面显示
VOID CSCAManager::FlashShowMenu(int iAccoundID)
{
    if (AdapterBox_IsExistSCAMenu()
            && acc_IsAccountIDValid(iAccoundID)
            && m_stScaMenuData.iAccount == iAccoundID)
    {
        if (ShowSCAStatus(iAccoundID))
        {
            SCA_INFO("Flash SCA Status Menu !");
            return;
        }
        else
        {
            AdapterBox_ExitSCAMenu();
        }
    }
}

void CSCAManager::UpdateAllLightInfo()
{
    AccountSCAMap::iterator iter = m_mapSCA.begin();
    for (; iter != m_mapSCA.end(); ++iter)
    {
        CSCAStatusManager* pManager = iter->second;
        if (pManager != NULL)
        {
            pManager->UpDateLightInfo();
        }
    }
}

// 更新灯状态
VOID CSCAManager::UpdateLightInfo(int iAccoundID)
{
    CSCAStatusManager* pStatusMng = GetManagerByAccount(iAccoundID);
    if (pStatusMng == NULL)
    {
        return;
    }
    pStatusMng->UpDateLightInfo();
}

// BLA占线
VOID CSCAManager::SeizeLine(CCallInfo* pCallInfo, BOOL bSeize/* = TRUE*/)
{
    if (pCallInfo == NULL)
    {
        return;
    }

    if (!pCallInfo->m_bNeedToSeizeLine)
    {
        // 该话路不需要去占线，比如ConfTo
        return;
    }

    if (pCallInfo->m_bBLASeizeLine == bSeize)
    {
        // 已经占线则不处理
        return;
    }

    int iAccount = pCallInfo->GetAccountID();
    if (!acc_IsAccountIDValid(iAccount))
    {
        // 账号为空则不处理
        return;
    }

    BLA_TYPE eBLAType = (BLA_TYPE)acc_GetBLAType(iAccount);
    int iIndex = 0;
    yl::string strBLA;
    if (eBLAType == BLA_BROADSOFT_SCA
            && pCallInfo->m_iBLAIndex != 0)
    {
        iIndex = pCallInfo->m_iBLAIndex;
    }
    else if (eBLAType == BLA_DRAFT_BLA
             && !commonAPI_IsEmpty(configParser_GetCfgItemStringValue(kszBLANumber, iAccount).c_str()))
    {
        iIndex = pCallInfo->m_iBLAIndex;
    }
    else
    {
        return;
    }

    SCA_INFO("[CSCAManager] SeizeLine [seize:%d][type:%d][iIndex:%d]", bSeize, eBLAType, iIndex);
    if (eBLAType == BLA_DRAFT_BLA)
    {
        // 逻辑层index和sca一样从1开始、服务器从0开始(03只支持一个为0的index，所以必须从0开始)
        modVoip_DraftBLASeizeLine(iAccount, pCallInfo->GetCallID(), bSeize, iIndex - 1);
    }
    else if (eBLAType == BLA_BROADSOFT_SCA)
    {
        modVoip_BLASeizeLine(iAccount, pCallInfo->GetCallID(), bSeize, iIndex);
    }

    if (!bSeize
            && eBLAType == BLA_BROADSOFT_SCA)
    {
        // 取消占线
        SetFreeIndex(pCallInfo->m_iBLAIndex, pCallInfo->GetAccountID());
    }

    pCallInfo->m_bBLASeizeLine = bSeize;
}

int CSCAManager::GetBLAIndex(int iAccountID, int& iBLAIndex, int iKeyCounts)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccountID);
    if (pManager != NULL)
    {
        return pManager->GetBLAIndex(iBLAIndex, iKeyCounts);
    }
    return -1;
}

CSessionProxy CSCAManager::GetDsskeySessionByStatus(int iAccount, BLF_STATUS_CODE eStatus)
{
    CSCAStatusManager* pSCAStatusMng = GetManagerByAccount(iAccount);
    if (NULL != pSCAStatusMng)
    {
        return pSCAStatusMng->GetSessionBySCAStatus(eStatus);
    }
    return CSessionProxy();
}

// 获取dsskey对应的线路状态
BLF_STATUS_CODE CSCAManager::GetStatusByDsskeyID(int iDsskeyID)
{
    BLF_STATUS_CODE eStatus = CN_BLA_STATUS_UNKOWN;
    int iAccoutID = dsskey_GetDsskeyLine(iDsskeyID);
    CSCAStatusManager* pSCAStatusMng = GetManagerByAccount(iAccoutID);
    if (NULL != pSCAStatusMng)
    {
        eStatus = pSCAStatusMng->GetStatusByDsskeyID(iDsskeyID);
    }

    return eStatus;
}

bool CSCAManager::ConfigChange(msgObject& objMsg)
{
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        CSCAStatusManager* pStatusMng = GetManagerByAccount(i);
        if (pStatusMng != NULL)
        {
            pStatusMng->ReloadConfig();
        }
    }

    return true;
}

ListSCAInfo* CSCAManager::GetListByAccount(int iAccount)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);
    if (pManager != NULL)
    {
        return pManager->GetList();
    }

    return NULL;
}

int CSCAManager::GetCurrentCallNumByAccount(int iAccount, int nDsskeyIndex/* = 0*/)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccount);

    if (pManager != NULL)
    {
        return pManager->GetCurrentCallNum(iAccount, nDsskeyIndex);
    }

    return 0;
}

BLF_STATUS_CODE CSCAManager::GetStatus2UIShow(int iAccountId, int iIndex)
{
    CSCAStatusManager* pManager = GetManagerByAccount(iAccountId);

    if (pManager != NULL)
    {
        return pManager->GetStatus2UIShow(iAccountId, iIndex);
    }

    return CN_BLA_STATUS_UNKOWN;
}

//未开启SCA时，设置index信息
bool CSCAManager::ParseSCAIndex(int iAccount, int iUpdatNum, void* pStateArray)
{
    if (iUpdatNum == m_iCurrentUpdateNum)
    {
        return false;
    }
    else
    {
        m_iCurrentUpdateNum = iUpdatNum;
        if (iUpdatNum < m_iCurrentUpdateNum)
        {
            return false;
        }
    }

    bool bRet = false;
    sip_sca_status_t* pStateData = (sip_sca_status_t*)pStateArray;
    if (pStateData != NULL)
    {
        for (int i = 0; i < iUpdatNum; i++)
        {
            bRet = SetCallInfoIndex(iAccount, &pStateData[i]);
            if (bRet)
            {
                return true;
            }
        }
    }

    return bRet;
}

bool CSCAManager::SetCallInfoIndex(int iAccount, sip_sca_status_t* pStateData)
{
    if (pStateData == NULL)
    {
        return false;
    }

    GetInfoByStatus vistor(iAccount, *pStateData);
    CCallInfo* pCallInfo = talklogic_TraverseCallInfo(vistor);
    if (pCallInfo != NULL)
    {
        pCallInfo->m_iBLAIndex = pStateData->appearance_index;
        return true;
    }

    return false;
}

//通过index设置conf_uri
bool CSCAManager::SetNetConfUribyIndex(int iAccount, int iIndex, const char* ConfUri)
{
    if (ConfUri == NULL)
    {
        return false;
    }

    GetInfoByNetConfURI vistor(iAccount, iIndex);
    CCallInfo* pCallInfo = talklogic_TraverseCallInfo(vistor);
    if (pCallInfo != NULL)
    {
        char TempUri[SIP_MAX_URI_LEN] = "";
        //yef::ad hoc参与方要验证下conf_uri格式是何种。
        if (1 == sscanf(ConfUri, "\"<%[^>\"]", TempUri))
        {
            pCallInfo->m_strNetConfUri = TempUri;
        }
        else
        {
            pCallInfo->m_strNetConfUri = TempUri;
        }

        return true;
    }

    return false;
}

int CSCAManager::GetCallIdBySCAInfo(int iAccount, int iIndex)
{
    GetInfoByIndex vistor(iAccount, iIndex);
    CCallInfo* pCallInfo = talklogic_TraverseCallInfo(vistor, false);
    if (pCallInfo != NULL)
    {
        return pCallInfo->GetCallID();
    }

    return -1;
}

#endif // #ifdef IF_FEATURE_BROADSOFT_SCA
