#if IF_FEATURE_METASWITCH_ACD
#include "callcenter_inc.h"
#include "mtswacdactioncmd.h"

//#ifdef 1 //IF_FEATURE_ACD

CMTSWACDActionCmd s_objMTSWACDActionCmd;

//#define CHECK_NULL(p) if (NULL == p){ return false;}

// 是否同时允许登录多个MLHG
#define kszDataTypeMultiLineHuntGroups              "Meta_BusinessGroup_MultiLineHuntGroups"
// MLHG的列表
#define kszDataTypeSubscriberGroupMemberships       "Meta_Subscriber_GroupMembershipsList"
// 获取MLHG组的BGMLHG
#define kszDataTypeBGMLHG                           "Meta_BusinessGroup_ChildrenList_MLHG"
// MLHG组的成员信息
#define kszDataTypeMLHGMembers                      "Meta_MLHG_MembersList"
// Disposition code 列表
#define kszDataTypeAutomatedCallDistribution        "Meta_MLHG_AutomatedCallDistribution"
// 获取状态列表
#define kszDataTypeUnavailList                      "Meta_BusinessGroup_MLHGAutomatedCallDistribution"
// ACD当前状态
#define kszDataTypeACDStatus                        "Meta_Subscriber_AutomatedCallDistribution"
// 获取queue列表
#define kszDataTypeQueues                           "/bg/department/mlhg*/data?version=8.3&data=Meta_Global_Statistics%3FobjectType%3DMeta_MLHG%26statistics%3Davg_qing_tm%252Bmembers_agents_curr_logged_in%252Bavg_alert_time%252Bqd_cls_currently_in_q_2dp%252Ball_incmg_cls_alrt%252B%26granularity%3Dreal_time"
#define kszDataTypeQueuesStatistics                 "Meta_Global_Statistics"


CACDModelMTSW::CACDModelMTSW(void)
{
    Set_ACDType(ACD_METASWITCH);
    m_nRefreshInterval = 30;
    m_bMultipleLoginsAllowed = true;

    m_nDispCodeIndex = -1;
    m_strDirectoryNumber = configParser_GetConfigString(kszMTSWCommportalUser);
}

CACDModelMTSW::~CACDModelMTSW(void)
{
    if (m_objTimer.IsTimerRuning())
    {
        //关闭超时计时器
        m_objTimer.KillTimer();
    }
}

// 登录ACD
bool CACDModelMTSW::ReqLogin(acd_accout * pAcd)
{
    return ReqChangeACDStatus(AA_LOGIN);
}

//请求状态改变，包括登陆，登陆时需考虑考虑初始化状态
bool CACDModelMTSW::ReqChangeACDStatus(ACD_ACTION eACDAction)
{
    MtswActionType nType = EMTSWLOGDTYPE_Post;
    yl::string strData;
    switch (eACDAction)
    {
    case AA_LOGIN:
    case AA_LOGOUT:
        {
            m_strDirectoryNumber = configParser_GetConfigString(kszMTSWCommportalUser);

            nType = EMTSWLOGDTYPE_Login;
            yl::string strMembers;
            YLVector<SMLHGMemberData> & vecMember = m_mapMemberList[m_strCurMLHGName];
            for (int i = 0 ; i < vecMember.size(); i++)
            {
                if (vecMember[i].strDirectoryNumber == m_strDirectoryNumber)
                {
                    if (eACDAction == AA_LOGIN)
                    {
                        vecMember[i].bLoggedIn = true;
                    }
                    else
                    {
                        vecMember[i].bLoggedIn = false;
                    }
                }

                yl::string strTemp =
                    commonAPI_FormatString("{\"Department\":{\"_\":\"%s\"},\"LoggedIn\":{\"_\":%s},\"LoginLogoutSupported\":{\"_\":%s},\"MemberDirectoryNumber\":{\"_\":\"%s\"},\"Name\":{\"_\":\"%s\"},\"SubscriberType\":{\"_\":\"%s\"}},",
                                           vecMember[i].strDepartment.c_str(),
                                           vecMember[i].bLoggedIn ? "true" : "false",
                                           vecMember[i].bLoginLogoutSupported ? "true" : "false",
                                           vecMember[i].strDirectoryNumber.c_str(),
                                           vecMember[i].strName.c_str(),
                                           vecMember[i].strSubscriberType.c_str());

                strMembers.append(strTemp.c_str());
            }
            // 如果没有数据则直接返回
            if (strMembers.empty())
            {
                return false;
            }

            strMembers = strMembers.substr(0, strMembers.size() - 1);
            strData = commonAPI_FormatString("\r\nContent-Disposition: form-data; name=\"Meta_MLHG_MembersList?mlhg=%s\"\r\n\r\n{\"Member\":[%s],\"_SequenceNumber\":%d}\r\n",
                                             m_strCurMLHGName.c_str(),
                                             strMembers.c_str(),
                                             m_mapSequenceNumber[m_strCurMLHGName]);
        }
        break;
    case AA_AVAIL:
    case AA_UNAVAIL:
        {
            if (m_nDispCodeIndex == -1)
            {
                // 等待设置disposition code
                m_eChangeDelayAcdAction =  eACDAction;
                return m_nDispCodeIndex;
            }

            nType = EMTSWLOGDTYPE_ChangeState;
            char szBuf[1024] = {0};
            yl::string strState;
            if (eACDAction == AA_AVAIL)
            {
                strState = "Available";
            }
            else
            {
                strState = m_strTempUnavailReason;
            }

            if (m_eACDLineState == AS_WRAPUP)
            {
                sprintf(szBuf,
                        "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_AutomatedCallDistribution\"\r\n\r\n{\"Availability\":\"%s\",\"DispositionCode\":%d,\"LastAnsweredMLHG\":\"%s\"}\r\n",
                        strState.c_str(),
                        m_nDispCodeIndex,
                        m_strCurMLHGName.c_str());
            }
            else
            {
                sprintf(szBuf,
                        "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_AutomatedCallDistribution\"\r\n\r\n{\"Availability\":\"%s\"}\r\n",
                        strState.c_str());
            }
            strData = szBuf;
        }
        break;
    default:
        break;
    }
    CALLCENTER_INFO("CACDModelMTSW::ReqChangeACDStatus: strData = %s, nType =%d", strData.c_str(),
                    nType);

    MTSW_ExecPost(&s_objMTSWACDActionCmd, "/data.js?", strData, nType);
    return true;
}

bool CACDModelMTSW::DownloadData(MtswActionType eDownloadType, yl::string strMlhgName /*= ""*/)
{
    if (strMlhgName.empty())
    {
        strMlhgName = m_strCurMLHGName;
    }

    yl::string strRequestData = "/data?data=";
    switch (eDownloadType)
    {
    case EMTSWLOGDTYPE_All:
        {
            strRequestData.append(kszDataTypeMultiLineHuntGroups);
            strRequestData.append(",");
            strRequestData.append(kszDataTypeSubscriberGroupMemberships);
            strRequestData.append(",");
            strRequestData.append(kszDataTypeBGMLHG);
            strRequestData.append(",");
            strRequestData.append(kszDataTypeUnavailList);
            strRequestData.append(",");
            strRequestData.append(kszDataTypeACDStatus);
        }
        break;
    case EMTSWLOGDTYPE_MLHG:
        {
            strRequestData.append(kszDataTypeSubscriberGroupMemberships);
            strRequestData.append(",");
            strRequestData.append(kszDataTypeBGMLHG);

            yl::string strTemp;
            int nSize = m_vecMLHG.size();
            for (int i = 0; i < nSize; i++)
            {
                char buf[256] = { 0 };
                sprintf(buf, ",%s?mlhg=%s", kszDataTypeMLHGMembers, m_vecMLHG[i].strName.c_str());
                strTemp.append(buf);
            }

            CALLCENTER_INFO("MLHG Size [%d] List [%s]", nSize, strTemp.c_str());

            strRequestData.append(strTemp);
        }
        break;
    case EMTSWLOGDTYPE_AcdState:
        {
            strRequestData.append(kszDataTypeACDStatus);
        }
        break;
    case EMTSWLOGDTYPE_Queues:
        {
            strRequestData = kszDataTypeQueues;
        }
        break;
    case EMTSWLOGDTYPE_MemberList:
        {
            for (int i = 0; i < m_vecMLHG.size(); i++)
            {
                char buf[256] = {0};
                sprintf(buf, "%s?mlhg=%s,", kszDataTypeMLHGMembers, m_vecMLHG[i].strName.c_str());
                strRequestData.append(buf);
            }

            if (m_vecMLHG.size() > 0)
            {
                strRequestData = strRequestData.substr(0, strRequestData.size() - 1);
            }
        }
        break;
    case EMTSWLOGDTYPE_DispCode:
        {
            for (int i = 0; i < m_vecMLHG.size(); i++)
            {
                char buf[256] = {0};
                sprintf(buf, "%s?mlhg=%s,", kszDataTypeAutomatedCallDistribution, m_vecMLHG[i].strName.c_str());
                strRequestData.append(buf);
            }

            if (m_vecMLHG.size() > 0)
            {
                strRequestData = strRequestData.substr(0, strRequestData.size() - 1);
            }
        }
        break;
    default:
        break;
    }

    if (strRequestData != "/data?data=")
    {
        MTSW_ExecDownLoad(&s_objMTSWACDActionCmd, strRequestData, eDownloadType);
    }

    return true;
}

bool CACDModelMTSW::GetMLHGMemberList(YLVector<SMLHGMemberData> & vecData, yl::string strMlhgName)
{
    if (strMlhgName.empty())
    {
        strMlhgName = m_strCurMLHGName;
    }

    CALLCENTER_INFO("strMlhgName = %s", strMlhgName.c_str());
    YLHashMap<yl::string, YLVector<SMLHGMemberData> >::iterator it = m_mapMemberList.find(strMlhgName);
    if (it != m_mapMemberList.end())
    {
        vecData = it->second;
        CALLCENTER_INFO("Find size = %d", vecData.size());
        return true;
    }
    CALLCENTER_INFO("Not Find");
    return false;
}

bool CACDModelMTSW::GetMLHGList(YLVector<SMLHGData> & vecData)
{
    vecData = m_vecMLHG;
    return true;
}

bool CACDModelMTSW::GetUnavailReasonList(YLList<yl::string> & listUnavailReason)
{
    listUnavailReason.clear();
    YLHashMap<yl::string, yl::string>::iterator it = m_mapUnavailabilityReason.begin();
    for (; it != m_mapUnavailabilityReason.end(); it ++)
    {
        listUnavailReason.push_back(it->first);
    }
    return listUnavailReason.size() > 0;
}

bool CACDModelMTSW::GetDispositionCodeList(YLVector<yl::string> & vecData, yl::string strMLHGName)
{
    vecData.clear();

    // 只有当高级acd的时候才有disposition
    for (int i = 0; i < m_vecMLHG.size(); i++)
    {
        if (strMLHGName == m_vecMLHG[i].strName
                && m_vecMLHG[i].eACDType != EMTSWACDSL_PremiumACD)
        {
            return true;
        }
    }

    YLHashMap<yl::string, yl::string> * pDispCodesList = NULL;
    YLHashMap<yl::string, YLHashMap<yl::string, yl::string> >::iterator itDCList =
        m_mapDispCodesList.find(strMLHGName);
    if (itDCList != m_mapDispCodesList.end())
    {
        pDispCodesList = &(itDCList->second);
    }

    if (pDispCodesList == NULL)
    {
        return false;
    }

    YLHashMap<yl::string, yl::string>::iterator it = pDispCodesList->begin();
    for (; it != pDispCodesList->end(); it ++)
    {
        vecData.push_back(it->first);
    }
    return vecData.size() > 0;
}

const SMLGHQueuesData * CACDModelMTSW::GetQueuesData(yl::string strMlhgName)
{
    if (strMlhgName.empty())
    {
        strMlhgName = m_strCurMLHGName;
    }

    CALLCENTER_INFO("GetQueuesData strMlhgName = (%s) size %d", strMlhgName.c_str(),
                    m_mapQueuesList.size());

    YLHashMap<yl::string, SMLGHQueuesData>::iterator it = m_mapQueuesList.find(strMlhgName);
    if (it != m_mapQueuesList.end())
    {
        CALLCENTER_INFO("Find");
        return &(it->second);
    }
    CALLCENTER_INFO("Not Find");
    return NULL;
}

void CACDModelMTSW::SetAccountId(int iAccountId)
{
    CACDModelBase::SetAccountId(iAccountId);
}

bool CACDModelMTSW::BeHasPreminumAcd()
{
    YLVector<SMLHGData>::iterator it = m_vecMLHG.begin();
    for (; it != m_vecMLHG.end(); it++)
    {
        if (it->eACDType == EMTSWACDSL_PremiumACD)
        {
            return true;
        }
    }

    return false;
}

bool CACDModelMTSW::SetCurrentMLHG(yl::string strMLHGName)
{
    CALLCENTER_INFO("SetCurrentMLHG strMLHGName = %s", strMLHGName.c_str());
    m_strCurMLHGName = strMLHGName;
    return true;
}

// 获取当前焦点组
yl::string CACDModelMTSW::GetCurrentMLHG()
{
    return m_strCurMLHGName;
}

void CACDModelMTSW::OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType)
{
    etl_NotifyApp(false, MTSW_ACD_GET_FAILED, nType, 0);
}

bool CACDModelMTSW::OnMTSWActionResponse(cJSON * pOjbParse, int nType)
{
    CALLCENTER_INFO("ACDModelMTSW::OnMTSWActionResponse(%p, %d)", pOjbParse, nType);
    if (nType == EMTSWLOGDTYPE_Login)
    {
        // 登录成功后重新加载组信息
        DownloadData(EMTSWLOGDTYPE_MLHG);
        return true;
    }
    else if (nType == EMTSWLOGDTYPE_ChangeState)
    {
        DownloadData(EMTSWLOGDTYPE_AcdState);
        return true;
    }

    if (pOjbParse == NULL)
    {
        return false;
    }

    bool bRet = true;

    while (pOjbParse)
    {
        bRet &= ParseData(pOjbParse, nType);
        pOjbParse = pOjbParse->next;
    }

    if (bRet)
    {
        etl_NotifyApp(false, MTSW_ACD_GET_RESULT, nType, 0);
    }
    else
    {
        etl_NotifyApp(false, MTSW_ACD_GET_FAILED, nType, 0);
    }

    return true;
}

bool CACDModelMTSW::ParseMultipleLoginsAllowed(cJSON * pObj)
{
    if (pObj == NULL)
    {
        return false;
    }

    cJSON * pObjData = cJSON_GetObjectItem(pObj, "MultipleLoginsAllowed");
    if (pObjData && pObjData->child)
    {
        m_bMultipleLoginsAllowed = pObjData->child->valueint;
        return true;
    }

    return false;
}

bool CACDModelMTSW::ParseMLHGGroup(cJSON * pObj)
{
    if (pObj == NULL)
    {
        return false;
    }

    SMLHGData sMLGHData;

    // 解析类型
    cJSON * pGroupType = cJSON_GetObjectItem(pObj, "GroupType");
    if (pGroupType && pGroupType->child
            && pGroupType->child->valuestring != NULL)
    {
        if (strcmp(pGroupType->child->valuestring, "MLHG") != 0)
        {
            // 如果不是MLHG则丢弃
            return false;
        }
    }

    // 解析名字
    char * szName = cJSON_GetFistChildValueByObjectName(pObj, "Name");
    if (szName != NULL)
    {
        sMLGHData.strName = szName;
    }

    // 是否登录
    sMLGHData.eStatus = cJSON_GetFirstChildBoolByObjectName(pObj,
                        "LoggedIn") == cJSON_True ? AS_AVAILABLE : AS_IDLE;

    sMLGHData.bLoginLogoutSupport = cJSON_GetFirstChildBoolByObjectName(pObj,
                                    "LoginLogoutSupported") == cJSON_True;

    m_vecMLHG.push_back(sMLGHData);

    return true;
}

bool CACDModelMTSW::ParseMLHGList(cJSON * pObj, int nType)
{
    if (pObj == NULL)
    {
        return false;
    }

    cJSON * pGroupArr = cJSON_GetObjectItem(pObj, "Group");
    if (pGroupArr == NULL)
    {
        m_vecMLHG.clear();
        return true;
    }

    // 先清除组信息再解析
    m_vecMLHG.clear();

    cJSON * pData = pGroupArr->child;
    while (pData != NULL)
    {
        ParseMLHGGroup(pData);
        pData = pData->next;
    }

    if (nType == EMTSWLOGDTYPE_All)
    {
        // 只有加载all后才去加载memberlist和dispcode
        DownloadData(EMTSWLOGDTYPE_MemberList);
        DownloadData(EMTSWLOGDTYPE_DispCode);
    }

    // 组更新后需要通知ui刷新acd状态
    etl_NotifyAppEx(false,
                    ACD_MESSAGE_STATUS_CHANGE, 0, 0, m_strCurUnavailReason.size(),
                    (LPCVOID)m_strCurUnavailReason.c_str());

    return true;
}

// 解析mlhg组类型
bool CACDModelMTSW::ParseMLHGServiceLevel(cJSON * pObj)
{
    if (pObj == NULL)
    {
        return false;
    }

    cJSON * pMLHGArr = cJSON_GetObjectItem(pObj, "MLHG");
    if (pMLHGArr == NULL)
    {
        return false;
    }

    cJSON * pData = pMLHGArr->child;
    while (pData != NULL)
    {
        // 获取mlhg组名
        yl::string strMLHGName;
        char * szName = cJSON_GetFistChildValueByObjectName(pData, "Name");
        if (szName != NULL)
        {
            strMLHGName = szName;
        }

        // 根据组名查找mlhg组
        int nIndex = 0;
        for (; nIndex < m_vecMLHG.size(); nIndex++)
        {
            if (m_vecMLHG[nIndex].strName == strMLHGName)
            {
                break;
            }
        }

        // 设置mlhg组类型
        if (nIndex < m_vecMLHG.size())
        {
            char * szServiceLevel = cJSON_GetFistChildValueByObjectName(pData, "ServiceLevel");
            if (szServiceLevel != NULL)
            {
                m_vecMLHG[nIndex].SetACDType(szServiceLevel);
            }
        }

        pData = pData->next;
    }

    return true;
}

// 解析unvailreason列表
bool CACDModelMTSW::ParseUnavailReasonList(cJSON * pObj)
{
    if (pObj == NULL)
    {
        return false;
    }

    // 获取 UnavailabilityReason列表
    m_mapUnavailabilityReason.clear();
    yl::string strUnavailabilityReason = "UnavailabilityReason";

    cJSON * pData = pObj->child;
    while (pData != NULL)
    {
        if (strncmp(pData->string, strUnavailabilityReason.c_str(), strUnavailabilityReason.size()) == 0)
        {
            yl::string strReason;
            if (pData->child != NULL)
            {
                strReason = pData->child->valuestring;
            }

            if (!strReason.empty())
            {
                m_mapUnavailabilityReason[strReason] = pData->string;
            }
        }
        else if (strcmp(pData->string, "AgentStatusRefreshInterval") == 0)
        {
            // 获取刷新时间
            if (pData->child)
            {
                m_nRefreshInterval = pData->child->valueint;
                // 重新设置定时器
                SetTimer();
            }
        }

        pData = pData->next;
    }

    return true;
}

// 解析acd状态
bool CACDModelMTSW::ParseACDStatus(cJSON * pObj)
{
    if (pObj == NULL)
    {
        return false;
    }

    cJSON * pTemp = cJSON_GetObjectItem(pObj, "Availability");
    CHECK_NULL(pTemp, false);

    pTemp = cJSON_GetObjectItem(pTemp, "_");
    CHECK_NULL(pTemp, false);

    yl::string strAvail = pTemp->valuestring;
    if (strAvail.empty())
    {
        return false;
    }

    //m_strCurUnavailReason = "";
    m_nDispCodeIndex = 0;

    ACD_STATUS eACDLineState = AS_IDLE;
    yl::string strUnavailReason;

    if (strAvail == "Available")
    {
        eACDLineState = AS_AVAILABLE;
    }
    else if (strAvail == "Wrap-up")
    {
        eACDLineState = AS_WRAPUP;
        m_nDispCodeIndex = -1;  //需要设置dispostion后才可以切换状态
    }
    else if (strAvail.find("UnavailabilityReason") != yl::string::npos)
    {
        eACDLineState = AS_UNAVAILABLE;
        strUnavailReason = strAvail;
    }

    // 获取最后一次应答的组信息
    pTemp =  cJSON_GetObjectItem(pObj, "LastAnsweredMLHG");
    if (pTemp != NULL
            && pTemp->child != NULL
            && pTemp->child->valuestring != NULL)
    {
        m_strLastAnsweredMHLG = pTemp->child->valuestring;
    }
    else
    {
        m_strLastAnsweredMHLG = "";
    }

    // 如果所有的组都处理登出状态，则acd状态设为idle
    if (IsAllMLHGLogout())
    {
        eACDLineState = AS_IDLE;
    }

    // 先不判断，直接发消息刷新。 主要原因是当设置为Unavailable, m_strCurUnavailReason会与strUnavailReason
    // 一样，导致不会发消息。
//      if (eACDLineState != m_eACDLineState
//          || (eACDLineState == AS_UNAVAILABLE && strUnavailReason != m_strCurUnavailReason))
    {
        m_eACDLineState = eACDLineState;
        m_strCurUnavailReason = strUnavailReason;

        CALLCENTER_INFO("Send ACD_MESSAGE_STATUS_CHANGE m_eACDLineState = %d", m_eACDLineState);
        // 通知ui刷新
        etl_NotifyAppEx(false,
                        ACD_MESSAGE_STATUS_CHANGE, GetAccountId(), m_eACDLineState, m_strCurUnavailReason.size(),
                        (LPCVOID)m_strCurUnavailReason.c_str());
    }

    return true;
}

bool CACDModelMTSW::ParseData(cJSON * pObj, int nType)
{
    CALLCENTER_INFO("ParseData(pObj = %p, nType = %d)", pObj, nType);
    if (pObj == NULL)
    {
        return false;
    }

    yl::string strDataType;
    yl::string strMlhgName;

    cJSON * pObjectIdentity = cJSON_GetObjectItem(pObj, "objectIdentity");
    if (pObjectIdentity)
    {
        cJSON * pMLHGName = cJSON_GetObjectItem(pObjectIdentity, "mlhg");
        if (pMLHGName != NULL
                && pMLHGName->valuestring)
        {
            strMlhgName = pMLHGName->valuestring;
        }
    }

    cJSON * pDataType = cJSON_GetObjectItem(pObj, "dataType");
    if (pDataType
            && pDataType->valuestring)
    {
        strDataType = pDataType->valuestring;
    }

    CALLCENTER_INFO("ParseData(strDataType = %s)", strDataType.c_str());

    if (!IsCurrentDataType(strDataType, nType))
    {
        // http://10.2.1.199/Bug.php?BugID=116117
        // 返回数据里面如果和当前类型和下载类型不匹配，不解析改类型，当成成功处理
        return true;
    }

    cJSON * pObjData = cJSON_GetObjectItem(pObj, "data");

    if (pObjData == NULL)
    {
        return false;
    }

    bool bRet = true;
    if (strDataType == kszDataTypeMultiLineHuntGroups)
    {
        bRet &= ParseMultipleLoginsAllowed(pObjData);
        CALLCENTER_INFO("ParseMultipleLoginsAllowed %s", bRet ? "TRUE" : "FALSE");
    }
    else if (strDataType == kszDataTypeSubscriberGroupMemberships)
    {
        bRet &= ParseMLHGList(pObjData, nType);
        CALLCENTER_INFO("ParseMLHGList %s", bRet ? "TRUE" : "FALSE");
    }
    else if (strDataType == kszDataTypeUnavailList)
    {
        bRet &= ParseUnavailReasonList(pObjData);
        CALLCENTER_INFO("ParseUnavailReasonList %s", bRet ? "TRUE" : "FALSE");
    }
    else if (strDataType == kszDataTypeACDStatus)
    {
        bRet &= ParseACDStatus(pObjData);
        CALLCENTER_INFO("ParseACDStatus %s", bRet ? "TRUE" : "FALSE");
    }
    else if (strDataType == kszDataTypeBGMLHG)
    {
        bRet &= ParseMLHGServiceLevel(pObjData);
        CALLCENTER_INFO("ParseMLHGServiceLevel %s", bRet ? "TRUE" : "FALSE");
    }
    else if (strDataType == kszDataTypeMLHGMembers)
    {
        bRet &= ParseMembers(pObjData, strMlhgName);
        CALLCENTER_INFO("ParseMembers %s", bRet ? "TRUE" : "FALSE");
    }
    else if (strDataType == kszDataTypeAutomatedCallDistribution)
    {
        bRet &= ParseDispCode(pObjData, strMlhgName);
        CALLCENTER_INFO("ParseDispCode %s", bRet ? "TRUE" : "FALSE");
    }
    else if (strDataType == kszDataTypeQueuesStatistics)
    {
        bRet &= ParseQueueDetail(pObjData);
        CALLCENTER_INFO("ParseQueueDetail %s", bRet ? "TRUE" : "FALSE");
    }
    CALLCENTER_INFO("ParseData(nType = %d) %s", nType, bRet ? "TRUE" : "FALSE");
    return bRet;
}

bool CACDModelMTSW::ParseMembers(cJSON * pObj, yl::string strMlhgName)
{
    if (pObj == NULL
            || strMlhgName.empty())
    {
        return false;
    }

    cJSON * pObjData = cJSON_GetObjectItem(pObj, "_SequenceNumber");
    if (pObjData != NULL)
    {
        m_mapSequenceNumber[strMlhgName] = pObjData->valueint;
        CALLCENTER_INFO("mlhg = %s m_nSequenceNumber = %d",
                        strMlhgName.c_str(), m_mapSequenceNumber[strMlhgName]);
    }

    pObjData = cJSON_GetObjectItem(pObj, "Member");
    CHECK_NULL(pObjData, false);

    YLVector<SMLHGMemberData> vecMembers;

    cJSON * pMember = pObjData->child;
    while (pMember != NULL)
    {
        SMLHGMemberData sData;
        cJSON * pTemp = cJSON_GetObjectItem(pMember, "MemberDirectoryNumber");
        if (pTemp && pTemp->child && pTemp->child->valuestring)
        {
            sData.strDirectoryNumber = pTemp->child->valuestring;
        }

        pTemp = cJSON_GetObjectItem(pMember, "LoggedIn");
        if (pTemp && pTemp->child)
        {
            sData.bLoggedIn = pTemp->child->valueint;
        }

        pTemp = cJSON_GetObjectItem(pMember, "Department");
        if (pTemp && pTemp->child && pTemp->child->valuestring)
        {
            sData.strDepartment = pTemp->child->valuestring;
        }

        pTemp = cJSON_GetObjectItem(pMember, "LoginLogoutSupported");
        if (pTemp && pTemp->child)
        {
            sData.bLoginLogoutSupported = pTemp->child->valueint;
        }

        pTemp = cJSON_GetObjectItem(pMember, "Name");
        if (pTemp && pTemp->child && pTemp->child->valuestring)
        {
            sData.strName = pTemp->child->valuestring;
        }

        pTemp = cJSON_GetObjectItem(pMember, "SubscriberType");
        if (pTemp && pTemp->child && pTemp->child->valuestring)
        {
            sData.strSubscriberType = pTemp->child->valuestring;
        }

        vecMembers.push_back(sData);

        pMember = pMember->next;
    }

    m_mapMemberList[strMlhgName] = vecMembers;
    return true;
}

bool CACDModelMTSW::ParseDispCode(cJSON * pObj, yl::string strMlhgName)
{
    if (pObj == NULL
            || strMlhgName.empty())
    {
        return false;
    }

    // 获取 UnavailabilityReason列表
    yl::string strDispositionCode = "DispositionCode";
    YLHashMap<yl::string, yl::string> mapDispCode;

    cJSON * pData = pObj->child;
    while (pData != NULL)
    {
        if (strcmp(pData->string, "DispositionCodesRequired") == 0)
        {
            if (pData->child)
            {
                m_mapDispCodesRequired[strMlhgName] = pData->child->valueint;
            }
        }
        else if (strncmp(pData->string, strDispositionCode.c_str(), strDispositionCode.size()) == 0)
        {
            yl::string strDispCode;
            if (pData->child != NULL)
            {
                strDispCode = pData->child->valuestring;
            }

            if (!strDispCode.empty())
            {
                mapDispCode[strDispCode] = pData->string;
            }
        }

        pData = pData->next;
    }

    CALLCENTER_INFO("CACDModelMTSW::ParseDispoCode strMlhgName = %s", strMlhgName.c_str());
    CALLCENTER_INFO("CACDModelMTSW::ParseDispoCode mapDispCode.size = %d", mapDispCode.size());
    m_mapDispCodesList[strMlhgName] = mapDispCode;

    return true;
}

bool CACDModelMTSW::IsAllowMultipleLogin()
{
    return m_bMultipleLoginsAllowed;
}

void CACDModelMTSW::GetCurReason(yl::string & strReasonName)
{
    YLHashMap<yl::string, yl::string>::iterator it = m_mapUnavailabilityReason.begin();
    for (; it != m_mapUnavailabilityReason.end(); it ++)
    {
        if (it->second == m_strCurUnavailReason)
        {
            strReasonName = it->first;
            break;
        }
    }
}

void CACDModelMTSW::SetCurReason(const yl::string & strReason)
{
    m_strTempUnavailReason = m_mapUnavailabilityReason[strReason];
}

bool CACDModelMTSW::SetDispCode(yl::string strCode, yl::string strMlhgName)
{
    if (!strCode.empty())
    {
        yl::string strCurDispCode = m_mapDispCodesList[strMlhgName][strCode];
        char szBuf[32] = {0};
        sscanf(strCurDispCode.c_str(), "%[^0-9]%d", szBuf, &m_nDispCodeIndex);
    }

    if (m_nDispCodeIndex == -1)
    {
        m_nDispCodeIndex = 0;
    }

    m_strCurMLHGName = strMlhgName;

    // 重新改变状态
    ReqChangeACDStatus(m_eChangeDelayAcdAction);
    return true;
}

//{"Name":{"_":"avg_qing_tm"},"Values":{"Value":[{"_":"0.0"}]}},
yl::string CACDModelMTSW::GetQueueStatisticValue(cJSON * pObj)
{
    if (pObj == NULL)
    {
        return "";
    }

    cJSON * pValue = cJSON_GetObjectItem(pObj, "Values");
    if (pValue == NULL)
    {
        return "";
    }

    pValue = cJSON_GetObjectItem(pValue, "Value");
    if (pValue == NULL)
    {
        return "";
    }

    for (int i = 0 ; i < cJSON_GetArraySize(pValue); i++)
    {
        pValue = cJSON_GetArrayItem(pValue, i);
        if (pValue == NULL)
        {
            continue;
        }

        pValue = cJSON_GetObjectItem(pValue, "_");
        if (pValue != NULL
                && pValue->valuestring != NULL)
        {
            return pValue->valuestring;
        }
    }
    return "";
}

bool CACDModelMTSW::IsCurrentDataType(yl::string & strDataType, int & nType)
{
    bool bIsCurrentType = true;
    switch (nType)
    {
    case EMTSWLOGDTYPE_DispCode:
        {
            bIsCurrentType = strDataType == kszDataTypeAutomatedCallDistribution ? true : false;
        }
        break;
    default:
        break;
    }

    return bIsCurrentType;
}

bool CACDModelMTSW::ParseQueueDetail(cJSON * pObj)
{
    if (pObj == NULL)
    {
        return false;
    }

    cJSON * pObject = cJSON_GetObjectItem(pObj, "Objects");
    CHECK_NULL(pObject, false);
    pObject = cJSON_GetObjectItem(pObject, "Object");
    CHECK_NULL(pObject, false);

    for (int i = 0 ; i < cJSON_GetArraySize(pObject); i++)
    {
        cJSON * pObjectData = cJSON_GetArrayItem(pObject, i);
        CHECK_NULL(pObjectData, false);
        char * szIdentifiers = cJSON_GetFistChildValueByObjectName(pObjectData, "Identifiers");
        CHECK_NULL(szIdentifiers, false);
        yl::string strMlghName = szIdentifiers;
        yl::string::size_type nIndex = 0;
        if ((nIndex = strMlghName.rfind('/')) != yl::string::npos)
        {
            strMlghName = strMlghName.substr(nIndex + 1);
        }

        cJSON * pStatistic = cJSON_GetObjectItem(pObjectData, "Statistics");
        CHECK_NULL(pStatistic, false);
        pStatistic = cJSON_GetObjectItem(pStatistic, "Statistic");
        CHECK_NULL(pStatistic, false);
        SMLGHQueuesData sQueueData;
        int nAVGWaitingTime = 0;
        for (int j = 0 ; j < cJSON_GetArraySize(pStatistic); j++)
        {
            cJSON * pData = cJSON_GetArrayItem(pStatistic, j);
            if (pData == NULL)
            {
                continue;
            }
            char * szName = cJSON_GetFistChildValueByObjectName(pData, "Name");
            if (szName == NULL)
            {
                continue;
            }
            yl::string strName = szName;

            if (strName == "avg_qing_tm"
                    || strName == "avg_alert_time")
            {
                nAVGWaitingTime += atoi(GetQueueStatisticValue(pData).c_str());
            }
            else if (strName == "all_incmg_cls_alrt")
            {
                sQueueData.nCountInQueue = atoi(GetQueueStatisticValue(pData).c_str());
            }
            else if (strName == "members_agents_curr_logged_in")
            {
                sQueueData.nLogged = atoi(GetQueueStatisticValue(pData).c_str());
            }
        }

        if (nAVGWaitingTime <= 2)
        {
            nAVGWaitingTime = 0;
        }
        char szTemp[16] = {0};
        sprintf(szTemp, "%02d:%02d", nAVGWaitingTime / 60, nAVGWaitingTime % 60);
        sQueueData.strAVGWaitingTime = szTemp;

        // 获取登录状态
        for (int j = 0; j < m_vecMLHG.size(); j++)
        {
            if (m_vecMLHG[j].strName == strMlghName)
            {
                sQueueData.bLoggedIn = m_vecMLHG[j].IsLogin();
                break;
            }
        }
        m_mapQueuesList[strMlghName] = sQueueData;
    }

    return true;
}

bool CACDModelMTSW::SetTimer()
{
    CALLCENTER_INFO("CACDModelMTSW::SetTimer m_nRefreshInterval=%d", m_nRefreshInterval);
    if (m_objTimer.IsTimerRuning())
    {
        //关闭超时计时器
        m_objTimer.KillTimer();
    }

    // 只有当有高级acd时才需要去请求状态
    if (BeHasPreminumAcd())
    {
        // 启动超时计时器.
        m_objTimer.SetTimer((m_nRefreshInterval < 5 ? 5 : m_nRefreshInterval) * 1000,
                            NULL, MK_MSG_ENTRY(this, CACDModelMTSW::OnTimeoutMessage));
    }

    return true;
}

BOOL CACDModelMTSW::OnTimeoutMessage(msgObject & refObj)
{
    if (!talklogic_SessionExist()
            && !IsAllMLHGLogout())
    {
        // 下载acd状态
        DownloadData(EMTSWLOGDTYPE_AcdState);
    }
    return TRUE;
}

ACD_STATUS CACDModelMTSW::GetAcdStatus()
{
    // 如果所有的组都登出，则状态为登出状态
    if (IsAllMLHGLogout())
    {
        return AS_IDLE;
    }

    for (int i = 0 ; i < m_vecMLHG.size(); i++)
    {
        if (m_vecMLHG[i].IsLogin())
        {
            return m_eACDLineState;
        }
    }

    // 都登出的情况下状态为idle
    return AS_IDLE;
}

bool CACDModelMTSW::IsDispCodeRequired(yl::string strMlhgName)
{
    return m_mapDispCodesRequired[strMlhgName];
}

bool CACDModelMTSW::IsAllMLHGLogout()
{
    for (int i = 0 ; i < m_vecMLHG.size(); i++)
    {
        if (m_vecMLHG[i].IsLogin())
        {
            return false;
        }
    }

    return true;
}

yl::string CACDModelMTSW::GetLastAnseredMHLG()
{
    return m_strLastAnsweredMHLG;
}



#endif
