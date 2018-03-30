#include "exedata_inc.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#define _DEBUG_THIS_MOD  0

#define THROW_TYPE_ERROR  "CExecutiveAssistantData type error"

#define OBJ_DELETE(pobj) \
    if (pobj) { delete pobj; pobj = 0; }

//   Action类型                   XSI关键字字段                         用户类型
static const ActionItem ActionMap[] =
{
    {EAA_ExeCommon,      "ExecutiveAssistants",    EET_Executive                },
    {EAA_Filter,      "ExecutiveCallFiltering",    EET_Executive | EET_Assistant  },
    {EAA_Screen,      "ExecutiveScreening",    EET_Executive | EET_Assistant  },
    {EAA_Alter,      "ExecutiveAlerting",    EET_Executive | EET_Assistant  },
    {EAA_AssisCommon,      "ExecutiveAssistant",    EET_Assistant                },
    {EAA_CallPush,      "executiveAssistantCallPush",    EET_Assistant                },
    {EAA_CallInitiation,      "executiveAssistantInitiateCall",    EET_Assistant                },
    {EAA_SilentMonitoe,      "",    EET_Executive                },
};

static LRESULT OnMessageReceive(msgObject & msg)
{
    return g_refExeAssisProcessor.OnMessage(msg);
}

CExecutiveAssistantData::CExecutiveAssistantData() : m_iLine(0), m_iSplitCount(0)
{
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnMessageReceive);

    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        m_vecExeNumber.push_back("");
    }
}

CExecutiveAssistantData::~CExecutiveAssistantData()
{
    for (MapExeAssia::iterator it = m_mapExeAssisInfo.begin(); it != m_mapExeAssisInfo.end(); ++it)
    {
        OBJ_DELETE(it->second);
    }

    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnMessageReceive);
}

LRESULT CExecutiveAssistantData::OnMessage(msgObject & msg)
{
    XSI_INFO("CExecutiveAssistantData::OnMessage msg=[%#x][%d] w=[%d] l=[%d]", msg.message, msg.message,
             msg.wParam, msg.lParam);
    BOOL bHanded = TRUE;
    switch (msg.message)
    {
    case ACCOUNT_STATUS_CHANGED:
        {
            int iAccount = msg.wParam;

            // 最终注册状态或者被禁用则需要清理Map数据
            if (acc_IsInFinalRegisterState(iAccount)
                    || !acc_IsAccountEnable(iAccount))
            {
                MapExeAssia::iterator it = m_mapExeAssisInfo.find(iAccount);
                if (it != m_mapExeAssisInfo.end())
                {
                    XSI_INFO("Clear map[%d]", iAccount);
                    OBJ_DELETE(it->second);
                    m_mapExeAssisInfo.erase(it);
                }
            }
        }
        break;
    default:
        bHanded = FALSE;
        break;
    }

    return bHanded;
}

bool CExecutiveAssistantData::GetData(CExeAssisInspector * pData, int iAccount)
{
    if (pData == NULL)
    {
        return false;
    }

    ExeAssisData * pMapData = GetInfoById(iAccount);
    if (pMapData == NULL || pMapData->IsInvalid())
    {
        return false;
    }

    XSI_INFO("GetData type=[%d] account=[%d]", pData->GetAction(), iAccount);
    switch (pData->GetAction())
    {
    case EAA_Filter:
        {
            *static_cast<CExeFilterInfo *>(pData) = pMapData->tExeData.tFilter;
        }
        break;
    case EAA_Screen:
        {
            *static_cast<CExeScreenInfo *>(pData) = pMapData->tExeData.tScreen;
        }
        break;
    case EAA_Alter:
        {
            *static_cast<CExeAlterInfo *>(pData) = pMapData->tExeData.tAlter;
        }
        break;
    case EAA_ExeCommon:
        {
            *static_cast<CExeCommon *>(pData) = pMapData->tExeData.tCommonData;
        }
        break;
    case EAA_AssisCommon:
        {
            *static_cast<CAssCommon *>(pData) = pMapData->tAssData.tCommonData;
        }
        break;
    default:
        return false;
    }

    return true;
}

bool CExecutiveAssistantData::SetData(const CExeAssisInspector * pData, int iAccount,
                                      ExeAssis_Action eAction/* = EAA_AutoSelect*/)
{
    // 不能改变 用户类型
    if (Save2Local(pData, iAccount))
    {
        // 分片标志归0
        m_iSplitCount = 0;
        ExeAssis_Action eInnerAction = pData->GetAction();

        XSI_INFO("SetData inaction=[%d] action=[%d] account=[%d]", eInnerAction, eAction, iAccount);

        if (eInnerAction == EAA_AssisCommon && EAA_AutoSelect != eAction)
        {
            // Executive必须分片，将链表拆开一个一个发
            bool bRet = false;
            int iCount = m_ExeAssInfo.tAssData.tCommonData.listData.size();
            for (int iIndex = 0; iIndex < iCount; ++iIndex)
            {
                yl::string strFile = XsiAction::GetRandomFile();
                if (SaveRequestCmdToFileSplit(strFile, iIndex, iAccount))
                {
                    // 设置分片序号
                    if (g_refExeAssisProcessor.CreateExeXsiAction(iAccount, eInnerAction, false, "", strFile,
                            iIndex) != NULL)
                    {
                        bRet = true;
                        SplitRefOperator(true);

                        XSI_INFO("SetData split succ index=[%d]", iIndex);
                    }
                }
            }

            return bRet;
        }

        yl::string strFile = XsiAction::GetRandomFile();
        if (SaveRequestCmdToFile(strFile, eInnerAction, iAccount))
        {
            // 设置-1用于区分分片序号
            return g_refExeAssisProcessor.CreateExeXsiAction(iAccount, eInnerAction, false, "",
                    strFile) != NULL;
        }
    }

    return false;
}

bool CExecutiveAssistantData::Save2Local(const CExeAssisInspector * pData, int iAccount)
{
    if (pData == NULL || iAccount != m_iLine)
    {
        return false;
    }

    // 先清空再设置
    m_ExeAssInfo.Clean();
    int iType = GetUserType(pData->GetAction());

    if (!m_ExeAssInfo.SetType(iType))
    {
        return false;
    }

    switch (pData->GetAction())
    {
    case EAA_Filter:
        {
            m_ExeAssInfo.tExeData.tFilter = *static_cast<const CExeFilterInfo *>(pData);
        }
        break;
    case EAA_Screen:
        {
            m_ExeAssInfo.tExeData.tScreen = *static_cast<const CExeScreenInfo *>(pData);
        }
        break;
    case EAA_Alter:
        {
            m_ExeAssInfo.tExeData.tAlter = *static_cast<const CExeAlterInfo *>(pData);
        }
        break;
    case EAA_AssisCommon:
        {
            m_ExeAssInfo.tAssData.tCommonData = *static_cast<const CAssCommon *>(pData);
        }
        break;
    case EAA_ExeCommon:
        {
            m_ExeAssInfo.tExeData.tCommonData = *static_cast<const CExeCommon *>(pData);
        }
        break;
    default:
        return false;
    }

    return true;
}

bool CExecutiveAssistantData::Local2Map(int iAccount, ExeAssis_Action eActionType,
                                        int iIndex/* = -1*/)
{
    XSI_INFO("Local2Map section=[%d] account=[%d] index=[%d]", eActionType, iAccount, iIndex);

    ExeAssisData * pData = GetInfoById(iAccount);
    if (pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    switch (eActionType)
    {
    case EAA_ExeCommon:
        {
            pData->tExeData.tCommonData = m_ExeAssInfo.tExeData.tCommonData;
        }
        break;
    case EAA_Filter:
        {
            pData->tExeData.tFilter = m_ExeAssInfo.tExeData.tFilter;
        }
        break;
    case EAA_Screen:
        {
            pData->tExeData.tScreen = m_ExeAssInfo.tExeData.tScreen;
        }
        break;
    case EAA_Alter:
        {
            pData->tExeData.tAlter = m_ExeAssInfo.tExeData.tAlter;
        }
        break;
    case EAA_AssisCommon:
        {
            int iCount = chMIN(m_ExeAssInfo.tAssData.tCommonData.listData.size(),
                               pData->tAssData.tCommonData.listData.size());
            if (iIndex >= 0 && iIndex < iCount)
            {
                pData->tAssData.tCommonData.listData[iIndex] = m_ExeAssInfo.tAssData.tCommonData.listData[iIndex];
            }
        }
        break;
    default:
        return false;
    }
    return true;
}

ExeAssisData * CExecutiveAssistantData::GetInfoById(int iAccount)
{
    MapExeAssia::iterator it = m_mapExeAssisInfo.find(iAccount);
    if (it != m_mapExeAssisInfo.end())
    {
        return it->second;
    }

    return NULL;
}

ExeAssis_Action CExecutiveAssistantData::GetActionType(const yl::string & strAction)
{
    for (int i = 0; i < (sizeof(ActionMap) / sizeof(ActionItem)); ++i)
    {
        if (strAction == ActionMap[i].strAction)
        {
            return ActionMap[i].eAction;
        }
    }

    return EAA_None;
}

yl::string CExecutiveAssistantData::GetActionByType(ExeAssis_Action eActionType)
{
    for (int i = 0; i < (sizeof(ActionMap) / sizeof(ActionItem)); ++i)
    {
        if (eActionType == ActionMap[i].eAction)
        {
            return ActionMap[i].strAction;
        }
    }

    return yl::string();
}

bool CExecutiveAssistantData::SetUserType(EEA_TYPE eType, int iAccount,
        const CExeAssisInspector * pInfo)
{
    if (iAccount < 0 || iAccount >= m_vecExeNumber.size())
    {
        XSI_ERR(THROW_TYPE_ERROR);
        return false;
    }

    ExeAssisData * pData = GetInfoById(iAccount);

    XSI_INFO("SetUserType user=[%d] line=[%d] addr=[%p]", eType, iAccount, pData);

    if (pData == NULL)
    {
        // 创建逻辑层数据
#ifndef IF_NO_EXCEPTION
        try
        {
#endif // ENABLE_NOEXCEPTION
            pData = new ExeAssisData;
            m_mapExeAssisInfo[iAccount] = pData;
#ifndef IF_NO_EXCEPTION
        }
        catch (...)
        {
            XSI_ERR(THROW_FATAL_ERROR);
        }
#endif // IF_NO_EXCEPTION
    }

    // 设置账号角色类型
    if (!pData->SetType(eType))
    {
        return false;
    }

    XSI_INFO("SetUserType type=[%d] action=[%d] ", eType, pInfo->GetAction());

    if (eType & EET_Executive)
    {
        // 类型检测
        if (pInfo->GetAction() != EAA_Filter)
        {
            XSI_ERR(THROW_TYPE_ERROR);
            return false;
        }

        pData->tExeData.tFilter = *static_cast<const CExeFilterInfo *>(pInfo);
    }
    else if (eType & EET_Assistant)
    {
        if (pInfo->GetAction() != EAA_AssisCommon)
        {
            XSI_ERR(THROW_TYPE_ERROR);
            return false;
        }

        // 功能键同步不修改Opt in状态，仍以本地存储的为准
        ExeAssisList listData = pData->tAssData.tCommonData.listData;
        pData->tAssData.tCommonData = *static_cast<const CAssCommon *>(pInfo);

        ExeAssisList & maplistData = pData->tAssData.tCommonData.listData;
        for (int i = 0; i < maplistData.size(); ++i)
        {
            for (int j = 0; j < listData.size(); ++j)
            {
                if (maplistData[i].ItemName.strUserId == listData[j].ItemName.strUserId)
                {
                    maplistData[i].bOptIn = listData[j].bOptIn;
                }
            }
        }
    }

    PrintInfo();
    return true;
}

bool CExecutiveAssistantData::CheckAction(ExeAssis_Action eActionType, int iAccount)
{
    ExeAssisData * pData = GetInfoById(iAccount);
    if (pData == NULL || pData->IsInvalid())
    {
        XSI_WARN("CheckAccessAuthority failed for not inited with account=[%d] action=[%d]", iAccount,
                 eActionType);
        return false;
    }

    XSI_INFO("CheckAccessAuthority account=[%d] action=[%d] user=[%d]", iAccount, eActionType,
             pData->eType);
    // 操作权限必须和账户权限匹配
    return (GetUserType(eActionType) & pData->eType);
}

int CExecutiveAssistantData::GetUserType(int iAccount)
{
    ExeAssisData * pData = GetInfoById(iAccount);
    if (pData != NULL && pData->IsValid())
    {
        return pData->eType;
    }

    return EET_None;
}

int CExecutiveAssistantData::GetUserType(ExeAssis_Action eActionType)
{
    for (int i = 0; i < (sizeof(ActionMap) / sizeof(ActionItem)); ++i)
    {
        if (eActionType == ActionMap[i].eAction)
        {
            return ActionMap[i].iAccessMode;
        }
    }

    return EET_None;
}

yl::string CExecutiveAssistantData::GetInitiationNumber(int iAccount)
{
    int iCount = GetExecutiveListSize(iAccount);
    if (iCount == 1)
    {
        // 只有一个Executive的情况下不需要选，直接呼出就好
        ExeAssisData * pData = GetInfoById(iAccount);
        if (pData != NULL && pData->IsAssistant())
        {
            const CUserName & userInfo = pData->tAssData.tCommonData.listData[0].ItemName;
            // 优先用EXT号码
            return userInfo.strExt.empty() ? userInfo.strUserId : userInfo.strExt;
        }
    }
    else if (iCount > 1)
    {
        if (iAccount >= 0 && iAccount < m_vecExeNumber.size())
        {
            return m_vecExeNumber[iAccount];
        }
    }

    return yl::string();
}

bool CExecutiveAssistantData::SetCallInitiationNumber(const yl::string & strNumber, int iAccount)
{
    if (iAccount >= 0 && iAccount < m_vecExeNumber.size())
    {
        ExeAssisData * pData = GetInfoById(iAccount);
        if (pData != NULL && pData->IsAssistant())
        {
            ExeAssisList & listData = pData->tAssData.tCommonData.listData;
            for (ExeAssisListIt it = listData.begin(); it != listData.end(); ++it)
            {
                const CUserName & item = it->ItemName;
                // 优先使用Ext发起Initiation
                if (item.strUserId == strNumber && !item.strExt.empty())
                {
                    m_vecExeNumber[iAccount] = item.strExt;
                    return true;
                }
            }
        }

        m_vecExeNumber[iAccount] = strNumber;
        return true;
    }

    return false;
}

int CExecutiveAssistantData::GetExecutiveListSize(int iAccount)
{
    ExeAssisData * pData = GetInfoById(iAccount);
    if (pData == NULL || !pData->IsAssistant())
    {
        return 0;
    }

    return pData->tAssData.tCommonData.listData.size();
}

int CExecutiveAssistantData::GetAccountListByType(YLVector<int> & listData, EEA_TYPE eType)
{
    if (eType == EET_None)
    {
        return 0;
    }

    MapExeAssia::iterator it = m_mapExeAssisInfo.begin();
    for (; it != m_mapExeAssisInfo.end(); ++it)
    {
        if (it->second == NULL || it->second->IsInvalid())
        {
            continue;
        }
        if (it->second->eType & eType)
        {
            listData.push_back(it->first);
        }
    }

    return listData.size();
}

bool CExecutiveAssistantData::ParseReponseCmdFromFile(const yl::string & strFilePath, int iAccount)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status != status_ok)
    {
        return false;
    }

    xml_node nodeRoot = doc.first_child();
    if (nodeRoot.empty())
    {
        return false;
    }

    ExeAssis_Action eActionType = GetActionType(nodeRoot.name());
    ExeAssisData * pData = GetInfoById(iAccount);
    if (pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    bool bParseSucc = false;
    switch (eActionType)
    {
    case EAA_ExeCommon:
        bParseSucc = ParseAssistantList(nodeRoot, pData);
        break;
    case EAA_Filter:
        bParseSucc = ParseFiltering(nodeRoot, pData);
        break;
    case EAA_Screen:
        bParseSucc = ParseScreening(nodeRoot, pData);
        break;
    case EAA_Alter:
        bParseSucc = ParseAlter(nodeRoot, pData);
        break;
    case EAA_AssisCommon:
        bParseSucc = ParseExecutiveList(nodeRoot, pData);
        break;
    default:
        break;
    }

    XSI_INFO("ParseReponseCmdFromFile action=[%d][%s] account=[%d] result=[%d]", eActionType,
             nodeRoot.name(), iAccount, bParseSucc);
    PrintInfo();
    PrintFile(strFilePath, iAccount);

    return bParseSucc;
}

bool CExecutiveAssistantData::SaveRequestCmdToFile(const yl::string & strFilePath,
        ExeAssis_Action eAction, int iAccount)
{
    //增加Xml头
    xml_document doc;
    xml_node nodeRoot;

    if (!SaveCommonHeader(doc, eAction, nodeRoot))
    {
        return false;
    }

    bool bSaveSucc = false;
    switch (eAction)
    {
    case EAA_ExeCommon:
        bSaveSucc = FillAssistantList(nodeRoot, &m_ExeAssInfo);
        break;
    case EAA_Filter:
        bSaveSucc = FillFiltering(nodeRoot, &m_ExeAssInfo);
        break;
    case EAA_Screen:
        bSaveSucc = FillScreening(nodeRoot, &m_ExeAssInfo);
        break;
    case EAA_Alter:
        bSaveSucc = FillAltering(nodeRoot, &m_ExeAssInfo);
        break;
    case EAA_AssisCommon:
        bSaveSucc = FillExecutiveList(nodeRoot, &m_ExeAssInfo, GetInfoById(iAccount));
        break;
    default:
        break;
    }

    if (bSaveSucc)
    {
        doc.save_file(strFilePath.c_str());

        PrintInfo();
        PrintFile(strFilePath, iAccount);
    }

    return bSaveSucc;
}

bool CExecutiveAssistantData::SaveRequestCmdToFileSplit(const yl::string & strFilePath, int iIndex,
        int iAccount)
{
    //增加Xml头
    xml_document doc;
    xml_node nodeRoot;

    if (!SaveCommonHeader(doc, EAA_AssisCommon, nodeRoot))
    {
        return false;
    }

    if (FillExecutiveList(nodeRoot, &m_ExeAssInfo, GetInfoById(iAccount), iIndex))
    {
        doc.save_file(strFilePath.c_str());

        PrintInfo();
        PrintFile(strFilePath, iAccount);

        return true;
    }

    return false;
}

bool CExecutiveAssistantData::SaveCommonHeader(xml_document & doc, ExeAssis_Action eAction,
        xml_node & nodeRoot)
{
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "UTF-8");

    yl::string strAction = GetActionByType(eAction);
    if (strAction.empty())
    {
        return false;
    }

    //增加根结点
    nodeRoot = doc.append_child(strAction.c_str());

    // 设置属性
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");
    nodeRoot.append_attribute("xmlns:xsi1", "http://www.w3.org/2001/XMLSchema-instance");

    return true;
}

//yl::string CExecutiveAssistantData::GetSipNumber(const yl::string& strNum)
//{
//  char szSIPName[128] = {0};
//  char szServerName[128] = {0};
//  GetSipInfoFromURI(strNum, szSIPName, szServerName);
//
//  return szSIPName;
//}

void CExecutiveAssistantData::SplitRefOperator(bool bAdd /*= true*/,
        ExeAssis_Action eAction /*= EAA_AutoSelect*/)
{
    // 处理引用计数
    if (bAdd)
    {
        ++m_iSplitCount;
    }
    else
    {
        --m_iSplitCount;

        if (m_iSplitCount == 0)
        {
            // 通告该组操作完成
            etl_NotifyApp(false, XSI_MESSAGE_FINISH_ALL, eAction, m_iLine);
        }
    }
}

bool CExecutiveAssistantData::IsExecutiveNumber(const yl::string & strNumber, int iAccount)
{
    if (!CheckAction(EAA_CallPush, iAccount))
    {
        return false;
    }

    XSI_INFO("IsExecutiveNumber num=[%s] account=[%d]", strNumber.c_str(), iAccount);

    if (!strNumber.empty())
    {
        ExeAssisData * pData = GetInfoById(iAccount);
        if (pData == NULL || !pData->IsAssistant())
        {
            return false;
        }

        ExeAssisList & listData = pData->tAssData.tCommonData.listData;
        for (ExeAssisListIt it = listData.begin(); it != listData.end(); ++it)
        {
            if (strNumber == it->ItemName.strUserId
                    || strNumber == it->ItemName.strExt)
            {
                return true;
            }
        }
    }

    return false;
}

void CExecutiveAssistantData::PrintInfo()
{
#if _DEBUG_THIS_MOD
    printf("\n   ----------[******** Debug ExecutiveAssistant print begin ******** ] mapsize=[%d] \n",
           m_mapExeAssisInfo.size());

    const char * pDsp = "[map] ";
    for (MapExeAssia::iterator it = m_mapExeAssisInfo.begin(); it != m_mapExeAssisInfo.end(); ++it)
    {
        XSI_INFO("[map]  line=[%d] initd=[%s]", it->first, it->second ? "false" : "true");
        PrintItem(pDsp, it->second);
    }

    PrintItem("[Cache] ", &m_ExeAssInfo);

    printf("   ----------[******** Debug ExecutiveAssistant print end   ******** ]\n\n");
#endif
}

void CExecutiveAssistantData::PrintItem(const char * pDsp, ExeAssisData * pData)
{
#if _DEBUG_THIS_MOD
    if (pData != NULL)
    {
        XSI_INFO("%s exe=[%d] assis=[%d] valid=[%d]", pDsp, pData->IsExecutive(), pData->IsAssistant(),
                 pData->IsValid());

        if (pData->IsExecutive())
        {
            CExecutive_Data & exeData = pData->tExeData;
            XSI_INFO("[Executive optin ] switch=[%d] Exe::Assis size=[%d]", exeData.tCommonData.bAllowOpt,
                     exeData.tCommonData.listData.size());
            XSI_INFO("[Executive filter] switch=[%d] mode=[%s] type=[%s]",
                     exeData.tFilter.bSwitchOn, exeData.tFilter.GetFilterMode(), exeData.tFilter.GetFilterType());

            int iIndex = 0;
            CExeFilterInfo::CriteriaList & targetCriteriaList = exeData.tFilter.tCriteriaList;
            for (CExeFilterInfo::CriteriaListIt it = targetCriteriaList.begin(); it != targetCriteriaList.end();
                    ++it)
            {
                XSI_INFO("[Executive filter list] index=[%d] name=[%s] active=[%d] uri=[%s]",
                         iIndex, it->strName.c_str(), it->bActive, it->strUri.c_str());
                iIndex++;
            }

            XSI_INFO("[Executive screen] switch=[%d] type=[%s] location=[%d][%d][%d]",
                     exeData.tScreen.bScreeningOn, exeData.tScreen.GetAlterType(),
                     exeData.tScreen.bAlertMobility, exeData.tScreen.bAlertAnywhere, exeData.tScreen.bAlertSCA);
            XSI_INFO("[Executive alter] mode=[%s] name=[%s][%s][%s] num=[%s][%s] next=[%d] recall=[%d] Rollover after=[%d] action=[%s] to=[%s]",
                     exeData.tAlter.GetAlterMode(), exeData.tAlter.GetCallInfoName(),
                     exeData.tAlter.strCustomCallName.c_str(),
                     exeData.tAlter.strUnicodeCustomCallName.c_str(), exeData.tAlter.GetCallInfoNumber(),
                     exeData.tAlter.strCustomCallNumber.c_str(),
                     exeData.tAlter.iAdvanceToNextAssistantAfter, exeData.tAlter.iCallPushRecallAfter,
                     exeData.tAlter.iRolloverAfter, exeData.tAlter.GetRolloverAction(),
                     exeData.tAlter.strRolloverTo.c_str());

            iIndex = 0;
            ExeAssisList & targetList = exeData.tCommonData.listData;
            for (ExeAssisListIt it = targetList.begin(); it != targetList.end(); ++it)
            {
                XSI_INFO("[Executive list] index=[%d] optin=[%d] pri=[%d] filter=[%d] userid=[%s] ext=[%s] name=[%s] dn=[%s] uri=[%s] first=[%s] last=[%s]",
                         iIndex, it->bOptIn, it->iPriority, it->bFilterOn, it->ItemName.strUserId.c_str(),
                         it->ItemName.strExt.c_str(),
                         it->ItemName.strUserName.c_str(), it->ItemName.strDN.c_str(), it->ItemName.strUri.c_str(),
                         it->ItemName.strFirstName.c_str(), it->ItemName.strLastName.c_str());
                iIndex++;
            }
        }

        if (pData->IsAssistant())
        {
            CAssistant_Data & exeData = pData->tAssData;
            XSI_INFO("[Assistant divert ] switch=[%d] number=[%s]  Assis::Exe size=[%d]",
                     exeData.tCommonData.bDivertOn, exeData.tCommonData.strDivertNumber.c_str(),
                     exeData.tCommonData.listData.size());

            XSI_INFO("[Assistant filter] switch=[%d] mode=[%s] type=[%s]",
                     exeData.tFilter.bSwitchOn, exeData.tFilter.GetFilterMode(), exeData.tFilter.GetFilterType());

            int iIndex = 0;
            CExeFilterInfo::CriteriaList & targetCriteriaList = exeData.tFilter.tCriteriaList;
            for (CExeFilterInfo::CriteriaListIt it = targetCriteriaList.begin(); it != targetCriteriaList.end();
                    ++it)
            {
                XSI_INFO("[Assistant filter list] index=[%d] name=[%s] active=[%d] uri=[%s]",
                         iIndex, it->strName.c_str(), it->bActive, it->strUri.c_str());
                iIndex++;
            }

            XSI_INFO("[Assistant screen] switch=[%d] type=[%s] location=[%d][%d][%d]",
                     exeData.tScreen.bScreeningOn, exeData.tScreen.GetAlterType(),
                     exeData.tScreen.bAlertMobility, exeData.tScreen.bAlertAnywhere, exeData.tScreen.bAlertSCA);
            XSI_INFO("[Assistant alter ] mode=[%s] name=[%s][%s][%s] num=[%s][%s] next=[%d] recall=[%d] Rollover after=[%d] action=[%s] to=[%s]",
                     exeData.tAlter.GetAlterMode(), exeData.tAlter.GetCallInfoName(),
                     exeData.tAlter.strCustomCallName.c_str(),
                     exeData.tAlter.strUnicodeCustomCallName.c_str(), exeData.tAlter.GetCallInfoNumber(),
                     exeData.tAlter.strCustomCallNumber.c_str(),
                     exeData.tAlter.iAdvanceToNextAssistantAfter, exeData.tAlter.iCallPushRecallAfter,
                     exeData.tAlter.iRolloverAfter, exeData.tAlter.GetRolloverAction(),
                     exeData.tAlter.strRolloverTo.c_str());

            iIndex = 0;
            ExeAssisList & targetList = exeData.tCommonData.listData;
            for (ExeAssisListIt it = targetList.begin(); it != targetList.end(); ++it)
            {
                XSI_INFO("[Assistant list] index=[%d] optin=[%d] pri=[%d] filter=[%d] userid=[%s] ext=[%s] name=[%s] dn=[%s] uri=[%s] first=[%s] last=[%s]",
                         iIndex, it->bOptIn, it->iPriority, it->bFilterOn, it->ItemName.strUserId.c_str(),
                         it->ItemName.strExt.c_str(),
                         it->ItemName.strUserName.c_str(), it->ItemName.strDN.c_str(), it->ItemName.strUri.c_str(),
                         it->ItemName.strFirstName.c_str(), it->ItemName.strLastName.c_str());
                iIndex++;
            }
        }
    }
#endif
}

void CExecutiveAssistantData::PrintFile(const yl::string & strFile, int iAccount)
{
#if _DEBUG_THIS_MOD
    XSI_INFO("--------------------------PrintFile In Line=[%d]-----------------------------------\n",
             iAccount);
    yl::string strCmd = commonAPI_FormatString("cat %s", strFile.c_str());
    system(strCmd.c_str());
    printf("\n");
    XSI_INFO("\n--------------------------------------------------------------------------------");
#endif
}
#endif


