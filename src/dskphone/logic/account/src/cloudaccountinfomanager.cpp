#ifdef IF_FEATURE_CLOUD
#include "cloudaccountinfomanager.h"
#include "threadtask/include/networkfileaction.h"
#include "taskaction/modtaskaction.h"
#include "cloudaccountaction.h"
#include "account/include/modaccount.h"

#include "contacts/directory/include/moddirinterface.h"


#define kszCloudInfoNodeRoot "accountlist"

#define kszNodeAccountItem "account"

#define kszFileSavedName  TEMP_PATH "CloudAccount.xml"

#define kszDownLoadEmail "user-account/email"
#define kszPinDownLoadData "/pin"
#define kszPinRemove "/pin/revoke"

#define kszAesDefaultKey "1105_VCCloud_Key"

#define MAX_INFO_NUM 10

#define ACCOUNT_SERVER_PARAM1 1
#define ACCOUNT_SERVER_PARAM2 2

static bool CompareAccountInfo(CloudAccountInfo & stPreAccount, CloudAccountInfo & stNextAccount);

CCloudAccountInfoManager::CCloudAccountInfoManager()
{
    //m_listHistory.clear();
}

CCloudAccountInfoManager::~CCloudAccountInfoManager()
{

}

CCloudAccountInfoManager & CCloudAccountInfoManager::GetInstance()
{
    static CCloudAccountInfoManager objCloudAccountManager;
    return objCloudAccountManager;
}

bool CCloudAccountInfoManager::Init()
{
    m_eStatus = STATUS_LOGIN_NONE;

    // 加载历史记录
    LoadCloudAccount(m_listHistory, szCloudAccountHistoryFile);

    // 加载登录请求
    LoadCloudAccount(m_listReq, szCloudAccountRequestFile);

    // 加载默认配置：第三方平台没有开放端口、传输方式等配置，需要从配置里面取
    LoadCloudAccount(m_listDefault, szCloudAccountDefaultFile);

    // 检测云账号配置
    CheckCloud(true);

    if (netIsNetworkReady())
    {
        // 网络正常
        ProcessCloudLogin();
    }

    return true;
}

void CCloudAccountInfoManager::AddUrlParameter(yl::string & strUrl, const yl::string & strName,
        const yl::string & strValue,
        bool isPreMatch, bool isCaseSensitive/* = true*/, bool isAddUrlParamer /*= true*/)
{
    if (isAddUrlParamer)
    {
        if (strUrl.find("?") == yl::string::npos)
        {
            strUrl.append("?");
        }
        else
        {
            strUrl.append("&");
        }
    }
    else
    {
        if (!strUrl.empty())
        {
            strUrl.append("&");
        }
    }

    strUrl.append(strName);
    strUrl.append("=");
    if (isPreMatch)
    {
        strUrl.append("*");
    }
    strUrl.append(strValue);
    if (isPreMatch)
    {
        strUrl.append("*");
    }

    if (!isCaseSensitive)
    {
        //不区分大小写
        strUrl.append("/i");
    }
}

void CCloudAccountInfoManager::ProcessCloudLoginMsg(msgObject & objMessage)
{
    // 处理配置变更
    if (!acc_IsCloudEnable())
    {
        CheckCloud(true);

        // 云功能不可用，不处理消息
        return;
    }

    m_listReq.clear();
    LoadCloudAccount(m_listReq, szCloudAccountRequestFile);

    ProcessCloudLogin();
}

bool CCloudAccountInfoManager::ReqLogin(CloudAccountInfo & stInfo)
{
    if (!netIsNetworkReady())
    {
        return false;
    }

    ACCOUNT_INFO("Req login cloud [%d] type[%d]", stInfo.m_nAccountId, stInfo.m_eType);
    stInfo.m_eStatus = STATUS_LOGIN_REQUEST;

    // 替换新的请求
    if (!ReplaceCloudAccountInfo(stInfo))
    {
        ACCOUNT_ERR("repleace fail");
    }

    ProcessCloudLogin();

    return true;
}

void CCloudAccountInfoManager::ProcessCloudLogin()
{
    if (!acc_IsCloudEnable())
    {
        return;
    }

    bool bHandled = false;
    YLVector<int> listTmp;
    for (int i = 0; i < m_listReq.size(); i++)
    {
        CloudAccountInfo & info = m_listReq[i];

        bool bIgnore = false;
        // 相同账号id的登录请求只处理第一次
        for (int j = 0; j < listTmp.size(); j++)
        {
            int nId = listTmp[j];

            if (nId == info.m_nAccountId)
            {
                bIgnore = true;
                break;
            }
        }

        // 记录已经处理过的账号
        listTmp.push_back(info.m_nAccountId);

        ACCOUNT_INFO("ignore[%d] [%d] state[%d] type[%d]", bIgnore, info.m_nAccountId, info.m_eStatus,
                     info.m_eType);
        if (bIgnore)
        {
            continue;
        }

        if (info.m_eStatus != STATUS_LOGIN_REQUEST)
        {
            continue;
        }

        // 修改默认配置
        FixDefaultInfo(info);

        if (info.m_eType == ACCOUNT_YEALINK)
        {
            bHandled |= CloudYLAccountLogin(info);
        }
        else
        {
            bHandled |= Cloud3rdAccountLogin(info);
        }
    }

    return;
}

bool CCloudAccountInfoManager::ProcessBeforeLogin(int nAccountId, SipAccountType eType)
{
    if (nAccountId == -1)
    {
        return false;
    }

    if (!netIsNetworkReady())
    {
        etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId, REG_NETWORK_UNAVAILABLE);
        return false;
    }

    // Yealink云限制登录一个
    if (ACCOUNT_YEALINK == eType)
    {
        int iId = acc_GetRegisterYlCloudAccount();
        if (iId != -1
                && iId != nAccountId
                && acc_IsAccountAvailable(iId))
        {
            ACCOUNT_INFO("CCloudAccountInfoManager::CloudAccountLogin Has Login Account");
            return false;
        }
    }

    m_stAccountInfo.Clear();

    return true;
}

bool CCloudAccountInfoManager::CloudYLAccountLogin(const CloudAccountInfo & stInfo)
{
    ACCOUNT_INFO("Yl account login[%d]", stInfo.m_nAccountId);
    if (stInfo.m_strUserName.empty()
            && !stInfo.m_strPassWord.empty())
    {
        // PIN登录方式
        return CloudYLPinLogin(stInfo.m_nAccountId, stInfo.m_strPassWord);
    }

    // 其他情况, 则下载失败.
    return CloudYLUserLogin(stInfo);
}

bool CCloudAccountInfoManager::Cloud3rdAccountLogin(const CloudAccountInfo & stInfo)
{
    // 登录第三方云账号
    return SaveAccountLoginData(stInfo, true);
}

bool CCloudAccountInfoManager::CloudYLPinLogin(int nAccountId, const yl::string & strPinCode)
{
    CloudAccountInfo stInfo;
    stInfo.m_strPinCode = strPinCode;
    stInfo.m_nAccountId = nAccountId;
    stInfo.m_eType = ACCOUNT_YEALINK;
    stInfo.m_eStatus = STATUS_LOGIN_REQUEST;

    FixDefaultInfo(stInfo);

    return CloudYLPinLogin(stInfo);
}

bool CCloudAccountInfoManager::CloudYLPinLogin(CloudAccountInfo & stInfo)
{
    ACCOUNT_INFO("Yl pin login [%d]", stInfo.m_nAccountId);
    if (!ProcessBeforeLogin(stInfo.m_nAccountId, ACCOUNT_YEALINK))
    {
        ACCOUNT_WARN("Not allow pin login");
        return false;
    }

    m_stAccountInfo = stInfo;

    yl::string strUrl = configParser_GetConfigString(kszCloudHttpServer);
    if (!strUrl.empty() && strUrl.substr(strUrl.length() - 1, 1) != "/")
    {
        strUrl.append("/");
    }
    strUrl.append(kszPinDownLoadData);

    // 生成本地保存路径.
    yl::string strFullPath(kszFileSavedName);

    yl::string strEncrypt = "";
    yl::string strRandomNum = GetRandomNumString();

    AddUrlParameter(strEncrypt, "pin", stInfo.m_strPinCode, false, true, false);
    AddUrlParameter(strEncrypt, "random", strRandomNum, false, true, false);

    strEncrypt = commonUnits_PwAesDataEncrypt(strEncrypt, kszAesDefaultKey);
    AddUrlParameter(strUrl, "pin", strEncrypt, false);

    ACCOUNT_INFO("CCloudAccountInfoManager::CloudPinLogin url[%s]", strUrl.c_str());

    if (!strUrl.empty())
    {
        CCloudAccountAction * pAction = new CCloudAccountAction(strFullPath.c_str(), strUrl.c_str());
        if (pAction != NULL)
        {
            m_eStatus = STATUS_LOGINING;
            m_nTaskId = (UINT)pAction;

            pAction->SetRead(true);  // 读取文件.
            pAction->SetCallback(this);  // 回调函数.
            pAction->SetSync(false);  // 异步.
            pAction->SetOptType(CLOUD_GET_PIN_INFO);
            pAction->SetExtraParam(stInfo.m_nAccountId);
            bool bRet = TaskAction_ExecAction(pAction);

            if (!bRet)
            {
                delete pAction;
            }

            return bRet;
        }
    }
    // 其他情况, 则下载失败.
    return false;
}

bool CCloudAccountInfoManager::CloudYLUserLogin(const CloudAccountInfo & stInfo)
{
    int nAccountId = stInfo.m_nAccountId;
    if (!ProcessBeforeLogin(nAccountId, stInfo.m_eType))
    {
        ACCOUNT_INFO("Not allow user login");
        return false;
    }

    m_stAccountInfo = stInfo;

    if (m_eStatus == STATUS_LOGINING)
    {
        // 取消登录请求
        m_nTaskId = 0;
        return false;
    }

    // web要求登录就写类型:先清空
    // 此处清空sip配置，防止云登录失败后，sip账号在显示上次的注册上状态
    SaveAccountLoginData(m_stAccountInfo,
                         false); //disable 账号是因为，可能sip先发来失败，然后在发来注册成功
    //configParser_SetCfgItemIntValue(kszAccountType, m_stAccountInfo.m_nAccountId, m_stAccountInfo.m_eType);

    yl::string strUrl = configParser_GetConfigString(kszCloudHttpServer);
    if (!strUrl.empty() && strUrl.substr(strUrl.length() - 1, 1) != "/")
    {
        strUrl.append("/");
    }
    strUrl.append(kszDownLoadEmail);

    // 生成本地保存路径.
    yl::string strFullPath(kszFileSavedName);


    yl::string strEncrypt = "";
    yl::string strRandomNum = GetRandomNumString();

    AddUrlParameter(strEncrypt, "phone", m_stAccountInfo.m_strUserName, false, true, false);
    AddUrlParameter(strEncrypt, "random", strRandomNum, false, true, false);

    strEncrypt = commonUnits_PwAesDataEncrypt(strEncrypt, kszAesDefaultKey);
    AddUrlParameter(strUrl, "phone", strEncrypt, false);

    ACCOUNT_INFO("CCloudAccountInfoManager::CloudAccountLogin url[%s]", strUrl.c_str());

    bool bHandled = false;
    if (!strUrl.empty())
    {
        CCloudAccountAction * pAction = new CCloudAccountAction(strFullPath.c_str(), strUrl.c_str());
        if (pAction != NULL)
        {
            m_eStatus = STATUS_LOGINING;
            m_nTaskId = (int)pAction;

            pAction->SetRead(true);  // 读取文件.
            pAction->SetCallback(this);  // 回调函数.
            pAction->SetSync(false);  // 异步.
            pAction->SetOptType(CLOUD_GET_EMAIL);
            pAction->SetExtraParam(nAccountId);
            bHandled = TaskAction_ExecAction(pAction);
            if (!bHandled)
            {
                ACCOUNT_ERR("yealink login task exe fail");
                delete pAction;
            }
        }
    }

    return bHandled;
}

bool CCloudAccountInfoManager::OpenXMLFile(const char * lpFile, xml_document & doc)
{
    if (NULL == lpFile
            || 0 == strlen(lpFile))
    {
        return false;
    }

    xml_parse_result ret = doc.load_file(lpFile, parse_default | parse_declaration);

    xml_node nodeXml = doc.child("xml");

    if (!nodeXml.empty())
    {
        xml_attribute AttrEncoding =  nodeXml.attribute("encoding");
        if (!AttrEncoding.empty())
        {
            yl::string strValue;
            strValue.assign(AttrEncoding.value(), strlen(AttrEncoding.value()));
            if (strValue.compare("ISO-8859-1") == 0)
            {
                ret = doc.load_file(lpFile, parse_default | parse_declaration, encoding_latin1);
            }
        }
    }

    return (status_ok == ret.status);
}

bool CCloudAccountInfoManager::MyLoadEmailFromFile(const yl::string & strFilePath,
        yl::string & strEmail, int & code)
{
    ACCOUNT_INFO("CCloudAccountInfoManager::MyLoadEmailFromFile [%s]", strFilePath.c_str());

#define kszNodeEmailList "EmailList"
#define kszNodeEmail "email"

    if (strFilePath.empty())
    {
        return false;
    }

    // 打开Xml文件.
    if (!pathFileExist(strFilePath.c_str()))
    {
        return false;
    }

    xml_document doc;
    OpenXMLFile(strFilePath.c_str(), doc);

    code = GetResultCodeFromXmlFile(doc);
    if (code != 0)
    {
        ACCOUNT_INFO("MyLoadEmailFromFile Error Code [%d]", code);
        return false;
    }

    yl::string strNodeName;

    xml_node rootNode = doc.child(kszNodeEmailList);
    if (!rootNode)
    {
        return false;
    }

    for (xml_node node = rootNode.child(kszNodeEmail);
            node;
            node = node.next_sibling(kszNodeEmail))
    {
        //目前只有一个Email
        strEmail = node.child_value();
        break;
    }

    return !strEmail.empty();
}


bool CCloudAccountInfoManager::MyLoadAccountInfoFromFile(const yl::string & strFilePath,
        CloudAccountInfo & stAccountInfo, int & code)
{
    ACCOUNT_INFO("Parse cloud account info[%s]", strFilePath.c_str());

#define kszNodeAccount "Account"
#define kszNodeUsername "username"
#define kszNodePassword "password"
#define kszNodeEmailList "EmailList"
#define kszNodeEmail "email"

    if (strFilePath.empty())
    {
        return false;
    }

    // 打开Xml文件.
    if (!pathFileExist(strFilePath.c_str()))
    {
        return false;
    }

    xml_document doc;
    OpenXMLFile(strFilePath.c_str(), doc);

    code = GetResultCodeFromXmlFile(doc);
    if (code != 0)
    {
        ACCOUNT_INFO("MyLoadAccountInfoFromFile Error Code [%d]", code);
        return false;
    }

    yl::string strNodeName;

    xml_node rootNode = doc.child(kszNodeAccount);
    if (!rootNode)
    {
        return false;
    }

    xml_node node = rootNode.child(kszNodeUsername);
    if (node)
    {
        stAccountInfo.m_strUserName = node.child_value();
    }

    node = rootNode.child(kszNodePassword);
    if (node)
    {
        yl::string strPsd = node.child_value();
        stAccountInfo.m_strPassWord = commonUnits_PwAesDataDecrypt(strPsd, kszAesDefaultKey);
    }

    node = rootNode.child(kszNodeEmailList);
    if (node)
    {
        for (xml_node emailNode = node.child(kszNodeEmail);
                emailNode;
                emailNode = emailNode.next_sibling(kszNodeEmail))
        {
            //目前只有一个Email
            yl::string strEmail = emailNode.child_value();
            stAccountInfo.m_strRegName = commonUnits_PwAesDataDecrypt(strEmail, kszAesDefaultKey);
            break;
        }
    }

    return true;
}


void CCloudAccountInfoManager::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    ACCOUNT_INFO("Cloud acount task action call back begin");
    if (pTaskAction == NULL)
    {
        return;
    }

    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        return;
    }

    // 返回结果的类型为请求网络文件.
    CCloudAccountAction * pNetworkAction =
        dynamic_cast<CCloudAccountAction *>(pTaskAction);
    if (pNetworkAction == NULL)
    {
        return;
    }

    //Post
    if (!pNetworkAction->IsRead())
    {
        return;
    }

    // 回调时，任务id已经变化就忽略掉此次请求
    if (m_nTaskId != (UINT)pNetworkAction
            || STATUS_CANCEL_LOGINING == m_eStatus)
    {
        // 如果下载回来，任务已经被取消，就使用当前内容从新登录
        remove(pNetworkAction->GetFilePath().c_str());

        if (STATUS_CANCEL_LOGINING != m_eStatus)
        {
            ACCOUNT_INFO("Login on tast call back");
            m_eStatus = STATUS_LOGIN_NONE;
            if (!CloudYLAccountLogin(m_stAccountInfo))
            {
                etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, m_stAccountInfo.m_nAccountId, REG_OTHER_ERROR);
            }
        }

        ACCOUNT_INFO("ignore call back");
        return;
    }

    if (!pNetworkAction->IsOperationSucceed())
    {
        m_eStatus = STATUS_LOGIN_FAIL;
        int nAccountId = pNetworkAction->GetExtraParam();
        ACCOUNT_INFO("Send Message to UI! result false");
        if (TA_URL_TIMEOUT == pNetworkAction->GetTaskActionErrorType())
        {
            etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId, REG_TIMEOUT);
        }
        else
        {
            etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId, REG_OTHER_ERROR);
        }
        //configParser_SetCfgItemIntValue(kszAccountType, nAccountId, 0);

        return;
    }

    CLOUD_OPERATION_TYPE eType = pNetworkAction->GetOptType();
    bool bSuc = false;
    int nCode = 0;

    int nAccountId = pNetworkAction->GetExtraParam();

    switch (eType)
    {
    case CLOUD_GET_EMAIL:
        {
            if (nAccountId != m_stAccountInfo.m_nAccountId)
            {
                break;
            }

            bSuc = MyLoadEmailFromFile(pNetworkAction->GetFilePath(), m_stAccountInfo.m_strRegName, nCode);
            if (bSuc)
            {
                m_stAccountInfo.m_strRegName = commonUnits_PwAesDataDecrypt(m_stAccountInfo.m_strRegName,
                                               kszAesDefaultKey);
                m_stAccountInfo.m_strServer = configParser_GetConfigString(kszCloudSipServer);

                SaveAccountLoginData(m_stAccountInfo);
            }
            else
            {
                m_eStatus = STATUS_LOGIN_FAIL;
            }
        }
        break;
    case CLOUD_GET_PIN_INFO:
        {
            if (nAccountId != m_stAccountInfo.m_nAccountId)
            {
                break;
            }

            bSuc = MyLoadAccountInfoFromFile(pNetworkAction->GetFilePath(), m_stAccountInfo, nCode);
            if (bSuc)
            {
                m_stAccountInfo.m_bSavePass = true;
                m_stAccountInfo.m_strServer = configParser_GetConfigString(kszCloudSipServer);

                SaveAccountLoginData(m_stAccountInfo);
            }
            else
            {
                m_eStatus = STATUS_LOGIN_FAIL;
            }
        }
        break;
    default:
        break;
    }

    if (!bSuc)
    {
        etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId,
                      nCode == 404 ? REG_PHONE_NOT_FOUND : REG_OTHER_ERROR);
    }

    remove(pNetworkAction->GetFilePath().c_str());

    return;
}

bool CCloudAccountInfoManager::GetLoginCloudAccountInfo(CloudAccountInfo & refData)
{
    for (int i = 0; i < m_listReq.size(); i++)
    {
        if (m_listReq[i].m_nAccountId == refData.m_nAccountId)
        {
            refData = m_listReq[i];
            return true;
        }
    }

    return false;
}

bool CCloudAccountInfoManager::LoadCloudAccount(YLVector<CloudAccountInfo> & listAccount,
        const yl::string & strPath)
{
    ACCOUNT_INFO("Load account [%s]", strPath.c_str());

    // 获取配置文件全路径.
    //yl::string strFullPath(szCloudAccountInfoFile);

    if (!pathFileExist(strPath.c_str()))
    {
        CALLLOG_WARN("file [%s] not exsit", strPath.c_str());
        return false;
    }

    xml_document doc;
    doc.load_file(strPath.c_str());

    xml_node rootNode = doc.child(kszCloudInfoNodeRoot);
    if (!rootNode)
    {
        return false;
    }

    for (xml_node node = rootNode.child(kszNodeAccountItem);
            node;
            node = node.next_sibling(kszNodeAccountItem))
    {
        CloudAccountInfo stInfo;
        stInfo.ParseFromXMLNode(node);

        listAccount.push_back(stInfo);
    }

    ACCOUNT_INFO("Load account [%d]", listAccount.size());

    return true;
}

bool CCloudAccountInfoManager::SaveToFile(YLVector<CloudAccountInfo> & list,
        const yl::string & strPath)
{
    ACCOUNT_INFO("save account [%s] size[%d]", strPath.c_str(), list.size());

    //yl::string strFullPath(szCloudAccountHistoryFile);

    if (!strPath.empty())
    {
        // 设置Xml文件头
        xml_document docSave;
        xml_node nodeDecl = docSave.prepend_child(node_declaration);
        nodeDecl.append_attribute("version", "1.0");
        nodeDecl.append_attribute("encoding", "utf-8");

        xml_node nodeRootNode = docSave.append_child(kszCloudInfoNodeRoot);

        int nSize = list.size();
        //保存数据
        for (int i = 0; i < nSize; i++)
        {
            CloudAccountInfo stInfo = list[i];
            xml_node nodeItem = nodeRootNode.append_child(kszNodeAccountItem);
            stInfo.ToXMLNode(nodeItem);
        }
        return docSave.save_file(strPath.c_str());
    }
    return false;
}

bool CCloudAccountInfoManager::GetCloudAccountInfo(VectorInfo & refData, SipAccountType eType,
        OrderType eOrderType/* = REVERSE_ORDER*/)
{
    refData.clear();
    if (REVERSE_ORDER == eOrderType)
    {
        for (int i = m_listHistory.size() - 1; i >= 0; i--)
        {
            CloudAccountInfo & info = m_listHistory[i];
            if (info.m_eType == eType)
            {
                refData.push_back(m_listHistory[i]);
            }
        }

        return true;
    }

    for (int i = 0; i < m_listHistory.size(); ++i)
    {
        CloudAccountInfo & info = m_listHistory[i];
        if (info.m_eType == eType)
        {
            refData.push_back(m_listHistory[i]);
        }
    }

    if (LEXICOGRAPHIC_ORDER == eOrderType)
    {
        yl::stable_sort(refData.begin(), refData.end(), CompareAccountInfo);
    }

    return true;
}

bool CCloudAccountInfoManager::GetCloudInfoByType(YLVector<CloudAccountInfo> & listInfo,
        CloudAccountInfo & refData)
{
    for (int i = 0; i < listInfo.size(); i++)
    {
        CloudAccountInfo & info = listInfo[i];
        if (info.m_eType == refData.m_eType)
        {
            refData = info;
            return true;
        }
    }

    return false;
}

void CCloudAccountInfoManager::FixDefaultInfo(CloudAccountInfo & info)
{
    CloudAccountInfo infoDefault;
    infoDefault.m_eType = info.m_eType;
    if (GetCloudInfoByType(m_listDefault, infoDefault))
    {
        FixCloudAccountInfo(info, infoDefault);
    }

    return;
}

void CCloudAccountInfoManager::FixCloudAccountInfo(CloudAccountInfo & infoTo,
        CloudAccountInfo & infoFrom)
{
    ACCOUNT_INFO("Fix default config type[%d]", infoFrom.m_eType);
    // 修正部分未处理的配置
    if (infoTo.m_nTransport == -1)
    {
        infoTo.m_nTransport = infoFrom.m_nTransport;
    }

    if (infoTo.m_nSrtp == -1)
    {
        infoTo.m_nSrtp = infoFrom.m_nSrtp;
    }

    if (infoTo.m_nDtmfType == -1)
    {
        infoTo.m_nDtmfType = infoFrom.m_nDtmfType;
    }

    if (infoTo.m_nDtmfInfoType == -1)
    {
        infoTo.m_nDtmfInfoType = infoFrom.m_nDtmfInfoType;
    }

    if (infoTo.m_nDtmfPayload == -1)
    {
        infoTo.m_nDtmfPayload = infoFrom.m_nDtmfPayload;
    }

    if (infoTo.m_nKeepAlive == -1)
    {
        infoTo.m_nKeepAlive = infoFrom.m_nKeepAlive;
    }

    if (infoTo.m_nPort == -1)
    {
        infoTo.m_nPort = infoFrom.m_nPort;
    }

    if (infoTo.m_nBfcp == -1)
    {
        infoTo.m_nBfcp = infoFrom.m_nBfcp;
    }

    if (infoTo.m_nFecc == -1)
    {
        infoTo.m_nFecc = infoFrom.m_nFecc;
    }

    if (infoTo.m_nExtvaddonce == -1)
    {
        infoTo.m_nExtvaddonce = infoFrom.m_nExtvaddonce;
    }

    if (infoTo.m_nExpires == -1)
    {
        infoTo.m_nExpires = infoFrom.m_nExpires;
    }

    return;
}

bool CCloudAccountInfoManager::RemoveHistoryCloudAccountInfo(const yl::string & strUserName,
        SipAccountType eType /*= ACCOUNT_YEALINK*/,
        bool bSaveToFile /*= true*/)
{
    int nSize = m_listHistory.size();

    bool bHandled = false;
    for (int i = 0; i < nSize; ++i)
    {
        CloudAccountInfo stTemp = m_listHistory[i];
        if (stTemp.m_strUserName == strUserName
                && stTemp.m_eType == eType)
        {
            m_listHistory.removeAt(i);
            bHandled = true;
            break;
        }
    }

    if (bHandled
            && bSaveToFile)
    {
        SaveToFile(m_listHistory, szCloudAccountHistoryFile);
    }

    return bHandled;
}

bool CCloudAccountInfoManager::ReplaceCloudAccountInfo(CloudAccountInfo & refData)
{
    for (int i = 0; i < m_listReq.size(); i++)
    {
        if (m_listReq[i].m_nAccountId == refData.m_nAccountId)
        {
            m_listReq[i] = refData;
            return true;
        }
    }

    return false;
}

bool CCloudAccountInfoManager::AddHistoryAccountInfo(const CloudAccountInfo & refData,
        bool bSaveToFile)
{
    // 保存到历史记录就认为已经处理过了
    //if (refData.m_eType != ACCOUNT_YEALINK)
//  {
//  //  refData.m_eStatus = STATUS_LOGIN_SUCC;
//  }

    // 移除历史记录中相同云平台的相同账号
    RemoveHistoryCloudAccountInfo(refData.m_strUserName, refData.m_eType, false);

    // 移除当前登录的账号
    // RemoveCloudAccountInfo(refData.m_nAccountId, false);

    int nSize = m_listHistory.size();

    // 同云平台的历史记录个数
    int nNum = 0;
    int nIndex = -1;
    //int nAccountIndex = -1;
    for (int i = 0; i < nSize; i++)
    {
        CloudAccountInfo & stInfo = m_listHistory[i];

        if (stInfo.m_eType != refData.m_eType)
        {
            continue;
        }

        // 注意：历史记录保存到xml是不记录账号id的
        if (nIndex == -1)
        {
            // 记住最后一条历史记录
            nIndex = i;
        }

        nNum++;
    }

    if (nNum >= MAX_INFO_NUM
            && nIndex != -1)
    {
        // 删除多余历史记录
        m_listHistory.removeAt(nIndex);
    }

    // 保存历史记录
    m_listHistory.push_back(refData);

    if (bSaveToFile)
    {
        SaveToFile(m_listHistory, szCloudAccountHistoryFile);
    }

    return true;
}


bool CCloudAccountInfoManager::ProcessLoginHistory(int nAccount, LINE_STATE eState)
{
    if (eState != LS_REGISTERED
            && eState != LS_REGISTER_FAIL)
    {
        // 只处理登录成功跟失败
        return false;
    }

    int nSize = m_listReq.size();

    if (nSize == 0)
    {
        return false;
    }

    bool bHangdled = false;
    for (int i = 0; i < nSize; i++)
    {
        CloudAccountInfo & stInfo = m_listReq[i];

        if (stInfo.m_nAccountId != nAccount)
        {
            continue;
        }

        if (stInfo.m_eStatus != STATUS_LOGIN_REQUEST)
        {
            // 登录请求才处理
            break;
        }

        if (eState == LS_REGISTER_FAIL)
        {
            bHangdled = true;
            // 注册失败就标记注册失败，防止继续注册
            stInfo.m_eStatus = STATUS_LOGIN_FAIL;
            break;
        }

        if (eState == LS_REGISTERED)
        {
            bHangdled = true;
            // 请求状态下，注册成功就标记登录成功，同时保存历史记录
            stInfo.m_eStatus = STATUS_LOGIN_SUCC;
            AddHistoryAccountInfo(stInfo, true);
        }

        break;
    }

    if (bHangdled)
    {
        // 登录请求处理成功
        SaveToFile(m_listReq, szCloudAccountRequestFile);
    }

    return true;
}

bool CCloudAccountInfoManager::SaveAccountLoginData(const CloudAccountInfo & stAccountInfo,
        bool bIsLogin/* = true*/)
{
    int nAccountId = stAccountInfo.m_nAccountId;
    ACCOUNT_INFO("Save account data AccountId [%d] type[%d]", stAccountInfo.m_nAccountId,
                 stAccountInfo.m_eType);

    if (nAccountId < 0
            || nAccountId > acc_AccountNum())
    {
        // 云账号只允许登录到0-15
        return false;
    }

    bool bXSIConfigChanged = false;
    bool bChanged = false;

    if (!stAccountInfo.m_strLabel.empty())
    {
        // Set account label.
        bChanged |= configParser_SetCfgItemStringValue(kszAccountLabel, nAccountId,
                    stAccountInfo.m_strLabel, CONFIG_LEVEL_PHONE);
    }
    else
    {
        // Set account label.
        bChanged |= configParser_SetCfgItemStringValue(kszAccountLabel, nAccountId,
                    stAccountInfo.m_strUserName, CONFIG_LEVEL_PHONE);
    }

    // Set account display name.
    bChanged = configParser_SetCfgItemStringValue(kszAccountDisplayName, nAccountId,
               stAccountInfo.m_strUserName,
               CONFIG_LEVEL_PHONE) || bChanged;
    // Set account register name.
    bool bRegisterChanged = configParser_SetCfgItemStringValue(kszAccountRegisterName, nAccountId,
                            stAccountInfo.m_strRegName, CONFIG_LEVEL_PHONE);
    bChanged = bRegisterChanged || bChanged;
    bXSIConfigChanged = bRegisterChanged || bXSIConfigChanged;

    // Set account username.
    bChanged = configParser_SetCfgItemStringValue(kszAccountUserName, nAccountId,
               stAccountInfo.m_strUserName, CONFIG_LEVEL_PHONE) || bChanged;
    // Set account password.
    bool bPwdChanged = configParser_SetCfgItemStringValue(kszAccountPassword, nAccountId,
                       stAccountInfo.m_strPassWord, CONFIG_LEVEL_PHONE);

    if (stAccountInfo.m_nDtmfType != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountDTMFType, nAccountId,
                    stAccountInfo.m_nDtmfType,
                    CONFIG_LEVEL_PHONE);
    }

    if (stAccountInfo.m_nDtmfInfoType != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountDTMFInfoType, nAccountId,
                    stAccountInfo.m_nDtmfInfoType, CONFIG_LEVEL_PHONE);
    }

    if (stAccountInfo.m_nDtmfPayload != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountDTMFPayload, nAccountId,
                    stAccountInfo.m_nDtmfPayload,
                    CONFIG_LEVEL_PHONE);
    }

    if (stAccountInfo.m_nKeepAlive != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountNATUDPUpdateTime, nAccountId,
                    stAccountInfo.m_nKeepAlive, CONFIG_LEVEL_PHONE);
    }

    if (stAccountInfo.m_nSrtp != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountSrtpEncryption, nAccountId,
                    stAccountInfo.m_nSrtp, CONFIG_LEVEL_PHONE);
    }

    bChanged = bPwdChanged || bChanged;
    bXSIConfigChanged = bPwdChanged || bXSIConfigChanged;

    bChanged |= configParser_SetCfgItemStringValue(kszAccountServerAddr, nAccountId,
                ACCOUNT_SERVER_PARAM1, bIsLogin ? stAccountInfo.m_strServer : "", CONFIG_LEVEL_PHONE);

    if (ACCOUNT_YEALINK == stAccountInfo.m_eType)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, nAccountId, 0,
                    CONFIG_LEVEL_PHONE);
    }

    if (stAccountInfo.m_nPort != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountSIPServer1Port, nAccountId,
                    ACCOUNT_SERVER_PARAM1,
                    stAccountInfo.m_nPort, CONFIG_LEVEL_PHONE);
    }

    if (stAccountInfo.m_nTransport != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszSipserverTransport, nAccountId,
                    ACCOUNT_SERVER_PARAM1, stAccountInfo.m_nTransport, CONFIG_LEVEL_PHONE);
    }
    else if (ACCOUNT_YEALINK == stAccountInfo.m_eType)
    {
#define TRANSPORT_TLS 2
        // PIN登录情况下外部没有指定传输方式，此处容错，强制写成TLS http://10.2.1.199/Bug.php?BugID=93704
        bChanged |= configParser_SetCfgItemIntValue(kszSipserverTransport, nAccountId,
                    ACCOUNT_SERVER_PARAM1, TRANSPORT_TLS, CONFIG_LEVEL_PHONE) || bChanged;
    }

    bChanged |= configParser_SetCfgItemIntValue(kszAccountEnable, nAccountId, bIsLogin ? 1 : 0,
                CONFIG_LEVEL_PHONE);

    configParser_SetCfgItemIntValue(kszAccountType, nAccountId, stAccountInfo.m_eType,
                                    CONFIG_LEVEL_PHONE);

    // 云账号bfcp默认开启;登出默认关闭
    int nBfcp = bIsLogin ? stAccountInfo.m_nBfcp : 0;
    if (nBfcp != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountBfcpSwitch, nAccountId, nBfcp);
    }

    // stAccountInfo.m_nFecc Fecc 不支持，暂不处理

    // Pexip 平台会设置为1
    if (stAccountInfo.m_nExtvaddonce != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountExtvAddOnce, nAccountId,
                    stAccountInfo.m_nExtvaddonce,
                    CONFIG_LEVEL_PHONE);
    }

    if (stAccountInfo.m_nExpires != -1)
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountSipserverExpire, nAccountId,
                    ACCOUNT_SERVER_PARAM1, stAccountInfo.m_nExpires, CONFIG_LEVEL_PHONE);
    }

    // 方案变更：配置不再写入sip ip账号
//  if (bIsLogin
//      && (stAccountInfo.m_eType == ACCOUNT_ZOOM
//      || stAccountInfo.m_eType == ACCOUNT_MIND
//      || stAccountInfo.m_eType == ACCOUNT_BLUEJEANS))
//  {
//      // 特殊写配置到账号16
//      int nP2PAccount = acc_GetDirectIPAccountID();
//      bool bP2PChanged = false;
//      // ip账号不能写地址
//      //bP2PChanged |= configParser_SetCfgItemStringValue(kszAccountServerAddr, nP2PAccount,
//      //  ACCOUNT_SERVER_PARAM1, bIsLogin ? stAccountInfo.m_strServer : "", CONFIG_LEVEL_PHONE);
//      if (stAccountInfo.m_nDtmfType != -1)
//      {
//          bP2PChanged |= configParser_SetCfgItemIntValue(kszAccountDTMFType, nP2PAccount, stAccountInfo.m_nDtmfType, CONFIG_LEVEL_PHONE);
//      }
//
//      if (stAccountInfo.m_nDtmfInfoType != -1)
//      {
//          bP2PChanged |= configParser_SetCfgItemIntValue(kszAccountDTMFInfoType, nP2PAccount, stAccountInfo.m_nDtmfInfoType, CONFIG_LEVEL_PHONE);
//      }
//
//      if (stAccountInfo.m_nDtmfPayload != -1)
//      {
//          bP2PChanged |= configParser_SetCfgItemIntValue(kszAccountDTMFPayload, nP2PAccount, stAccountInfo.m_nDtmfPayload, CONFIG_LEVEL_PHONE);
//      }
//
//      if (stAccountInfo.m_nTransport != -1)
//      {
//          bP2PChanged |= configParser_SetCfgItemIntValue(kszSipserverTransport, nP2PAccount,
//              ACCOUNT_SERVER_PARAM1, stAccountInfo.m_nTransport, CONFIG_LEVEL_PHONE);
//      }
//
//      if (stAccountInfo.m_nSrtp != -1)
//      {
//          bP2PChanged |= configParser_SetCfgItemIntValue(kszAccountSrtpEncryption, nP2PAccount,
//              stAccountInfo.m_nSrtp, CONFIG_LEVEL_PHONE);
//      }
//
//      if (stAccountInfo.m_nKeepAlive != -1)
//      {
//          bP2PChanged |= configParser_SetCfgItemIntValue(kszAccountNATUDPUpdateTime, nP2PAccount, stAccountInfo.m_nKeepAlive, CONFIG_LEVEL_PHONE);
//      }
//
//      if (stAccountInfo.m_nExpires != -1)
//      {
//          bP2PChanged |= configParser_SetCfgItemIntValue(kszAccountExpire, nP2PAccount, stAccountInfo.m_nExpires);
//      }
//
//      // 云账号bfcp默认开启
//      bP2PChanged |= configParser_SetCfgItemIntValue(kszAccountBfcpSwitch, nP2PAccount, (stAccountInfo.m_nBfcp != 1 ? 0 : 1));
//
//      if (bP2PChanged)
//      {
//          etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nP2PAccount);
//
//          //Notify Config Change
//          msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nP2PAccount);
//      }
//  }

    if (bIsLogin || bChanged)
    {
        ACCOUNT_INFO("Account Info Changed");
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nAccountId);

        //Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nAccountId);
    }

    if (bXSIConfigChanged)
    {
        //通知xsi信息改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_XSI_CONFIGE, nAccountId);
    }
    return true;
}

bool CCloudAccountInfoManager::CloudAccountLogout(int nAccountId)
{
    // 1.yealink登出要清空此数据
    if (acc_GetSipAccountType(nAccountId) == ACCOUNT_YEALINK)
    {
        m_stAccountInfo.Clear();
    }

    // 2.清空sip账号下的信息，关闭bfcp
    // 登出时，不改sip账号功能配置属性
    CloudAccountInfo stInfo;
    stInfo.m_nAccountId = nAccountId;
    SaveAccountLoginData(stInfo, false);

    // vc110/vcd 登出功能：disable账号，不清空登录请求数据
//  // 3.恢复account-login-list.xml文件到yealink默认设置
//  stInfo.m_eType = ACCOUNT_YEALINK;
//  CloudAccountInfo infoDefault;
//  infoDefault.m_eType = stInfo.m_eType;
//  if (GetCloudInfoByType(m_listDefault, infoDefault))
//  {
//      FixCloudAccountInfo(stInfo, infoDefault);
//  }
//  // 登出后，同时去掉请求文件的数据：（液晶UI会显示缓存）
//  ReplaceCloudAccountInfo(stInfo);
//  SaveToFile(m_listReq, szCloudAccountRequestFile);

    return true;
}

bool CCloudAccountInfoManager::ProcessYLLoginChange(int nAccountId, LINE_STATE eState,
        account_register_reason_t & sReason)
{
    if (acc_GetSipAccountType(nAccountId) != ACCOUNT_YEALINK)
    {
        ACCOUNT_WARN("not yealink cloud account");
        return false;
    }

    if (eState == LS_REGISTERED)
    {
        if (m_stAccountInfo.m_nAccountId == -1)
        {
            m_stAccountInfo.m_nAccountId = nAccountId;
        }

        // 注册上云账号，就将其他云账号标志位去掉
        for (int i = 0; i < acc_AccountNum(); i++)
        {
            if (i == nAccountId)
            {
                continue;
            }

            if (acc_GetSipAccountType(i) == ACCOUNT_YEALINK)
            {
                ACCOUNT_INFO("Set account [%d]", nAccountId);
                configParser_SetCfgItemIntValue(kszAccountType, i, ACCOUNT_COMMON);
            }
        }
    }

    if (nAccountId != m_stAccountInfo.m_nAccountId)
    {
        ACCOUNT_WARN("yealink cloud[%d] [%d]", nAccountId, m_stAccountInfo.m_nAccountId);
        return false;
    }

    m_eStatus = STATUS_LOGIN_SUCC;
    if (eState != LS_REGISTERED)
    {
        if (LS_REGISTER_FAIL == eState)
        {
            m_eStatus = STATUS_LOGIN_FAIL;

            // 注册失败，要标记注册失败
            ProcessLoginHistory(nAccountId, eState);

            //注册失败
            etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId,
                          GetFailCodeByReasonCode(sReason.m_iCode));
            ACCOUNT_INFO("Fail Code : %d", sReason.m_iCode);
        }
        return false;
    }

    if (!m_stAccountInfo.m_strPinCode.empty())
    {
        ACCOUNT_INFO("save pin login data");
        // PIN登录成功，要保存账号及密码，默认是记住密码
        yl::string strUserName = configParser_GetCfgItemStringValue(kszAccountUserName, nAccountId);
        if (m_stAccountInfo.m_strUserName == strUserName)
        {
            ACCOUNT_INFO("save pin login data");
            //AddHistoryAccountInfo(m_stAccountInfo, true);

            // 特殊处理：pin登录成功后，写入登录文件，用于web显示（否则网页会显示空）
            ReplaceCloudAccountInfo(m_stAccountInfo);

            SaveToFile(m_listReq, szCloudAccountRequestFile);
        }

        // 移除服务器上的PIN码
        RemovePinCode();

        m_stAccountInfo.m_strPinCode.clear();
    }

    // 注册成功要标记注册成功，然后写入历史记录
    ProcessLoginHistory(nAccountId, eState);

    configParser_SetCfgItemIntValue(kszAccountType, nAccountId, ACCOUNT_YEALINK);

    etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId, REG_SUCC);

    ACCOUNT_INFO("Process cloud change login Name[%s] Account[%d]",
                 m_stAccountInfo.m_strUserName.c_str(), nAccountId);

    return true;
}

bool CCloudAccountInfoManager::Process3rdLoginChange(int nAccountId, LINE_STATE eState,
        account_register_reason_t & sReason)
{
    SipAccountType eType = acc_GetSipAccountType(nAccountId);

    if (eType == ACCOUNT_ZOOM
            || eType ==  ACCOUNT_MIND
            || eType == ACCOUNT_PEXIP
            || eType == ACCOUNT_BLUEJEANS
            || eType == ACCOUNT_CUSTOM)
    {
        if (eState == LS_REGISTERED)
        {
            // 保存历史记录
            ProcessLoginHistory(nAccountId, eState);

            etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId, REG_SUCC);
        }
        else if (eState == LS_REGISTER_FAIL)
        {
            // 注册失败，要标记注册失败
            ProcessLoginHistory(nAccountId, eState);

            //注册失败
            etl_NotifyApp(FALSE, ACCOUNT_CLOUD_LOGIN_RESULT, nAccountId,
                          GetFailCodeByReasonCode(sReason.m_iCode));
        }

        return true;
    }

    return false;
}

bool CCloudAccountInfoManager::ProcessContactChange(int nAccountId, LINE_STATE eState)
{
    ACCOUNT_INFO("Process cloud contact change [%d] [%d]", nAccountId, eState);
    if (eState == LS_REGISTERED)
    {
        //重启重新注册，获者账号登陆
        if (acc_GetSipAccountType(nAccountId) == ACCOUNT_YEALINK
                || nAccountId == m_stAccountInfo.m_nAccountId)
        {
            CloudDir_Init(nAccountId);
        }
    }
    else if (LS_REGISTER_FAIL == eState
             || LS_DISABLE == eState
             || LS_BE_UNREGISTERED == eState)
    {
        if (CloudDir_IsAccountBindCloudDir(nAccountId))
        {
            CloudDir_UnInit(true);
        }
    }
    return false;
}

bool CCloudAccountInfoManager::ProcessCloudStateChange(int iAccountId, LINE_STATE eState,
        account_register_reason_t & sReason)
{
    if (!acc_IsCloudEnable())
    {
        return false;
    }

    ACCOUNT_INFO("ProcessCloudChange Accout[%d] State[%d] LoginID[%d] CurrType[%d]",
                 iAccountId, eState, m_stAccountInfo.m_nAccountId, acc_GetSipAccountType(iAccountId));
    // 处理第三方云账号状态改变
    if (Process3rdLoginChange(iAccountId, eState, sReason))
    {
        return true;
    }

    // 处理YL云账号登录改变
    ProcessContactChange(iAccountId, eState);

    ProcessYLLoginChange(iAccountId, eState, sReason);
    return false;
}

yl::string CCloudAccountInfoManager::GetRandomNumString()
{
    srand((unsigned) time(NULL));
    int nRandNum = rand() % 1000;
    return commonAPI_FormatString("%d", nRandNum);
}

CloudAccountRegFailCode CCloudAccountInfoManager::GetFailCodeByReasonCode(int ReasonCode)
{
    switch (ReasonCode)
    {
    case 401:
        return REG_UNAUTHORIZED;
        break;
    case 403:
        return REG_FORBIDDEN;
        break;
    case 404:
        return REG_NOT_FOUND;
        break;
    case 408:
        return REG_TIMEOUT;
        break;
    default:
        return REG_OTHER_ERROR;
        break;
    }
    return REG_OTHER_ERROR;
}

int CCloudAccountInfoManager::GetResultCodeFromXmlFile(const xml_document & doc)
{
#define kszCloudResult "Result"
#define kszCloudCode  "Code"
    xml_node rootNode = doc.child(kszCloudResult);
    if (rootNode)
    {
        xml_node node = rootNode.child(kszCloudCode);
        if (node)
        {
            yl::string strCode = node.child_value();
            return atoi(strCode.c_str());
        }
    }
    return 0;
}

bool CCloudAccountInfoManager::CancelLogin(int nAccount)
{
    if (nAccount != m_stAccountInfo.m_nAccountId)
    {
        ACCOUNT_INFO("CCloudAccountInfoManager::CancelLogin Failed [%d] [%d]",
                     nAccount, m_stAccountInfo.m_nAccountId);
        return true;
    }
    m_eStatus = STATUS_CANCEL_LOGINING;
    m_stAccountInfo.Clear();
    CloudAccountInfo stInfo;
    stInfo.m_nAccountId = nAccount;
    SaveAccountLoginData(stInfo, false);
    return true;
}


bool CCloudAccountInfoManager::OnNetworkStateChange()
{
    //LoadCloudAccount(m_listReq, szCloudAccountRequestFile);
    if (!netIsNetworkReady())
    {
        return false;
    }

    ProcessCloudLogin();
    return true;
}

bool CCloudAccountInfoManager::RemovePinCode()
{
    ACCOUNT_INFO("remove pin");
    // 向服务器请求删除Pin code;登录上就删除掉
    yl::string strUrl = configParser_GetConfigString(kszCloudHttpServer);
    if (!strUrl.empty() && strUrl.substr(strUrl.length() - 1, 1) != "/")
    {
        strUrl.append("/");
    }
    strUrl.append(kszPinRemove);

    // 生成本地保存路径.
    yl::string strFullPath(kszFileSavedName);

    AddUrlParameter(strUrl, "username", m_stAccountInfo.m_strUserName, false);
    yl::string strPsw = commonUnits_PwAesDataEncrypt(m_stAccountInfo.m_strPassWord, kszAesDefaultKey);
    AddUrlParameter(strUrl, "password", strPsw, false);
    AddUrlParameter(strUrl, "pin", m_stAccountInfo.m_strPinCode, false);

    ACCOUNT_INFO("CCloudAccountInfoManager::CloudPinLogin url[%s]", strUrl.c_str());

    if (!strUrl.empty())
    {
        CCloudAccountAction * pAction = new CCloudAccountAction(strFullPath.c_str(), strUrl.c_str());
        if (pAction != NULL)
        {
            pAction->SetRead(false);
            pAction->SetCallback(this);  // 回调函数.
            pAction->SetSync(false);  // 异步.
            pAction->SetOptType(CLOUD_REMOVE_PIN_CODE);
            //pAction->SetExtraParam(nAccountId);
            bool bRet = TaskAction_ExecAction(pAction);

            if (!bRet)
            {
                delete pAction;
            }

            return bRet;
        }
    }
    // 其他情况, 则下载失败.
    return false;
}

void CCloudAccountInfoManager::CheckCloud(bool bNotify)
{
    if (acc_IsCloudEnable())
    {
        YLVector<int> listTmp;
        for (int i = 0; i < m_listReq.size(); i++)
        {
            CloudAccountInfo & info = m_listReq[i];

            bool bError = false;
            // 相同账号id的登录请求只处理第一次
            for (int j = 0; j < listTmp.size(); j++)
            {
                int nId = listTmp[j];

                if (nId == info.m_nAccountId)
                {
                    bError = true;
                    break;
                }
            }

            if (bError)
            {
                // 此处依赖账号id从0到16排序
                // 容错处理，如果账号id已经被用过，就设置为下一个账号id
                info.m_nAccountId = i;
            }

            if (info.m_eType <= ACCOUNT_COMMON
                    || info.m_eType >= ACCOUNT_TYPE_END)
            {
                // 容错处理，如果账号id是非法值，就默认设置为yealink
                info.m_eType = ACCOUNT_YEALINK;
            }

            // 记录已经处理过的账号
            listTmp.push_back(info.m_nAccountId);
        }
        return;
    }

    // 登录账号清空：要保持账号格式
    ClearCloudAccount(m_listReq);
    SaveToFile(m_listReq, szCloudAccountRequestFile);

    // 历史记录清空
    if (m_listHistory.size() > 0)
    {
        m_listHistory.clear();
        SaveToFile(m_listHistory, szCloudAccountHistoryFile);
    }

    // 关闭云功能，把云账号都置为sip账号
    for (int i = 0; i < acc_AccountNum(); i++)
    {
        // 关闭云
        if (acc_GetSipAccountType(i) != ACCOUNT_COMMON)
        {
            ACCOUNT_INFO("Clear cloud account [%d]", i);
            bool bChanged = configParser_SetCfgItemIntValue(kszAccountEnable, i, 0);
            bChanged |= configParser_SetCfgItemStringValue(kszAccountLabel, i, "");
            bChanged |= configParser_SetCfgItemStringValue(kszAccountDisplayName, i, "");
            bChanged |= configParser_SetCfgItemStringValue(kszAccountUserName, i, "");
            bChanged |= configParser_SetCfgItemStringValue(kszAccountRegisterName, i, "");
            bChanged |= configParser_SetCfgItemStringValue(kszAccountPassword, i, "");
            bChanged |= configParser_SetCfgItemStringValue(kszAccountServerAddr, i, ACCOUNT_SERVER_PARAM1, "",
                        CONFIG_LEVEL_PHONE);
            // 强制关闭bfcp：本地测试服务器不支持，会导致无法呼出
            bChanged = configParser_SetCfgItemIntValue(kszAccountType, i, ACCOUNT_COMMON);
            bChanged |= configParser_SetCfgItemIntValue(kszAccountBfcpSwitch, i, 0);

            // http://10.2.1.199/Bug.php?BugID=94594

            if (bChanged
                    && bNotify)
            {
                // 账号类型是实时读取配置，因此GUI不需要更新
                etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, i);

                // 关闭vc account enable，dsskey可以及时刷新
                msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, i);
            }
        }
    }

    return;
}

void CCloudAccountInfoManager::ClearCloudAccount(YLVector<CloudAccountInfo> & listData)
{
    int nSize = listData.size();
    int nAccount;

    for (int i = 0; i < nSize; i++)
    {
        CloudAccountInfo & stInfo = listData[i];
        nAccount = stInfo.m_nAccountId;
        stInfo.Clear();
        stInfo.m_nAccountId = nAccount;
        stInfo.m_eType = ACCOUNT_YEALINK;

        FixDefaultInfo(stInfo);
    }

    return;
}

static bool CompareAccountInfo(CloudAccountInfo & stPreAccount, CloudAccountInfo & stNextAccount)
{
    if (stPreAccount.m_strUserName.empty())
    {
        return false;
    }

    if (stNextAccount.m_strUserName.empty())
    {
        return true;
    }

    return strcasecmp(stPreAccount.m_strUserName.c_str(), stNextAccount.m_strUserName.c_str()) < 0;
}
#endif //#ifdef IF_FEATURE_CLOUD

