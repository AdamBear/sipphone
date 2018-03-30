#include "talkhelp_inc.h"

#define BW_SECURITY_CLASS_EVENT "xBw:SecurityClassEvent"
#define BW_SECURITY_CLASS_ALLOW_LEVELS "xBw:allowedLevels"
#define BW_SECURITY_CLASS_OVERRIDE_LEVEL "xBw:overrideLevel"


IMPLEMENT_GETINSTANCE(CSecurityClassification)

CSecurityClassification::CSecurityClassification()
{
}

CSecurityClassification::~CSecurityClassification()
{
}

void CSecurityClassification::Init()
{
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        m_vecSecurityLev.push_back(SecurityLevel());
    }
}

// 该功能是否可用
bool CSecurityClassification::IsShowSecurity(int iAccount)
{
    // 可设置的列表为空
    if (iAccount < 0
            || iAccount >= m_vecSecurityLev.size())
    {
        return false;
    }

    // BSFT开关没开
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return false;
    }

    // 配置没开
    if (configParser_GetCfgItemIntValue(kszAccountEnableSecurityClassification, iAccount) != 1)
    {
        return false;
    }

    // 账号不是注册上的
    if (!acc_IsAccountAvailable(iAccount))
    {
        return false;
    }

    YLList<yl::string>& lstAllowLevels = m_vecSecurityLev[iAccount].lstAllowLevels;
    return lstAllowLevels.size() > 0;
}

// 获取账号的安全等级
bool CSecurityClassification::GetAccountSecurityLevel(yl::string& strCurLevel, int iAccount)
{
    // 账号合法性
    if (iAccount < 0
            || iAccount >= m_vecSecurityLev.size())
    {
        return false;
    }

    strCurLevel = m_vecSecurityLev[iAccount].strAccountLevel;
    return true;
}

// 获取账号的覆盖安全等级：通话中设置的等级
bool CSecurityClassification::GetAccountOverrideLevel(yl::string& strCurLevel, int iAccount)
{
    // 账号合法性
    if (iAccount < 0
            || iAccount >= m_vecSecurityLev.size())
    {
        return false;
    }

    strCurLevel = m_vecSecurityLev[iAccount].strOverrideLevel;
    return true;
}

// 获取相应账号的可设置列表
bool CSecurityClassification::GetAllowSecurityLevels(YLList<yl::string>& lstAllowLevels,
        int iAccount)
{
    // 账号合法性
    if (iAccount < 0
            || iAccount >= m_vecSecurityLev.size())
    {
        return false;
    }

    lstAllowLevels = m_vecSecurityLev[iAccount].lstAllowLevels;
    return true;
}

// 通话中修改账号的安全等级
bool CSecurityClassification::ModifyAccountOverrideLevel(int iAccount, const yl::string& strLevel)
{
    // 账号合法性
    if (iAccount < 0
            || iAccount >= m_vecSecurityLev.size())
    {
        return false;
    }

    // 设置的等级就是覆盖的等级
    if (strLevel == m_vecSecurityLev[iAccount].strOverrideLevel)
    {
        return false;
    }

    // strLevel必须要在允许的列表内
    if (!IsInAllowSecurityLevels(strLevel, m_vecSecurityLev[iAccount].lstAllowLevels))
    {
        return false;
    }

    // 组装要发送给服务器的XML命令
    yl::string strActionXml = FormatActionXml(strLevel);
    return modVoip_FeatureKeySync(iAccount, iAccount, strActionXml.c_str());
}

// 获取话路对应的安全等级
yl::string CSecurityClassification::GetTalkSecurityLevel(int iSessId)
{
    yl::string strLevel = "";

    if (m_mapTalkLev.find(iSessId) != m_mapTalkLev.end())
    {
        strLevel = m_mapTalkLev[iSessId];
    }
    else
    {
        // 该Session是本地会议, 则取子话路中最小等级
        GetLeastLevelInfoFromLocalConf(iSessId, strLevel);
    }

    return strLevel;
}

// 获取本地会议中安全等级最低的相关信息
int CSecurityClassification::GetLeastLevelInfoFromLocalConf(int iSessId, yl::string& strLevel)
{
    int iAccountId = -1;
    strLevel = "";

    if (!talklogic_IsLocalConfBySessionID(iSessId))
    {
        return false;
    }

    YLVector<int> list;
    if (!talklogic_GetSubSesseionID(iSessId, list))
    {
        // 不是本地会议
        return iAccountId;
    }

    for (YLVector<int>::iterator it = list.begin(); it != list.end(); ++it)
    {
        int iSessionID = *it;
        int iSubAccountID = talklogic_GetSessionAccount(iSessionID);

        if (iSubAccountID < 0 || iSubAccountID >= m_vecSecurityLev.size())
        {
            continue;
        }

        // 配置开启才处理
        if (IsShowSecurity(iSubAccountID))
        {
            // 第一次赋值账号
            if (iAccountId == -1)
            {
                iAccountId = iSubAccountID;
            }
            else
            {
                // 比较取到安全等级最小的账号
                yl::string& strOldLev = m_vecSecurityLev[iAccountId].strAccountLevel;
                yl::string& strNewLev = m_vecSecurityLev[iSubAccountID].strAccountLevel;
                if (TranslateSecurityLevel(strNewLev) > TranslateSecurityLevel(strOldLev))
                {
                    iAccountId = iSubAccountID;
                }
            }
        }

        // 最小等级的获取
        // 会议中若有服务器不推送的情况, 全部按无等级算
        yl::string strNewLevel = "Unclassified";
        if (m_mapTalkLev.find(iSessionID) != m_mapTalkLev.end())
        {
            strNewLevel = m_mapTalkLev[iSessionID];
        }

        if (TranslateSecurityLevel(strNewLevel) > TranslateSecurityLevel(strLevel))
        {
            strLevel = strNewLevel;
        }
    }

    TALK_INFO("SecurityClassification Account[%d], SecurityLev[%s]", iAccountId, strLevel.c_str());
    return iAccountId;
}

// 添加话路对应的安全等级
void CSecurityClassification::AddTalkSecurityLevel(int iSessId, const yl::string& strLevel)
{
    if (iSessId < 0
            || strLevel.empty())
    {
        return ;
    }

    m_mapTalkLev[iSessId] = strLevel;
}

// 删除话路对应的安全等级
void CSecurityClassification::DeleteTalkSecurityLevel(int iSessId)
{
    YLHashMap<int, yl::string>::iterator it = m_mapTalkLev.find(iSessId);
    if (it != m_mapTalkLev.end())
    {
        m_mapTalkLev.erase(it);

        TALK_WARN("SecurityClassification, TalkLev nSize=[%d]", m_mapTalkLev.size());
    }
}

// 解析XML
bool CSecurityClassification::ParseXML(int iAccount, LPCSTR pContent)
{
    // 账号合法性
    if (iAccount < 0
            || iAccount >= acc_AccountNum())
    {
        return false;
    }

    xml_document docXmlDoc;
    docXmlDoc.load_buffer(pContent, strlen(pContent));
    xml_node xmlNode = docXmlDoc.first_child();

    // 并非安全等级相关的数据, 不处理
    if (strcmp(xmlNode.name(), BW_SECURITY_CLASS_EVENT) != 0)
    {
        return false;
    }

    xml_node xmlTmp = xmlNode.child(BW_SECURITY_CLASS_ALLOW_LEVELS);
    if (xmlTmp.empty())
    {
        return false;
    }

    SecurityLevel& sInfo = m_vecSecurityLev[iAccount];
    sInfo.Clear();

    // 获取允许的安全等级列表
    xml_node child = xmlTmp.first_child();
    for (; child; child = child.next_sibling())
    {
        if (strcmp(child.child_value(), "") != 0)
        {
            TALK_DBG("ParseXML list level [%s]", child.child_value());
            sInfo.lstAllowLevels.push_back(child.child_value());
        }
    }

    // 设置账号的安全等级
    if (sInfo.lstAllowLevels.size() > 0)
    {
        sInfo.strAccountLevel = sInfo.lstAllowLevels.front();
    }

    // 设置覆盖的安全等级
    xmlTmp = xmlNode.child(BW_SECURITY_CLASS_OVERRIDE_LEVEL);
    if (0 == strcmp("", xmlTmp.child_value()))
    {
        sInfo.strOverrideLevel = sInfo.strAccountLevel;
    }
    else
    {
        sInfo.strOverrideLevel = xmlTmp.child_value();
    }

    TALK_INFO("SecurityClassification, Parser iAccount[%d], AccountLevel[%s], OverrideLevel[%s]", iAccount, sInfo.strAccountLevel.c_str(), sInfo.strOverrideLevel.c_str());
    return (sInfo.lstAllowLevels.size() > 0);
}

// 某个安全等级是否在可允许设置的列表中
bool CSecurityClassification::IsInAllowSecurityLevels(const yl::string& strLevel,
        YLList<yl::string>& lstAllowLevels)
{
    return yl::find(lstAllowLevels.begin(), lstAllowLevels.end(), strLevel) != lstAllowLevels.end();
}

// 组装修改的XML
yl::string CSecurityClassification::FormatActionXml(const yl::string& strLevel)
{
    yl::string strActionXml = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n"
                              "<xBw:SetSecurityClass xmlns:xBw=\"http://schema.broadsoft.com/as-feature-event\">\r\n"
                              "  <xBw:overrideLevel>";
    strActionXml += strLevel;
    strActionXml += "  </xBw:overrideLevel>\r\n"
                    "</xBw:SetSecurityClass>\r\n";

    return strActionXml;
}

// 解析话路对应的安全等级
yl::string CSecurityClassification::ParseTalkSecurityLevel(const yl::string& strContent)
{
    if (strContent.empty())
    {
        return "";
    }

    yl::string::size_type nPosStart = strContent.find_first_of('\"');
    if (yl::string::npos == nPosStart)
    {
        return "";
    }

    yl::string::size_type nPosEnd = strContent.find_last_of('\"');
    if (yl::string::npos == nPosEnd)
    {
        return "";
    }

    if (nPosStart + 1 >= nPosEnd - 1)
    {
        return "";
    }

    return strContent.substr(nPosStart + 1, nPosEnd - nPosStart - 1);
}

// 安全等级的级别数
int CSecurityClassification::TranslateSecurityLevel(const yl::string& strLevel)
{
    if (strLevel == "Top Secret")
    {
        return 1;
    }
    else if (strLevel == "Secret")
    {
        return 2;
    }
    else if (strLevel == "Restricted")
    {
        return 3;
    }
    else if (strLevel == "Classified")
    {
        return 4;
    }
    else if (strLevel == "Unclassified")
    {
        return 5;
    }

    return 0;
}

// 账号状态变更
bool CSecurityClassification::OnAccountStatusChange(int iAccountID)
{
    if (iAccountID < 0 || iAccountID >= m_vecSecurityLev.size())
    {
        return false;
    }

    LINE_STATE eState = acc_GetState(iAccountID);

    BOOL bStateChanging = eState != LS_REGISTERED
                          && eState != LS_BE_UNREGISTERED
                          && eState != LS_DISABLE
                          && eState != LS_REGISTER_FAIL;
    if (bStateChanging)
    {
        TALK_INFO("Security classification skip line state change!");
        return false;
    }

    if (!acc_IsAccountAvailable(iAccountID))
    {
        m_vecSecurityLev[iAccountID].Clear();
    }

    return true;
}
